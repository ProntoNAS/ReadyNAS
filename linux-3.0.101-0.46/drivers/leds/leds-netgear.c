/*
 * drivers/leds/leds-h1940.c
 * Copyright (c) Arnaud Patard <arnaud.patard@rtp-net.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 *
 * H1940 leds driver
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include "boardEnv/mvBoardEnvSpec.h"

static struct gpio_led rn102_leds[] = {
	{ .name = "backup",		.default_trigger = "none", .gpio = 56, .active_low = 1, .default_state = 2},
	{ .name = "SATA1_PRESENT",	.default_trigger = "none", .gpio = 15, .active_low = 1, .default_state = 2},
	{ .name = "SATA2_PRESENT",	.default_trigger = "none", .gpio = 14, .active_low = 1, .default_state = 2},
	{ .name = "power",		.default_trigger = "none", .gpio = 57, .active_low = 1, .default_state = 2},
};

static struct gpio_led rn104_leds[] = {
	{ .name = "backup",		.default_trigger = "none", .gpio = 63, .active_low = 0, .default_state = 2},
	{ .name = "power",		.default_trigger = "none", .gpio = 64, .active_low = 1, .default_state = 2},
	{ .name = "SATA1_PRESENT",	.default_trigger = "none", .gpio = 69, .active_low = 1, .default_state = 2},
	{ .name = "SATA2_PRESENT",	.default_trigger = "none", .gpio = 70, .active_low = 1, .default_state = 2},
	{ .name = "SATA3_PRESENT",	.default_trigger = "none", .gpio = 71, .active_low = 1, .default_state = 2},
	{ .name = "SATA4_PRESENT",	.default_trigger = "none", .gpio = 72, .active_low = 1, .default_state = 2},
};

static struct gpio_led s2000_leds[] = {
	{ .name = "backup",		.default_trigger = "none", .gpio = 56, .active_low = 1, .default_state = 2},
	{ .name = "SATA1_PRESENT",	.default_trigger = "none", .gpio = 15, .active_low = 1, .default_state = 2},
	{ .name = "SATA2_PRESENT",	.default_trigger = "none", .gpio = 14, .active_low = 1, .default_state = 2},
	{ .name = "power",		.default_trigger = "none", .gpio = 53, .active_low = 1, .default_state = 2},
};

static struct gpio_led_platform_data netgear_rn102_led_data = {
	.num_leds = ARRAY_SIZE(rn102_leds),
	.leds = rn102_leds,
};

static struct gpio_led_platform_data netgear_rn104_led_data = {
	.num_leds = ARRAY_SIZE(rn104_leds),
	.leds = rn104_leds,
};

static struct gpio_led_platform_data netgear_s2000_led_data = {
	.num_leds = ARRAY_SIZE(s2000_leds),
	.leds = s2000_leds,
};

static struct platform_device netgear_rn102_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &netgear_rn102_led_data,
	}
};

static struct platform_device netgear_rn104_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &netgear_rn104_led_data,
	}
};

static struct platform_device netgear_s2000_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &netgear_s2000_led_data,
	}
};

static struct platform_device *netgear_rn102_devs[] = {
	&netgear_rn102_leds,
};

static struct platform_device *netgear_rn104_devs[] = {
	&netgear_rn104_leds,
};

static struct platform_device *netgear_s2000_devs[] = {
	&netgear_s2000_leds,
};
static int __init netgear_setup(void)
{
	if (get_board_type() & NETGEAR_BD_TYPE_RN102)
		return platform_add_devices(netgear_rn102_devs, ARRAY_SIZE(netgear_rn102_devs));
	if (get_board_type() & NETGEAR_BD_TYPE_RN104)
		return platform_add_devices(netgear_rn104_devs, ARRAY_SIZE(netgear_rn104_devs));
	if (get_board_type() & NETGEAR_BD_TYPE_S2000)
		return platform_add_devices(netgear_s2000_devs, ARRAY_SIZE(netgear_s2000_devs));
	return -1;
}

module_init(netgear_setup);

MODULE_AUTHOR("andrew");
MODULE_DESCRIPTION("LED driver for Netgear Rn102 & Rn104 & S2000");
MODULE_LICENSE("GPL");
