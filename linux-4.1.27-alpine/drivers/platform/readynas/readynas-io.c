/*
 * readynas-io.c - NETGEAR ReadyNAS Intel platform I/O assortment
 *
 * Copyright (c) 2015,2016 NETGEAR, Inc.
 *
 * Author: Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/acpi.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/input.h>
#include <linux/kthread.h>

#include <../drivers/gpio/gpiolib.h>
#include "hd44780-lcd.h"

static struct proc_dir_entry *readynas_proc_root;

/*
 * Generic GPIO and model name search functions.
 */
static int gpio_match_labels(struct gpio_chip *gc, void *data)
{
	return !strcmp(gc->label, (const char *)data);
}

static struct gpio_chip *gpiochip_find_by_label(const char *label)
{
	return gpiochip_find((void *)label, gpio_match_labels);
}

static bool readynas_model_compatible(char const *dmi, char const **model)
{
	if (dmi) {
		while (*model)
			if (strstr(dmi, *model++))
				return true;
	}
	return false;
}

bool readynas_io_compatible(char const **model)
{
	const char *dmi = dmi_get_system_info(DMI_PRODUCT_NAME);

	if (readynas_model_compatible(dmi, model))
		return true;

	dmi = dmi_get_system_info(DMI_PRODUCT_VERSION);
	if (readynas_model_compatible(dmi, model))
		return true;
	return false;
}

/*
 * List of our NAS models supported here.
 */
static const char *compatible_ultra2[] = {
	"ReadyNAS-ProUltra2", NULL};

static const char *compatible_ultra4[] = {
	"ReadyNAS-NVX-V2", "Ultra4", NULL};

static const char *compatible_ultra6[] = {
	"FLAME6-2", "Ultra6", NULL};

static const char *compatible_pro6[] = {
	"FLAME6-MB", "Pro6", NULL};

static const char *compatible_rn312[] = {
	"ReadyNAS 312", "RN312", NULL };

static const char *compatible_rn314[] = {
	"ReadyNAS 314", "RN314", NULL };

static const char *compatible_rn316[] = {
	"ReadyNAS 316", "RN316", NULL };

static const char *compatible_rn516[] = {
	"ReadyNAS 516", "RN516", "ReadyNAS 716", "RN716", NULL };

static const char *compatible_rn526[] = {
	"ReadyNAS 524", "RN524", "ReadyNAS 624", "RN624",
	"ReadyNAS 526", "RN526", "ReadyNAS 626", "RN626",
	"ReadyNAS 528", "RN528", "ReadyNAS 628", "RN628", NULL };

static const char *compatible_rn3130[] = {
	"ReadyNAS 3130", "ReadyNAS3130", "RN3130", NULL};

static const char *compatible_rnx220[] = {
	"ReadyNAS 3220", "ReadyNAS3220", "RN3220",
	"ReadyNAS 4220", "ReadyNAS4220", "RN4220", NULL};

static const char *compatible_rrx312[] = {
	"ReadyNAS 3312", "ReadyNAS3312", "RR3312",
	"ReadyNAS 4312", "ReadyNAS4312", "RR4312", NULL};

/*
 * Hitach HD44780 for RN314/Ultra4 LCD alphanumeric display
 */
static const struct gpio_lcd_map
rn314_lcd_map = {
	.magic		= GPIO_LCD_MAGIC,
	.gpio_label	= "gpio_ich",
	.map = {
		{"lcm-rs",	33},
		{"lcm-rw",	19},
		{"lcm-en",	32},
		{"lcm-bl",	48, true},
		{"lcm-bit4",	36},
		{"lcm-bit5",	37},
		{"lcm-bit6",	38},
		{"lcm-bit7",	39},
		{"lcm-lines",	2},
		{"lcm-cols",	16},
		{"lcm-data-4bit",1},
		{}
	}
},
ultra4_lcd_map = {
	.magic		= GPIO_LCD_MAGIC,
	.gpio_label	= "gpio_ich",
	.map = {
		{"lcm-rs",	33},
		{"lcm-rw",	34},
		{"lcm-en",	32},
		{"lcm-bl",	48, true},
		{"lcm-bit4",	36},
		{"lcm-bit5",	37},
		{"lcm-bit6",	38},
		{"lcm-bit7",	39},
		{"lcm-lines",	2},
		{"lcm-cols",	16},
		{"lcm-data-4bit",1},
		{}
	}
},
*rnx4_lcd_map;

static int rnx4_lcd_probe(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, (void *)rnx4_lcd_map);
	return 0;
}

static int rnx4_lcd_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver
rn314_lcd_driver = {
	.probe	= rnx4_lcd_probe,
	.remove	= rnx4_lcd_remove,
	.driver = {
		.name	= "rn314-lcd",
	},
},
ultra4_lcd_driver = {
	.probe	= rnx4_lcd_probe,
	.remove	= rnx4_lcd_remove,
	.driver = {
		.name	= "ultra4-lcd",
	},
},
*rnx4_lcd_driver;

static struct platform_device *rnx4_lcd_pdev;
static int __init rnx4_lcd_init(void)
{
	int err;

	if (readynas_io_compatible(compatible_rn314)) {
		rnx4_lcd_driver = &rn314_lcd_driver;
		rnx4_lcd_map = &rn314_lcd_map;
	} else if (readynas_io_compatible(compatible_ultra4)) {
		rnx4_lcd_driver = &ultra4_lcd_driver;
		rnx4_lcd_map = &ultra4_lcd_map;
	} else
		return 0;

	if ((err = platform_driver_register(rnx4_lcd_driver))) {
		pr_err(
		"%s: registering RN314/Ultra4 LCD driver failed.\n", __func__);
		return err;
	}

	rnx4_lcd_pdev =
		platform_device_register_simple(rnx4_lcd_driver->driver.name,
						-1, NULL, 0);
	if (IS_ERR_OR_NULL(rnx4_lcd_pdev))
		err = PTR_ERR(rnx4_lcd_pdev);
	else if (!rnx4_lcd_pdev)
		err = -ENODEV;

	if (err) {
		pr_err(
		"%s: registering RN314/Ultra4 LCD device failed.\n", __func__);
		platform_driver_unregister(rnx4_lcd_driver);
	}
	return err;
}

static void rnx4_lcd_exit(void)
{
	if (rnx4_lcd_pdev) {
		platform_device_unregister(rnx4_lcd_pdev);
		platform_driver_unregister(rnx4_lcd_driver);
		rnx4_lcd_pdev = NULL;
	}
}

/*
 * Hitach HD44780 similation for RN316/516/716/526/726's OLED bitmap display
 */
static struct rnx16_oled_gpio {
	const char **compatible;
	const char *gpio_label;
	unsigned int oled_sdin, oled_sclk, oled_dc,
			oled_cs, oled_ctrl, oled_reset;
	struct gpio_desc *oled_sdind, *oled_sclkd, *oled_dcd,
			*oled_csd, *oled_ctrld, *oled_resetd;
	struct hd44780_lcd lcd;
} rnx16_oled_gpios[] = {{
	.compatible	= compatible_rn316,
	.gpio_label	= "gpio_ich",
	.oled_sdin	= 21,
	.oled_sclk	= 19,
	.oled_dc	= 16,
	.oled_cs	= 7,
	.oled_ctrl	= 32,
	.oled_reset	= 24,
	.lcd = {
		.magic	= LCD_PLATFORM_MAGIC,
	},
},{
	.compatible	= compatible_rn516,
	.gpio_label	= "gpio_ich",
	.oled_sdin	= 54,
	.oled_sclk	= 52,
	.oled_dc	= 32,
	.oled_cs	= 50,
	.oled_ctrl	= 6,
	.oled_reset	= 7,
	.lcd = {
		.magic	= LCD_PLATFORM_MAGIC,
	},
},{
	.compatible	= compatible_rn526,
	.gpio_label	= "gpio_ich",
	.oled_sdin	= 54,
	.oled_sclk	= 1,
	.oled_dc	= 32,
	.oled_cs	= 50,
	.oled_ctrl	= 6,
	.oled_reset	= 7,
	.lcd = {
		.magic	= LCD_PLATFORM_MAGIC,
	},
}, {}}, *rnx16_oled_gpio;

#define OLED_COLS	16
#define OLED_ROWS	2
#define OLED_GPIOD_SET(name, val)	gpiod_set_value(gp->oled_##name##d, val)

static void spi_send(struct rnx16_oled_gpio *gp, unsigned char c, bool cmd)
{
	unsigned char mask = 0x80;

	OLED_GPIOD_SET(cs, 0);
	OLED_GPIOD_SET(dc, !cmd);

	/* transfer data */
	while (mask) {
		OLED_GPIOD_SET(sclk, 0);
		OLED_GPIOD_SET(sdin, !!(mask & c));
		OLED_GPIOD_SET(sclk, 1);
		mask >>= 1;
	}
	OLED_GPIOD_SET(dc, 1);
	OLED_GPIOD_SET(cs, 1);
}

static void inline spi_send_data(struct rnx16_oled_gpio *gp, unsigned char d)
{
	spi_send(gp, d, false);
}

static void inline spi_send_cmd(struct rnx16_oled_gpio *gp, unsigned char c)
{
	spi_send(gp, c, true);
}

static unsigned char oled_row, oled_col;
static void oled_set_cursor_pos(struct hd44780_lcd *gl,
				unsigned char row, unsigned char col)
{
	oled_row = min(row, (unsigned char)1);
	oled_col = min(col, (unsigned char)(OLED_COLS - 1));
}

static void oled_return_home(struct hd44780_lcd *gl)
{
	oled_set_cursor_pos(gl, 0, 0);
}

static void oled_shift_cursor(struct hd44780_lcd *gl, bool right)
{
	if (right) {
		if (oled_col < OLED_COLS && oled_row < OLED_ROWS) {
			oled_col++;
			oled_row += oled_col / OLED_COLS;
			oled_col %= OLED_COLS;
		}
	} else {
		if (oled_col > 0)
			oled_col--;
		else if (oled_row > 0) {
			oled_col = OLED_COLS - 1;
			oled_row = 0;
		}
	}
}

static void oled_backlight_on(struct hd44780_lcd *gl, bool on)
{
	struct rnx16_oled_gpio *gp =
		container_of(gl, struct rnx16_oled_gpio, lcd);

	spi_send_cmd(gp, on ? 0xaf : 0xae);
}

#define	OLED_X_OFFSET	4

static void clear_oled(struct hd44780_lcd *gl)
{
	struct rnx16_oled_gpio *gp =
		container_of(gl, struct rnx16_oled_gpio, lcd);
	int page, col;

	spi_send_cmd(gp, 0x40);	/* Start position */

	for (page = 0; page < 4; page++) {
		OLED_GPIOD_SET(cs, 0);
		OLED_GPIOD_SET(dc, 0);
		spi_send_cmd(gp, 0xb0 + page);
		spi_send_cmd(gp, 0x10);
		spi_send_cmd(gp, OLED_X_OFFSET);
		OLED_GPIOD_SET(dc, 1);

		for(col = 0; col < 128; col++)
			spi_send_data(gp, 0x0);
	}
	oled_return_home(gl);
}

static int init_oled(struct hd44780_lcd *gl)
{
	int i;
	struct rnx16_oled_gpio *gp =
		container_of(gl, struct rnx16_oled_gpio, lcd);
	static const unsigned char oled_init_cmd[] = {
		0xae,	/* Turn off display. */

		0xd5,	/* Display ClocDivide Ratio/Oscillator Frequency */
		0x71,	/* to 105Hz. */

		0xa8,	/* Multiplex Ratio */
		0x1f,	/* to 32mux. */

		0xd9,	/* Precharge period. */
		0x22,

		0x20,	/* Memory addressing mode. */
		0x02,

		0xa1,	/* Seg re-map 127->0. */
		0xc8,	/* COM scan direction COM(N-1)-->COM0. */

		0xda,	/* COM pins hardware configuration. */
		0x12,

		0xd8,	/* Color_mode_set */
		0x00,	/* to monochrome mode & normal power mode. */

		0x81,	/* Contrast control. */
		0xcf,

		0xb0,	/* Page start address for page Addressing mode. */

		0xd3,	/* Display offset. */
		0x00,

		0x21,	/* Column address. */
		OLED_X_OFFSET,		/* Colum address start. */
		0x7f+OLED_X_OFFSET,	/* Colum address end. */

		0x22,	/* Page address. */
		0x00,	/* Page address start. */
		0x03,	/* Page address end. */

		0x10,	/* Higher column start addr for page addressing mode. */
		0x00,	/* Lower column start addr for page addressing mode. */
		0x40,	/* Display start line */
		0xa6,	/* Normal (non-inverted). */

		0xa4,	/* Entire display Off. */

		0xdb,	/* VCOMH Level */
		0x18,	/* to 0.83*VCC 0x3c, change from 0x20 to 0x18 avoid power peek issue. */
	};

	OLED_GPIOD_SET(reset, 0);
	udelay(5);	/* L pulse > 3us */
	OLED_GPIOD_SET(reset, 1);
	udelay(5);

	for (i = 0; i < ARRAY_SIZE(oled_init_cmd); i++)
		spi_send_cmd(gp, oled_init_cmd[i]);

	clear_oled(gl);

	OLED_GPIOD_SET(ctrl, 1);

	oled_backlight_on(gl, true);
	return 0;
}

/* RNx16 128x32 OLED display 8x16 font */
#define _FONT(c,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_a,_b,_c,_d,_e,_f)	\
	[(c)] = { 0x##_0, 0x##_1,0x##_2, 0x##_3,0x##_4, 0x##_5,0x##_6, 0x##_7, \
		  0x##_8, 0x##_9,0x##_a, 0x##_b,0x##_c, 0x##_d,0x##_e, 0x##_f }
static const unsigned char ascii[128][16] = {
	_FONT(' ', 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00),
	_FONT('!', 00,00,00,f8,00,00,00,00,00,00,00,33,30,00,00,00),
	_FONT('"', 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00),
	_FONT('#', 40,c0,78,40,c0,78,40,00,04,3f,04,04,3f,04,04,00),
	_FONT('$', 00,70,88,fc,08,30,00,00,00,18,20,ff,21,1e,00,00),
	_FONT('%', f0,08,f0,00,e0,18,00,00,00,21,1c,03,1e,21,1e,00),
	_FONT('&', 00,f0,08,88,70,00,00,00,1e,21,23,24,19,27,21,10),
	_FONT('\'',10,16,0e,00,00,00,00,00,00,00,00,00,00,00,00,00),
	_FONT('(', 00,00,00,e0,18,04,02,00,00,00,00,07,18,20,40,00),
	_FONT(')', 00,02,04,18,e0,00,00,00,00,40,20,18,07,00,00,00),
	_FONT('*', 40,40,80,f0,80,40,40,00,02,02,01,0f,01,02,02,00),
	_FONT('+', 00,00,00,f0,00,00,00,00,01,01,01,1f,01,01,01,00),
	_FONT(',', 00,00,00,00,00,00,00,00,00,00,00,00,00,10,16,0e),
	_FONT('-', 00,00,00,00,00,00,00,00,00,01,01,01,01,01,01,01),
	_FONT('.', 00,00,00,00,00,00,00,00,00,30,30,00,00,00,00,00),
	_FONT('/', 00,00,00,00,80,60,18,04,00,60,18,06,01,00,00,00),

	_FONT('0', 00,e0,10,08,08,10,e0,00,00,0f,10,20,20,10,0f,00),
	_FONT('1', 00,10,10,f8,00,00,00,00,00,20,20,3f,20,20,00,00),
	_FONT('2', 00,70,08,08,08,88,70,00,00,30,28,24,22,21,30,00),
	_FONT('3', 00,30,08,88,88,48,30,00,00,18,20,20,20,11,0e,00),
	_FONT('4', 00,00,c0,20,10,f8,00,00,00,07,04,24,24,3f,24,00),
	_FONT('5', 00,f8,08,88,88,08,08,00,00,19,21,20,20,11,0e,00),
	_FONT('6', 00,e0,10,88,88,18,00,00,00,0f,11,20,20,11,0e,00),
	_FONT('7', 00,38,08,08,c8,38,08,00,00,00,00,3f,00,00,00,00),
	_FONT('8', 00,70,88,08,08,88,70,00,00,1c,22,21,21,22,1c,00),
	_FONT('9', 00,e0,10,08,08,10,e0,00,00,00,31,22,22,11,0f,00),
	_FONT(':', 00,00,00,c0,c0,00,00,00,00,00,00,30,30,00,00,00),
	_FONT(';', 00,00,00,80,00,00,00,00,00,00,80,60,00,00,00,00),
	_FONT('<', 00,00,80,40,20,10,08,00,00,01,02,04,08,10,20,00),
	_FONT('=', 40,40,40,40,40,40,40,00,04,04,04,04,04,04,04,00),
	_FONT('>', 00,08,10,20,40,80,00,00,00,20,10,08,04,02,01,00),
	_FONT('?', 00,70,48,08,08,08,f0,00,00,00,00,30,36,01,00,00),

	_FONT('@', c0,30,c8,28,e8,10,e0,00,07,18,27,24,23,14,0b,00),
	_FONT('A', 00,00,c0,38,e0,00,00,00,20,3c,23,02,02,27,38,20),
	_FONT('B', 08,f8,88,88,88,70,00,00,20,3f,20,20,20,11,0e,00),
	_FONT('C', c0,30,08,08,08,08,38,00,07,18,20,20,20,10,08,00),
	_FONT('D', 08,f8,08,08,08,10,e0,00,20,3f,20,20,20,10,0f,00),
	_FONT('E', 08,f8,88,88,e8,08,10,00,20,3f,20,20,23,20,18,00),
	_FONT('F', 08,f8,88,88,e8,08,10,00,20,3f,20,00,03,00,00,00),
	_FONT('G', c0,30,08,08,08,38,00,00,07,18,20,20,22,1e,02,00),
	_FONT('H', 08,f8,08,00,00,08,f8,08,20,3f,21,01,01,21,3f,20),
	_FONT('I', 00,08,08,f8,08,08,00,00,00,20,20,3f,20,20,00,00),
	_FONT('J', 00,00,08,08,f8,08,08,00,c0,80,80,80,7f,00,00,00),
	_FONT('K', 08,f8,88,c0,28,18,08,00,20,3f,20,01,26,38,20,00),
	_FONT('L', 08,f8,08,00,00,00,00,00,20,3f,20,20,20,20,30,00),
	_FONT('M', 08,f8,f8,00,f8,f8,08,00,20,3f,00,3f,00,3f,20,00),
	_FONT('N', 08,f8,30,c0,00,08,f8,08,20,3f,20,00,07,18,3f,00),
	_FONT('O', e0,10,08,08,08,10,e0,00,0f,10,20,20,20,10,0f,00),

	_FONT('P', 08,f8,08,08,08,08,f0,00,20,3f,21,01,01,01,00,00),
	_FONT('Q', e0,10,08,08,08,10,e0,00,0f,18,24,24,38,50,4f,00),
	_FONT('R', 08,f8,88,88,88,88,70,00,20,3f,20,00,03,0c,30,20),
	_FONT('S', 00,70,88,08,08,08,38,00,00,38,20,21,21,22,1c,00),
	_FONT('T', 18,08,08,f8,08,08,18,00,00,00,20,3f,20,00,00,00),
	_FONT('U', 08,f8,08,00,00,08,f8,08,00,1f,20,20,20,20,1f,00),
	_FONT('V', 08,78,88,00,00,c8,38,08,00,00,07,38,0e,01,00,00),
	_FONT('W', f8,08,00,f8,00,08,f8,00,03,3c,07,00,07,3c,03,00),
	_FONT('X', 08,18,68,80,80,68,18,08,20,30,2c,03,03,2c,30,20),
	_FONT('Y', 08,38,c8,00,c8,38,08,00,00,00,20,3f,20,00,00,00),
	_FONT('Z', 10,08,08,08,c8,38,08,00,20,38,26,21,20,20,18,00),
	_FONT('[', 00,00,00,fe,02,02,02,00,00,00,00,7f,40,40,40,00),
	_FONT('\\',00,0c,30,c0,00,00,00,00,00,00,00,01,06,38,c0,00),
	_FONT(']', 00,02,02,02,fe,00,00,00,00,40,40,40,7f,00,00,00),
	_FONT('^', 00,00,04,02,02,02,04,00,00,00,00,00,00,00,00,00),
	_FONT('_', 00,00,00,00,00,00,00,00,80,80,80,80,80,80,80,80),

	_FONT('`', 00,02,02,04,00,00,00,00,00,00,00,00,00,00,00,00),
	_FONT('a', 00,00,80,80,80,80,00,00,00,19,24,22,22,22,3f,20),
	_FONT('b', 08,f8,00,80,80,00,00,00,00,3f,11,20,20,11,0e,00),
	_FONT('c', 00,00,00,80,80,80,00,00,00,0e,11,20,20,20,11,00),
	_FONT('d', 00,00,00,80,80,88,f8,00,00,0e,11,20,20,10,3f,20),
	_FONT('e', 00,00,80,80,80,80,00,00,00,1f,22,22,22,22,13,00),
	_FONT('f', 00,80,80,f0,88,88,88,18,00,20,20,3f,20,20,00,00),
	_FONT('g', 00,00,80,80,80,80,80,00,00,6b,94,94,94,93,60,00),
	_FONT('h', 08,f8,00,80,80,80,00,00,20,3f,21,00,00,20,3f,20),
	_FONT('i', 00,80,98,98,00,00,00,00,00,20,20,3f,20,20,00,00),
	_FONT('j', 00,00,00,80,98,98,00,00,00,c0,80,80,80,7f,00,00),
	_FONT('k', 08,f8,00,00,80,80,80,00,20,3f,24,02,2d,30,20,00),
	_FONT('l', 00,08,08,f8,00,00,00,00,00,20,20,3f,20,20,00,00),
	_FONT('m', 80,80,80,80,80,80,80,00,20,3f,20,00,3f,20,00,3f),
	_FONT('n', 80,80,00,80,80,80,00,00,20,3f,21,00,00,20,3f,20),
	_FONT('o', 00,00,80,80,80,80,00,00,00,1f,20,20,20,20,1f,00),

	_FONT('p', 80,80,00,80,80,00,00,00,80,ff,a1,20,20,11,0e,00),
	_FONT('q', 00,00,00,80,80,80,80,00,00,0e,11,20,20,a0,ff,80),
	_FONT('r', 80,80,80,00,80,80,80,00,20,20,3f,21,20,00,01,00),
	_FONT('s', 00,00,80,80,80,80,80,00,00,33,24,24,24,24,19,00),
	_FONT('t', 00,80,80,e0,80,80,00,00,00,00,00,1f,20,20,00,00),
	_FONT('u', 80,80,00,00,00,80,80,00,00,1f,20,20,20,10,3f,20),
	_FONT('v', 80,80,80,00,00,80,80,80,00,01,0e,30,08,06,01,00),
	_FONT('w', 80,80,00,80,00,80,80,80,0f,30,0c,03,0c,30,0f,00),
	_FONT('x', 00,80,80,00,80,80,80,00,00,20,31,2e,0e,31,20,00),
	_FONT('y', 80,80,80,00,00,80,80,80,80,81,8e,70,18,06,01,00),
	_FONT('z', 00,80,80,80,80,80,80,00,00,21,30,2c,22,21,30,00),
	_FONT('{', 00,00,00,00,80,7c,02,02,00,00,00,00,00,3f,40,40),
	_FONT('|', 00,00,00,00,ff,00,00,00,00,00,00,00,ff,00,00,00),
	_FONT('}', 00,02,02,7c,80,00,00,00,00,40,40,3f,00,00,00,00),
	_FONT('~', 00,06,01,01,02,02,04,04,00,00,00,00,00,00,00,00),

	_FONT(1, 00,00,00,00,00,00,00,00,00,ff,ff,ff,ff,ff,ff,ff),
	_FONT(127, 00,ff,ff,ff,ff,ff,ff,ff,00,ff,ff,ff,ff,ff,ff,ff),
};

static void oled_data_write(struct hd44780_lcd *gl, unsigned char c)
{
	struct rnx16_oled_gpio *gp =
		container_of(gl, struct rnx16_oled_gpio, lcd);
	const unsigned char *cg = ascii[c & 0x7f];
	unsigned int xpix = oled_col * 8 + OLED_X_OFFSET;
	int page;

	if (oled_row >= OLED_ROWS || oled_col >= OLED_COLS)
		return;

	spi_send_cmd(gp, 0xa6);
	spi_send_cmd(gp, 0x40);	/* Start line. */

	/* line1: page 0,1  line2: page 2,3  */
	/* one char takes 2 page 16 pbits height; */
	for (page = 0; page < 2; page++) {
		int i;

		OLED_GPIOD_SET(cs, 0);
		OLED_GPIOD_SET(dc, 0);

		spi_send_cmd(gp, 0xb0 + oled_row * 2 + page);
		spi_send_cmd(gp, (xpix >> 4) | 0x10);
		spi_send_cmd(gp, xpix & 0xf);

		OLED_GPIOD_SET(dc, 1);

		for (i = 0; i < 8; i++) {
			unsigned int raster = cg[page * 8 + i];
			if (c != 0xff && (c & 0x80))
				raster ^= ~0;
			spi_send_data(gp, raster);
		}
	}

	oled_shift_cursor(gl, true);
}

static int rnx16_oled_probe(struct platform_device *pdev)
{
	int err = 0;
	struct gpio_chip *gc =
		gpiochip_find_by_label(rnx16_oled_gpio->gpio_label);

	if (IS_ERR_OR_NULL(gc))
		err = PTR_ERR(gc);
	else if (!gc)
		err = -ENODEV;
	if (err) {
		pr_err("%s: cannot find OLED GPIO %s.\n", __func__,
			rnx16_oled_gpio->gpio_label);
		return err;
	}

#define	GET_GPIOD(name)	\
	do {	\
		struct gpio_desc *_gd = 	\
			gpiochip_get_desc(gc, rnx16_oled_gpio->oled_##name); \
		if (IS_ERR(_gd)) {	\
			err = PTR_ERR(_gd);	\
			goto out;	\
		} else if (!_gd) {	\
			err = -ENODEV;	\
			goto out;	\
		} else if ((err = gpiod_direction_output(_gd, 0)))	\
			goto out;	\
		rnx16_oled_gpio->oled_##name##d = _gd;	\
	} while (0)

	GET_GPIOD(sdin);
	GET_GPIOD(sclk);
	GET_GPIOD(dc);
	GET_GPIOD(cs);
	GET_GPIOD(ctrl);
	GET_GPIOD(reset);

	rnx16_oled_gpio->lcd.init	= init_oled;
	rnx16_oled_gpio->lcd.clear_lcd		= clear_oled;
	rnx16_oled_gpio->lcd.return_home	= oled_return_home;
	rnx16_oled_gpio->lcd.shift_cursor	= oled_shift_cursor;
	rnx16_oled_gpio->lcd.set_cursor_pos	= oled_set_cursor_pos;
	rnx16_oled_gpio->lcd.backlight_on	= oled_backlight_on;
	rnx16_oled_gpio->lcd.entry_display_on	= oled_backlight_on;
	rnx16_oled_gpio->lcd.data_write		= oled_data_write;

	rnx16_oled_gpio->lcd.lcm_num_cols	= OLED_COLS;
	rnx16_oled_gpio->lcd.lcm_num_rows	= OLED_ROWS;

	mutex_init(&rnx16_oled_gpio->lcd.lcm_lock);

	err = init_oled(&rnx16_oled_gpio->lcd);
out:
	if (err)
		pr_err("%s: failed to install OLED driver (err=%d).\n",
			__func__, err);
	else {
		platform_set_drvdata(pdev, &rnx16_oled_gpio->lcd);
		pr_info("%s: OLED driver successfully installed.\n", __func__);
	}
	return err;
}

static int rnx16_oled_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver rnx16_oled_driver = {
	.probe	= rnx16_oled_probe,
	.remove	= rnx16_oled_remove,
	.driver = {
		.name	= "rnx16-oled",
	},
};

static struct platform_device *rnx16_oled_pdev;
static int __init rnx16_oled_init(void)
{
	int err;

	for (rnx16_oled_gpio = &rnx16_oled_gpios[0];
		rnx16_oled_gpio->compatible; rnx16_oled_gpio++)
		if (readynas_io_compatible(rnx16_oled_gpio->compatible))
			break;
	if (!rnx16_oled_gpio->compatible) {
		pr_info("%s: platform incompatible with OLED.\n", __func__);
		return 0;
	}

	if ((err = platform_driver_register(&rnx16_oled_driver))) {
		pr_err("%s: installing RN316/5x6/7x6 LCD driver failed.\n",
			__func__);
		return err;
	}

	rnx16_oled_pdev =
		platform_device_register_simple(rnx16_oled_driver.driver.name,
						-1, NULL, 0);
	if (IS_ERR_OR_NULL(rnx16_oled_pdev))
		err = PTR_ERR(rnx16_oled_pdev);
	else if (!rnx16_oled_pdev)
		err = -ENODEV;

	if (err) {
		pr_err("%s: installing RN316/5x6/7x6 LCD device failed.\n",
			__func__);
		platform_driver_unregister(&rnx16_oled_driver);
		rnx16_oled_pdev = NULL;
	}
	return err;
}

static void rnx16_oled_exit(void)
{
	if (rnx16_oled_pdev) {
		platform_device_unregister(rnx16_oled_pdev);
		platform_driver_unregister(&rnx16_oled_driver);
		rnx16_oled_pdev = NULL;
	}
}

/*
 * Common button event and state handling.
 */
static struct input_dev *rn_button_input_dev;

#define	BUTTON_ATTR_IT87	(0<<16)
#define	BUTTON_ATTR_ACPI	(1<<16)
#define MK_BUTTON(type, e)	(BUTTON_ATTR_##type | (e))
#define IS_BUTTON(type, e)	((~0<<16)&(e) == BUTTON_ATTR_##type)

/*
 * struct button_work - front and rear panel button handling
 *
 * @name - button name appearing as name-button in procfs
 * @gpio_label - handling GPIO's chip name
 * @gpio_num - handling GPIO's pin number
 * @gpio_active_low - GPIO pin's polatiry
 * @gpio_desc - internally filled and used GPIO descriptor
 * @event - input layer event code: KEY_*
 * @input_devp - pointer to relevant input device struct pointer
 * @worker - button handling thread
 * @poller - button polling thread
 * @prev_button_state - for polling
 * @button - internal button number: BUTTON_BACKUP, BUTTON_RESET, ...
 * @type - event type: IRQ: spawns thread for each depress to poll for release,
 *	  THREAD: continues to poll button for release,
 *	  POLL: a common thread polls for press and release
 *	  PRIVATE: device specific
 * @init, @exit, @depressed - button-specific initializer, exiter,
 *	  and depress sensor
 * @irq: IRQ number for IRQ event button
 * @init_inx -  it8732-specific initialization index for shared button resource
 * @notifier - internal optional notifier
 * @private -  button-specific private item
 */
enum button_num {
	BUTTON_BACKUP	= 0,
	BUTTON_RESET	= 1,
	BUTTON_UID	= 2,

	BUTTON_FB_BASE	= 8,
	BUTTON_LEFT	= 8,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_OK,
};

enum button_event_type {
	BUTTON_TYPE_IRQ,
	BUTTON_TYPE_THREAD,
	BUTTON_TYPE_POLL,
	BUTTON_TYPE_PRIVATE,
};

static struct button_work {
	const char *name;
	const char *gpio_label;
	unsigned int gpio_num;
	bool	gpio_active_low;
	struct	gpio_desc *gpio_desc;
	unsigned int	event;
	struct	input_dev **input_devp;
	struct	work_struct worker;
	unsigned int button;
	enum	button_event_type type;
	bool	busy;
	int  (*init)(struct button_work *);
	void (*exit)(struct button_work *);
	int  (*depressed)(struct button_work *);
	int	prev_button_state;
	struct proc_dir_entry *status_proc;
	int  irq;
	int  init_inx;
	void (*notifier)(struct button_work *, bool);
	void *private;
} *button_work;	/* Point to platform-specific button array. */

static const char *button_type_str(struct button_work *bw)
{
	const char *p = "UNKWON";

	switch (bw->type) {
	case BUTTON_TYPE_IRQ:
		p = "IRQ"; break;
	case BUTTON_TYPE_POLL:
		p = "POLL"; break;
	case BUTTON_TYPE_THREAD:
		p = "THREAD"; break;
	case BUTTON_TYPE_PRIVATE:
		p = "PRIVATE"; break;
	default:
		break;
	}
	return p;
}

static void button_reporter(struct button_work *bw, bool depressed)
{
	input_event(*bw->input_devp, EV_KEY, bw->event, depressed);
	input_sync(*bw->input_devp);
	if (bw->notifier)
		bw->notifier(bw, depressed);
}

static void button_worker(struct work_struct *work)
{
	struct button_work *bw = container_of(work, struct button_work, worker);

	/* Ignore bouncy button. */
	msleep(10);
	if (bw->depressed(bw) > 0) {
		button_reporter(bw, true);
		while (bw->depressed(bw) > 0)
			msleep(10);
		button_reporter(bw, false);
	}
	bw->busy = false;
}

static void schedule_button_work(unsigned int button)
{
	struct button_work *bw;

	for (bw = button_work; bw->depressed; bw++) {
		if (bw->button != button)
			continue;
		else if (bw->busy || bw->depressed(bw) <= 0)
			return;

		bw->busy = true;
		switch (bw->type) {
		case BUTTON_TYPE_IRQ:
			schedule_work(&bw->worker);
			break;
		case BUTTON_TYPE_THREAD:
			button_worker(&bw->worker);
			break;
		default:
			break;
		}
		return;
	}
}

static struct gpio_desc *setup_gpio_input(const char *gpio_label,
					int gpio_num, int active_low)
{
	int err;
	struct gpio_desc *gd;
	struct gpio_chip *gc = gpiochip_find_by_label(gpio_label);

	if (IS_ERR_OR_NULL(gc)) {
		pr_err("%s: cannot find button GPIO %s\n",
					__func__, gpio_label);
		return gc ? gc : ERR_PTR(ENODEV);
	}

	gd = gpiochip_get_desc(gc, gpio_num);
	if (IS_ERR_OR_NULL(gd)) {
		pr_err("%s: cannot find button GPIO %s:%d\n",
			 __func__, gpio_label, gpio_num);
		return gd;
	}

	if (active_low)
		set_bit(FLAG_ACTIVE_LOW, &gd->flags);
	err = gpiod_direction_input(gd);
	return err ? ERR_PTR(err) : gd;
}

static int __init rn_button_input_init(void)
{
	int err = 0;

	rn_button_input_dev = input_allocate_device();
	if (!rn_button_input_dev)
		return -ENODEV;

	rn_button_input_dev->name = "rn_button";
	rn_button_input_dev->id.bustype = BUS_HOST;

	/* We support up 2 buttons. */
	input_set_capability(rn_button_input_dev, EV_KEY, KEY_COPY);
	input_set_capability(rn_button_input_dev, EV_KEY, KEY_RESTART);

	if ((err = input_register_device(rn_button_input_dev))) {
		pr_err("%s: installing button input class device failed.\n",
			__func__);
		input_unregister_device(rn_button_input_dev);
		rn_button_input_dev = NULL;
	}
	return err;
}

static void rn_button_input_exit(void)
{
	if (rn_button_input_dev) {
		input_unregister_device(rn_button_input_dev);
		rn_button_input_dev = NULL;
	}
}

/*
 * ACPI event based button handling.
 */
static void acpi_notify(struct acpi_device *device, u32 button)
{
	schedule_button_work(MK_BUTTON(ACPI, button));
}

static inline int acpi_add(struct acpi_device *device) { return 0; }
static int acpi_remove(struct acpi_device *device) { return 0; }
static const struct acpi_device_id button_device_ids[] = {
	{ "ACPI0006", 0 },
	{ "", 0},
};

static struct acpi_driver acpi_driver = {
	.name = "ACPI Button",
	.class = "Button",
	.ids = button_device_ids,
	.ops = {
		.add = acpi_add,
		.remove = acpi_remove,
		.notify = acpi_notify,
	},
};

static int button_gpio_depressed(struct button_work *bw)
{
	return gpiod_get_value(bw->gpio_desc);
}

static int acpi_driver_registgered;
static int button_acpi_init(struct button_work *bw)
{
	int err = 0;

	pr_info("%s: installing ACPI button notifier.\n", __func__);
	if (!acpi_driver_registgered)
		err = acpi_bus_register_driver(&acpi_driver);
	if (!err)
		acpi_driver_registgered++;
	return 0;
}

static void button_acpi_exit(struct button_work *bw)
{
	if (!--acpi_driver_registgered)
		acpi_bus_unregister_driver(&acpi_driver);
}

#include <linux/mfd/core.h>
#include <linux/mfd/lpc_ich.h>

#if 0
/*
 * Linux 4.1 kernel mangles some platform device names by appending .NNN
 * or similar. Compare only the root part for device search.
 */
static int match_dev_name_root(struct device *dev, void *data)
{
	const char *p = data, *devname = dev_name(dev);

	if (strncmp(p, devname, strlen(p)))
		return 0;
	return !devname[strlen(p)] || (devname[strlen(p)] == '.');
}
#endif

static int locate_acpi_devs(struct button_work *bw,
				struct platform_device **pdevp,
				struct pci_dev **pci_devp)
{
	struct device *dev;
	struct gpio_chip *gc = gpiochip_find_by_label(bw->gpio_label);

	if (!gc || !(dev = gc->dev))
		return -ENODEV;

	if (pdevp)
		*pdevp = container_of(dev, struct platform_device, dev);
	if (pci_devp)
		*pci_devp = container_of(dev->parent, struct pci_dev, dev);
	return 0;
}

/* We have not discovered how to enable ACPI event for reset on RN314/316. */
#ifdef CONFIG_READYNAS_BUTTON_ACPI
/*
 * GPE0_EN for the GPIO input pin must be enabled.
 * lpc_ich driver has set mfd_cell to the platforma device under the gpio
 * name.
 */
#define	PCI_ICH_GPIO_ROUT	0xb8
static int button_acpi_lpc_ich_init(struct button_work *bw)
{
	u32 gpe0_en, gpio_rout = 0;
	const struct mfd_cell *mc;
	struct platform_device *pdev;
	struct pci_dev *pci_dev;
	int err = locate_acpi_devs(bw, &pdev, &pci_dev);

	pr_info("%s: installing ACPI/ICH button notifier.\n", __func__);
	if (err)
		return err;

	if (!(mc = mfd_get_cell(pdev)))
		return -ENODEV;

	while (mc->name) {
		if (!strcmp(mc->name, bw->gpio_label))
			break;
		mc++;
	}
	if (!mc->name)
		return -ENODEV;
	gpe0_en = mc->resources[ICH_RES_GPE0].start;
	outl(inl(gpe0_en)|(1 << (bw->gpio_num + 16)), gpe0_en);

	/* 00:no effect  01:SMI#  10:SCI  11:reserved */
	pci_read_config_dword(pci_dev, PCI_ICH_GPIO_ROUT, &gpio_rout);
	gpio_rout &= ~(3 << (bw->gpio_num * 2));
	gpio_rout &= ~(2 << (bw->gpio_num * 2));
	pci_write_config_dword(pci_dev, PCI_ICH_GPIO_ROUT, gpio_rout);

	bw->private = pci_dev;
	return button_acpi_init(bw);
}

static void button_acpi_lpc_ich_exit(struct button_work *bw)
{
	button_acpi_exit(bw);
}
#endif

static struct button_work button_work_rnx220[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 1,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.init = button_acpi_init,
		.exit = button_acpi_exit,
		.type	= BUTTON_TYPE_THREAD,
		.button = MK_BUTTON(ACPI, 176),
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
#if 0	/* Not working */
	{
		.name = "uid",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 52,
		.gpio_active_low	= false,
		.type	= BUTTON_TYPE_POLL,
		.button = MK_BUTTON(ACPI, 177),
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
#endif
	{}
};

static struct button_work button_work_rn314[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 4,
		.gpio_active_low	= 1,
		.event	= KEY_COPY,
		.init	= button_acpi_init,
		.exit	= button_acpi_exit,
		.type	= BUTTON_TYPE_THREAD,
		.button	= MK_BUTTON(ACPI, 187),
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
/* We have not discovered how to enable ACPI event for reset on RN314... */
#ifdef CONFIG_RN314_BUTTON_ACPI
		.init	= button_acpi_lpc_ich_init,
		.exit	= button_acpi_lpc_ich_exit,
		.type	= BUTTON_TYPE_THREAD,
		.button	= MK_BUTTON(ACPI, 176),
#else
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
#endif
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

static struct button_work button_work_rn316[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

static struct button_work button_work_rn516[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 1,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

/*
 * Ultra2/4/6 GPIO poll-base button handler
 */
static struct button_work button_work_ultra2_4[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 4,
		.gpio_active_low	= 1,
		.event	= KEY_COPY,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_BACKUP,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

static struct button_work button_work_ultra6[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 15,
		.gpio_active_low	= 1,
		.event	= KEY_COPY,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_BACKUP,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 8,
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

static struct button_work button_work_pro6[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 15,
		.gpio_active_low	= 0,
		.event	= KEY_COPY,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_BACKUP,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 8,
		.gpio_active_low	= 0,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.button	= BUTTON_RESET,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{}
};

/*
 * RN3130 IT8732 GPIO interrupt-base button handler
 */
#define IT8732_RESET_IRQ	10
#define IT8732_UID_IRQ		11

#include <../drivers/hwmon/it87-gpio.h>

static irqreturn_t it8732_btn_interrupt(int, void *);
static int button_init_it87(struct button_work *bw)
{
	static unsigned long irq_set;
	static unsigned long init_inx;
	struct gpio_chip *gc = bw->gpio_desc->chip;

	if (init_inx >= 2)
		return -ENODEV;
	if (!gc->control)
		return -ENODEV;

	/* Set active-low. */
	gc->control(gc, IT8732_CMD_GPIO_POLARITY,
			(void *)((1UL << 16) | bw->gpio_num));
	gc->control(gc, MK_IT8732_CMD_GPIO_IRQ_REGISTER(bw->irq), 
			it8732_btn_interrupt);
	irq_set |= bw->irq << (4 * init_inx);
	gc->control(gc, IT8732_CMD_GPIO_IRQ_SET, (void *)irq_set);
	gc->control(gc, IT8732_CMD_GPIO_IRQ_EXT_ROUTING,
			(void *)((init_inx << 16) | bw->gpio_num));
	bw->init_inx = init_inx++;
	return 0;
}

static void button_exit_it87(struct button_work *bw)
{
	unsigned long irq_set;
	struct gpio_chip *gc = bw->gpio_desc->chip;

	if (!gc->control)
		return;

	/* Disable routing first. */
	gc->control(gc, IT8732_CMD_GPIO_IRQ_EXT_ROUTING, (void *)~0UL);
	irq_set = gc->control(gc, IT8732_CMD_GPIO_IRQ_GET, NULL);
	irq_set &= ~(0xf << (4 * bw->init_inx));
	gc->control(gc, IT8732_CMD_GPIO_IRQ_SET, (void *)irq_set);
	gc->control(gc, MK_IT8732_CMD_GPIO_IRQ_REGISTER(bw->irq), NULL);
}

static struct button_work button_work_rn3130[] = {
#if 0	/* Not working */
	{/* GPIO#54 (a.k.a. GPIO76 is incapable of ext. routed IRQ */
		.name 		= "uid",
		.gpio_label	= "gpio_it8732",
		.gpio_num	= 54,	/* 12 in original code (+32 ?) */
		.button	= BUTTON_UID,
		.type	= BUTTON_TYPE_POLL,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
#endif
	{
		.name		= "reset",
		.gpio_label	= "gpio_it8732",
		.gpio_num	= 21,
		.event	= KEY_RESTART,
		.init	= button_init_it87,
		.exit	= button_exit_it87,
		.button	= MK_BUTTON(IT87, BUTTON_RESET),
		.type	= BUTTON_TYPE_IRQ,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
		.irq = IT8732_RESET_IRQ,
	},
	{}
};

static irqreturn_t it8732_btn_interrupt(int irq, void *dev_id)
{
	int i;

	for (i = 0; button_work[i].name; i++) {
		if (button_work[i].irq == irq) {
			schedule_button_work(MK_BUTTON(IT87,
					button_work[i].button));
			break;
		}
	}
	return IRQ_HANDLED;
}

/*
 * RN526/626 Front Board button handler.
 */
#include <linux/i2c.h>

#define I2CFB_ADAPTER		0
#define I2CFB_I2C_ADDR		0x1c

#define I2CFB_ID		0x0
#define I2CFB_LED_STATE		0x1
#define I2CFB_LED_BRIGHTNESS	0x2
#define I2CFB_BUTTON_EVENT	0x3
#define I2CFB_BUTTON_STATE	0x4
#define I2CFB_BUTTON_EVENT_MASK	0x5

static struct input_dev *fb_button_input_dev;

static struct i2cfb_work {
	const char *name;
	const char *gpio_label;
	unsigned int gpio_num;
	int	irq;
	bool	gpio_active_low;
	struct	gpio_desc *gpio_desc;
	u8	prev_button_state;
	struct	i2c_adapter *i2c_adapter;
	struct	mutex	lock;
	struct	work_struct worker;
} i2cfb_work = {
	.name		= "i2cfb_work",
	.gpio_label	= "gpio_ich",
	.gpio_num	= 2,
	.irq		= 0,	/* Set same as ACPI IRQ (9). */
};

static int i2cfb_button_depressed(struct button_work *bw);

static struct button_work button_work_rn526[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it8732",
		.gpio_num	= 23,	/* i.e. "GPIO37" */
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.input_devp	= &rn_button_input_dev,
		.depressed = button_gpio_depressed,
	},
	{
		.name		= "left",
		.gpio_num	= 0,	/* BUTTON_STATE bit number */
		.event	= KEY_LEFT,
		.type	= BUTTON_TYPE_PRIVATE,
		.input_devp	= &fb_button_input_dev,
		.private = &i2cfb_work,
		.depressed = i2cfb_button_depressed,
	},
	{
		.name		= "right",
		.gpio_num	= 1,	/* BUTTON_STATE bit number */
		.event	= KEY_RIGHT,
		.type	= BUTTON_TYPE_PRIVATE,
		.input_devp	= &fb_button_input_dev,
		.private = &i2cfb_work,
		.depressed = i2cfb_button_depressed,
	},
	{
		.name		= "up",
		.gpio_num	= 2,	/* BUTTON_STATE bit number */
		.event	= KEY_UP,
		.type	= BUTTON_TYPE_PRIVATE,
		.input_devp	= &fb_button_input_dev,
		.private = &i2cfb_work,
		.depressed = i2cfb_button_depressed,
	},
	{
		.name		= "down",
		.gpio_num	= 3,	/* BUTTON_STATE bit number */
		.event	= KEY_DOWN,
		.type	= BUTTON_TYPE_PRIVATE,
		.input_devp	= &fb_button_input_dev,
		.private = &i2cfb_work,
		.depressed = i2cfb_button_depressed,
	},
	{
		.name		= "ok",
		.gpio_num	= 4,	/* BUTTON_STATE bit number */
		.event	= KEY_OK,
		.type	= BUTTON_TYPE_PRIVATE,
		.input_devp	= &fb_button_input_dev,
		.private = &i2cfb_work,
		.depressed = i2cfb_button_depressed,
	},
	{}
};

static s32 inline i2cfb_smbus_read_byte(struct i2cfb_work *iw, u16 reg)
{
	union i2c_smbus_data i2c_data;
	int status = i2c_smbus_xfer(iw->i2c_adapter, I2CFB_I2C_ADDR, 0,
				I2C_SMBUS_READ, reg,
				I2C_SMBUS_BYTE_DATA, &i2c_data);

	return (status < 0) ? status : i2c_data.byte;
}

static s32 inline i2cfb_smbus_write_byte(struct i2cfb_work *iw, u16 reg, u8 d)
{
	union i2c_smbus_data i2c_data = {.byte = d};

	return i2c_smbus_xfer(iw->i2c_adapter, I2CFB_I2C_ADDR, 0,
				I2C_SMBUS_WRITE, reg,
				I2C_SMBUS_BYTE_DATA, &i2c_data);
}

static int i2cfb_button_depressed(struct button_work *bw)
{
	struct i2cfb_work *iw = bw->private;
	int i2cret = i2cfb_smbus_read_byte(iw, I2CFB_BUTTON_STATE);

	return !!(i2cret & (1 << bw->gpio_num));
}

/*
 * FB button processor scheduled by GPIO2 IRQ.
 * Compare current I2c reading with previous one.
 */
static void i2cfb_button_worker(struct work_struct *work)
{
	u8 diff;
	struct button_work *bw;
	struct i2cfb_work *iw = container_of(work, struct i2cfb_work, worker);
	int i2cret = i2cfb_smbus_read_byte(iw, I2CFB_BUTTON_STATE);

	if (i2cret < 0)
		return;

	if (!(diff = iw->prev_button_state ^ (i2cret & 0xff)))
		return;

	for (bw = button_work; bw->name; bw++)
		if (bw->type == BUTTON_TYPE_PRIVATE) {
			u8 button_mask = (1 << bw->gpio_num);

			if (diff & button_mask)
				button_reporter(bw, !!(i2cret & button_mask));
		}

	iw->prev_button_state = i2cret;
}

/*
 * GPIO2 IRQ handler FB button events.
 * Accessing I2c may sleep. Schedule work for processing.
 * FB button events flip GPIO2 signal. Prepare for next event by inverting
 * GPIO2 IRQ polarity.
 */
static irqreturn_t i2cfb_ichx_lpt_irq_handler(int irq, void *arg)
{
	struct gpio_chip *gc = i2cfb_work.gpio_desc->chip;

	schedule_work(&i2cfb_work.worker);
	return gc->control(gc, MK_ICHX_LPT_POL_GPIO_IRQ(INV,
					i2cfb_work.gpio_num), NULL)
				? IRQ_NONE : IRQ_HANDLED;
}

static void i2cfb_exit(void)
{
	if (!IS_ERR_OR_NULL(i2cfb_work.gpio_desc) &&
		i2cfb_work.gpio_desc->chip &&
		i2cfb_work.gpio_desc->chip->control)
		i2cfb_work.gpio_desc->chip->control(
				i2cfb_work.gpio_desc->chip,
				MK_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(
						i2cfb_work.gpio_num,
						i2cfb_work.irq,
						ICHX_LPT_TYP_GPIO_IRQ_SCI),
				NULL);
	i2cfb_work.gpio_desc = NULL;

	if (!IS_ERR_OR_NULL(fb_button_input_dev))
		input_unregister_device(fb_button_input_dev);
	fb_button_input_dev = NULL;
}

static int i2cfb_init(void)
{
	int err;
	struct gpio_chip *gc;
	struct i2cfb_work *iw = &i2cfb_work;

	if (!readynas_io_compatible(compatible_rn526))
		return 0;

	pr_info("%s: Initializing I2c FB buttons using %s#%d (IRQ)\n",
		__func__, iw->gpio_label, iw->gpio_num);
	mutex_init(&iw->lock);
	INIT_WORK(&iw->worker, i2cfb_button_worker);

	iw->gpio_desc = setup_gpio_input(iw->gpio_label,
					iw->gpio_num,
					iw->gpio_active_low);
	if (IS_ERR_OR_NULL(iw->gpio_desc))
		return iw->gpio_desc ?  PTR_ERR(iw->gpio_desc) : -ENODEV;

	iw->i2c_adapter = i2c_get_adapter(I2CFB_ADAPTER);
	if (IS_ERR_OR_NULL(iw->i2c_adapter)) {
		return iw->i2c_adapter ? PTR_ERR(iw->i2c_adapter) : -ENODEV;
	}

	if ((err = i2cfb_smbus_read_byte(iw, I2CFB_ID)) < 0)
		return err;
	if (err != 'F') {
		pr_err("%s: ID='%02x' mismatches Front Board.", __func__, err);
		return -ENODEV;
	}

	if ((err = i2cfb_smbus_write_byte(iw, I2CFB_LED_BRIGHTNESS, 15)))
		return err;

	fb_button_input_dev = input_allocate_device();
	if (IS_ERR_OR_NULL(fb_button_input_dev)) {
		i2cfb_exit();
		return -ENODEV;
	}

	fb_button_input_dev->name = "fb_button";
	fb_button_input_dev->id.bustype = BUS_HOST;

	/* We support 5 buttons. */
	input_set_capability(fb_button_input_dev, EV_KEY, KEY_LEFT);
	input_set_capability(fb_button_input_dev, EV_KEY, KEY_RIGHT);
	input_set_capability(fb_button_input_dev, EV_KEY, KEY_UP);
	input_set_capability(fb_button_input_dev, EV_KEY, KEY_DOWN);
	input_set_capability(fb_button_input_dev, EV_KEY, KEY_OK);

	if ((err = input_register_device(fb_button_input_dev))) {
		pr_err("%s: installing button input class device failed.\n",
			__func__);
		goto exit2;
	}
	if ((err = i2cfb_smbus_read_byte(iw, I2CFB_BUTTON_STATE)) < 0)
		goto exit;
	iw->prev_button_state = err & 0xff;
	if ((err = i2cfb_smbus_write_byte(iw, I2CFB_BUTTON_EVENT_MASK, 0)))
		goto exit;

	gc = iw->gpio_desc->chip;
	if (!gc->control)
		err = -ENODEV;

	if (!err)
		err = gc->control(gc, MK_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(
						iw->gpio_num,
						iw->irq,
						ICHX_LPT_TYP_GPIO_IRQ_SCI),
					i2cfb_ichx_lpt_irq_handler);

	if (!err)
		err = gc->control(gc, MK_ICHX_LPT_POL_GPIO_IRQ(INV,
					iw->gpio_num), NULL);
exit:
	if (err) {
		pr_err("%s: gpio->control failed in IRQ registration.\n",
			__func__);
exit2:
		i2cfb_exit();
	}
	return err;
}

/*
 * RR[34]312 buttons
 */
extern void *rnled_get_info(const char *);
extern int rnled_toggle(void *);
static void notifier_rnx312_uid_led_toggle(struct button_work *bw,
						bool depressed)
{
	if (depressed) {
		if (!bw->private)
			bw->private = rnled_get_info("readynas:blue:uid");
		if (bw->private)
			rnled_toggle(bw->private);
	}
}

static struct button_work button_work_rrx312[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it8732",
		.gpio_num	= 31,	/* i.e. "GPIO47" */
		.gpio_active_low	= 1,
		.event	= KEY_RESTART,
		.type	= BUTTON_TYPE_POLL,
		.input_devp	= &rn_button_input_dev,
		.depressed	= button_gpio_depressed,
	},
	{
		.name		= "uid",
		.gpio_label	= "gpio_it8732",
		.gpio_num	= 49,	/* i.e. "GPIO71" */
		.gpio_active_low	= 0,
		.event	= KEY_FIND,
		.type	= BUTTON_TYPE_POLL,
		.input_devp	= &rn_button_input_dev,
		.depressed	= button_gpio_depressed,
		.notifier	= notifier_rnx312_uid_led_toggle,
	},
	{}
};

/*
 * Array of compatible button handlers.
 * Generic button state API.
 */
static struct button_set {
	const char **compatible;
	struct button_work *work;
} button_set[] = {
	{ compatible_ultra2,	button_work_ultra2_4 },
	{ compatible_ultra4,	button_work_ultra2_4 },
	{ compatible_ultra6,	button_work_ultra6 },
	{ compatible_pro6,	button_work_pro6 },
	{ compatible_rn312,	button_work_rn314 },
	{ compatible_rn314,	button_work_rn314 },
	{ compatible_rn316,	button_work_rn316 },
	{ compatible_rn516,	button_work_rn516 },
	{ compatible_rn526,	button_work_rn526 },
	{ compatible_rn3130,	button_work_rn3130 },
	{ compatible_rnx220,	button_work_rnx220 },
	{ compatible_rrx312,	button_work_rrx312 },
	{}
};

static int button_state_show(struct seq_file *s, void *v)
{
	struct button_work *bw = s->private;

	seq_printf(s, "%d\n", bw->depressed(bw));
	return 0;
}

static int button_state_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, button_state_show, PDE_DATA(inode));
}

static const struct file_operations button_state_proc_fop = {
	.open		= button_state_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int create_button_state_proc(struct button_work *bw)
{
	int err;
	char name[strlen(bw->name) + strlen("-button") + 2];

	snprintf(name, sizeof name, "%s-button", bw->name);
	bw->status_proc = proc_create_data(name, 0444,
					readynas_proc_root,
					&button_state_proc_fop, bw);
	err = bw->status_proc ? 0 : -EIO;
	if (err)
		pr_err("%s: failed to create procfs %s\n", __func__, name);
	return err;
}

/*
 * Polling mode button handler kthread
 */
#define POLL_INTERVAL_MIN	50
#define POLL_INTERVAL_MAX	200
static unsigned int	poll_interval = POLL_INTERVAL_MIN;
static struct mutex	poller_lock;
struct task_struct	*poller_kthread;

static int button_thread_show(struct seq_file *s, void *v)
{
	mutex_lock(&poller_lock);
	seq_printf(s, "%u\n", poller_kthread ? poller_kthread->pid : 0);
	mutex_unlock(&poller_lock);
	return 0;
}

static int button_poller(void *data)
{
	while (!kthread_should_stop()) {
		struct button_work *bw;

		for (bw = data; bw->name; bw++) {
			int state;

			if (bw->type != BUTTON_TYPE_POLL)
				continue;
			if (bw->depressed(bw) == bw->prev_button_state) 
				continue;
			msleep(10);	/* Wait for debouncing. */
			if ((state = bw->depressed(bw)) ==
					bw->prev_button_state) 
				continue;
			button_reporter(bw, state);
			bw->prev_button_state = state;
		}
		msleep_interruptible(poll_interval);
	}
	return 0;
}

static int start_button_poller(void)
{
	int err = 0;

	mutex_lock(&poller_lock);
	if (!poller_kthread) {
		poller_kthread = kthread_run(button_poller, button_work,
						"readynas-button");
		if (IS_ERR_OR_NULL(poller_kthread)) {
			err = -ENOMEM;
			poller_kthread = NULL;
		}
	}
	mutex_unlock(&poller_lock);
	return err;
}

static void stop_button_poller(void)
{
	mutex_lock(&poller_lock);
	if (poller_kthread) {
		kthread_stop(poller_kthread);
		poller_kthread = NULL;
	}
	mutex_unlock(&poller_lock);
}

static ssize_t button_thread_write(struct file *file, const char __user *buffer,
					size_t count, loff_t *off)
{
	size_t size = min_t(size_t, count, 10);
	char buf[size];
	unsigned int val;

	copy_from_user(buf, buffer, size - 1);
	buf[size - 1] = '\0';
	if (kstrtouint(buf, 0, &val) < 0)
		return -EINVAL;

	if (val) {
		if (val < POLL_INTERVAL_MIN)
			val = POLL_INTERVAL_MIN;
		if (val > POLL_INTERVAL_MAX)
			val = POLL_INTERVAL_MAX;
		poll_interval = val;
		start_button_poller();
	} else
		stop_button_poller();
	return count;
}

static int button_thread_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, button_thread_show, PDE_DATA(inode));
}

static const struct file_operations button_thread_proc_fop = {
	.open		= button_thread_proc_open,
	.read		= seq_read,
	.write		= button_thread_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

struct proc_dir_entry *poller_proc;
static int create_button_thread_proc(void)
{
	int err;

	if (poller_proc)
		return 0;

	poller_proc = proc_create_data("button-poll", 0666,
					readynas_proc_root,
					&button_thread_proc_fop, NULL);
	err = poller_proc ? 0 : -EIO;
	if (err)
		pr_err("%s: failed to create procfs button-poll\n", __func__);
	return err;
}

/*
 * Common button handler initializer
 */
static const char *button_input_dev_name(struct button_work *bw)
{
	return (*bw->input_devp && (*bw->input_devp)->name) ?
					(*bw->input_devp)->name : "button";
}

static int button_init(void)
{
	struct button_set *bs;
	int err = 0;
	int i;

	for (bs = button_set; bs->compatible; bs++) {
		if (readynas_io_compatible(bs->compatible))
			break;
	}
	if (!bs->compatible)
		return 0;

	button_work = &bs->work[0];
	mutex_init(&poller_lock);

	for (i = 0; button_work[i].name; i++) {
		struct button_work *bw = &button_work[i];

		if (bw->gpio_label) {
			struct gpio_desc *gd =
				setup_gpio_input(bw->gpio_label, bw->gpio_num,
						bw->gpio_active_low);
			pr_info("%s: %s '%s' using %s#%d%s (%s)\n", __func__,
				button_input_dev_name(bw),
				bw->name,
				bw->gpio_label, bw->gpio_num,
				bw->gpio_active_low ? "n" : "",
				button_type_str(bw));
			if (IS_ERR_OR_NULL(gd)) {
				pr_err("%s: cannot find button GPIO %s:%d\n",
					 __func__,
					bw->gpio_label, bw->gpio_num);
				err = gd ? PTR_ERR(gd) : -ENODEV;
				break;
			}
			bw->gpio_desc = gd;
		} else
			pr_info("%s: %s '%s' (%s)\n", __func__,
				button_input_dev_name(bw),
				bw->name, button_type_str(bw));

		if (bw->init && (err = bw->init(bw))) {
			pr_err("%s: failed to initialize button '%s'\n",
				__func__, bw->name);
			break;
		}

		if ((err = create_button_state_proc(bw)))
			break;

		bw->prev_button_state = bw->depressed(bw);
		switch (bw->type) {
		case BUTTON_TYPE_IRQ:
			INIT_WORK(&bw->worker, button_worker);
			break;
		case BUTTON_TYPE_POLL:
			if (!(err = start_button_poller()))
				err = create_button_thread_proc();
			break;
		case BUTTON_TYPE_THREAD:
			/* No worker. */
		default:
			break;
		}

		if (err) {
			pr_err("%s: failed to set up button worker %s (%d).\n",
				__func__, bw->name, err);
			break;
		}
	}

	if (err) {
		if (poller_proc) {
			proc_remove(poller_proc);
			poller_proc = NULL;
		}

		stop_button_poller();

		while (--i >= 0) {
			struct button_work *bw = &button_work[i];
			if (bw->status_proc) {
				proc_remove(bw->status_proc);
				bw->status_proc = NULL;
			}
			if (bw->type == BUTTON_TYPE_IRQ)
				cancel_work_sync(&bw->worker);
			if (bw->exit)
				bw->exit(bw);
		}

		return err;
	}

	return 0;
}

static void button_exit(void)
{
	struct button_work *bw;

	if (!button_work)
		return;

	if (poller_proc)
		proc_remove(poller_proc);

	stop_button_poller();

	for (bw = button_work; bw->name; bw++) {
		if (bw->status_proc) {
			proc_remove(bw->status_proc);
			bw->status_proc = NULL;
		}
		if (bw->type == BUTTON_TYPE_IRQ)
			cancel_work_sync(&bw->worker);
		if (bw->exit)
			bw->exit(bw);
	}
}

/*
 * PWR button status procfs handlers
 */
static struct button_work pwr_button_state_bw = {
	.name		= "pwr",
	.gpio_label	= "gpio_ich",
};

#define C2388_GEN_PMCON2	0x24
#define C2388_PWRBTN_LVL_BIT	9
static int c3288_pwr_button_depressed(struct button_work *bw)
{
	u32 pmcon2;
	void __iomem *p = ioremap((unsigned long)bw->private, 1024);

	pmcon2 = *(u32 *)(p + C2388_GEN_PMCON2) & (1 << C2388_PWRBTN_LVL_BIT);
	iounmap(p);
	return !!pmcon2;
}

#define PCI_C2338_PBASE_ADDR	0x44
static int __init c3288_pwr_button_state_init(void)
{
	struct pci_dev *pci_dev;
	u32 pm_base;
	int err = locate_acpi_devs(&pwr_button_state_bw, NULL, &pci_dev);

	if (err) {
		pr_err(
	"%s: initializing C2338 pwr button state handler failed.\n", __func__);
		return err;
	}

	/* Get PM base address */
	pci_read_config_dword(pci_dev, PCI_C2338_PBASE_ADDR, &pm_base);
	pm_base &= PCI_BASE_ADDRESS_IO_MASK;
	if (!pm_base) {
		pr_err("%s: failed to get pm base.", __func__);
		return -ENODEV;
	}

	pwr_button_state_bw.private = (void *)(unsigned long)pm_base;
	pwr_button_state_bw.depressed = c3288_pwr_button_depressed;
	return 0;
}

/* PCH C236 for RR[34]312 */
static int __init rrx312_pwr_button_state_init(void)
{
	static const struct button_work rrx312_pwr_button_state_bw = {
		.name		= "pwr",
		.gpio_label	= "INT345D:00",
		.depressed	= button_gpio_depressed,
		.gpio_num	= 195,
		.gpio_active_low	= 1,
	};

	pwr_button_state_bw = rrx312_pwr_button_state_bw;
	pwr_button_state_bw.gpio_desc =
		setup_gpio_input(pwr_button_state_bw.gpio_label,
					pwr_button_state_bw.gpio_num,
					pwr_button_state_bw.gpio_active_low);
	if (IS_ERR_OR_NULL(pwr_button_state_bw.gpio_desc))
		return pwr_button_state_bw.gpio_desc ?
			PTR_ERR(pwr_button_state_bw.gpio_desc) : -ENODEV;
	return 0;
}

/* PCA9505 I2C GPIO for RR[34]312 */
#define	PCA9505_I2CBUS	0
#define	PCA9505_I2C_CHIPADDR	0x20
static struct i2c_client *pca9505_i2c_client;

static void pca9505_gpio_exit(void)
{
	if (!IS_ERR_OR_NULL(pca9505_i2c_client))
		i2c_unregister_device(pca9505_i2c_client);
	pca9505_i2c_client = NULL;
}

static int __init pca9505_gpio_init(void)
{
	static struct i2c_board_info pca9505_i2c_info = {
		I2C_BOARD_INFO("pca9505", PCA9505_I2C_CHIPADDR),
	};
	int err;
	struct i2c_adapter *i2c_adapter = i2c_get_adapter(PCA9505_I2CBUS);

	if (IS_ERR_OR_NULL(i2c_adapter)) {
		pr_err("%s: I2C bus #%d allocation failed.\n",
			__func__, PCA9505_I2CBUS);
		return i2c_adapter ? PTR_ERR(i2c_adapter) : -ENODEV;
	}

	pca9505_i2c_client = i2c_new_device(i2c_adapter, &pca9505_i2c_info);
	if (IS_ERR_OR_NULL(pca9505_i2c_client)) {
		pr_err("%s: pca9505 I2C client allocation failed.\n", __func__);
		return pca9505_i2c_client
			? PTR_ERR(pca9505_i2c_client) : -ENOMEM;
	}

	if ((err = device_attach(&pca9505_i2c_client->dev)) < 0) {
		pr_err("%s: attaching pca9505 device failed.\n", __func__);
		pca9505_gpio_exit();
	} else /* Positive 1 is returned for no interrupt support. */
		err = 0;
	return err;
}

#define PCI_ICH_GEN_PMCON_1	0xa0
#define ICH_PWRBTN_LVL_BIT	9
static int ich_pwr_button_depressed(struct button_work *bw)
{
	struct pci_dev *pci_dev = bw->private;
	u16 pci_ich_gen_pmcon_1;

	pci_read_config_word(pci_dev,
				PCI_ICH_GEN_PMCON_1, &pci_ich_gen_pmcon_1);

	return !(pci_ich_gen_pmcon_1 & (1 << ICH_PWRBTN_LVL_BIT));
}

static int __init ich_pwr_button_state_init(void)
{
	struct pci_dev *pci_dev = NULL;
	int err = locate_acpi_devs(&pwr_button_state_bw, NULL, &pci_dev);

	if (err)
		pr_err(
		"%s: initializing ICH pwr button state handler failed.\n",
			__func__);
	else {
		pwr_button_state_bw.private = pci_dev;
		pwr_button_state_bw.depressed = ich_pwr_button_depressed;
	}
	return err;
}

static int __init pwr_button_state_init(void)
{
	/* TODO: Check cputype to see if C3288. */
	int err;

	if (readynas_io_compatible(compatible_rn3130))
		err = c3288_pwr_button_state_init();
	else if (readynas_io_compatible(compatible_rrx312))
		err = rrx312_pwr_button_state_init();
	else
		err = ich_pwr_button_state_init();

	return err ? err : create_button_state_proc(&pwr_button_state_bw);
}

static void pwr_button_state_exit(void)
{
	if (pwr_button_state_bw.status_proc)
		proc_remove(pwr_button_state_bw.status_proc);
	pwr_button_state_bw.status_proc = NULL;
}


/***********************************************************************
 * RNS3220/42220 SMBUS-based GPIO
 */
static struct mutex smbus_lock;
static const u16 smbus_base	= 0xa40;

#define SM_REG(X)		(smbus_base + SMBUS_##X)
#define SMBUS_CTRL3		0x0e
#define SMBUS_ERR_STAT		0x09
#define SMBUS_FREQ		0x06
#define SMBUS_ADDR		0x05
#define SMBUS_CTRL		0x04
#define SMBUS_CMD		0x03
#define SMBUS_OP		0x02
#define SMBUS_DATA_SIZE		0x01
#define SMBUS_DATA		0x00

/* smctl3 (SMBUS_CTRL3) - bit0: 0-empty 1-not empty bit1:0-fifo no full 1-fifo full */
#define SMBUS_FIFO_EMPTY	0
#define SMBUS_FIFO_FULL		1

static int check_fifo_empty(void)
{
	unsigned long status;

	status = inb(SM_REG(CTRL3));

	return test_bit(SMBUS_FIFO_EMPTY, &status);
}

/*
 * Read data from device via smbus;
 *	off:device offset attached on smbus;
 *	cmd: wanted item, like device state(0x78),voltage(0x88);  
 *	buf: fill with data read from smbus;
 *	bytes: count of bytes, length;
 */
#define SMBUS_READ_BYTE		0x00
#define SMBUS_READ_WORD		0x01
#define SMBUS_READ_BLOCK	0x02

#define SMBUS_CTRL_CLR_FAL	0x03
#define SMBUS_CTRL_RESET_PSU	0x88
#define SMBUS_CTRL_EN_MAN	0x80
#define SMBUS_CTRL_RESET	0x40
#define SMBUS_CLK_92K		7
#define SMBUS_CLK_83K		8
#define SMBUS_CLK_76K		9
#define SMBUS_CLK_71K		10
#define SMBUS_CLK_65K		11

static void smbus_read(u8 off, u8 cmd, u8 *buf, size_t bytes)
{
	u8 op;
	int i;
	static unsigned long retry;

	/* If PIC was recently found unresponsive, do not bother. */
	if (!bytes || retry > jiffies)
		return;

	memset(buf, 0, bytes);

	mutex_lock(&smbus_lock);

	for (i = 0; i < 50 && !check_fifo_empty(); i++)
		msleep_interruptible(100);
	if (!check_fifo_empty()) {
		retry = jiffies + msecs_to_jiffies(30000);
		mutex_unlock(&smbus_lock);
		return;
	}
	retry = 0;

	switch(bytes) {
	case 1:
		op = SMBUS_READ_BYTE;
		break;
	case 2:
		op = SMBUS_READ_WORD;
		break;
	default:
		op = SMBUS_READ_BLOCK;
		break;
	}

	outb(off, SM_REG(ADDR));
	outb(cmd, SM_REG(CMD));	
	outb(op, SM_REG(OP));
	outb(SMBUS_CTRL_EN_MAN, SM_REG(CTRL));	/* Enable Manual Mode */

	for (i = 0; i < bytes; i++) {
		msleep(300);
		buf[i] = inb(SM_REG(DATA));
	}       

	mutex_unlock(&smbus_lock);
}

/*************************************************
 Write bytes to device via smbus;
	off:   device offset attached on smbus;
	cmd:   index in command list;
	bytes: count of bytes;
***************************************************/
#define SMBUS_WRITE_BYTE	0x08
#define SMBUS_WRITE_WORD	0x09
#define SMBUS_WRITE_BLOCK	0x0a

static void smbus_write(u8 off, u8 cmd, u8 *buf, size_t bytes)
{
	int i;

	mutex_lock(&smbus_lock);

	outb(off, SM_REG(ADDR));
	outb(cmd, SM_REG(CMD));
	outb(SMBUS_WRITE_BYTE, SM_REG(OP));
	outb(bytes, SM_REG(DATA_SIZE));

	/* Discard junk. */
	for (i = 0; i < 1000 && !check_fifo_empty(); i++) {
		inb(SM_REG(DATA));
		mdelay(1);
	}

	if (!check_fifo_empty()) {
		mutex_unlock(&smbus_lock);
		return;
	}

	outb(buf[0],SM_REG(DATA)); 
	outb(SMBUS_CTRL_EN_MAN, SM_REG(CTRL));	/* Enable Manual Mode */

	mdelay(5);

	for (i = 1; i < bytes; i++) {
		outb(buf[i], SM_REG(DATA));
		mdelay(5);
	}

	mutex_unlock(&smbus_lock);
}

static void smbus_reset(void)
{
	mutex_lock(&smbus_lock);
	outb(SMBUS_CTRL_RESET, SM_REG(CTRL));
	msleep(300);
	mutex_unlock(&smbus_lock);
}

static void smbus_clock_set(u8 clock)
{
	mutex_lock(&smbus_lock);
	if (0 < clock && clock < 15)
		outb(clock, SM_REG(FREQ));
	mutex_unlock(&smbus_lock);
}

static void smbus_clear(u8 off)
{
	mutex_lock(&smbus_lock);

	outb(off, SM_REG(ADDR));	/*Select the PSU module,B0 is above one,B2 is below one */
	outb(SMBUS_CTRL_CLR_FAL, SM_REG(CMD));	/*command */
	outb(SMBUS_WRITE_BYTE, SM_REG(OP));	/*Write byte Mode */
	outb(SMBUS_CTRL_CLR_FAL, SM_REG(DATA));	/*Write data to clear warning */
	outb(SMBUS_CTRL_EN_MAN, SM_REG(CTRL));	/*Enable Manual Mode */

	msleep(300);
	mutex_unlock(&smbus_lock);
}

static int rnx220_smbus_init(void)
{
	mutex_init(&smbus_lock);
	smbus_clock_set(SMBUS_CLK_76K);
	return 0;
}

static void rnx220_smbus_exit(void)
{
}

/*
 * RNS3220/42220 PSU HWMON SYSFS
 */
#define PMBUS_DEV_STAT_BYTE	0x78
#define PMBUS_DEV_STAT_WORD	0x79
#define PMBUS_DEV_READ_VIN	0x88

static const u8 rnx220_psu_addr[2]	= {
	0xb0,	/* RN3220_PSU_UP_ADDR */
	0xb2,	/* RN3220_PSU_BOTT_ADDR */
};

static void rn3220_psu_clear_warn(int index)
{
	smbus_clear(rnx220_psu_addr[index]);
}

static void rn3220_psu_reset(int index)
{
	smbus_reset();
}

static u16 rn3220_get_voltage(int index)
{      
	u8 vals[2];

	smbus_read(rnx220_psu_addr[index],
		PMBUS_DEV_READ_VIN, vals, sizeof vals);
	/* Little endian */
	return (vals[1] << 8) | vals[0];
}

static u8 rn3220_psu_state(int index)
{
	u8 val = 0;

	smbus_read(rnx220_psu_addr[index], PMBUS_DEV_STAT_BYTE, &val, 1);
	return val;
}

static ssize_t rn_show_voltage(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int ch = !strstr(attr->attr.name, "voltage1");

	return sprintf(buf, "%u\n", rn3220_get_voltage(ch));
}

static ssize_t rn_show_psu_state(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int ch = !strstr(attr->attr.name, "psu1");

	return ch ? sprintf(buf, "%u\n", rn3220_psu_state(1))
		  : sprintf(buf, "%u\n", inb(0xA46));
}

static ssize_t rn_store_psu(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	int ch = !strstr(attr->attr.name, "psu1");

	if (!memcmp(buf, "reset", 5))
		rn3220_psu_reset(ch);
	else if (!memcmp(buf, "clear",5))
		rn3220_psu_clear_warn(ch);

	return count;
}

#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

static SENSOR_DEVICE_ATTR(psu1,		S_IWUSR, NULL, rn_store_psu, 0);
static SENSOR_DEVICE_ATTR(psu2,		S_IWUSR, NULL, rn_store_psu, 1);
static SENSOR_DEVICE_ATTR(voltage1,	S_IRUGO, rn_show_voltage, NULL, 2);
static SENSOR_DEVICE_ATTR(voltage2,	S_IRUGO, rn_show_voltage, NULL, 3);
static SENSOR_DEVICE_ATTR(psu1_state,	S_IRUGO, rn_show_psu_state, NULL, 4);
static SENSOR_DEVICE_ATTR(psu2_state,	S_IRUGO, rn_show_psu_state, NULL, 5);

static struct attribute *rnx220_psu_attributes[] = {
	&sensor_dev_attr_psu1.dev_attr.attr,
	&sensor_dev_attr_psu2.dev_attr.attr,
	&sensor_dev_attr_voltage1.dev_attr.attr,
	&sensor_dev_attr_voltage2.dev_attr.attr,
	&sensor_dev_attr_psu1_state.dev_attr.attr,
	&sensor_dev_attr_psu2_state.dev_attr.attr,
	NULL
};

static const struct attribute_group rnx220_psu_group = {
	.attrs = rnx220_psu_attributes,
};

static const struct attribute_group *rnx220_psu_groups[] = {
	&rnx220_psu_group,
	NULL
};

static struct platform_device *rnx220_psu_pdev;
struct device *rnx220_psu_hwmon_dev;

static int rnx220_psu_probe(struct platform_device *pdev)
{
	rnx220_psu_hwmon_dev = 
		hwmon_device_register_with_groups(&pdev->dev,
						"rnx220_psu",
						NULL,
						rnx220_psu_groups);
	return PTR_ERR_OR_ZERO(rnx220_psu_hwmon_dev);
}

static int rnx220_psu_remove(struct platform_device *pdev)
{
	hwmon_device_unregister(rnx220_psu_hwmon_dev);
	return 0;
}

static struct platform_driver rnx220_psu_driver = {
	.probe	= rnx220_psu_probe,
	.remove	= rnx220_psu_remove,
	.driver = {
		.name = "rnx220-psu",
	},
};

static int rnx220_psu_inited;
static int rnx220_psu_init(void)
{
	int err;

	pr_info("%s: initializing RN3220/4220 PSU handler.\n", __func__);
	if ((err = platform_driver_register(&rnx220_psu_driver)))
		goto out;
	rnx220_psu_inited = 1;
	rnx220_psu_pdev = platform_device_register_simple(
				rnx220_psu_driver.driver.name, -1, NULL, 0);
	if (IS_ERR_OR_NULL(rnx220_psu_pdev)) {
		err = PTR_ERR(rnx220_psu_pdev);
		platform_driver_unregister(&rnx220_psu_driver);
		goto out;
	}
	rnx220_psu_inited = 2;
out:
	if (err)
		pr_err("%s: failed to create RN3220/4220 PSU procfs\n",
			__func__);
	return err;
}

static void rnx220_psu_exit(void)
{
	if (rnx220_psu_inited == 2)
		platform_device_unregister(rnx220_psu_pdev);
	if (rnx220_psu_inited >= 1)
		platform_driver_unregister(&rnx220_psu_driver);
}

/***********************************************************************
 * RNS3220/42220 PIC-based GPIO for sataN LEDs
 */
static void rnx220pic_gpio_set(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	static u8 led_bitmap[3];
	static const u8 led_reg[3] = {0xc4, 0xc0, 0xc2};
	u8 row = gpio_num / 4;
	u8 mask = 1 << (gpio_num % 4);

	if (val)
		led_bitmap[row] |= mask;
	else
		led_bitmap[row] &= ~mask;
	smbus_write(led_reg[row], 0, &led_bitmap[row], 1);
}

static struct gpio_chip rnx220pic_gpio_chip = {
	.label		= "gpio_rnx220pic",
	.owner		= THIS_MODULE,
	.set		= rnx220pic_gpio_set,
	.base		= -1,
	.ngpio		= 12,
};

static bool rnx220pic_gpio_inited;
static int __init rnx220pic_gpio_init(void)
{
	int err = gpiochip_add(&rnx220pic_gpio_chip);

	if (err)
		pr_err("%s: initializing RN3220/4220 PIC GPIO failed.\n",
			__func__);
	else {
		int i;

		for (i = 0; i < 12; i++)
			rnx220pic_gpio_set(&rnx220pic_gpio_chip, i, 0);
		rnx220pic_gpio_inited = true;
	}
	return err;
}

static void rnx220pic_gpio_exit(void)
{
	if (rnx220pic_gpio_inited)
		gpiochip_remove(&rnx220pic_gpio_chip);
}

static struct marvell_data {
	const int ndata;
	const u8 reg0;
	const u8 data[];
}
	marvell_preamble =
		{ .ndata = 4, .reg0 = 0xa8, .data = {0x58, 0x02, 0x00, 0x00}},
	marvell_init_1 =
		{ .ndata = 4, .reg0 = 0xac, .data = {0x10, 0x32, 0x54, 0x00}},
	marvell_disk3_1 =
		{ .ndata = 4, .reg0 = 0xac, .data = {0x10, 0x37, 0x54, 0x00}},
	marvell_disk3_2 =
		{ .ndata = 4, .reg0 = 0xa8, .data = {0x24, 0x02, 0x00, 0x00}},
	marvell_disk3_3 =
		{ .ndata = 1, .reg0 = 0xac, .data = {0xfb}},
	marvell_disk4_1 =
		{ .ndata = 4, .reg0 = 0xac, .data = {0x10, 0x32, 0x57, 0x00}},
	marvell_disk4_2 =
		{ .ndata = 4, .reg0 = 0xa8, .data = {0x24, 0x02, 0x00, 0x00}},
	marvell_disk4_3 =
		{ .ndata = 1, .reg0 = 0xac, .data = {0xef}, },
	marvell_disk34_1 =
		{ .ndata = 4, .reg0 = 0xac, .data = {0x10, 0x37, 0x57, 0x00}},
	marvell_disk34_2 =
		{ .ndata = 4, .reg0 = 0xa8, .data = {0x24, 0x02, 0x00, 0x00}},
	marvell_disk34_3 =
		{ .ndata = 1, .reg0 = 0xac, .data = {0xeb}},
	marvell_disk_1 =
		{ .ndata = 4, .reg0 = 0xac, .data = {0x10, 0x37, 0x57, 0x00}},
	marvell_disk_2 =
		{ .ndata = 4, .reg0 = 0xa8, .data = {0x24, 0x02, 0x00, 0x00}},
	marvell_disk_3 =
		{ .ndata = 1, .reg0 = 0xac, .data = {0xff}};

static struct marvell_data_seq {
	int num;
	struct marvell_data *md[];
}
	marvell_init = {
	/* Set the GPIO to control the HDD LED. */
	/* Set [AB:A8][def:0x00000008] memory to 0x00000258,Set GPIO_ACTIVITY_SELECT */
		.num = 2,
		.md = {&marvell_preamble, &marvell_init_1},
	},
	marvell_disk3 = {
	/* Set [AB:A8] memory to 0x00000258,Set GPIO_ACTIVITY_SELECT */
		.num = 4,
		.md = {&marvell_preamble,
			&marvell_disk3_1, &marvell_disk3_2, &marvell_disk3_3},
	},
	marvell_disk4 = {
	/* Set [AF:AC] memory bit[19:16]/GPIO4/HDD2 to 7H. */
	/* Set [AB:A8] memory to 0x00000258,Set GPIO_ACTIVITY_SELECT */
	/* Set [AF:AC][def:0x00543210] memory bit[11:8]/GPIO2/HHD1 to 7h. */
		.num = 4,
		.md = {&marvell_preamble,
			&marvell_disk4_1, &marvell_disk4_2, &marvell_disk4_3},
		},
	marvell_disk34 = {
	/* Set [AB:A8] memory to 0x00000224,Set AC[bit2&4]/GPIO2&4 to 0 */
		.num = 4,
		.md = {&marvell_preamble,
			&marvell_disk34_1, &marvell_disk34_2,&marvell_disk34_3},
	},
	marvell_disk = {
	/* Set [AB:A8] memory to 0x00000224,Set AC[bit2&4]/GPIO2&4 to 1 */
		.num = 4,
		.md = {&marvell_preamble,
		&marvell_disk_1, &marvell_disk_2, &marvell_disk_3},
	};

static void __iomem *marvell_mem;
static void marvell_write(struct marvell_data_seq *mds)
{
	/**************************************************
	1. Enter the Marvell 9170 chip configure space
	   through PCIE.  Bus 3,Device 0,Function 0.
	2. Read the BAR Address (offset 0x24-0x27)
	3. Enter the BAR memory space.
	4. Write 0x258	to [AB:A8] ,Set  GPIO ACTIVITY_SELECT
	5. HHD1 ,GPIO02 :	Set 0x00AC bit [11:8] to 7H
	   HDD2, GPIO04 :	 Set 0x00AC bit [23:20] to 7H
	6. Write 0x224	to [AB:A8] ,
	7. HHD1 ,GPIO02 :	 Set bit 2 to on/off
	   HHD2 ,GPIO04 :	Set Bit 4 to on/off
	 ****************************************************/

	int i;

	for (i = 0; i < mds->num; i++) {
		struct marvell_data *md = mds->md[i];
		u8 reg = md->reg0;
		int j;

		for (j = 0; j < md->ndata; j++)
			iowrite8(md->data[j], marvell_mem + reg++);
	}
}

/* gpio_num: 0 => DISK3, 1 => DISK4 */
static void rnx220marvell_gpio_set(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	static u8 state;

	u8 nstate = state;
	u8 mask = 1 << gpio_num;

	if (val)
		nstate |= mask;
	else
		nstate &= ~mask;

	if (nstate == state)
		return;
	switch (nstate) {
	case 0:
		marvell_write(&marvell_disk);
		break;
	case 1:
		marvell_write(&marvell_disk3);
		break;
	case 2:
		marvell_write(&marvell_disk4);
		break;
	case 3:
		marvell_write(&marvell_disk34);
		break;
	default:
		return;
	}
	state = nstate;
}

static struct gpio_chip rnx220marvell_gpio_chip = {
	.label		= "gpio_rnx220marvell",
	.owner		= THIS_MODULE,
	.set		= rnx220marvell_gpio_set,
	.base		= -1,
	.ngpio		= 2,
};

static unsigned int read_pci_config32(unsigned char bus,
		unsigned char slot, unsigned char func, unsigned char offset)
{
	outl((1<<31) | (bus<<16) | (slot<<11) | (func<<8) | offset, 0xcf8);
	return inl(0xcfc);
}

#define rnx220marvell_gpio_inited (marvell_mem != NULL)
static int __init rnx220marvell_gpio_init(void)
{
#define MARVELL_BUS_ID   0x03
#define MARVELL_DEV_ID   0x00
#define MARVELL_FUNC   	 0x00
#define MARVELL_BAR_ADDR 0x24

	int err;
	u32 marvell_reg;

	marvell_reg = read_pci_config32(MARVELL_BUS_ID, MARVELL_DEV_ID,
					MARVELL_FUNC, MARVELL_BAR_ADDR);	
	if (!(marvell_mem = ioremap(marvell_reg, 1024)))
		return -EIO;

	marvell_write(&marvell_init);
	err =  gpiochip_add(&rnx220marvell_gpio_chip);
	if (err) {
		pr_err("%s: initializing RN3220/4220 Marvell GPIO failed.\n",
			__func__);
		iounmap(marvell_mem);
		marvell_mem = NULL;
	}
	return err;
}

static void rnx220marvell_gpio_exit(void)
{
	if (rnx220marvell_gpio_inited) {
		gpiochip_remove(&rnx220marvell_gpio_chip);
		iounmap(marvell_mem);
	}
}

/*
 * Common /proc/readynas entries
 */
static int bootmenu_show(struct seq_file *s, void *p)
{
	const char *key = s->private;
	const struct dmi_device *dmi =
		dmi_find_device(DMI_DEV_TYPE_OEM_STRING, NULL, NULL);

	if (!dmi) {
		pr_err("%s: cannot find DM device\n", __func__);
		return -EIO;
	} else if (!strcmp(key, "enable"))
		seq_printf(s, "%d\n", !memcmp(dmi->name, "Manual", 6));
	else if (!strcmp(key, "option"))
		seq_printf(s, "%s\n", dmi->name);
	else {
		pr_err("%s: invalid bootmenu key: %s", __func__, key);
		return -EINVAL;
	}
	return 0;
}

static int bootmenu_open(struct inode *inode, struct file *file)
{
	return single_open(file, bootmenu_show, PDE_DATA(inode));
}

static const struct file_operations bootmenu_fop = {
	.open	= bootmenu_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release= single_release,
};

static struct proc_dir_entry *proc_bootmenu_enable, *proc_bootmenu_option;

static void procfs_exit(void)
{
	if (proc_bootmenu_option);
		proc_remove(proc_bootmenu_option);
	if (proc_bootmenu_enable);
		proc_remove(proc_bootmenu_enable);
	if (readynas_proc_root);
		proc_remove(readynas_proc_root);
	proc_bootmenu_option	= NULL;
	proc_bootmenu_enable	= NULL;
	readynas_proc_root	= NULL;
}

static int __init procfs_init(void)
{
	readynas_proc_root = proc_mkdir("readynas", NULL);
	if (!readynas_proc_root) {
		pr_err("%s: failed for create /proc/readynas\n", __func__);
		return -EIO;
	}
	if (!(proc_bootmenu_enable =
			proc_create_data("bootmenu-enable", 0444,
				readynas_proc_root,
				&bootmenu_fop, (void *)"enable")) ||
		!(proc_bootmenu_option =
			proc_create_data("bootmenu-option", 0444,
				readynas_proc_root,
				&bootmenu_fop, (void *)"option"))) {
		pr_err("%s: installing procfs failed.\n", __func__);
		procfs_exit();
		return -EIO;
	}
	return 0;
}

static int __init readynas_io_init(void)
{
	int ret;

	pr_info("%s: initializing ReadyNAS I/O.\n", __func__);
	if ((ret = procfs_init()))
		return ret;
	if ((ret = pwr_button_state_init()))
		goto fail;
	if (readynas_io_compatible(compatible_rnx220)) {
		if ((ret = rnx220_smbus_init()))
			goto fail;
		if ((ret = rnx220pic_gpio_init()))
			goto fail;
		if ((ret = rnx220marvell_gpio_init()))
			goto fail;
		if ((ret = rnx220_psu_init()))
			goto fail;
	}
	if (readynas_io_compatible(compatible_rrx312)) {
		if ((ret = pca9505_gpio_init()))
			goto fail;
	}
	if ((ret = i2cfb_init()))
		goto fail;
	if ((ret = rn_button_input_init()))
		goto fail;
	if ((ret = button_init()))
		goto fail;
	if ((ret = rnx4_lcd_init()))
		goto fail;
	if ((ret = rnx16_oled_init()))
		goto fail;
	pr_info("%s: initialization successfully completed.\n", __func__);
	return 0;

fail:
	rnx16_oled_exit();
	rnx4_lcd_exit();
	button_exit();
	rn_button_input_exit();
	i2cfb_exit();
	pca9505_gpio_exit();
	rnx220_psu_exit();
	rnx220marvell_gpio_exit();
	rnx220pic_gpio_exit();
	rnx220_smbus_exit();
	pwr_button_state_exit();
	procfs_exit();
	pr_err("%s: initialization failed (%d).\n", __func__, ret);
	return ret;
}

static void __exit readynas_io_exit(void)
{
	rnx16_oled_exit();
	rnx4_lcd_exit();
	button_exit();
	rn_button_input_exit();
	i2cfb_exit();
	pca9505_gpio_exit();
	rnx220_psu_exit();
	rnx220marvell_gpio_exit();
	rnx220pic_gpio_exit();
	rnx220_smbus_exit();
	pwr_button_state_exit();
	procfs_exit();
}

/* This module must be loaded after GPIO and I2C. */
device_initcall_sync(readynas_io_init);
module_exit(readynas_io_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NETGEAR Intel I/O driver v3");
MODULE_AUTHOR("hiro.sugawara@netgear.com");
