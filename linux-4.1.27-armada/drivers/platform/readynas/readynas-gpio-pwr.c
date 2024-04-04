/*
 * NETGEAR ReadyNAS RN2xx GPIO power contoller
 *
 * Copyright (C) 2015 NETGEAR, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/gpio.h>
#include "../../gpio/gpiolib.h"
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>

static struct gpio_desc *power_off_desc;

static void gpio_power_off(void)
{
	gpiod_set_value(power_off_desc, 0);
	mdelay(10);
	gpiod_set_value(power_off_desc, 1);
	pr_emerg("Unable to shut down!!\n");
}

static int gpio_pwr_probe(struct platform_device *pdev)
{
	enum of_gpio_flags flags;
	struct device_node *np = pdev->dev.of_node;
	struct gpio_desc *desc =
			of_get_named_gpiod_flags(np, "gpios", 0, &flags);

	if (IS_ERR(desc))
		return (int)desc;

	if (flags & OF_GPIO_ACTIVE_LOW)
		set_bit(FLAG_ACTIVE_LOW, &desc->flags);
	power_off_desc = desc;
	pm_power_off = gpio_power_off;
	return 0;
}

static struct of_device_id gpio_pwr_dt_ids[] = {
	{
		.compatible = "gpio-power-off",
	}, {},
};

static struct platform_driver gpio_pwr_driver = {
	.probe = gpio_pwr_probe,
	.driver = {
		.name = "gpio-pwr",
		.of_match_table = gpio_pwr_dt_ids,
	},
};

module_platform_driver(gpio_pwr_driver);
