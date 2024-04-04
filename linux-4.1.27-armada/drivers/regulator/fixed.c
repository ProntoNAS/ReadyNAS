/*
 * fixed.c
 *
 * Copyright 2008 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * Copyright (c) 2009 Nokia Corporation
 * Roger Quadros <ext-roger.quadros@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This is useful for systems with mixed controllable and
 * non-controllable regulators, as well as for allowing testing on
 * systems with no controllable regulators.
 */

#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/fixed.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>

struct fixed_voltage_data {
	struct regulator_desc desc;
	struct regulator_dev *dev;
	struct work_struct toggle_work;
	int enable_gpio;
	int toggle_gpio;
	unsigned toggle_gpio_high:1;
	unsigned enable_gpio_high:1;
};

static void _check_toggle_needed(struct fixed_voltage_data *data)
{
	int curval, expval;

	if (!data->toggle_gpio)
		return;

	/* XXX we can do better: */
	expval = !!gpio_get_value(data->toggle_gpio);
	if (data->toggle_gpio_high)
		expval = !expval;

	curval = !!gpio_get_value(data->enable_gpio);
	if (data->enable_gpio_high)
		curval = !curval;

	/* Only modify if not already */
	if (expval == curval) {
		dev_dbg(&data->dev->dev, "toggling value\n");
		curval = !gpio_get_value(data->enable_gpio);
		gpio_set_value(data->enable_gpio, curval);
	} else {
		dev_dbg(&data->dev->dev, "not toggling value\n");
	}
	/*
	 * XXX
	 * - Read IRQ value, i.e. if we are now high or low,
	 * - check against logic
	 * - act if needed
	 */
	/* XXX see if we need to mask/unmask irq during that work */
}

static void toggle_worker(struct work_struct *ws)
{
	struct fixed_voltage_data *data;

	data = container_of(ws, struct fixed_voltage_data, toggle_work);

	_check_toggle_needed(data);
}

static irqreturn_t toggle_irq_handler(int irq, void *data)
{
	struct fixed_voltage_data *handler_data = data;

	/* XXX should we somehow mask out interrupt here and reenable it
	 * at the end of our worker */
	schedule_work(&handler_data->toggle_work);

	return IRQ_NONE;
}

/**
 * of_get_fixed_voltage_config - extract fixed_voltage_config structure info
 * @dev: device requesting for fixed_voltage_config
 * @desc: regulator description
 *
 * Populates fixed_voltage_config structure by extracting data from device
 * tree node, returns a pointer to the populated structure of NULL if memory
 * alloc fails.
 */
static struct fixed_voltage_config *
of_get_fixed_voltage_config(struct device *dev,
			    const struct regulator_desc *desc)
{
	struct fixed_voltage_config *config;
	struct device_node *np = dev->of_node;
	struct regulator_init_data *init_data;

	config = devm_kzalloc(dev, sizeof(struct fixed_voltage_config),
								 GFP_KERNEL);
	if (!config)
		return ERR_PTR(-ENOMEM);

	config->init_data = of_get_regulator_init_data(dev, dev->of_node, desc);
	if (!config->init_data)
		return ERR_PTR(-EINVAL);

	init_data = config->init_data;
	init_data->constraints.apply_uV = 0;

	config->supply_name = init_data->constraints.name;
	if (init_data->constraints.min_uV == init_data->constraints.max_uV) {
		config->microvolts = init_data->constraints.min_uV;
	} else {
		dev_err(dev,
			 "Fixed regulator specified with variable voltages\n");
		return ERR_PTR(-EINVAL);
	}

	if (init_data->constraints.boot_on)
		config->enabled_at_boot = true;

	config->gpio = of_get_named_gpio(np, "gpio", 0);
	/*
	 * of_get_named_gpio() currently returns ENODEV rather than
	 * EPROBE_DEFER. This code attempts to be compatible with both
	 * for now; the ENODEV check can be removed once the API is fixed.
	 * of_get_named_gpio() doesn't differentiate between a missing
	 * property (which would be fine here, since the GPIO is optional)
	 * and some other error. Patches have been posted for both issues.
	 * Once they are check in, we should replace this with:
	 * if (config->gpio < 0 && config->gpio != -ENOENT)
	 */
	if ((config->gpio == -ENODEV) || (config->gpio == -EPROBE_DEFER))
		return ERR_PTR(-EPROBE_DEFER);

	of_property_read_u32(np, "startup-delay-us", &config->startup_delay);

	config->enable_high = of_property_read_bool(np, "enable-active-high");
	config->gpio_is_open_drain = of_property_read_bool(np,
							   "gpio-open-drain");

	if (of_gpio_named_count(dev->of_node, "toggle-gpio") > 0) {
		enum of_gpio_flags flags;
		int ret;

		ret = of_get_named_gpio_flags(dev->of_node, "toggle-gpio",
					      0, &flags);
		if (ret < 0)
			return ERR_PTR(ret);

		config->toggle_gpio = ret;
		config->toggle_gpio_high = !(flags & OF_GPIO_ACTIVE_LOW);
	}

	if (of_find_property(np, "vin-supply", NULL))
		config->input_supply = "vin";

	return config;
}

static struct regulator_ops fixed_voltage_ops = {
};

static int reg_fixed_voltage_probe(struct platform_device *pdev)
{
	struct fixed_voltage_config *config;
	struct fixed_voltage_data *drvdata;
	struct regulator_config cfg = { };
	int ret;

	drvdata = devm_kzalloc(&pdev->dev, sizeof(struct fixed_voltage_data),
			       GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	if (pdev->dev.of_node) {
		config = of_get_fixed_voltage_config(&pdev->dev,
						     &drvdata->desc);
		if (IS_ERR(config))
			return PTR_ERR(config);
	} else {
		config = dev_get_platdata(&pdev->dev);
	}

	if (!config)
		return -ENOMEM;

	drvdata->desc.name = devm_kstrdup(&pdev->dev,
					  config->supply_name,
					  GFP_KERNEL);
	if (drvdata->desc.name == NULL) {
		dev_err(&pdev->dev, "Failed to allocate supply name\n");
		return -ENOMEM;
	}
	drvdata->desc.type = REGULATOR_VOLTAGE;
	drvdata->desc.owner = THIS_MODULE;
	drvdata->desc.ops = &fixed_voltage_ops;

	drvdata->desc.enable_time = config->startup_delay;

	if (config->toggle_gpio) {
		int toggle_irq;

		/* Add a useful name (from GPIO or .dts) instead of NULL */
		ret = devm_gpio_request_one(&pdev->dev, config->toggle_gpio,
					    GPIOF_DIR_IN, NULL);
		if (ret) {
			dev_err(&pdev->dev, "Unable to request gpio %d\n", ret);
			return ret;
		}

		toggle_irq = gpio_to_irq(config->toggle_gpio);
		if (toggle_irq < 0) {
			dev_err(&pdev->dev, "GPIO %d unusable as interrupt\n",
				config->toggle_gpio);
			return toggle_irq;
		}

		INIT_WORK(&drvdata->toggle_work, toggle_worker);
		/*
		 * XXX Why setting IRQ_TYPE_EDGE_BOTH via irq_set_irq_type() instead
		 * of using IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING in
		 * devm_request_irq()
		 * XXX Add a way to pass a name for the GPIO via the .dts */
		irq_set_irq_type(toggle_irq, IRQ_TYPE_EDGE_BOTH);
		ret = devm_request_irq(&pdev->dev, toggle_irq,
				       toggle_irq_handler, IRQF_SHARED,
				       "Toggling GPIO", drvdata);
		if (ret) {
			dev_err(&pdev->dev, "Unable to request IRQ %d\n",
				toggle_irq);
			return ret;
		}

		drvdata->enable_gpio = config->gpio;
		drvdata->toggle_gpio = config->toggle_gpio;
		drvdata->toggle_gpio_high = config->toggle_gpio_high;
		drvdata->enable_gpio_high = config->enable_high;
	}

	if (config->input_supply) {
		drvdata->desc.supply_name = devm_kstrdup(&pdev->dev,
					    config->input_supply,
					    GFP_KERNEL);
		if (!drvdata->desc.supply_name) {
			dev_err(&pdev->dev,
				"Failed to allocate input supply\n");
			return -ENOMEM;
		}
	}

	if (config->microvolts)
		drvdata->desc.n_voltages = 1;

	drvdata->desc.fixed_uV = config->microvolts;

	if (gpio_is_valid(config->gpio)) {
		cfg.ena_gpio = config->gpio;
		if (pdev->dev.of_node)
			cfg.ena_gpio_initialized = true;
	}
	cfg.ena_gpio_invert = !config->enable_high;
	if (config->enabled_at_boot) {
		if (config->enable_high)
			cfg.ena_gpio_flags |= GPIOF_OUT_INIT_HIGH;
		else
			cfg.ena_gpio_flags |= GPIOF_OUT_INIT_LOW;
	} else {
		if (config->enable_high)
			cfg.ena_gpio_flags |= GPIOF_OUT_INIT_LOW;
		else
			cfg.ena_gpio_flags |= GPIOF_OUT_INIT_HIGH;
	}
	if (config->gpio_is_open_drain)
		cfg.ena_gpio_flags |= GPIOF_OPEN_DRAIN;

	cfg.dev = &pdev->dev;
	cfg.init_data = config->init_data;
	cfg.driver_data = drvdata;
	cfg.of_node = pdev->dev.of_node;

	drvdata->dev = devm_regulator_register(&pdev->dev, &drvdata->desc,
					       &cfg);
	if (IS_ERR(drvdata->dev)) {
		ret = PTR_ERR(drvdata->dev);
		dev_err(&pdev->dev, "Failed to register regulator: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, drvdata);

	dev_dbg(&pdev->dev, "%s supplying %duV\n", drvdata->desc.name,
		drvdata->desc.fixed_uV);

	_check_toggle_needed(drvdata);

	return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id fixed_of_match[] = {
	{ .compatible = "regulator-fixed", },
	{},
};
MODULE_DEVICE_TABLE(of, fixed_of_match);
#endif

static struct platform_driver regulator_fixed_voltage_driver = {
	.probe		= reg_fixed_voltage_probe,
	.driver		= {
		.name		= "reg-fixed-voltage",
		.of_match_table = of_match_ptr(fixed_of_match),
	},
};

static int __init regulator_fixed_voltage_init(void)
{
	return platform_driver_register(&regulator_fixed_voltage_driver);
}
subsys_initcall(regulator_fixed_voltage_init);

static void __exit regulator_fixed_voltage_exit(void)
{
	platform_driver_unregister(&regulator_fixed_voltage_driver);
}
module_exit(regulator_fixed_voltage_exit);

MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_DESCRIPTION("Fixed voltage regulator");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:reg-fixed-voltage");
