/*
 * Intel ICH6-10, Series 5 and 6, Atom C2000 (Avoton/Rangeley) GPIO driver
 *
 * Copyright (C) 2010 Extreme Engineering Solutions.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/mfd/lpc_ich.h>

#define DRV_NAME "gpio_ich"

/*
 * GPIO register offsets in GPIO I/O space.
 * Each chunk of 32 GPIOs is manipulated via its own USE_SELx, IO_SELx, and
 * LVLx registers.  Logic in the read/write functions takes a register and
 * an absolute bit number and determines the proper register offset and bit
 * number in that register.  For example, to read the value of GPIO bit 50
 * the code would access offset ichx_regs[2(=GPIO_LVL)][1(=50/32)],
 * bit 18 (50%32).
 */
enum GPIO_REG {
	GPIO_USE_SEL = 0,
	GPIO_IO_SEL,
	GPIO_LVL,
	GPO_BLINK,

	GPI_INV,
	GPI_IRQ_EN,
	GPI_IRQ_STS,

	GPIO_REGS
};

static const u8 ich9_regs[GPIO_REGS][3] = {
	{0x00, 0x30, 0x40},
	{0x04, 0x34, 0x44},
	{0x0c, 0x38, 0x48},
	{0x18, 0xff, 0xff},

	{0x2c, 0xff, 0xff},
	{0x28, 0x2c, 0x2c},
	{0x20, 0x24, 0x24},
};

static const u8 ich9_reglen[3] = {
	0x30, 0x10, 0x10,
};
static const u8 ichx_regs[GPIO_REGS][3] = {
	{0x00, 0x30, 0xff},	/* USE_SEL[1-3] offsets */
	{0x04, 0x34, 0xff},	/* IO_SEL[1-3] offsets */
	{0x0c, 0x38, 0xff},	/* LVL[1-3] offsets */
	{0x18, 0xff, 0xff},	/* BLINK offsets */

	{0x2c, 0xff, 0xff},	/* INV offsets */
	{0x28, 0xff, 0xff},	/* GPE0_EN offsets */
	{0x20, 0xff, 0xff},	/* GPE0_STS offsets */
};

static const u8 ichx_reglen[3] = {
	0x30, 0x10, 0x10,
};

static const u8 avoton_regs[GPIO_REGS][3] = {
	{0x00, 0x80, 0x00},
	{0x04, 0x84, 0x00},
	{0x08, 0x88, 0x00},
};

static const u8 avoton_reglen[3] = {
	0x10, 0x10, 0x00,
};

static const u8 lpt_regs[GPIO_REGS][3] = {
	{0x00, 0x30, 0x40},
	{0x04, 0x34, 0x44},
	{0x0c, 0x38, 0x48},
	{0x18, 0xff, 0xff},

	{0x2c, 0xff, 0xff},
	{0x28, 0xff, 0x50},
	{0x20, 0x3e, 0x54},
};

static const u8 lpt_reglen[3] = {
	0x30, 0x10, 0x10,
};

#define ICHX_PM1_CNT_REG	0x4
#define ICHX_PM1_CNT_SCI_EN	0x1

#define ICHX_WRITE(val, reg, base_res)	outl(val, (reg) + (base_res)->start)
#define ICHX_READ(reg, base_res)	inl((reg) + (base_res)->start)

struct ichx_desc {
	/* Max GPIO pins the chipset can have */
	uint ngpio;

	/* chipset registers */
	const u8 (*regs)[3];
	const u8 *reglen;

	/* GPO_BLINK is available on this chipset */
	bool have_blink;

	/* Whether the chipset has GPIO in GPE0_STS in the PM IO region */
	bool uses_gpe0;

	/* USE_SEL is bogus on some chipsets, eg 3100 */
	u32 use_sel_ignore[3];

	/* Some chipsets have quirks, let these use their own request/get */
	int (*request)(struct gpio_chip *chip, unsigned offset);
	int (*get)(struct gpio_chip *chip, unsigned offset);

	/*
	 * Some chipsets don't let reading output values on GPIO_LVL register
	 * this option allows driver caching written output values
	 */
	bool use_outlvl_cache;

	/* Nmber of GPIOs (from 0) capable of IRQ generation. */
	u8 ngpi_irq;
};

static struct ichx_priv {
	spinlock_t lock;
	struct platform_device *dev;
	struct gpio_chip chip;
	struct resource *gpio_base;	/* GPIO IO base */
	struct resource *pm_base;	/* Power Mangagment IO base */
	struct ichx_desc *desc;	/* Pointer to chipset-specific description */
	u32 orig_gpio_ctrl;	/* Orig CTRL value, used to restore on exit */
	u8 use_gpio;		/* Which GPIO groups are usable */
	int outlvl_cache[3];	/* cached output values */
	struct pci_dev *pci_dev;	/* PCI device holder from lpt_ich */
	u8 hwirq;
} ichx_priv;

static int modparam_gpiobase = -1;	/* dynamic */
module_param_named(gpiobase, modparam_gpiobase, int, 0444);
MODULE_PARM_DESC(gpiobase, "The GPIO number base. -1 means dynamic, "
			   "which is the default.");

static int ichx_write_bit(int reg, unsigned nr, int val, int verify)
{
	unsigned long flags;
	u32 data, tmp;
	int reg_nr = nr / 32;
	int bit = nr & 0x1f;
	int ret = 0;

	spin_lock_irqsave(&ichx_priv.lock, flags);

	if (reg == GPIO_LVL && ichx_priv.desc->use_outlvl_cache)
		data = ichx_priv.outlvl_cache[reg_nr];
	else
		data = ICHX_READ(ichx_priv.desc->regs[reg][reg_nr],
				 ichx_priv.gpio_base);

	if (val)
		data |= 1 << bit;
	else
		data &= ~(1 << bit);
	ICHX_WRITE(data, ichx_priv.desc->regs[reg][reg_nr],
			 ichx_priv.gpio_base);
	if (reg == GPIO_LVL && ichx_priv.desc->use_outlvl_cache)
		ichx_priv.outlvl_cache[reg_nr] = data;

	tmp = ICHX_READ(ichx_priv.desc->regs[reg][reg_nr],
			ichx_priv.gpio_base);
	if (verify && data != tmp)
		ret = -EPERM;

	spin_unlock_irqrestore(&ichx_priv.lock, flags);

	return ret;
}

static int ichx_read_bit(int reg, unsigned nr)
{
	unsigned long flags;
	u32 data;
	int reg_nr = nr / 32;
	int bit = nr & 0x1f;

	spin_lock_irqsave(&ichx_priv.lock, flags);

	data = ICHX_READ(ichx_priv.desc->regs[reg][reg_nr],
			 ichx_priv.gpio_base);

	if (reg == GPIO_LVL && ichx_priv.desc->use_outlvl_cache)
		data = ichx_priv.outlvl_cache[reg_nr] | data;

	spin_unlock_irqrestore(&ichx_priv.lock, flags);

	return data & (1 << bit) ? 1 : 0;
}

static bool ichx_gpio_check_available(struct gpio_chip *gpio, unsigned nr)
{
	return !!(ichx_priv.use_gpio & (1 << (nr / 32)));
}

static int ichx_gpio_get_direction(struct gpio_chip *gpio, unsigned nr)
{
	return ichx_read_bit(GPIO_IO_SEL, nr) ? GPIOF_DIR_IN : GPIOF_DIR_OUT;
}

static int ichx_gpio_direction_input(struct gpio_chip *gpio, unsigned nr)
{
	/*
	 * Try setting pin as an input and verify it worked since many pins
	 * are output-only.
	 */
	if (ichx_write_bit(GPIO_IO_SEL, nr, 1, 1))
		return -EINVAL;

	return 0;
}

static int ichx_gpio_direction_output(struct gpio_chip *gpio, unsigned nr,
					int val)
{
	/* Disable blink hardware which is available for GPIOs from 0 to 31. */
	if (nr < 32 && ichx_priv.desc->have_blink)
		ichx_write_bit(GPO_BLINK, nr, 0, 0);

	/* Set GPIO output value. */
	ichx_write_bit(GPIO_LVL, nr, val, 0);

	/*
	 * Try setting pin as an output and verify it worked since many pins
	 * are input-only.
	 */
	if (ichx_write_bit(GPIO_IO_SEL, nr, 0, 1))
		return -EINVAL;

	return 0;
}

static int ichx_gpio_get(struct gpio_chip *chip, unsigned nr)
{
	return ichx_read_bit(GPIO_LVL, nr);
}

static int ich6_gpio_get(struct gpio_chip *chip, unsigned nr)
{
	unsigned long flags;
	u32 data;

	/*
	 * GPI 0 - 15 need to be read from the power management registers on
	 * a ICH6/3100 bridge.
	 */
	if (nr < 16) {
		if (!ichx_priv.pm_base)
			return -ENXIO;

		spin_lock_irqsave(&ichx_priv.lock, flags);

		/* GPI 0 - 15 are latched, write 1 to clear*/
		ICHX_WRITE(1 << (16 + nr), 0, ichx_priv.pm_base);
		data = ICHX_READ(0, ichx_priv.pm_base);

		spin_unlock_irqrestore(&ichx_priv.lock, flags);

		return (data >> 16) & (1 << nr) ? 1 : 0;
	} else {
		return ichx_gpio_get(chip, nr);
	}
}

static int ichx_gpio_request(struct gpio_chip *chip, unsigned nr)
{
	if (!ichx_gpio_check_available(chip, nr))
		return -ENXIO;

	/*
	 * Note we assume the BIOS properly set a bridge's USE value.  Some
	 * chips (eg Intel 3100) have bogus USE values though, so first see if
	 * the chipset's USE value can be trusted for this specific bit.
	 * If it can't be trusted, assume that the pin can be used as a GPIO.
	 */
	if (ichx_priv.desc->use_sel_ignore[nr / 32] & (1 << (nr & 0x1f)))
		return 0;

	return ichx_read_bit(GPIO_USE_SEL, nr) ? 0 : -ENODEV;
}

static int ich6_gpio_request(struct gpio_chip *chip, unsigned nr)
{
	/*
	 * Fixups for bits 16 and 17 are necessary on the Intel ICH6/3100
	 * bridge as they are controlled by USE register bits 0 and 1.  See
	 * "Table 704 GPIO_USE_SEL1 register" in the i3100 datasheet for
	 * additional info.
	 */
	if (nr == 16 || nr == 17)
		nr -= 16;

	return ichx_gpio_request(chip, nr);
}

static void ichx_gpio_set(struct gpio_chip *chip, unsigned nr, int val)
{
	ichx_write_bit(GPIO_LVL, nr, val, 0);
}

static void ichx_gpiolib_setup(struct gpio_chip *chip)
{
	chip->owner = THIS_MODULE;
	chip->label = DRV_NAME;
	chip->dev = &ichx_priv.dev->dev;

	/* Allow chip-specific overrides of request()/get() */
	chip->request = ichx_priv.desc->request ?
		ichx_priv.desc->request : ichx_gpio_request;
	chip->get = ichx_priv.desc->get ?
		ichx_priv.desc->get : ichx_gpio_get;

	chip->set = ichx_gpio_set;
	chip->get_direction = ichx_gpio_get_direction;
	chip->direction_input = ichx_gpio_direction_input;
	chip->direction_output = ichx_gpio_direction_output;
	chip->base = modparam_gpiobase;
	chip->ngpio = ichx_priv.desc->ngpio;
	chip->can_sleep = false;
	chip->dbg_show = NULL;
}

/* ICH6-based, 631xesb-based */
static struct ichx_desc ich6_desc = {
	/* Bridges using the ICH6 controller need fixups for GPIO 0 - 17 */
	.request = ich6_gpio_request,
	.get = ich6_gpio_get,

	/* GPIO 0-15 are read in the GPE0_STS PM register */
	.uses_gpe0 = true,

	.ngpio = 50,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
};

/* Intel 3100 */
static struct ichx_desc i3100_desc = {
	/*
	 * Bits 16,17, 20 of USE_SEL and bit 16 of USE_SEL2 always read 0 on
	 * the Intel 3100.  See "Table 712. GPIO Summary Table" of 3100
	 * Datasheet for more info.
	 */
	.use_sel_ignore = {0x00130000, 0x00010000, 0x0},

	/* The 3100 needs fixups for GPIO 0 - 17 */
	.request = ich6_gpio_request,
	.get = ich6_gpio_get,

	/* GPIO 0-15 are read in the GPE0_STS PM register */
	.uses_gpe0 = true,

	.ngpio = 50,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
};

/* ICH7 and ICH8-based */
static struct ichx_desc ich7_desc = {
	.ngpio = 50,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
};

/* ICH9-based */
static struct ichx_desc ich9_desc = {
	.ngpio = 61,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
	.ngpi_irq = 61,
};

/* ICH10-based - Consumer/corporate versions have different amount of GPIO */
static struct ichx_desc ich10_cons_desc = {
	.ngpio = 61,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
	.ngpi_irq = 16,
};
static struct ichx_desc ich10_corp_desc = {
	.ngpio = 72,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
};

/* Intel 5 series, 6 series, 3400 series, and C200 series */
static struct ichx_desc intel5_desc = {
	.ngpio = 76,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
	.ngpi_irq = 16,
};

/* Avoton */
static struct ichx_desc avoton_desc = {
	/* Avoton has only 59 GPIOs, but we assume the first set of register
	 * (Core) has 32 instead of 31 to keep gpio-ich compliance
	 */
	.ngpio = 60,
	.regs = avoton_regs,
	.reglen = avoton_reglen,
	.use_outlvl_cache = true,
};

/* Lynx Point */
static struct ichx_desc lpt_desc = {
	.ngpio = 76,
	.regs = lpt_regs,
	.reglen = lpt_reglen,
	.use_outlvl_cache = true,
	.ngpi_irq = 61,
};

#include <linux/interrupt.h>
static struct gpi_irq {
	u8	gpi_num;
	u8	_gpi_bank;
	u8	_rout_reg;
	u8	_rout_shift;
	u8	_gpe0_shift;
	u8	_irq_type;
	u32	_gpi_bit;
	char *name;
} ichx_lpt_gpi_irq[64];

static struct resource *ichx_lpt_pm_base;

#define gpi_valid(i)	((i) < ARRAY_SIZE(ichx_lpt_gpi_irq) &&	\
				ichx_lpt_gpi_irq[(i)].gpi_num == (i))
#define rout_reg(i)	ichx_lpt_gpi_irq[(i)]._rout_reg
#define rout_shift(i)	ichx_lpt_gpi_irq[(i)]._rout_shift
#define rout_bit(i)	(0x3 << rout_shift(i))
#define gpi_bank(i)	ichx_lpt_gpi_irq[(i)]._gpi_bank
#define gpi_bit(i)	ichx_lpt_gpi_irq[(i)]._gpi_bit
#define gpe0_shift(i)	ichx_lpt_gpi_irq[(i)]._gpe0_shift
#define gpe0_modbit(i)	(0x1UL << (gpe0_shift(i) % 32))
#define gpe0_reg_offset(i)	((gpe0_shift(i) >= 32) ? 0x4 : 0)
#define	irq_type(i)	ichx_lpt_gpi_irq[(i)]._irq_type

#define ICH_GPIO_IRQ_TYPE_NONE	0
#define ICH_GPIO_IRQ_TYPE_SMI	1
#define ICH_GPIO_IRQ_TYPE_SCI	2
#define ICH_GPIO_IRQ_TYPE_NMI	3

#define ICH_GPIO_IRQ_POLARITY_POS	0
#define ICH_GPIO_IRQ_POLARITY_NEG	1
#define ICH_GPIO_IRQ_POLARITY_INV	2

static inline struct ichx_priv *irq_data_to_ichx_priv(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	return container_of(gc, struct ichx_priv, chip);
}

static void ichx_gpio_irq_polarity(struct irq_data *d, unsigned int gpi,
					unsigned int polarity)
{
	unsigned long flags;
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ichx_priv *priv = irq_data_to_ichx_priv(d);
	u32 inv;
	u32 inv_reg = priv->desc->regs[GPI_INV][gpi_bank(gpi)];

	if (ichx_gpio_get_direction(gc, gpi) != GPIOF_DIR_IN)
		return;

	spin_lock_irqsave(&priv->lock, flags);
	inv = ICHX_READ(inv_reg, priv->gpio_base);

	switch (polarity) {
	case ICH_GPIO_IRQ_POLARITY_POS:
		inv &= ~gpi_bit(gpi);
		break;
	case ICH_GPIO_IRQ_POLARITY_NEG:
		inv |= gpi_bit(gpi);
		break;
	case ICH_GPIO_IRQ_POLARITY_INV:
		inv ^= gpi_bit(gpi);
		break;
	}
	ICHX_WRITE(inv, inv_reg, priv->gpio_base);

	spin_unlock_irqrestore(&priv->lock, flags);
}

static int ichx_gpio_irq_setup(struct irq_data *d, unsigned int gpi,
				unsigned int type)
{
	unsigned long flags;
	u32 rout, gpe0_en;
	struct ichx_priv *priv = irq_data_to_ichx_priv(d);
	u32 gpe0_en_reg = priv->desc->regs[GPI_IRQ_EN][gpi_bank(gpi)];

	if (gpe0_en_reg == 0xff)
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	gpe0_en = ICHX_READ(gpe0_en_reg, ichx_lpt_pm_base);

	pci_read_config_dword(priv->pci_dev, rout_reg(gpi), &rout);
	rout &= ~rout_bit(gpi);

	/* Set TYPE */
	rout |= (type << rout_shift(gpi));
	pci_write_config_dword(priv->pci_dev, rout_reg(gpi), rout);

	if (type == ICH_GPIO_IRQ_TYPE_NONE)
		gpe0_en &= ~gpe0_modbit(gpi);
	else
		gpe0_en |= gpe0_modbit(gpi);

	ICHX_WRITE(gpe0_en, gpe0_en_reg, ichx_lpt_pm_base);
	spin_unlock_irqrestore(&priv->lock, flags);
	return 0;
}

static int ichx_gpio_irq_chk_ack(struct ichx_priv *priv,
				unsigned int gpi, bool ack)
{
	int err = 0;
	int gpe0_sts_reg = priv->desc->regs[GPI_IRQ_STS][gpi_bank(gpi)];
	u32 gpe0_sts;
	unsigned long flags;

	if (gpe0_sts_reg == 0xff)
		return -ENODEV;

	spin_lock_irqsave(&priv->lock, flags);
	gpe0_sts = ICHX_READ(gpe0_sts_reg, ichx_lpt_pm_base);

	if (!(gpe0_sts & gpe0_modbit(gpi)))
		err = -ENODEV;
	if (ack)
		ICHX_WRITE(gpe0_modbit(gpi), gpe0_sts_reg, ichx_lpt_pm_base);
	spin_unlock_irqrestore(&priv->lock, flags);
	return err;
}

static void ichx_gpio_irq_ack(struct irq_data *d)
{
	u32 gpi = irqd_to_hwirq(d);

	if (irq_type(gpi) == IRQ_TYPE_EDGE_BOTH)
		ichx_gpio_irq_polarity(d, gpi, ICH_GPIO_IRQ_POLARITY_INV);
	ichx_gpio_irq_chk_ack(irq_data_to_ichx_priv(d), gpi, true);
}

static irqreturn_t ichx_gpio_irq_handler(int hwirq, void *data)
{
	struct ichx_priv *priv = data;
	int gpi;

	for (gpi = 0; gpi < priv->desc->ngpi_irq; gpi++)
		if (irq_type(gpi) != IRQ_TYPE_NONE &&
				!ichx_gpio_irq_chk_ack(priv, gpi, false)) {
			unsigned int virq =
				irq_find_mapping(priv->chip.irqdomain, gpi);

			generic_handle_irq(virq);
		}

	return IRQ_HANDLED;
}

static void ichx_gpio_irq_unmask(struct irq_data *d) { }

static void ichx_gpio_irq_mask(struct irq_data *d) { }

static int ichx_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	u32 gpi = irqd_to_hwirq(d);
	int err = IRQ_SET_MASK_OK;

	if (!gpi_valid(gpi))
		return -EINVAL;

	if (type == IRQ_TYPE_NONE) {
		irq_type(gpi) = type;
		return ichx_gpio_irq_setup(d, gpi, 0);
	}

	switch (type) {
	case IRQ_TYPE_LEVEL_HIGH:
		ichx_gpio_irq_polarity(d, gpi, ICH_GPIO_IRQ_POLARITY_POS);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		ichx_gpio_irq_polarity(d, gpi, ICH_GPIO_IRQ_POLARITY_NEG);
		break;
	case IRQ_TYPE_EDGE_BOTH:
		ichx_gpio_irq_polarity(d, gpi, ICH_GPIO_IRQ_POLARITY_INV);
		break;
	default:
		err = -EINVAL;
		break;
	}

	if (!err)
		err = ichx_gpio_irq_setup(d, gpi, ICH_GPIO_IRQ_TYPE_SCI);
	if (!err)
		irq_type(gpi) = type;
	return err;
}


static struct irq_chip ichx_irqchip = {
	.name 		= DRV_NAME,
	.irq_mask	= ichx_gpio_irq_mask,
	.irq_unmask	= ichx_gpio_irq_unmask,
	.irq_set_type	= ichx_gpio_irq_set_type,
	.irq_ack	= ichx_gpio_irq_ack,
};

static void lpt_irq_exit(struct ichx_priv *priv)
{
	if (ichx_lpt_pm_base != priv->pm_base)
		kfree(ichx_lpt_pm_base);
	ichx_lpt_pm_base = NULL;
}

#define	PCI_ICH_PMBASE_OFFSET		0x40

static int lpt_irq_init(struct ichx_priv *priv)
{
	int gpi;
	u32 d;

	if (!(ichx_lpt_pm_base = priv->pm_base)) {
		u32 aba;
		int err = pci_read_config_dword(priv->pci_dev,
						PCI_ICH_PMBASE_OFFSET,
						&aba);
		if (err)
			return err;

		if (!(ichx_lpt_pm_base =
				kzalloc(sizeof *ichx_lpt_pm_base, GFP_KERNEL)))
			return -ENOMEM;

		ichx_lpt_pm_base->start = aba & 0xff80;
		ichx_lpt_pm_base->end = ichx_lpt_pm_base->start + 0x7f;
		ichx_lpt_pm_base->name = "ich_pmbase";
	}

	for (gpi = 0; gpi < priv->desc->ngpi_irq; gpi++) {
		ichx_lpt_gpi_irq[gpi].gpi_num = gpi;
		irq_type(gpi) = IRQ_TYPE_NONE;
		rout_reg(gpi) = 0xbc;	/* Assume GPI_ROUT2 */

		switch (gpi) {
		case 0 ... 15:
			rout_reg(gpi)	= 0xb8;	/* GPI_ROUT */
			gpi_bank(gpi)	= 0;
			break;
		case 17 ... 22:
			gpi_bank(gpi)	= 1;
			break;
		case 43 ... 60:
			gpi_bank(gpi)	= 2;
			break;
		default:
			gpi_bank(gpi)	= 0xff;
			break;
		}
		gpi_bit(gpi)	= 0x1 << (gpi - gpi_bank(gpi) * 16);

		switch (gpi) {
		case 0 ... 15:
			gpe0_shift(gpi)	= gpi + 16;
			rout_shift(gpi)	= gpi * 2;
			break;
		case 17:
			gpe0_shift(gpi)	= 56;
			rout_shift(gpi)	= 0;
			break;
		case 19:
			gpe0_shift(gpi)	= 57;
			rout_shift(gpi)	= 2;
			break;
		case 21:
			gpe0_shift(gpi)	= 58;
			rout_shift(gpi)	= 4;
			break;
		case 22:
			gpe0_shift(gpi)	= 59;
			rout_shift(gpi)	= 6;
			break;
		case 43:
			gpe0_shift(gpi)	= 60;
			rout_shift(gpi)	= 8;
			break;
		case 56:
			gpe0_shift(gpi)	= 61;
			rout_shift(gpi)	= 10;
			break;
		case 57:
			gpe0_shift(gpi)	= 62;
			rout_shift(gpi)	= 12;
			break;
		case 60:
			gpe0_shift(gpi)	= 63;
			rout_shift(gpi)	= 14;
			break;
		default:
			ichx_lpt_gpi_irq[gpi].gpi_num = 0xff;
			break;
		}
	}

	while (gpi < ARRAY_SIZE(ichx_lpt_gpi_irq))
		ichx_lpt_gpi_irq[gpi++].gpi_num = 0xff;

	d = ICHX_READ(ICHX_PM1_CNT_REG, ichx_lpt_pm_base);

	ICHX_WRITE(d | ICHX_PM1_CNT_SCI_EN, ICHX_PM1_CNT_REG, ichx_lpt_pm_base);
	return 0;
}

static int ichx_gpio_request_regions(struct resource *res_base,
						const char *name, u8 use_gpio)
{
	int i;

	if (!res_base || !res_base->start || !res_base->end)
		return -ENODEV;

	for (i = 0; i < ARRAY_SIZE(ichx_priv.desc->regs[0]); i++) {
		if (!(use_gpio & (1 << i)))
			continue;
		if (!request_region(
				res_base->start + ichx_priv.desc->regs[0][i],
				ichx_priv.desc->reglen[i], name))
			goto request_err;
	}
	return 0;

request_err:
	/* Clean up: release already requested regions, if any */
	for (i--; i >= 0; i--) {
		if (!(use_gpio & (1 << i)))
			continue;
		release_region(res_base->start + ichx_priv.desc->regs[0][i],
			       ichx_priv.desc->reglen[i]);
	}
	return -EBUSY;
}

static void ichx_gpio_release_regions(struct resource *res_base, u8 use_gpio)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ichx_priv.desc->regs[0]); i++) {
		if (!(use_gpio & (1 << i)))
			continue;
		release_region(res_base->start + ichx_priv.desc->regs[0][i],
			       ichx_priv.desc->reglen[i]);
	}
}

static int ichx_gpio_probe(struct platform_device *pdev)
{
	struct resource *res_base, *res_pm;
	int err, i;
	struct lpc_ich_info *ich_info = dev_get_platdata(&pdev->dev);

	if (!ich_info)
		return -ENODEV;

	ichx_priv.dev = pdev;

	switch (ich_info->gpio_version) {
	case ICH_I3100_GPIO:
		ichx_priv.desc = &i3100_desc;
		break;
	case ICH_V5_GPIO:
		ichx_priv.desc = &intel5_desc;
		break;
	case ICH_V6_GPIO:
		ichx_priv.desc = &ich6_desc;
		break;
	case ICH_V7_GPIO:
		ichx_priv.desc = &ich7_desc;
		break;
	case ICH_V9_GPIO:
		ichx_priv.desc = &ich9_desc;
		break;
	case ICH_V10CORP_GPIO:
		ichx_priv.desc = &ich10_corp_desc;
		break;
	case ICH_V10CONS_GPIO:
		ichx_priv.desc = &ich10_cons_desc;
		break;
	case AVOTON_GPIO:
		ichx_priv.desc = &avoton_desc;
		break;
	case LPT_GPIO:
		ichx_priv.desc = &lpt_desc;
		break;

	default:
		return -ENODEV;
	}

	spin_lock_init(&ichx_priv.lock);
	res_base = platform_get_resource(pdev, IORESOURCE_IO, ICH_RES_GPIO);
	ichx_priv.use_gpio = ich_info->use_gpio;
	ichx_priv.pci_dev = ich_info->pci_dev;
	err = ichx_gpio_request_regions(res_base, pdev->name,
					ichx_priv.use_gpio);
	if (err)
		return err;

	ichx_priv.gpio_base = res_base;

	if (ichx_priv.desc->ngpi_irq && (err = lpt_irq_init(&ichx_priv))) {
		pr_err("%s: failed to initialize IRQ for %s (%d).\n",
			__func__, ich_info->name, err);
		return err;
	}
	/*
	 * If necessary, determine the I/O address of ACPI/power management
	 * registers which are needed to read the the GPE0 register for GPI pins
	 * 0 - 15 on some chipsets.
	 */
	if (!ichx_priv.desc->uses_gpe0)
		goto init;

	res_pm = platform_get_resource(pdev, IORESOURCE_IO, ICH_RES_GPE0);
	if (!res_pm) {
		pr_warn("ACPI BAR is unavailable, GPI 0 - 15 unavailable\n");
		goto init;
	}

	if (!request_region(res_pm->start, resource_size(res_pm),
			pdev->name)) {
		pr_warn("ACPI BAR is busy, GPI 0 - 15 unavailable\n");

		goto init;
	}

	ichx_priv.pm_base = res_pm;

init:
	ichx_gpiolib_setup(&ichx_priv.chip);
	err = gpiochip_add(&ichx_priv.chip);
	if (err) {
		pr_err("Failed to register GPIOs\n");
		goto add_err;
	}

	if (ichx_priv.desc->ngpi_irq) {
		ichx_priv.hwirq = 9;	/* Same as ACPI */

		if ((err = gpiochip_irqchip_add(&ichx_priv.chip,
						&ichx_irqchip, 0,
						handle_simple_irq,
						IRQ_TYPE_NONE))) {
			pr_err("%s: failed to add irqchip %s (%d)\n",
					__func__, ichx_irqchip.name, err);
			gpiochip_remove(&ichx_priv.chip);
			goto add_err;
		}

		if ((err = request_irq(ichx_priv.hwirq,
					ichx_gpio_irq_handler,
					IRQF_SHARED,
					ichx_irqchip.name,
					&ichx_priv))) {
			pr_err("%s: failed to request irq %d (%d)\n",
					__func__, ichx_priv.hwirq, err);
			gpiochip_remove(&ichx_priv.chip);
			goto add_err;
		}
	}

	pr_info("GPIO from %d to %d on %s\n", ichx_priv.chip.base,
	       ichx_priv.chip.base + ichx_priv.chip.ngpio - 1,
	       ichx_priv.chip.label);

	for (i = 0; i < ARRAY_SIZE(ichx_priv.desc->regs[0]); i++) {
		ichx_priv.outlvl_cache[i] =
			ICHX_READ(ichx_priv.desc->regs[GPIO_LVL][i],
					ichx_priv.gpio_base);
	}

	return 0;

add_err:
	ichx_gpio_release_regions(ichx_priv.gpio_base, ichx_priv.use_gpio);
	if (ichx_priv.pm_base)
		release_region(ichx_priv.pm_base->start,
				resource_size(ichx_priv.pm_base));
	return err;
}

static int ichx_gpio_remove(struct platform_device *pdev)
{
	struct lpc_ich_info *ich_info = dev_get_platdata(&pdev->dev);

	if (ich_info->gpio_version == LPT_GPIO) {
		free_irq(ichx_priv.hwirq, &ichx_priv);
		lpt_irq_exit(&ichx_priv);
	}

	gpiochip_remove(&ichx_priv.chip);

	ichx_gpio_release_regions(ichx_priv.gpio_base, ichx_priv.use_gpio);
	if (ichx_priv.pm_base)
		release_region(ichx_priv.pm_base->start,
				resource_size(ichx_priv.pm_base));

	return 0;
}

static struct platform_driver ichx_gpio_driver = {
	.driver		= {
		.name	= DRV_NAME,
	},
	.probe		= ichx_gpio_probe,
	.remove		= ichx_gpio_remove,
};

module_platform_driver(ichx_gpio_driver);

MODULE_AUTHOR("Peter Tyser <ptyser@xes-inc.com>");
MODULE_DESCRIPTION("GPIO interface for Intel ICH series");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:"DRV_NAME);
