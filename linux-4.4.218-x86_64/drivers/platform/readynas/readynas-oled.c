/*
 * readynas-oled.c - NETGEAR ReadyNAS OLED bitmap display driver
 *
 * Copyright (c) 2015,2016 NETGEAR, Inc.
 *
 * Author: Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 */
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/file.h>
#include "readynas-io.h"

#define FONT_OVERRIDE 0

/*
 * Hitach HD44780 similation for OLED bitmap display
 */
static struct oled_gpio {
	const char *compatible;
	const char *gpio_label;
	unsigned int oled_sdin, oled_sclk, oled_dc,
			oled_cs, oled_ctrl, oled_reset;
	struct gpio_desc *oled_sdind, *oled_sclkd, *oled_dcd,
			*oled_csd, *oled_ctrld, *oled_resetd;
	struct hd44780_lcd lcd;
} oled_gpios[] = {{
	.compatible	= "rn316",
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
	.compatible	= "rn422_4",
	.gpio_label	= "gpio_dnv.0",
	.oled_sdin	= 34,
	.oled_sclk	= 35,
	.oled_dc	= 0,
	.oled_cs	= 122,
	.oled_ctrl	= 17,
	.oled_reset	= 31,
	.lcd = {
		.magic	= LCD_PLATFORM_MAGIC,
	},
},{
	.compatible	= "rn426_8",
	.gpio_label	= "gpio_dnv.0",
	.oled_sdin	= 30,
	.oled_sclk	= 28,
	.oled_dc	= 7,
	.oled_cs	= 8,
	.oled_ctrl	= 17,
	.oled_reset	= 31,
	.lcd = {
		.magic	= LCD_PLATFORM_MAGIC,
	},
},{
	.compatible	= "rnx16",
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
	.compatible	= "rnx24",
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
},{
	.compatible	= "rnx26",
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
},{
	.compatible	= "rnx28",
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
}, {}}, *oled_gpio;

#define OLED_COLS	16
#define OLED_ROWS	2
#define OLED_GPIOD_SET(name, val)	\
	gpiod_set_value(gp->oled_##name##d, (val))

static void spi_send(struct oled_gpio *gp, unsigned char c, bool cmd)
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

static void inline spi_send_data(struct oled_gpio *gp, unsigned char d)
{
	spi_send(gp, d, false);
}

static void inline spi_send_cmd(struct oled_gpio *gp, unsigned char c)
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
	struct oled_gpio *gp = container_of(gl, struct oled_gpio, lcd);

	spi_send_cmd(gp, on ? 0xaf : 0xae);
	gl->backlight_status = on;
}

#define	OLED_X_OFFSET	4

static void clear_oled(struct hd44780_lcd *gl)
{
	struct oled_gpio *gp = container_of(gl, struct oled_gpio, lcd);
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
	struct oled_gpio *gp =
		container_of(gl, struct oled_gpio, lcd);
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
	_FONT('-', 00,00,00,00,00,00,00,00,00,01,01,01,01,01,00,00),
	_FONT('.', 00,00,00,00,00,00,00,00,00,30,30,00,00,00,00,00),
	_FONT('/', 00,00,00,00,80,60,18,04,00,60,18,06,01,00,00,00),

	_FONT('0', e0,18,04,04,04,18,e0,00,07,18,20,20,20,18,07,00),
	_FONT('1', 00,10,08,fc,00,00,00,00,00,00,00,3f,00,00,00,00),
	_FONT('2', 00,08,04,04,88,70,00,00,00,30,28,26,21,20,00,00),
	_FONT('3', 00,08,84,84,48,38,00,00,00,10,20,20,11,0e,00,00),
	_FONT('4', 00,80,60,18,fc,00,00,00,06,05,04,04,3f,04,00,00),
	_FONT('5', 00,fc,84,84,04,00,00,00,00,10,20,20,11,0e,00,00),
	_FONT('6', 00,e0,98,44,44,84,08,00,00,07,19,20,20,10,0f,00),
	_FONT('7', 00,04,04,04,c4,3c,00,00,00,00,30,0e,01,00,00,00),
	_FONT('8', 30,48,84,84,84,48,30,00,0e,11,20,20,20,11,0e,00),
	_FONT('9', f0,08,04,04,98,e0,00,00,10,21,22,22,19,07,00,00),
	_FONT(':', 00,00,00,c0,c0,00,00,00,00,00,00,30,30,00,00,00),
	_FONT(';', 00,00,00,80,00,00,00,00,00,00,80,60,00,00,00,00),
	_FONT('<', 00,00,80,40,20,10,08,00,00,01,02,04,08,10,20,00),
	_FONT('=', 40,40,40,40,40,40,40,00,04,04,04,04,04,04,04,00),
	_FONT('>', 00,08,10,20,40,80,00,00,00,20,10,08,04,02,01,00),
	_FONT('?', 00,70,48,08,08,08,f0,00,00,00,00,30,36,01,00,00),

	_FONT('@', c0,30,c8,28,e8,10,e0,00,07,18,27,24,23,14,0b,00),
	_FONT('A', 00,00,f0,0c,f0,00,00,00,30,0f,04,04,04,0f,30,00),
	_FONT('B', 00,fc,04,04,d8,20,00,00,00,3f,21,21,12,0e,00,00),
	_FONT('C', e0,10,08,04,04,04,08,00,07,08,10,20,20,20,10,00),
	_FONT('D', fc,04,04,04,08,10,e0,00,3f,20,20,20,10,08,07,00),
	_FONT('E', fc,84,84,84,04,00,00,00,3f,20,20,20,20,00,00,00),
	_FONT('F', fc,84,84,84,84,04,00,00,3f,00,00,00,00,00,00,00),
	_FONT('G', e0,18,04,04,04,08,00,00,07,18,20,20,21,1f,00,00),
	_FONT('H', fc,80,80,80,80,80,fc,00,3f,00,00,00,00,00,3f,00),
	_FONT('I', 00,00,00,fc,00,00,00,00,00,00,00,3f,00,00,00,00),
	_FONT('J', 00,00,00,00,fc,00,00,00,00,20,20,20,1f,00,00,00),
	_FONT('K', fc,80,40,20,10,08,04,00,3f,01,02,04,08,10,20,00),
	_FONT('L', fc,00,00,00,00,00,00,00,3f,20,20,20,20,20,00,00),
	_FONT('M', fc,78,80,00,c0,38,fc,00,3f,00,07,38,07,00,3f,00),
	_FONT('N', fc,18,e0,00,00,fc,00,00,3f,00,00,07,18,3f,00,00),
	_FONT('O', e0,18,04,04,04,18,e0,00,07,18,20,20,20,18,07,00),

	_FONT('P', 00,fc,04,04,88,70,00,00,00,3f,01,01,00,00,00,00),
	_FONT('Q', e0,18,04,04,04,18,e0,00,03,0c,10,30,50,4c,43,00),
	_FONT('R', 00,fc,04,04,88,70,00,00,00,3f,01,01,06,18,20,00),
	_FONT('S', 00,38,44,84,04,08,00,00,00,10,20,20,11,0e,00,00),
	_FONT('T', 04,04,fc,04,04,04,00,00,00,00,3f,00,00,00,00,00),
	_FONT('U', fc,00,00,00,00,00,fc,00,0f,10,20,20,20,10,0f,00),
	_FONT('V', 0c,f0,00,00,00,e0,1c,00,00,00,0f,30,1e,01,00,00),
	_FONT('W', fc,00,f0,3c,c0,00,fc,00,03,3c,07,00,0f,3f,00,00),
	_FONT('X', 00,1c,60,c0,30,0c,00,00,20,18,07,01,06,38,00,00),
	_FONT('Y', 04,18,e0,00,e0,18,04,00,00,00,00,3f,00,00,00,00),
	_FONT('Z', 00,04,04,c4,34,0c,00,00,00,38,26,21,20,20,00,00),
	_FONT('[', 00,00,00,fe,02,02,02,00,00,00,00,7f,40,40,40,00),
	_FONT('\\',00,0c,30,c0,00,00,00,00,00,00,00,01,06,38,c0,00),
	_FONT(']', 00,02,02,02,fe,00,00,00,00,40,40,40,7f,00,00,00),
	_FONT('^', 00,00,04,02,02,02,04,00,00,00,00,00,00,00,00,00),
	_FONT('_', 00,00,00,00,00,00,00,00,80,80,80,80,80,80,80,80),

	_FONT('`', 00,02,02,04,00,00,00,00,00,00,00,00,00,00,00,00),
	_FONT('a', 00,00,40,40,40,80,00,00,00,1c,24,22,22,3f,00,00),
	_FONT('b', 00,fc,40,40,80,00,00,00,00,1f,20,20,10,0f,00,00),
	_FONT('c', 00,80,40,40,40,80,00,00,0f,10,20,20,20,10,00,00),
	_FONT('d', 00,80,40,40,40,fc,00,00,0f,10,20,20,20,3f,00,00),
	_FONT('e', 00,80,40,40,80,00,00,00,0f,12,22,22,22,13,00,00),
	_FONT('f', 00,40,f8,44,44,04,00,00,00,00,3f,00,00,00,00,00),
	_FONT('g', 00,c0,20,10,10,e0,00,00,00,13,24,24,24,1f,00,00),
	_FONT('h', fc,80,40,40,40,80,00,00,3f,00,00,00,00,3f,00,00),
	_FONT('i', 00,00,00,c8,00,00,00,00,00,00,00,3f,00,00,00,00),
	_FONT('j', 00,00,00,d0,00,00,00,00,00,00,80,7f,00,00,00,00),
	_FONT('k', fc,00,00,80,40,00,00,00,3f,02,0d,10,20,00,00,00),
	_FONT('l', 00,00,00,fc,00,00,00,00,00,00,00,3f,00,00,00,00),
	_FONT('m', c0,40,40,80,40,40,80,00,3f,00,00,3f,00,00,3f,00),
	_FONT('n', 00,c0,40,40,40,80,00,00,00,3f,00,00,00,3f,00,00),
	_FONT('o', 00,80,40,40,80,00,00,00,0f,10,20,20,10,0f,00,00),

	_FONT('p', c0,40,40,40,80,00,00,00,ff,10,10,10,08,07,00,00),
	_FONT('q', 00,80,40,40,40,80,00,00,07,08,10,10,10,ff,00,00),
	_FONT('r', 00,c0,80,40,40,40,00,00,00,3f,00,00,00,00,00,00),
	_FONT('s', c0,20,20,20,40,00,00,00,10,21,22,22,1c,00,00,00),
	_FONT('t', 00,40,f0,40,40,40,00,00,00,00,1f,20,20,20,00,00),
	_FONT('u', c0,00,00,00,00,00,c0,00,0f,10,20,20,20,10,3f,00),
	_FONT('v', c0,00,00,00,80,40,00,00,00,0f,30,1c,03,00,00,00),
	_FONT('w', c0,00,00,c0,80,00,c0,00,00,1f,3e,01,0f,30,0f,00),
	_FONT('x', 00,c0,00,00,80,40,00,00,20,10,0d,06,19,20,00,00),
	_FONT('y', c0,00,00,00,00,c0,40,00,80,87,68,18,07,00,00,00),
	_FONT('z', 00,40,40,40,40,c0,00,00,00,30,28,26,21,20,00,00),
	_FONT('{', 00,00,00,00,80,7c,02,02,00,00,00,00,00,3f,40,40),
	_FONT('|', 00,00,00,00,ff,00,00,00,00,00,00,00,ff,00,00,00),
	_FONT('}', 00,02,02,7c,80,00,00,00,00,40,40,3f,00,00,00,00),
	_FONT('~', 00,06,01,01,02,02,04,04,00,00,00,00,00,00,00,00),

	_FONT(1, 00,00,00,00,00,00,00,00,00,ff,ff,ff,ff,ff,ff,ff),
	_FONT(127, 00,ff,ff,ff,ff,ff,ff,ff,00,ff,ff,ff,ff,ff,ff,ff),
};

static void oled_data_write(struct hd44780_lcd *gl, unsigned char c)
{
	struct oled_gpio *gp = container_of(gl, struct oled_gpio, lcd);
	const unsigned char *cg = ascii[c & 0x7f];
	unsigned int xpix = oled_col * 8 + OLED_X_OFFSET;
	int page;
#if FONT_OVERRIDE
	struct file *f;
	mm_segment_t fs;
	char buf[18];

	fs = get_fs();
	set_fs(get_ds());
	sprintf(buf, "/root/font/%c", c);
	f = filp_open(buf, O_RDONLY, 0);
	if (!IS_ERR(f)) {
		loff_t off = 0;
		ssize_t ret;
		memset(&buf, '\0', sizeof(buf));
		ret = vfs_read(f, buf, 16, &off);
		if (ret > 0) {
			pr_info("Loaded font %c (%s)\n", c, buf);
			cg = buf;
		}
		else
			pr_info("Failed to load font %c: %ld\n", c, ret);
		filp_close(f, NULL);
	}
	set_fs(fs);
#endif

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

static int oled_probe(struct platform_device *pdev)
{
	int err = 0;
	struct gpio_chip *gc = rn_gpiochip_find_by_label(oled_gpio->gpio_label);

	if (IS_ERR(gc))
		err = PTR_ERR(gc);
	else if (!gc)
		err = -ENODEV;
	if (err) {
		pr_err("%s: cannot find OLED GPIO %s.\n", __func__,
			oled_gpio->gpio_label);
		return err;
	}

#define	GET_GPIOD(name)	\
	do {	\
		struct gpio_desc *_gd = 	\
			gpiochip_get_desc(gc, oled_gpio->oled_##name); \
		if (IS_ERR(_gd)) {	\
			err = PTR_ERR(_gd);	\
			goto out;	\
		} else if (!_gd) {	\
			err = -ENODEV;	\
			goto out;	\
		} else if ((err = gpiod_direction_output(_gd, 0)))	\
			goto out;	\
		oled_gpio->oled_##name##d = _gd;	\
	} while (0)

	GET_GPIOD(sdin);
	GET_GPIOD(sclk);
	GET_GPIOD(dc);
	GET_GPIOD(cs);
	GET_GPIOD(ctrl);
	GET_GPIOD(reset);

	oled_gpio->lcd.init	= init_oled;
	oled_gpio->lcd.clear_lcd		= clear_oled;
	oled_gpio->lcd.return_home	= oled_return_home;
	oled_gpio->lcd.shift_cursor	= oled_shift_cursor;
	oled_gpio->lcd.set_cursor_pos	= oled_set_cursor_pos;
	oled_gpio->lcd.backlight_on	= oled_backlight_on;
	oled_gpio->lcd.entry_display_on	= oled_backlight_on;
	oled_gpio->lcd.data_write		= oled_data_write;

	oled_gpio->lcd.lcm_num_cols	= OLED_COLS;
	oled_gpio->lcd.lcm_num_rows	= OLED_ROWS;

	mutex_init(&oled_gpio->lcd.lcm_lock);

	err = init_oled(&oled_gpio->lcd);
out:
	if (err)
		pr_err("%s: failed to install OLED driver.\n", __func__);
	else
		platform_set_drvdata(pdev, &oled_gpio->lcd);
	return err;
}

static int oled_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver oled_driver = {
	.probe	= oled_probe,
	.remove	= oled_remove,
	.driver = { .name	= "rnx16-oled", },
};

static struct platform_device *oled_pdev;
static int __init rn_oled_init(void)
{
	int err;

	for (oled_gpio = &oled_gpios[0];
		oled_gpio->compatible; oled_gpio++)
		if (readynas_io_compatible(oled_gpio->compatible))
			break;
	if (!oled_gpio->compatible)
		return 0;

	pr_info("%s: initializing %s OLED front display.\n", __func__,
			oled_gpio->compatible);
	if ((err = platform_driver_register(&oled_driver))) {
		pr_err("%s: failed to install OELD driver.\n",
			__func__);
		return err;
	}

	oled_pdev = platform_device_register_simple(oled_driver.driver.name,
							-1, NULL, 0);
	if (IS_ERR(oled_pdev))
		err = PTR_ERR(oled_pdev);
	else if (!oled_pdev)
		err = -ENODEV;

	if (err) {
		pr_err("%s: failed to install OELD device.\n", __func__);
		platform_driver_unregister(&oled_driver);
		oled_pdev = NULL;
	}
	return err;
}

static void rn_oled_exit(void)
{
	if (oled_pdev) {
		platform_device_unregister(oled_pdev);
		platform_driver_unregister(&oled_driver);
		oled_pdev = NULL;
	}
}

/* This module must be loaded after readynas-io. */
device_initcall_sync(rn_oled_init);
module_exit(rn_oled_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NETGEAR OLED bitmapped character display driver");
MODULE_AUTHOR("hiro.sugawara@netgear.com");
