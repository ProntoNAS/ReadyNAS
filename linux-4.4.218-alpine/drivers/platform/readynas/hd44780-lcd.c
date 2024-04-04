/*
 * hd44780-lcd.c
 *
 * LCD alpha numeric display driver for Hitach HD44780 driven by GPIO
 *
 * Copyright (c)2015 NETGEAR, INC.
 * Author: Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation
 */

#include "hd44780-lcd.h"
#include <../drivers/gpio/gpiolib.h>

static bool lcd_backlight_state(struct hd44780_lcd *gl)
{
	return !!gl->get_value(gl->lcm_bl);
}

static void pin_write(struct hd44780_lcd *gl, struct gpio_desc *pin, int value)
{
	gl->set_value(pin, value);
	udelay(1);
}

static unsigned char lcm_read4(struct hd44780_lcd *gl)
{
	unsigned char c = 0;

	c  =  gl->get_value(gl->lcm_data[7]) ? (1 << 7) : 0;
	c |= (gl->get_value(gl->lcm_data[6]) ? (1 << 6) : 0);
	c |= (gl->get_value(gl->lcm_data[5]) ? (1 << 5) : 0);
	c |= (gl->get_value(gl->lcm_data[4]) ? (1 << 4) : 0);

	return c;
}

static unsigned char lcm_read8(struct hd44780_lcd *gl)
{
	unsigned char c = lcm_read4(gl);

	c |=  gl->get_value(gl->lcm_data[3]) ? (1 << 3) : 0;
	c |= (gl->get_value(gl->lcm_data[2]) ? (1 << 2) : 0);
	c |= (gl->get_value(gl->lcm_data[1]) ? (1 << 1) : 0);
	c |= (gl->get_value(gl->lcm_data[0]) ? (1 << 0) : 0);

	return c;
}

static unsigned char lcm_read(struct hd44780_lcd *gl)
{
	unsigned char c;

	pin_write(gl, gl->lcm_rw, 1);
	pin_write(gl, gl->lcm_en, 1);
	udelay(50);
	c = (gl->lcm_data_4bit) ? lcm_read4(gl) : lcm_read8(gl);
	pin_write(gl, gl->lcm_en, 0);

	if (gl->lcm_data_4bit) {
		c &= 0xf0;
		pin_write(gl, gl->lcm_en, 1);
		udelay(50);
		c |= lcm_read4(gl) >> 4;
		pin_write(gl, gl->lcm_en, 0);
	}
	udelay(50);
	return c;
}

static void wait_busy(struct hd44780_lcd *gl)
{
	int try = 10;

	pin_write(gl, gl->lcm_rs, 0);
	while (try-- > 0) {
		if (!(lcm_read(gl) & 0x80))
			break;
		mdelay(1);
	}
}

static void lcm_write4(struct hd44780_lcd *gl, unsigned char c)
{
	gl->set_value(gl->lcm_data[7], !!(c & (1 << 7)));
	gl->set_value(gl->lcm_data[6], !!(c & (1 << 6)));
	gl->set_value(gl->lcm_data[5], !!(c & (1 << 5)));
	gl->set_value(gl->lcm_data[4], !!(c & (1 << 4)));
}

static void lcm_write8(struct hd44780_lcd *gl, unsigned char c)
{
	lcm_write4(gl, c);
	gl->set_value(gl->lcm_data[3], !!(c & (1 << 3)));
	gl->set_value(gl->lcm_data[2], !!(c & (1 << 2)));
	gl->set_value(gl->lcm_data[1], !!(c & (1 << 1)));
	gl->set_value(gl->lcm_data[0], !!(c & (1 << 0)));
}

static void lcm_write(struct hd44780_lcd *gl, unsigned char c)
{
	pin_write(gl, gl->lcm_rw, 0);
	pin_write(gl, gl->lcm_en, 1);
	if (gl->lcm_data_4bit)
		lcm_write4(gl, c);
	else
		lcm_write8(gl, c);
	udelay(50);
	pin_write(gl, gl->lcm_en, 0);
	udelay(50);
	if (gl->lcm_data_4bit) {
		pin_write(gl, gl->lcm_en, 1);
		lcm_write4(gl, c << 4);
		udelay(50);
		pin_write(gl, gl->lcm_en, 0);
		udelay(50);
	}
}

static void lcd_data_write(struct hd44780_lcd *gl, unsigned char c)
{
	wait_busy(gl);
	pin_write(gl, gl->lcm_rs, 1);
	lcm_write(gl, c);
}

static void command_write(struct hd44780_lcd *gl, unsigned char c)
{
	wait_busy(gl);
	pin_write(gl, gl->lcm_rs, 0);
	lcm_write(gl, c);
}

static void lcd_sync_data_len(struct hd44780_lcd *gl)
{
	/* Regardless of current data length...*/
	pin_write(gl, gl->lcm_rs, 0);
	pin_write(gl, gl->lcm_rw, 0);
	pin_write(gl, gl->lcm_en, 1);
	/* Force 8bit mode. */
	lcm_write4(gl, 0x30);
	udelay(100);
	pin_write(gl, gl->lcm_en, 0);
	mdelay(5);
	pin_write(gl, gl->lcm_en, 1);
	/* Force 8bit mode again. */
	lcm_write4(gl, 0x30);
	udelay(100);
	pin_write(gl, gl->lcm_en, 0);
	mdelay(5);
	pin_write(gl, gl->lcm_en, 1);
	/* Force 8bit mode again more. */
	lcm_write4(gl, 0x30);
	udelay(100);
	pin_write(gl, gl->lcm_en, 0);
	mdelay(5);
	if (!gl->lcm_data_4bit)
		return;
	pin_write(gl, gl->lcm_en, 1);
	lcm_write4(gl, 0x20);
	udelay(100);
	pin_write(gl, gl->lcm_en, 0);
	mdelay(5);
}

static void lcd_entry_mode_set(struct hd44780_lcd *gl, int l2r, int scroll)
{
	command_write(gl, 0x04 | (l2r << 1) | scroll);
}

static void lcd_cursor_set(struct hd44780_lcd *gl, int row, int col)
{
	command_write(gl, 0x80 | (row << 6) | col);
	udelay(53);
}

static void lcd_clear_display(struct hd44780_lcd *gl)
{
	command_write(gl, gl->lcm_clear_display);
	mdelay(2);
}

static void lcd_return_home(struct hd44780_lcd *gl)
{
	command_write(gl, gl->lcm_return_home);
	mdelay(2);
}

static void lcd_entry_mode_id(struct hd44780_lcd *gl, bool incr)
{
	gl->lcm_entry_mode &= ~0x02;
	if (incr)
		gl->lcm_entry_mode |= 0x02;
	command_write(gl, gl->lcm_entry_mode);
}

static void lcd_entry_mode_shift(struct hd44780_lcd *gl, bool shift)
{
	gl->lcm_entry_mode &= ~0x01;
	if (shift)
		gl->lcm_entry_mode |= 0x01;
	command_write(gl, gl->lcm_entry_mode);
}

static void lcd_entry_display_on(struct hd44780_lcd *gl, bool on)
{
	gl->lcm_display_control &= ~0x04;
	if (on)
		gl->lcm_display_control |= 0x04;
	command_write(gl, gl->lcm_display_control);
}

static void lcd_display_cursor_on(struct hd44780_lcd *gl, bool on)
{
	gl->lcm_display_control &= ~0x02;
	if (on)
		gl->lcm_display_control |= 0x02;
	command_write(gl, gl->lcm_display_control);
}

static void lcd_display_cursor_blink(struct hd44780_lcd *gl, bool blink)
{
	gl->lcm_display_control &= ~0x01;
	if (blink)
		gl->lcm_display_control |= 0x01;
	command_write(gl, gl->lcm_display_control);
}

static void lcd_shift_display(struct hd44780_lcd *gl, bool right)
{
	gl->lcm_display_shift &= ~0x04;
	gl->lcm_display_shift |= ~0x08;
	if (right)
		gl->lcm_display_shift |= 0x04;
	command_write(gl, gl->lcm_display_shift);
}

static void lcd_shift_cursor(struct hd44780_lcd *gl, bool right)
{
	gl->lcm_display_shift &= ~0x0c;
	if (right)
		gl->lcm_display_shift |= 0x04;
	command_write(gl, gl->lcm_display_shift);
}

static void lcd_function_8bit(struct hd44780_lcd *gl, bool _8bit)
{
	gl->lcm_function &= ~0x10;
	if (_8bit)
		gl->lcm_function |= 0x10;
	command_write(gl, gl->lcm_function);
}

static void lcd_function_2line(struct hd44780_lcd *gl, bool _2line)
{
	gl->lcm_function &= ~0x08;
	if (_2line)
		gl->lcm_function |= 0x08;
	command_write(gl, gl->lcm_function);
}

static void lcd_function_5x11(struct hd44780_lcd *gl, bool _5x11)
{
	gl->lcm_function &= ~0x04;
	if (_5x11)
		gl->lcm_function |= 0x04;
	command_write(gl, gl->lcm_function);
}

static void lcd_set_cgram_address(struct hd44780_lcd *gl, unsigned char offset)
{
	gl->lcm_cgram_address &= ~0x1f;
	if (offset)
		gl->lcm_cgram_address |= (offset & 0x1f);
	command_write(gl, gl->lcm_cgram_address);
}

static void lcd_set_cursor_pos(struct hd44780_lcd *gl,
				unsigned char row, unsigned char col)
{
	if (gl->lcm_num_rows == 2) {
		if (col > 0x27)
			col = 0x27;
	} else {
		if (col > 0x4f)
			col = 0x4f;
		row = 0;
	}
	command_write(gl, 0x80 | (row << 6) | col);
}

static void lcd_backlight_on(struct hd44780_lcd *gl, bool on)
{
	pin_write(gl, gl->lcm_bl, on);
	gl->backlight_status = on;
}

static void lcd_add_custom_character(struct hd44780_lcd *gl)
{
	command_write(gl, gl->lcm_cgram_address | (1 << 3)); /* slot 1 */
	lcd_data_write(gl, 0x00);
	lcd_data_write(gl, 0x00);
	lcd_data_write(gl, 0x00);
	lcd_data_write(gl, 0x00);
	lcd_data_write(gl, 0x1f);
	lcd_data_write(gl, 0x1f);
	lcd_data_write(gl, 0x1f);
	lcd_data_write(gl, 0x1f);
}

static int lcd_init(struct hd44780_lcd *gl)
{
	lcd_sync_data_len(gl);

	lcd_function_8bit(gl, !gl->lcm_data_4bit);
	lcd_function_2line(gl, (gl->lcm_num_rows == 2));

	lcd_clear_display(gl);
	lcd_entry_mode_set(gl, 1, 0);
	lcd_cursor_set(gl, 0, 0);	/* 1st line 1st col */
	lcd_entry_display_on(gl, 1);	/* turn LCD on */
	lcd_backlight_on(gl, 1);	/* turn backlight on */
	lcd_add_custom_character(gl);	/* add half block custom character */

	return 0;
}

static void lcd_exit(struct hd44780_lcd *gl)
{
}

#ifndef CONFIG_OF_GPIO
static int gpio_match_labels(struct gpio_chip *gc, void *data)
{
	return !strcmp(gc->label, (const char *)data);
}

static struct gpio_chip *gpiochip_find_by_label(const char *label)
{
	return gpiochip_find((void *)label, gpio_match_labels);
}

#define SETUP_GPIO(label, member)	\
	{ label, offsetof(struct hd44780_lcd, member), true }
#define SETUP_NON(label, member)	\
	{ label, offsetof(struct hd44780_lcd, member), false }
#define SETUP_GPIO2(name)	\
	SETUP_GPIO("lcm-"#name, lcm_##name)
static const struct {
	const char *label;
	const size_t offset;
	const bool is_gpiod;
} setup_item[] = {
	SETUP_GPIO2(rs),
	SETUP_GPIO2(rw),
	SETUP_GPIO2(en),
	SETUP_GPIO2(bl),
	SETUP_NON("lcm-lines", lcm_num_rows),
	SETUP_NON("lcm-cols", lcm_num_cols),
	SETUP_NON("lcm-data-4bit", lcm_data_4bit),
	SETUP_GPIO("lcm-bit0", lcm_data[0]),
	SETUP_GPIO("lcm-bit1", lcm_data[1]),
	SETUP_GPIO("lcm-bit2", lcm_data[2]),
	SETUP_GPIO("lcm-bit3", lcm_data[3]),
	SETUP_GPIO("lcm-bit4", lcm_data[4]),
	SETUP_GPIO("lcm-bit5", lcm_data[5]),
	SETUP_GPIO("lcm-bit6", lcm_data[6]),
	SETUP_GPIO("lcm-bit7", lcm_data[7]),
	{}
};

static int setup_gpiomap(struct hd44780_lcd *gl,
				struct gpio_lcd_map const *setup)
{
	struct gpio_chip *gc;
	int i = 0, err = 0;

	gl->lcm_num_rows = 2;
	gl->lcm_num_cols = 16;

	if (!setup)
		return -EINVAL;

	gc = gpiochip_find_by_label(setup->gpio_label);
	if (IS_ERR(gc))
		err = PTR_ERR(gc);
	else if (!gc)
		err = -ENODEV;
	if (err) {
		pr_err("%s: cannot find LCD GPIO %s.\n",
			__func__, setup->gpio_label);
		return err;
	}

	while (setup->map[i].label) {
		int j = 0;

		while (setup_item[j].label) {
			void *vp;

			if (strcmp(setup_item[j].label, setup->map[i].label)) {
				j++;
				continue;
			}

			vp = (void *)gl + setup_item[j].offset;
			if (setup_item[j].is_gpiod) {
				struct gpio_desc *gd =
					gpio_to_desc(setup->map[i].val +
							gc->base);
				if (setup->map[i].active_low)
					set_bit(FLAG_ACTIVE_LOW, &gd->flags);
				gpiod_direction_output(gd, 0);
				*(struct gpio_desc **)vp = gd;
			} else
				*(unsigned int *)vp = setup->map[i].val;
			break;
		}
		if (!setup_item[j].label)
			return -EINVAL;
		i++;
	}

	if (!gl->lcm_rs || !gl->lcm_rw || !gl->lcm_en || !gl->lcm_bl ||
		!gl->lcm_data[7] || !gl->lcm_data[6] ||
		!gl->lcm_data[5] || !gl->lcm_data[4])
		return -EINVAL;
	if (!gl->lcm_data_4bit &&
		(!gl->lcm_data[3] || !gl->lcm_data[2] ||
		!gl->lcm_data[1] || !gl->lcm_data[0]))
		return -EINVAL;

	if (gl->lcm_num_rows == 2)
		gl->function_2line(gl, 1);

	return 0;
}

static int gpio_lcd_match(struct device *dev, void *data)
{
	struct gpio_lcd_map *map = dev_get_drvdata(dev);

	return map ?
		!strncmp(map->magic, (const char *)data, sizeof map->magic) : 0;
}

static struct gpio_lcd_map *find_gpio_lcd_map(void)
{
	struct device *dev = bus_find_device(&platform_bus_type, NULL,
						GPIO_LCD_MAGIC, gpio_lcd_match);
	return dev ? dev_get_drvdata(dev) : NULL;
}
#endif

static int hd44780_lcd_probe(struct platform_device *pdev)
{
	struct hd44780_lcd *gl;
	int err = 0;
#ifdef CONFIG_OF_GPIO
	int i, j;
	struct device_node *np = pdev->dev.of_node;

	if (!np)
		return -ENODEV;
#else
	struct gpio_lcd_map *gpio_map = find_gpio_lcd_map();

	if (!gpio_map)
		return -ENODEV;
#endif
	if (!(gl = devm_kzalloc(&pdev->dev, sizeof *gl, GFP_KERNEL)))
		return -ENOMEM;

	memcpy(gl->magic, LCD_PLATFORM_MAGIC, sizeof gl->magic);
	platform_set_drvdata(pdev, gl);

	gl->lcm_clear_display	= 0x01;
	gl->lcm_return_home	= 0x02;
	gl->lcm_entry_mode	= 0x04;
	gl->lcm_display_control	= 0x08;
	gl->lcm_display_shift	= 0x10;
	gl->lcm_function	= 0x20;
	gl->lcm_cgram_address	= 0x40;
	gl->lcm_ddram_address	= 0x80;

	gl->init	= lcd_init;
	gl->exit	= lcd_exit;

	gl->get_value = gpiod_get_value;
	gl->set_value = gpiod_set_value;

	gl->clear_lcd 		= lcd_clear_display;
	gl->return_home		= lcd_return_home;
	gl->entry_mode_id	= lcd_entry_mode_id;
	gl->entry_mode_shift	= lcd_entry_mode_shift;
	gl->entry_display_on	= lcd_entry_display_on;
	gl->display_cursor_on	= lcd_display_cursor_on;
	gl->display_cursor_blink= lcd_display_cursor_blink;
	gl->shift_display	= lcd_shift_display;
	gl->shift_cursor	= lcd_shift_cursor;
	gl->function_8bit	= lcd_function_8bit;
	gl->function_2line	= lcd_function_2line;
	gl->function_5x11	= lcd_function_5x11;
	gl->set_cgram_address	= lcd_set_cgram_address;
	gl->set_cursor_pos	= lcd_set_cursor_pos;

	gl->backlight_on	= lcd_backlight_on;
	gl->backlight_state	= lcd_backlight_state;
	gl->data_write		= lcd_data_write;
	gl->command_write	= command_write;

	mutex_init(&gl->lcm_lock);
#ifdef CONFIG_OF_GPIO
	/* gpiolib is weird. flags' FLAG_ACTIVE_LOW needs to be set outisde. */
#define GET_GPIO_PIN(np, gl, pin, name, idx)	\
	do {					\
		struct gpio_desc *_desc;	\
		enum of_gpio_flags _flags;	\
		if (IS_ERR_OR_NULL(_desc =	\
			of_get_named_gpiod_flags((np),(name),(idx),&_flags))) {\
			err = PTR_ERR(_desc);	\
			goto fail;	\
		}	\
		if (_flags & OF_GPIO_ACTIVE_LOW)	\
			set_bit(FLAG_ACTIVE_LOW, &_desc->flags);	\
		gpiod_direction_output(_desc, 0);	\
		(gl)->pin = _desc;	\
	} while (0)

	for (i = 0, j = 4; i < 4; i++, j++)
		GET_GPIO_PIN(np, gl, lcm_data[j], "lcm-bits", i);

	if (!(gl->lcm_data_4bit = of_get_property(np, "lcm-data-4bit", NULL))) {
		for ( ; i < 8; i++, j++)
			GET_GPIO_PIN(np, gl, lcm_data[j], "lcm-bits", i);
	}

	GET_GPIO_PIN(np, gl, lcm_rs, "lcm-rs", 0);
	GET_GPIO_PIN(np, gl, lcm_rw, "lcm-rw", 0);
	GET_GPIO_PIN(np, gl, lcm_en, "lcm-en", 0);
	GET_GPIO_PIN(np, gl, lcm_bl, "lcm-bl", 0);
	if (of_property_read_u32(np, "lcm-lines", (u32 *)&gl->lcm_num_rows))
		gl->lcm_num_rows = 2;
	if (of_property_read_u32(np, "lcm-cols", (u32 *)&gl->lcm_num_cols))
		gl->lcm_num_cols = 16;
#else
	if ((err = setup_gpiomap(gl, gpio_map)))
		goto fail;
#endif
	if ((err = gl->init(gl)))
		goto fail;
	return 0;

fail:
	devm_kfree(&pdev->dev, gl);
	return err;
}

static int hd44780_lcd_remove(struct platform_device *pdev)
{
	struct hd44780_lcd *gl = platform_get_drvdata(pdev);

	if (gl && gl->exit)
		gl->exit(gl);
	devm_kfree(&pdev->dev, gl);
	return 0;
}

static struct of_device_id hd44780_lcd_dt_ids[] = {
	{
		.compatible = "hd44780-lcd",
	}, {
		.compatible = "gpio-lcd",
	}, {},
};

MODULE_DEVICE_TABLE(of, hd44780_lcd_ds_ids);

static struct platform_driver hd44780_lcd_driver = {
	.probe	= hd44780_lcd_probe,
	.remove = hd44780_lcd_remove,
	.driver = {
		.name = "hd44780-lcd",
		.of_match_table = hd44780_lcd_dt_ids,
	},
};

#ifdef CONFIG_OF_GPIO
module_platform_driver(hd44780_lcd_driver);
#else
static struct platform_device *hd44780_lcd_pdev;

static int __init hd44780_lcd_init(void)
{
	int err = platform_driver_register(&hd44780_lcd_driver);

	if (err)
		return err;

	hd44780_lcd_pdev =
		platform_device_register_simple(hd44780_lcd_driver.driver.name,
						-1, NULL, 0);
	if (IS_ERR(hd44780_lcd_pdev))
		err = PTR_ERR(hd44780_lcd_pdev);
	else if (!hd44780_lcd_pdev)
		err = -ENODEV;

	if (err)
		platform_driver_unregister(&hd44780_lcd_driver);
	return err;
}

static void __exit hd44780_lcd_exit(void)
{
	platform_device_unregister(hd44780_lcd_pdev);
	platform_driver_unregister(&hd44780_lcd_driver);
}

device_initcall_sync(hd44780_lcd_init);
module_exit(hd44780_lcd_exit);
#endif

MODULE_AUTHOR("hiro.sugawara@netgear.com");
MODULE_DESCRIPTION("generic HD44780 LCD driver");
