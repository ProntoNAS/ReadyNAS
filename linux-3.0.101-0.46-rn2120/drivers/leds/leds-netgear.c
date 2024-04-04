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

static struct gpio_led default_leds[] = {
	{ .name = "SATA1_ERR",	.default_trigger = "none", .gpio = 31, .active_low = 0, .default_state = 0},
	{ .name = "SATA2_ERR",	.default_trigger = "none", .gpio = 40, .active_low = 0, .default_state = 0},
	{ .name = "SATA3_ERR", .default_trigger = "none", .gpio = 44, .active_low = 0, .default_state = 0},
	{ .name = "SATA4_ERR", .default_trigger = "none", .gpio = 47, .active_low = 0, .default_state = 0},
	{ .name = "ERR_LED",	.default_trigger = "none", .gpio = 45, .active_low = 1, .default_state = 2},
};

static struct gpio_led_platform_data netgear_led_data = {
	.num_leds = ARRAY_SIZE(default_leds),
	.leds = default_leds,
};

static struct platform_device netgear_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &netgear_led_data,
	}
};

static struct platform_device *netgear_devs[] = {
	&netgear_leds,
};

static int __init netgear_setup(void)
{
	return platform_add_devices(netgear_devs, ARRAY_SIZE(netgear_devs));
}

module_init(netgear_setup);

MODULE_AUTHOR("andrew");
MODULE_DESCRIPTION("LED driver for Netgear ");
MODULE_LICENSE("GPL");
