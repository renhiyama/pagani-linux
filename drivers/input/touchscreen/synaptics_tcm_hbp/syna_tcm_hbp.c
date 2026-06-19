// SPDX-License-Identifier: GPL-2.0
/*
 * Synaptics TouchCom (TCM) high-bandwidth-protocol SPI touchscreen driver.
 *
 * Mainline bring-up driver for the Synaptics S3910 touch controller as
 * found on the OnePlus 13s/13T (pagani, SM8750), device-tree compatible
 * "synaptics,tcm-spi-hbp". The wire protocol is handled by the lifted
 * TouchComm core (synaptics_touchcom_*); this file provides the two things
 * the core leaves to the platform:
 *
 *   1. the hardware interface (SPI read/write, reset, irq gating, power)
 *   2. the upper input layer (probe, threaded irq, report -> input events)
 *
 * Command responses are read by polling (RESP_IN_POLLING) so the core masks
 * the irq for the duration of a command itself; the interrupt is therefore
 * used only for unsolicited touch reports, which keeps the concurrency model
 * simple - no completion handshake between the command path and the isr.
 *
 * The S3910 runs its application firmware from on-chip flash, so there is no
 * boot-time firmware download path here.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/spi/spi.h>
#include <linux/of.h>

#include "syna_tcm2.h"
#include "synaptics_touchcom_core_dev.h"
#include "synaptics_touchcom_func_base.h"
#include "synaptics_touchcom_func_touch.h"
#include "syna_tcm2_platform.h"

#define SYNA_HBP_NAME "synaptics_tcm_hbp"

/* SPI transfer chunk ceiling (the geni controller handles larger, but keep
 * a sane bound mirrored from the vendor default). */
#define SYNA_SPI_XFER_MAX (64 * 1024)

/* private hw interface, embeds the abstract interface the core sees */
struct syna_hw_spi {
	struct syna_hw_interface hw_if;
	struct spi_device *spi;
	struct gpio_desc *reset_gpiod;
	struct gpio_desc *irq_gpiod;
	struct regulator *vdd;
	struct regulator *avdd;
	struct syna_tcm *tcm;
};

#define to_hw_spi(p) container_of(p, struct syna_hw_spi, hw_if)

/* ---- hardware interface operations (called by the protocol core) ---- */

static int syna_spi_read(struct syna_hw_interface *hw_if,
		unsigned char *rd_data, unsigned int rd_len)
{
	struct syna_hw_spi *hw = to_hw_spi(hw_if);
	struct spi_transfer xfer = {
		.rx_buf = rd_data,
		.len = rd_len,
		.speed_hz = hw_if->bdata_io.frequency_hz,
	};
	struct spi_message msg;
	int retval;

	if (!rd_data || rd_len == 0)
		return 0;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);
	retval = spi_sync(hw->spi, &msg);
	if (retval < 0) {
		dev_err(&hw->spi->dev, "spi read (%u bytes) failed: %d\n",
			rd_len, retval);
		return retval;
	}
	return rd_len;
}

static int syna_spi_write(struct syna_hw_interface *hw_if,
		unsigned char *wr_data, unsigned int wr_len)
{
	struct syna_hw_spi *hw = to_hw_spi(hw_if);
	struct spi_transfer xfer = {
		.tx_buf = wr_data,
		.len = wr_len,
		.speed_hz = hw_if->bdata_io.frequency_hz,
	};
	struct spi_message msg;
	int retval;

	if (!wr_data || wr_len == 0)
		return 0;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);
	retval = spi_sync(hw->spi, &msg);
	if (retval < 0) {
		dev_err(&hw->spi->dev, "spi write (%u bytes) failed: %d\n",
			wr_len, retval);
		return retval;
	}
	return wr_len;
}

static void syna_hw_reset(struct syna_hw_interface *hw_if)
{
	struct syna_hw_spi *hw = to_hw_spi(hw_if);
	struct syna_hw_rst_data *rst = &hw_if->bdata_rst;

	if (!hw->reset_gpiod)
		return;

	/* assert reset (active low), hold, release, then wait for the IC to
	 * come back up and emit its identify report. */
	gpiod_set_value_cansleep(hw->reset_gpiod, 1);
	msleep(rst->reset_active_ms ? rst->reset_active_ms : 5);
	gpiod_set_value_cansleep(hw->reset_gpiod, 0);
	msleep(rst->reset_delay_ms ? rst->reset_delay_ms : 25);
}

static int syna_power_on(struct syna_hw_interface *hw_if, bool en)
{
	struct syna_hw_spi *hw = to_hw_spi(hw_if);
	int retval;

	if (en) {
		if (hw->vdd) {
			retval = regulator_enable(hw->vdd);
			if (retval < 0)
				return retval;
		}
		if (hw->avdd) {
			retval = regulator_enable(hw->avdd);
			if (retval < 0) {
				if (hw->vdd)
					regulator_disable(hw->vdd);
				return retval;
			}
		}
		msleep(hw_if->bdata_pwr.power_on_delay_ms ?
		       hw_if->bdata_pwr.power_on_delay_ms : 20);
	} else {
		if (hw->avdd)
			regulator_disable(hw->avdd);
		if (hw->vdd)
			regulator_disable(hw->vdd);
	}
	return 0;
}

static int syna_enable_irq(struct syna_hw_interface *hw_if, bool en)
{
	struct syna_hw_spi *hw = to_hw_spi(hw_if);
	struct syna_tcm *tcm = hw->tcm;

	if (!tcm || tcm->irq_id <= 0)
		return 0;

	if (en) {
		if (!tcm->irq_enabled) {
			enable_irq(tcm->irq_id);
			tcm->irq_enabled = true;
		}
	} else {
		if (tcm->irq_enabled) {
			disable_irq(tcm->irq_id);
			tcm->irq_enabled = false;
		}
	}
	return 0;
}

/* ---- input event forwarding ---- */

static void syna_report_input(struct syna_tcm *tcm)
{
	struct input_dev *input_dev = tcm->input_dev;
	struct tcm_touch_data_blob *touch_data = &tcm->tp_data;
	struct tcm_objects_data_blob *object_data;
	unsigned int idx;
	unsigned int max_objects = tcm->tcm_dev->max_objects;
	int status;

	if (!input_dev)
		return;

	object_data = &touch_data->object_data[0];

	for (idx = 0; idx < max_objects; idx++) {
		if (tcm->prev_obj_status[idx] == LIFT &&
		    object_data[idx].status == LIFT)
			status = NOP;
		else
			status = object_data[idx].status;

		switch (status) {
		case LIFT:
			input_mt_slot(input_dev, idx);
			input_mt_report_slot_state(input_dev, MT_TOOL_FINGER,
						   false);
			break;
		case NOP:
			break;
		default:
			/* any non-lift classification is a present contact */
			input_mt_slot(input_dev, idx);
			input_mt_report_slot_state(input_dev, MT_TOOL_FINGER,
						   true);
			input_report_abs(input_dev, ABS_MT_POSITION_X,
					 object_data[idx].x_pos);
			input_report_abs(input_dev, ABS_MT_POSITION_Y,
					 object_data[idx].y_pos);
			input_report_abs(input_dev, ABS_MT_TOUCH_MAJOR,
					 object_data[idx].x_width);
			input_report_abs(input_dev, ABS_MT_TOUCH_MINOR,
					 object_data[idx].y_width);
			break;
		}
		tcm->prev_obj_status[idx] = object_data[idx].status;
	}

	input_mt_sync_frame(input_dev);
	input_sync(input_dev);
}

/* (Re)enable REPORT_TOUCH streaming. The controller only starts streaming
 * when the LBP protocol-select is the last control command before the enable,
 * so the two must stay paired. Used at probe, and again whenever the controller
 * reboots and reverts to its non-streaming default - e.g. the in-cell touch
 * silicon power-cycles when the AA594 panel is blanked. */
static int syna_tcm_enable_touch_report(struct syna_tcm *tcm)
{
	struct device *dev = &tcm->spi->dev;
	int retval;

	retval = syna_tcm_set_dynamic_config(tcm->tcm_dev, DC_CONTROL_LBP_HBP,
					     0x02, RESP_IN_POLLING);
	if (retval < 0)
		dev_warn(dev, "failed to re-select LBP mode: %d\n", retval);

	retval = syna_tcm_enable_report(tcm->tcm_dev, REPORT_TOUCH, true);
	if (retval < 0) {
		dev_err(dev, "failed to enable touch report: %d\n", retval);
		return retval;
	}

	return 0;
}

/* Re-arm touch reporting whenever userspace opens the evdev node. The
 * controller can fall out of streaming after a spontaneous reset; opening the
 * node (evtest, libinput, a compositor) then re-enables it without needing a
 * driver reload. Resets that happen while the node is already open are handled
 * by the REPORT_IDENTIFY path in the ISR. */
static int syna_input_open(struct input_dev *input_dev)
{
	struct syna_tcm *tcm = input_get_drvdata(input_dev);

	syna_tcm_enable_touch_report(tcm);
	return 0;
}

/* ---- threaded interrupt: drain one report and forward it ---- */

static irqreturn_t syna_dev_isr(int irq, void *data)
{
	struct syna_tcm *tcm = data;
	unsigned char code = 0;
	int retval;

	retval = syna_tcm_get_event_data(tcm->tcm_dev, &code, &tcm->event_data);
	if (retval < 0)
		return IRQ_HANDLED;

	if (code == REPORT_TOUCH) {
		retval = syna_tcm_parse_touch_report(tcm->tcm_dev,
				tcm->event_data.buf,
				tcm->event_data.data_length,
				&tcm->tp_data);
		if (retval < 0) {
			dev_warn_ratelimited(&tcm->spi->dev,
					     "parse touch report failed: %d\n",
					     retval);
			return IRQ_HANDLED;
		}

		syna_report_input(tcm);
	} else if (code == REPORT_IDENTIFY) {
		/* Controller rebooted (e.g. in-cell touch lost power when the
		 * panel was blanked) and dropped to its non-streaming default.
		 * Re-arm touch reporting so taps keep working without a manual
		 * driver reload. */
		dev_info_ratelimited(&tcm->spi->dev,
				     "controller reset; re-enabling touch report\n");
		syna_tcm_enable_touch_report(tcm);
	}

	return IRQ_HANDLED;
}

/* ---- input device setup ---- */

static int syna_set_up_input_device(struct syna_tcm *tcm)
{
	struct tcm_dev *tcm_dev = tcm->tcm_dev;
	struct input_dev *input_dev;
	int retval;

	input_dev = devm_input_allocate_device(&tcm->spi->dev);
	if (!input_dev)
		return -ENOMEM;

	input_dev->name = "Synaptics TCM Touchscreen";
	input_dev->phys = "synaptics_tcm_hbp/input0";
	input_dev->id.bustype = BUS_SPI;
	input_dev->dev.parent = &tcm->spi->dev;

	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(BTN_TOUCH, input_dev->keybit);
	set_bit(BTN_TOOL_FINGER, input_dev->keybit);
	set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X,
			     0, tcm_dev->max_x, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y,
			     0, tcm_dev->max_y, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 0xff, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MINOR, 0, 0xff, 0, 0);

	retval = input_mt_init_slots(input_dev, tcm_dev->max_objects,
				     INPUT_MT_DIRECT);
	if (retval < 0)
		return retval;

	input_set_drvdata(input_dev, tcm);
	input_dev->open = syna_input_open;

	retval = input_register_device(input_dev);
	if (retval < 0)
		return retval;

	tcm->input_dev = input_dev;
	tcm->max_objects = tcm_dev->max_objects;
	tcm->max_x = tcm_dev->max_x;
	tcm->max_y = tcm_dev->max_y;
	return 0;
}

/* ---- probe / remove ---- */

static int syna_hbp_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	struct syna_hw_spi *hw;
	struct syna_hw_interface *hw_if;
	struct syna_tcm *tcm;
	int retval;

	hw = devm_kzalloc(dev, sizeof(*hw), GFP_KERNEL);
	if (!hw)
		return -ENOMEM;

	tcm = devm_kzalloc(dev, sizeof(*tcm), GFP_KERNEL);
	if (!tcm)
		return -ENOMEM;

	hw->spi = spi;
	hw->tcm = tcm;
	hw_if = &hw->hw_if;
	hw_if->pdev = spi;

	/* bus parameters */
	hw_if->bdata_io.type = 1; /* SPI */
	hw_if->bdata_io.frequency_hz = spi->max_speed_hz;
	hw_if->bdata_io.rd_chunk_size = SYNA_SPI_XFER_MAX;
	hw_if->bdata_io.wr_chunk_size = SYNA_SPI_XFER_MAX;
	syna_pal_mutex_alloc(&hw_if->bdata_io.io_mutex);

	/* reset / power timings: hold reset low briefly, then allow the
	 * controller time to boot its firmware and emit the identify report
	 * before the first bus transaction. */
	hw_if->bdata_rst.reset_active_ms = 10;
	hw_if->bdata_rst.reset_delay_ms = 100;
	hw_if->bdata_pwr.power_on_delay_ms = 20;

	/* reset is active-low (reset-gpios); start deasserted, hw_reset pulses */
	hw->reset_gpiod = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(hw->reset_gpiod))
		return dev_err_probe(dev, PTR_ERR(hw->reset_gpiod),
				     "failed to get reset gpio\n");
	if (!hw->reset_gpiod)
		dev_warn(dev, "no reset gpio found\n");

	/* regulators are optional: on this board the touch rails come from a
	 * PMIC that is not modelled in mainline yet and are left enabled by
	 * the bootloader, so absence is not fatal. */
	hw->vdd = devm_regulator_get_optional(dev, "vdd");
	if (IS_ERR(hw->vdd))
		hw->vdd = NULL;
	hw->avdd = devm_regulator_get_optional(dev, "vcc");
	if (IS_ERR(hw->avdd))
		hw->avdd = NULL;

	hw_if->ops_read_data = syna_spi_read;
	hw_if->ops_write_data = syna_spi_write;
	hw_if->ops_hw_reset = syna_hw_reset;
	hw_if->ops_power_on = syna_power_on;
	hw_if->ops_enable_irq = syna_enable_irq;
	hw_if->ops_wait_irq = NULL;   /* responses read by polling */
	hw_if->ops_bus_setup = NULL;  /* spi already configured */

	spi->bits_per_word = 8;
	retval = spi_setup(spi);
	if (retval < 0)
		return dev_err_probe(dev, retval, "spi_setup failed\n");

	tcm->spi = spi;
	tcm->hw_if = hw_if;
	tcm->pdev = spi;
	tcm->pwr_state = PWR_ON;
	tcm->health_monitor_support = false;
	syna_tcm_buf_init(&tcm->event_data);
	spi_set_drvdata(spi, tcm);

	/* power + reset the controller before talking to it */
	syna_power_on(hw_if, true);
	syna_hw_reset(hw_if);

	/* allocate the TouchComm core; responses read by polling */
	retval = syna_tcm_allocate_device(&tcm->tcm_dev, hw_if,
					  RESP_IN_POLLING);
	if (retval < 0)
		return dev_err_probe(dev, retval,
				     "failed to allocate tcm device\n");

	retval = syna_tcm_detect_device(tcm->tcm_dev);
	if (retval < 0) {
		dev_err(dev, "failed to detect device: %d\n", retval);
		goto err_remove_dev;
	}

	if (retval != MODE_APPLICATION_FIRMWARE) {
		dev_err(dev, "device not in application firmware mode (0x%02x)\n",
			retval);
		retval = -ENODEV;
		goto err_remove_dev;
	}

	retval = syna_tcm_get_app_info(tcm->tcm_dev, &tcm->tcm_dev->app_info);
	if (retval < 0) {
		dev_err(dev, "failed to get application info: %d\n", retval);
		goto err_remove_dev;
	}

	/* Select the low-bandwidth protocol (standard REPORT_TOUCH 0x11)
	 * BEFORE reading the touch report descriptor: the device exposes a
	 * different report layout per protocol, so the descriptor must be
	 * fetched while in the protocol we will actually parse. 2 = LBP,
	 * 1 = HBP.
	 */
	retval = syna_tcm_set_dynamic_config(tcm->tcm_dev, DC_CONTROL_LBP_HBP,
					     0x02, RESP_IN_POLLING);
	if (retval < 0)
		dev_warn(dev, "failed to select LBP mode: %d\n", retval);

	/* fetch the device's own touch report descriptor for the parser */
	retval = syna_tcm_preserve_touch_report_config(tcm->tcm_dev);
	if (retval < 0) {
		dev_err(dev, "failed to read touch report config: %d\n", retval);
		goto err_remove_dev;
	}

	dev_info(dev, "S3910 TCM-HBP: %ux%u, %u objects\n",
		 tcm->tcm_dev->max_x, tcm->tcm_dev->max_y,
		 tcm->tcm_dev->max_objects);

	retval = syna_set_up_input_device(tcm);
	if (retval < 0) {
		dev_err(dev, "failed to set up input device: %d\n", retval);
		goto err_remove_dev;
	}

	/* the irq starts masked; the threaded handler drains touch reports */
	tcm->irq_id = spi->irq;
	tcm->irq_enabled = false;
	retval = devm_request_threaded_irq(dev, spi->irq, NULL, syna_dev_isr,
					   hw_if->bdata_attn.irq_flags |
						   IRQF_ONESHOT,
					   SYNA_HBP_NAME, tcm);
	if (retval < 0) {
		dev_err(dev, "failed to request irq %d: %d\n", spi->irq, retval);
		goto err_remove_dev;
	}
	tcm->irq_enabled = true;

	/* Re-assert LBP immediately before enabling the report: the device
	 * only begins streaming REPORT_TOUCH when the protocol select is the
	 * last control command before the enable. Also re-armed from the ISR
	 * on REPORT_IDENTIFY if the controller ever reboots. */
	retval = syna_tcm_enable_touch_report(tcm);
	if (retval < 0)
		goto err_remove_dev;

	dev_info(dev, "Synaptics TCM-HBP touchscreen ready\n");
	return 0;

err_remove_dev:
	if (tcm->tcm_dev)
		syna_tcm_remove_device(tcm->tcm_dev);
	syna_tcm_buf_release(&tcm->event_data);
	syna_power_on(hw_if, false);
	return retval;
}

static void syna_hbp_remove(struct spi_device *spi)
{
	struct syna_tcm *tcm = spi_get_drvdata(spi);

	if (!tcm)
		return;

	if (tcm->tcm_dev) {
		syna_tcm_enable_report(tcm->tcm_dev, REPORT_TOUCH, false);
		syna_tcm_remove_device(tcm->tcm_dev);
	}
	syna_tcm_buf_release(&tcm->event_data);
	if (tcm->hw_if)
		syna_power_on(tcm->hw_if, false);
}

static const struct spi_device_id syna_hbp_spi_id[] = {
	{ "tcm-spi-hbp", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, syna_hbp_spi_id);

static const struct of_device_id syna_hbp_of_match[] = {
	{ .compatible = "synaptics,tcm-spi-hbp" },
	{ }
};
MODULE_DEVICE_TABLE(of, syna_hbp_of_match);

static struct spi_driver syna_hbp_driver = {
	.driver = {
		.name = SYNA_HBP_NAME,
		.of_match_table = syna_hbp_of_match,
	},
	.probe = syna_hbp_probe,
	.remove = syna_hbp_remove,
	.id_table = syna_hbp_spi_id,
};
module_spi_driver(syna_hbp_driver);

MODULE_AUTHOR("renhiyama");
MODULE_DESCRIPTION("Synaptics TCM high-bandwidth-protocol SPI touchscreen");
MODULE_LICENSE("GPL");
