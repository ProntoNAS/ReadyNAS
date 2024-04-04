/*
 * hd44780-lcd.h
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

#include <linux/module.h>
#include <linux/string.h>
#ifdef CONFIG_OF_GPIO
#include <linux/of_fdt.h>
#endif
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

#include <linux/slab.h>
#include <linux/err.h>
#include <linux/platform_device.h>

#ifdef CONFIG_OF_GPIO
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#else

#define	GPIO_LCD_MAGIC		"gpio-lcd"
struct gpio_lcd_map {
	char magic[16];
	const char *gpio_label;
	struct {
		const char *label;
		const unsigned int val;
		const bool active_low;
	} map[];
};
#endif

#define	LCD_PLATFORM_MAGIC	"lcd-device"
struct hd44780_lcd {
	char magic[12];
	bool lcm_data_4bit;
	int  lcm_num_rows;
	int  lcm_num_cols;
	struct gpio_desc *lcm_data[8];
	struct gpio_desc *lcm_rw;
	struct gpio_desc *lcm_en;
	struct gpio_desc *lcm_rs;
	struct gpio_desc *lcm_bl;
	struct mutex lcm_lock;

	int  (*init)(struct hd44780_lcd *);
	void (*exit)(struct hd44780_lcd *);

	int  (*get_value)(const struct gpio_desc *);
	void (*set_value)(struct gpio_desc *, int);

	void (*clear_lcd)(struct hd44780_lcd *);
	void (*return_home)(struct hd44780_lcd *);
	void (*entry_mode_id)(struct hd44780_lcd *, bool);
	void (*entry_mode_shift)(struct hd44780_lcd *, bool);
	void (*entry_display_on)(struct hd44780_lcd *, bool);
	void (*display_cursor_on)(struct hd44780_lcd *, bool);
	void (*display_cursor_blink)(struct hd44780_lcd *, bool);
	void (*shift_display)(struct hd44780_lcd *, bool);
	void (*shift_cursor)(struct hd44780_lcd *, bool);
	void (*function_8bit)(struct hd44780_lcd *, bool);
	void (*function_2line)(struct hd44780_lcd *, bool);
	void (*function_5x11)(struct hd44780_lcd *, bool);
	void (*set_cgram_address)(struct hd44780_lcd *, unsigned char);
	void (*set_cursor_pos)(struct hd44780_lcd *, unsigned char, unsigned char);

	void (*backlight_on)(struct hd44780_lcd *, bool);
	bool (*backlight_state)(struct hd44780_lcd *);
	void (*data_write)(struct hd44780_lcd *, unsigned char);
	void (*command_write)(struct hd44780_lcd *, unsigned char);

	struct platform_device *pdev;

	unsigned char lcm_clear_display;	/* 0x01 */
	unsigned char lcm_return_home;		/* 0x02 */
	unsigned char lcm_entry_mode;		/* 0x04 */
	unsigned char lcm_display_control;	/* 0x08 */
	unsigned char lcm_display_shift;	/* 0x10 */
	unsigned char lcm_function;		/* 0x20 */
	unsigned char lcm_cgram_address;	/* 0x40 */
	unsigned char lcm_ddram_address;	/* 0x80 */
};
