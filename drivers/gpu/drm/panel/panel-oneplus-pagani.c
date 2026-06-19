// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/display/drm_dsc.h>
#include <drm/display/drm_dsc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct panel_aa594_p_7_a0020_dsc {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct drm_dsc_config dsc;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data panel_aa594_p_7_a0020_dsc_supplies[] = {
	{ .supply = "vddio" },
	{ .supply = "vci" },
};

static inline
struct panel_aa594_p_7_a0020_dsc *to_panel_aa594_p_7_a0020_dsc(struct drm_panel *panel)
{
	return container_of_const(panel, struct panel_aa594_p_7_a0020_dsc, panel);
}

static void panel_aa594_p_7_a0020_dsc_reset(struct panel_aa594_p_7_a0020_dsc *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(25);
}

static int panel_aa594_p_7_a0020_dsc_on(struct panel_aa594_p_7_a0020_dsc *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x1e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x9b, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1, 0x70, 0x9c, 0xc9);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x1f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83, 0x2c, 0x00, 0x4e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x84, 0x1b, 0x00, 0x4e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85, 0x35, 0x00, 0x4e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x8a, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x8b, 0x21, 0xa0);
	/*
	 * Vendor DSC PPS blob (0x80) dropped: the panel decoder is programmed
	 * by mipi_dsi_picture_parameter_set() instead, so it matches exactly
	 * what the DPU DSC encoder uses (no encoder/decoder PPS mismatch).
	 */
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x91, 0x03);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x80, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x82, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc8, 0x62);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x21);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa4, 0x18);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_CABC_MIN_BRIGHTNESS,
				     0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x60, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x61, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6d, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x35);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc6, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd2, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd3, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xed,
				     0xff, 0xff, 0xff, 0x0f, 0x07, 0xff, 0xcf,
				     0x45, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xee,
				     0xff, 0xfd, 0xc1, 0xf8, 0x21, 0x00, 0x80,
				     0x50);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbc, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbd, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x17);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0, 0x0c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83,
				     0x10, 0x00, 0x40, 0x00, 0x00, 0x0f, 0x0c,
				     0x0c, 0x00, 0x00, 0x00, 0x14, 0x4c, 0xac);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x2f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x1f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2, 0xf0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x6e);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd0, 0x67, 0x47, 0x67, 0x7f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd1, 0x67, 0x6e, 0x66, 0x91);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd2, 0x67, 0x66, 0x66, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd3, 0x66, 0xbb, 0x66, 0xb3);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd4, 0x66, 0xaa, 0x66, 0xa2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd5, 0x66, 0xdd, 0x66, 0xd5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd6, 0x66, 0xcc, 0x66, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd7, 0x66, 0xf7, 0x66, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x6b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x9e, 0x11, 0x55, 0x23, 0x13);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x9f, 0x17, 0x53, 0x27, 0x33);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0, 0x9d, 0x1a, 0x2a, 0x82);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe6, 0xe0, 0x54, 0x25, 0x33);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe7, 0xf6, 0x43, 0x27, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe8, 0xbc, 0x0c, 0x2c, 0xfd);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x2f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x53);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85, 0xcc, 0xe1);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x60, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x2d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf2, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x51);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x80,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x81,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x82,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x84,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x86,
				     0xd4, 0xd4, 0xd4, 0xd4, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x87,
				     0xd4, 0xd4, 0x00, 0x00, 0x22, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_DDB_START,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_PPS_START,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa3,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa4,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa5,
				     0xb6, 0xb6, 0xb6, 0xb6, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa6,
				     0xb6, 0xb6, 0xb1, 0xb1, 0x22, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa7,
				     0xd4, 0xd4, 0x00, 0x00, 0x22, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0,
				     0x68, 0x36, 0xbe, 0xb4, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1,
				     0xaa, 0xaa, 0xa0, 0xa0, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc2,
				     0x96, 0x96, 0x8c, 0x8c, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc3,
				     0x8c, 0x82, 0x82, 0x78, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4,
				     0x78, 0x6e, 0x6e, 0x6e, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5,
				     0x64, 0x64, 0x64, 0x64, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc6,
				     0x64, 0x64, 0x64, 0x64, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc7,
				     0x54, 0x54, 0x00, 0x00, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x52);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x80,
				     0x64, 0x32, 0xba, 0xb0, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x81,
				     0xa6, 0xa6, 0x9c, 0x9c, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x82,
				     0x92, 0x92, 0x88, 0x88, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x83,
				     0x88, 0x7e, 0x7e, 0x74, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x84,
				     0x74, 0x6a, 0x6a, 0x6a, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x85,
				     0x60, 0x60, 0x5f, 0x5f, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x86,
				     0x5f, 0x5f, 0x5d, 0x5d, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x87,
				     0x52, 0x52, 0x00, 0x00, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa0,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_DDB_START,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_READ_PPS_START,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa3,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa4,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa5,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa6,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xa7,
				     0x54, 0x54, 0x00, 0x00, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc2,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc3,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc6,
				     0x54, 0x54, 0x54, 0x54, 0x11, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc7,
				     0x54, 0x54, 0x00, 0x00, 0x11, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x5a, 0xa5, 0x00);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);

	return dsi_ctx.accum_err;
}

static int panel_aa594_p_7_a0020_dsc_off(struct panel_aa594_p_7_a0020_dsc *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int panel_aa594_p_7_a0020_dsc_prepare(struct drm_panel *panel)
{
	struct panel_aa594_p_7_a0020_dsc *ctx = to_panel_aa594_p_7_a0020_dsc(panel);
	struct device *dev = &ctx->dsi->dev;
	struct drm_dsc_picture_parameter_set pps;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(panel_aa594_p_7_a0020_dsc_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	panel_aa594_p_7_a0020_dsc_reset(ctx);

	ret = panel_aa594_p_7_a0020_dsc_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(panel_aa594_p_7_a0020_dsc_supplies), ctx->supplies);
		return ret;
	}

	drm_dsc_pps_payload_pack(&pps, &ctx->dsc);

	ret = mipi_dsi_picture_parameter_set(ctx->dsi, &pps);
	if (ret < 0) {
		dev_err(panel->dev, "failed to transmit PPS: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_compression_mode(ctx->dsi, true);
	if (ret < 0) {
		dev_err(dev, "failed to enable compression mode: %d\n", ret);
		return ret;
	}

	msleep(28); /* TODO: Is this panel-dependent? */

	return 0;
}

static int panel_aa594_p_7_a0020_dsc_unprepare(struct drm_panel *panel)
{
	struct panel_aa594_p_7_a0020_dsc *ctx = to_panel_aa594_p_7_a0020_dsc(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = panel_aa594_p_7_a0020_dsc_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(panel_aa594_p_7_a0020_dsc_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode panel_aa594_p_7_a0020_dsc_mode = {
	.clock = (1216 + 16 + 2 + 16) * (2640 + 60 + 4 + 36) * 120 / 1000,
	.hdisplay = 1216,
	.hsync_start = 1216 + 16,
	.hsync_end = 1216 + 16 + 2,
	.htotal = 1216 + 16 + 2 + 16,
	.vdisplay = 2640,
	.vsync_start = 2640 + 60,
	.vsync_end = 2640 + 60 + 4,
	.vtotal = 2640 + 60 + 4 + 36,
	.width_mm = 67,
	.height_mm = 146,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int panel_aa594_p_7_a0020_dsc_get_modes(struct drm_panel *panel,
					       struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &panel_aa594_p_7_a0020_dsc_mode);
}

static const struct drm_panel_funcs panel_aa594_p_7_a0020_dsc_panel_funcs = {
	.prepare = panel_aa594_p_7_a0020_dsc_prepare,
	.unprepare = panel_aa594_p_7_a0020_dsc_unprepare,
	.get_modes = panel_aa594_p_7_a0020_dsc_get_modes,
};

static int panel_aa594_p_7_a0020_dsc_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness_large(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

// TODO: Check if /sys/class/backlight/.../actual_brightness actually returns
// correct values. If not, remove this function.
static int panel_aa594_p_7_a0020_dsc_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness_large(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness;
}

static const struct backlight_ops panel_aa594_p_7_a0020_dsc_bl_ops = {
	.update_status = panel_aa594_p_7_a0020_dsc_bl_update_status,
	.get_brightness = panel_aa594_p_7_a0020_dsc_bl_get_brightness,
};

static struct backlight_device *
panel_aa594_p_7_a0020_dsc_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		/* stock values from dsi-panel-AA594-...-dsc-cmd-evt.dtsi:
		 * default-level 1433, max 4094 (bl-min 1) */
		.brightness = 1433,
		.max_brightness = 4094,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &panel_aa594_p_7_a0020_dsc_bl_ops, &props);
}

static int panel_aa594_p_7_a0020_dsc_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct panel_aa594_p_7_a0020_dsc *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct panel_aa594_p_7_a0020_dsc, panel,
				   &panel_aa594_p_7_a0020_dsc_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(panel_aa594_p_7_a0020_dsc_supplies),
					    panel_aa594_p_7_a0020_dsc_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB101010;
	/*
	 * Downstream dsi-panel-AA594-P-7-A0020-dsc-cmd-evt.dtsi:
	 * qcom,mdss-dsi-panel-type = "dsi_cmd_mode" + te-using-te-pin.
	 * Command mode, NOT video. lmdpdg mis-guessed VIDEO_BURST.
	 */
	dsi->mode_flags = MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = panel_aa594_p_7_a0020_dsc_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	/* This panel only supports DSC; unconditionally enable it */
	dsi->dsc = &ctx->dsc;

	ctx->dsc.dsc_version_major = 1;
	ctx->dsc.dsc_version_minor = 2;

	/* TODO: Pass slice_per_pkt = 2 */
	ctx->dsc.slice_height = 20;
	ctx->dsc.slice_width = 608;
	/*
	 * TODO: hdisplay should be read from the selected mode once
	 * it is passed back to drm_panel (in prepare?)
	 */
	WARN_ON(1216 % ctx->dsc.slice_width);
	ctx->dsc.slice_count = 1216 / ctx->dsc.slice_width;
	ctx->dsc.bits_per_component = 10;
	ctx->dsc.bits_per_pixel = 8 << 4; /* 4 fractional bits, target 8 bpp */
	/*
	 * Downstream config (dsi-panel-AA594-...-dsc-cmd-evt.dtsi) has NO
	 * 4:2:0 property: bpp=30 (RGB101010), dsc-bit-per-component=10,
	 * dsc-bit-per-pixel=8 -> a standard RGB DSC panel. native_420 was a
	 * wrong assumption; leave it false so msm sets convert_rgb=1 and the
	 * 1.1-PRE_SCR RC params, with no CDM in the path.
	 */
	ctx->dsc.convert_rgb = true;
	ctx->dsc.block_pred_enable = true;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void panel_aa594_p_7_a0020_dsc_remove(struct mipi_dsi_device *dsi)
{
	struct panel_aa594_p_7_a0020_dsc *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id panel_aa594_p_7_a0020_dsc_of_match[] = {
	{ .compatible = "panel,aa594-p-7-a0020-dsc" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, panel_aa594_p_7_a0020_dsc_of_match);

static struct mipi_dsi_driver panel_aa594_p_7_a0020_dsc_driver = {
	.probe = panel_aa594_p_7_a0020_dsc_probe,
	.remove = panel_aa594_p_7_a0020_dsc_remove,
	.driver = {
		.name = "panel-panel-aa594-p-7-a0020-dsc",
		.of_match_table = panel_aa594_p_7_a0020_dsc_of_match,
	},
};
module_mipi_dsi_driver(panel_aa594_p_7_a0020_dsc_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for AA594 P 7 A0020 dsc cmd mode panel");
MODULE_LICENSE("GPL");
