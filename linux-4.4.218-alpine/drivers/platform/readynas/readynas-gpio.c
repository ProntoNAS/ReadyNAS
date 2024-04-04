/*
 * Platform-specific psuedo GPIO drivers for ReadyNAS x86_64 plarforms
 *
 * Copyright (c) 2016 Hiro Sugawara
 */

#include <linux/gpio.h>
#include <linux/i2c.h>

#include "readynas-io.h"

/*
 * PCA9505 I2C GPIO for RR[34]312
 * GPIO functionality is implemented in drivers/gpio/gpio-pca953x.c
 */
#define	PCA9505_I2C_CHIPADDR	0x20
static struct i2c_client *pca9505_i2c_client;

static void pca9505_gpio_exit(void *d)
{
	if (!IS_ERR_OR_NULL(pca9505_i2c_client))
		i2c_unregister_device(pca9505_i2c_client);
	pca9505_i2c_client = NULL;
}

static int __init pca9505_gpio_init(void *d)
{
	static struct i2c_board_info pca9505_i2c_info = {
		I2C_BOARD_INFO("pca9505", PCA9505_I2C_CHIPADDR),
	};
	int err;
	struct i2c_adapter *i2c_adapter =
				readynas_i2c_adapter_find(rn_i2c_adapters_ipch);

	pr_info("%s: initializing PCA9505 GPIO.\n", __func__);

	if (IS_ERR_OR_NULL(i2c_adapter)) {
		pr_err("%s: failed to locate I2C adapter.\n", __func__);
		return i2c_adapter ? PTR_ERR(i2c_adapter) : -ENODEV;
	}

	pca9505_i2c_client = i2c_new_device(i2c_adapter, &pca9505_i2c_info);
	if (IS_ERR_OR_NULL(pca9505_i2c_client)) {
		pr_err("%s: failed to allocate pca9505 I2C client.\n",
			__func__);
		return pca9505_i2c_client ?
			PTR_ERR(pca9505_i2c_client) : -ENOMEM;
	}

	if ((err = device_attach(&pca9505_i2c_client->dev)) != 1) {
		pr_err("%s: failed to attach pca9505 device.\n", __func__);
		pca9505_gpio_exit(NULL);
	} else if (!err)
		err = -ENODEV;
	else
		err = 0;
	return err;
}

/**
 * RNS3220/42220 PIC-based GPIO for sataN LEDs
 */
static void rnx220pic_gpio_set(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	int err;
	u8 row = gpio_num / 4;
	u8 mask = 1 << (gpio_num % 4);
	union i2c_smbus_data i2c_data;
	struct i2c_adapter *i2c_adapter =
			readynas_i2c_adapter_find(rn_i2c_adapters_nct677x);

	static u8 led_bitmap[3];
	static const u8 led_reg[3] = {0x62, 0x60, 0x61};

	if (IS_ERR_OR_NULL(i2c_adapter)) {
		pr_err("%s: cannot find compatible I2C chip.\n", __func__);
		return;
	}

	if (val)
		led_bitmap[row] |= mask;
	else
		led_bitmap[row] &= ~mask;

	i2c_data.byte = led_bitmap[row];
	if ((err = i2c_smbus_xfer(i2c_adapter,
			led_reg[row], 0,
			I2C_SMBUS_WRITE, 0,
			I2C_SMBUS_BYTE_DATA, &i2c_data)))
		pr_err("%s: error in SMBus write (%d)\n", __func__, err);
}

static int gpio_direction_output_always(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	return 0;
}

static struct gpio_chip rnx220pic_gpio_chip = {
	.label		= "gpio_rnx220pic",
	.owner		= THIS_MODULE,
	.set		= rnx220pic_gpio_set,
	.direction_output	= gpio_direction_output_always,
	.base		= -1,
	.ngpio		= 12,
};

static bool rnx220pic_gpio_inited;
static int __init rnx220pic_gpio_init(void *d)
{
	int err = gpiochip_add(&rnx220pic_gpio_chip);

	pr_info("%s: initializing RN3220/4220 PIC GPIO.\n", __func__);
	if (err) {
		pr_err("%s: failed to add RN3220/4220 PIC GPIO chip.\n",
			__func__);
		return err;
	}
	rnx220pic_gpio_inited = true;
	return 0;
}

static void rnx220pic_gpio_exit(void *d)
{
	if (rnx220pic_gpio_inited)
		gpiochip_remove(&rnx220pic_gpio_chip);
	rnx220pic_gpio_inited = false;
}

static struct marvell_data {
	const int ndata;
	const u8 reg0;
	const u8 data[];
}
	marvell_preamble = { 4, 0xa8, {0x58, 0x02, 0x00, 0x00}},
	marvell_init_1	=  { 4, 0xac, {0x10, 0x32, 0x54, 0x00}},
	marvell_disk3_1	=  { 4, 0xac, {0x10, 0x37, 0x54, 0x00}},
	marvell_disk3_2	=  { 4, 0xa8, {0x24, 0x02, 0x00, 0x00}},
	marvell_disk3_3	=  { 1, 0xac, {0xfb}},
	marvell_disk4_1	=  { 4, 0xac, {0x10, 0x32, 0x57, 0x00}},
	marvell_disk4_2	=  { 4, 0xa8, {0x24, 0x02, 0x00, 0x00}},
	marvell_disk4_3	=  { 1, 0xac, {0xef}, },
	marvell_disk34_1 = { 4, 0xac, {0x10, 0x37, 0x57, 0x00}},
	marvell_disk34_2 = { 4, 0xa8, {0x24, 0x02, 0x00, 0x00}},
	marvell_disk34_3 = { 1, 0xac, {0xeb}},
	marvell_disk_1	=  { 4, 0xac, {0x10, 0x37, 0x57, 0x00}},
	marvell_disk_2	=  { 4, 0xa8, {0x24, 0x02, 0x00, 0x00}},
	marvell_disk_3	=  { 1, 0xac, {0xff}};

static struct marvell_data_seq {
	int num;
	struct marvell_data *md[];
}
	marvell_init = {
	/* Set the GPIO to control the HDD LED. */
	/* Set [AB:A8][def:0x00000008] memory to 0x00000258 */
	/* Set GPIO_ACTIVITY_SELECT */
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
static void marvell_write(const struct marvell_data_seq *mds)
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
		u8 reg = mds->md[i]->reg0;
		int j, ndata = mds->md[i]->ndata;

		for (j = 0; j < ndata; j++)
			iowrite8(mds->md[i]->data[j], marvell_mem + reg++);
	}
}

/* gpio_num: 0 => DISK3, 1 => DISK4 */
static void rnx220marvell_gpio_set(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	static const struct marvell_data_seq *mds[] = {
		[0] = &marvell_disk,
		[1] = &marvell_disk3,
		[2] = &marvell_disk4,
		[3] = &marvell_disk34
	};
	static u8 state;

	u8 nstate = state;
	u8 mask = 1 << gpio_num;

	if (val)
		nstate |= mask;
	else
		nstate &= ~mask;

	if (nstate == state || nstate >= ARRAY_SIZE(mds))
		return;
	marvell_write(mds[nstate]);
	state = nstate;
}

static struct gpio_chip rnx220marvell_gpio_chip = {
	.label		= "gpio_rnx220marvell",
	.owner		= THIS_MODULE,
	.set		= rnx220marvell_gpio_set,
	.direction_output	= gpio_direction_output_always,
	.base		= -1,
	.ngpio		= 2,
};

static unsigned int read_pci_config32(unsigned char bus,
		unsigned char slot, unsigned char func, unsigned char offset)
{
	outl((1<<31) | (bus<<16) | (slot<<11) | (func<<8) | offset, 0xcf8);
	return inl(0xcfc);
}

static int __init rnx220marvell_gpio_init(void *d)
{
#define MARVELL_BUS_ID   0x03
#define MARVELL_DEV_ID   0x00
#define MARVELL_FUNC   	 0x00
#define MARVELL_BAR_ADDR 0x24

	int err;
	u32 marvell_reg;

	pr_info("%s: initializing RNX220 Marvell GPIO.\n", __func__);
	marvell_reg = read_pci_config32(MARVELL_BUS_ID, MARVELL_DEV_ID,
					MARVELL_FUNC, MARVELL_BAR_ADDR);
	if (!(marvell_mem = ioremap(marvell_reg, 1024))) {
		pr_err("%s: failed to remap registers.\n", __func__);
		return -EIO;
	}

	marvell_write(&marvell_init);
	if ((err =  gpiochip_add(&rnx220marvell_gpio_chip))) {
		pr_err("%s: failed to add GPIO chip.\n", __func__);
		iounmap(marvell_mem);
		marvell_mem = NULL;
	}
	return err;
}

static void rnx220marvell_gpio_exit(void *d)
{
	if (marvell_mem) {
		gpiochip_remove(&rnx220marvell_gpio_chip);
		iounmap(marvell_mem);
		marvell_mem = NULL;
	}
}

/*
 * Intel Apollo Lake GPIO consists of 4 individual GPIOs INT3452:00-03
 * spread amaong 4 (N, NW, SW, W) pinctrl communities.
 * This pseudo GPIO translates them into a linear single GPIO.
 */
enum { APL_NORTH, APL_NORTHWEST, APL_SOUTHWEST, APL_WEST };
static struct pinctrl_apl_gpio {
	const char *label;
	struct gpio_chip *chip;
} pinctrl_apl_gpio[] = {
	[APL_NORTH]	= { "INT3452:00", },
	[APL_NORTHWEST]	= { "INT3452:01", },
	[APL_SOUTHWEST]	= { "INT3452:03", },
	[APL_WEST]	= { "INT3452:02", }, {}
};

static struct gpio_chip
*pinctrl_apl_community_gpio_chip(unsigned int *gpio_num)
{
	struct gpio_chip *chip = NULL;

#define	CASE_APL_GPIO_N(base, end, off, gpio)	\
	case (base) ... (end):		\
		*gpio_num -= ((base) - ((off) - 0x500)/8);	\
		chip = pinctrl_apl_gpio[APL_##gpio].chip;	\
		break

	switch (*gpio_num) {
	CASE_APL_GPIO_N(0,   73,  0x500, NORTH);
	CASE_APL_GPIO_N(74,  103, 0x610, NORTHWEST);
	CASE_APL_GPIO_N(104, 123, 0x6e8, NORTHWEST);
	CASE_APL_GPIO_N(124, 155, 0x500, WEST);
	CASE_APL_GPIO_N(156, 172, 0x520, SOUTHWEST);
	CASE_APL_GPIO_N(173, 178, 0x5b0, SOUTHWEST);
	CASE_APL_GPIO_N(179, 179, 0x5a8, SOUTHWEST);
	/* 180 & 181 do not exist.*/
	CASE_APL_GPIO_N(182, 183, 0x5e8, SOUTHWEST);
	/* 184 & 185 do not exist.*/
	CASE_APL_GPIO_N(186, 186, 0x5e0, SOUTHWEST);
	CASE_APL_GPIO_N(187, 204, 0x500, NORTHWEST);
	CASE_APL_GPIO_N(205, 208, 0x500, SOUTHWEST);
	CASE_APL_GPIO_N(209, 212, 0x5d0, WEST);
	CASE_APL_GPIO_N(213, 215, 0x5d0, NORTHWEST);
	default:
		break;
	}
	return chip;
}

#define __PINCTRL_APL_CHIP(gpio, rval)	\
	({	\
		struct gpio_chip *chip =	\
			pinctrl_apl_community_gpio_chip(&(gpio)); \
		if (!chip)	\
			return rval;	\
		chip;		\
	})

#define PINCTRL_APL_CHIP_INT(gpio)	__PINCTRL_APL_CHIP(gpio, -EINVAL)
#define PINCTRL_APL_CHIP_VOID(gpio)	__PINCTRL_APL_CHIP(gpio, )

static int pinctrl_apl_gpio_get(struct gpio_chip *gc, unsigned int gpio_num)
{
	gc = PINCTRL_APL_CHIP_INT(gpio_num);
	return gc->get(gc, gpio_num);
}

static void pinctrl_apl_gpio_set(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	gc = PINCTRL_APL_CHIP_VOID(gpio_num);
	gc->set(gc, gpio_num, val);
}

static int pinctrl_apl_gpio_direction_output(struct gpio_chip *gc,
				unsigned int gpio_num, int val)
{
	gc = PINCTRL_APL_CHIP_INT(gpio_num);
	return gc->direction_output(gc, gpio_num, val);
}

static int pinctrl_apl_gpio_direction_input(struct gpio_chip *gc,
				unsigned int gpio_num)
{
	gc = PINCTRL_APL_CHIP_INT(gpio_num);
	return gc->direction_input(gc, gpio_num);
}

static struct gpio_chip pinctrl_apl_gpio_chip = {
	.label		= "gpio_apl",
	.owner		= THIS_MODULE,
	.get		= pinctrl_apl_gpio_get,
	.set		= pinctrl_apl_gpio_set,
	.direction_output	= pinctrl_apl_gpio_direction_output,
	.direction_input	= pinctrl_apl_gpio_direction_input,
	.base		= -1,
	/* INT3452:0x add up to 245 ports and CASE_APL_GPIO_N counts
	 * 214 ports. We, however, enables only the first 100 ports
	 * for saving available GPIO port number.
	 */
	.ngpio		= 100,
};

static int gpio_match(struct gpio_chip *chip, void *data)
{
	return !strcmp(chip->label, (const char *)data);
}

static int pinctrl_apl_gpio_init(void *d)
{
	int i;

	for (i = 0; pinctrl_apl_gpio[i].label; i++) {
		if (!(pinctrl_apl_gpio[i].chip =
			gpiochip_find((void *)pinctrl_apl_gpio[i].label,
					gpio_match))) {
			pr_err("%s: GPIO chip %s not found\n", __func__,
					pinctrl_apl_gpio[i].label);
			return -ENODEV;
		}
	}
	pr_info("%s: Adding Apollo Lake pinctrl psuedo GPIO %s.\n",
		__func__, pinctrl_apl_gpio_chip.label);
	return gpiochip_add(&pinctrl_apl_gpio_chip);
}

static void pinctrl_apl_gpio_exit(void *d)
{
}

static const struct {
	const char *compatible;
	int (*init)(void *);
	void (*exit)(void *);
	void *data;
} rn_gpio_list[] = {
	{ "rnx220",  rnx220pic_gpio_init,    rnx220pic_gpio_exit, },
	{ "rnx220",  rnx220marvell_gpio_init,rnx220marvell_gpio_exit, },
	{ "rrx312",  pca9505_gpio_init,	     pca9505_gpio_exit, },
	{ "rr2304",  pinctrl_apl_gpio_init,  pinctrl_apl_gpio_exit, },
	{}
};

void __readynas_gpio_exit(int idx)
{
	while (--idx >= 0)
		if (readynas_io_compatible(rn_gpio_list[idx].compatible) &&
						rn_gpio_list[idx].exit)
			rn_gpio_list[idx].exit(rn_gpio_list[idx].data);
}

void readynas_gpio_exit(void)
{
	int i;

	for (i = 0; rn_gpio_list[i].compatible; i++)
		;
	__readynas_gpio_exit(i);
}

int __init readynas_gpio_init(void)
{
	int i;

	for (i = 0; rn_gpio_list[i].compatible; i++) {
		int err;

		if (readynas_io_compatible(rn_gpio_list[i].compatible) &&
			(err = rn_gpio_list[i].init(rn_gpio_list[i].data))) {
			__readynas_gpio_exit(i);
			return err;
		}
	}
	return 0;
}
