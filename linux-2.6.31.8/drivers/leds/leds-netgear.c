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
#ifdef CONFIG_USI_BOARD_DUO_V3
	{ .name = "backup", .default_trigger = "timer", .gpio = 29, .active_low = 1, .default_state = 2},
#else
	{ .name = "backup", .default_trigger = "timer", .gpio = 22, .active_low = 1, .default_state = 2},
#endif
	{ .name = "sys", .default_trigger = "timer", .gpio = 38, .active_low = 1, .default_state = 2},
	{ .name = "sys_fail", .default_trigger = "timer", .gpio = 37, .active_low = 1, .default_state = 2},
#ifdef CONFIG_USI_BOARD_DUO_V3
	{ .name = "SATA1_act", .default_trigger = "timer", .gpio = 23, .active_low = 1, .default_state = 2},
	{ .name = "SATA2_act", .default_trigger = "timer", .gpio = 22, .active_low = 1, .default_state = 2},
#else
	{ .name = "SATA1_act", .default_trigger = "timer", .gpio = 20, .active_low = 1, .default_state = 2},
	{ .name = "SATA2_act", .default_trigger = "timer", .gpio = 23, .active_low = 1, .default_state = 2},
	{ .name = "SATA3_act", .default_trigger = "timer", .gpio = 24, .active_low = 1, .default_state = 2},
	{ .name = "SATA4_act", .default_trigger = "timer", .gpio = 29, .active_low = 1, .default_state = 2},
#endif
	{ .name = "power", .default_trigger = "timer", .gpio = 31, .active_low = 1, .default_state = 2},
};

//static struct gpio_led_platform_data kacemini_led_data = {
static struct gpio_led_platform_data netgear_led_data = {
	.num_leds = ARRAY_SIZE(default_leds),
	.leds = default_leds,
	//.gpio_blink_set = ,
};

//static struct platform_device kacemini_leds = {
static struct platform_device netgear_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		//.platform_data = &kacemini_led_data,
		.platform_data = &netgear_led_data,
	}
};

//static struct platform_device *kacemini_devs[] = {
//	&kacemini_leds,
//};
static struct platform_device *netgear_devs[] = {
	&netgear_leds,
};

//static int __init kacemini_setup(void)
//{
//	return platform_add_devices(kacemini_devs, ARRAY_SIZE(kacemini_devs));
//}

static int __init netgear_setup(void)
{
	return platform_add_devices(netgear_devs, ARRAY_SIZE(netgear_devs));
}
//module_init(kacemini_setup);
module_init(netgear_setup);

MODULE_AUTHOR("andrew ");
MODULE_DESCRIPTION("LED driver for kacemini");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:kacemini-mv6282");
