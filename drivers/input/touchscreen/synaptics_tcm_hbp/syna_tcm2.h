/* SPDX-License-Identifier: GPL-2.0
 *
 * Synaptics TouchCom (TCM) high-bandwidth-protocol SPI touchscreen driver.
 *
 * Minimal mainline upper-layer header. The TouchComm protocol core
 * (synaptics_touchcom_*) is lifted verbatim from the Synaptics/OPPO
 * downstream "synaptics_hbp" driver; this header replaces the large
 * vendor framework header it expects, providing only the symbols the
 * core actually references: struct syna_tcm (the per-device context that
 * embeds the parsed touch-data blob), the power-state enums and the
 * health-info no-op shim. Everything else lives in the protocol core.
 */

#ifndef _SYNAPTICS_TCM2_H_
#define _SYNAPTICS_TCM2_H_

#include <linux/input.h>
#include <linux/spi/spi.h>
#include <linux/regulator/consumer.h>

#include "synaptics_touchcom_core_dev.h"
#include "synaptics_touchcom_func_base.h"
#include "synaptics_touchcom_func_touch.h"
#include "syna_tcm2_platform.h"

/* Power states. Only PWR_ON and SUB_PWR_RESUME_DONE are read by the
 * protocol core (in the gesture path); the rest mirror the vendor enum
 * order so future suspend/resume work stays source-compatible.
 */
enum power_state {
	PWR_OFF = 0,
	PWR_ON,
	LOW_PWR,
	BARE_MODE,
};

enum sub_power_state {
	SUB_PWR_NONE = 0,
	SUB_PWR_RESUMING,
	SUB_PWR_RESUME_DONE,
	SUB_PWR_EARLY_SUSPENDING,
	SUB_PWR_SUSPENDING,
	SUB_PWR_SUSPEND_DONE,
};

/**
 * struct syna_tcm - per-device context for the TCM-HBP touchscreen
 *
 * @tcm_dev:               handle returned by syna_tcm_allocate_device()
 * @hw_if:                 hardware interface abstraction passed to the core
 * @spi:                   the backing SPI device
 * @input_dev:             multitouch input device
 * @pdev:                  backing platform/spi device pointer
 * @tp_data:               parsed touch report (target of container_of() in
 *                         the protocol core's touch parser - keep the name)
 * @event_data:            scratch buffer that receives raw report payloads
 * @prev_obj_status:       per-slot previous object status for lift tracking
 * @pwr_state/@sub_pwr_state: read by the core gesture path
 * @health_monitor_support: read by the core; always false in this port
 * @max_objects/@max_x/@max_y: geometry copied from the application info
 * @irq_id:                requested interrupt number
 * @irq_enabled:           soft state mirroring the hardware mask
 */
struct syna_tcm {
	struct tcm_dev *tcm_dev;
	struct syna_hw_interface *hw_if;
	struct spi_device *spi;
	struct input_dev *input_dev;
	void *pdev;

	struct tcm_touch_data_blob tp_data;
	struct tcm_buffer event_data;
	unsigned char prev_obj_status[MAX_NUM_OBJECTS];

	int pwr_state;
	int sub_pwr_state;
	bool health_monitor_support;

	unsigned int max_objects;
	unsigned int max_x;
	unsigned int max_y;

	int irq_id;
	bool irq_enabled;
};

#endif /* _SYNAPTICS_TCM2_H_ */
