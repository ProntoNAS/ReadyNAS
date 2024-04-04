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

static const u8 ichx_regs[GPIO_REGS][3] = {
	{0x00, 0x30, 0x40},	/* USE_SEL[1-3] offsets */
	{0x04, 0x34, 0x44},	/* IO_SEL[1-3] offsets */
	{0x0c, 0x38, 0x48},	/* LVL[1-3] offsets */
	{0x18, 0x18, 0x18},	/* BLINK offset */
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

/* libolibo Add for RN526/626 */
static const u8 lpt_regs[GPIO_REGS][3] = {
	{0x00, 0x30, 0x40},
	{0x04, 0x34, 0x44},
	{0x0c, 0x38, 0x48},
	{0x18, 0x18, 0x18},

	{0x2c, 0x2c, 0x2c},
	{0x28, 0x3c, 0x50},
	{0x2a, 0x3e, 0x54},
};

static const u8 lpt_reglen[3] = {
	0x30, 0x10, 0x20,
};


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
static unsigned long n;
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
};

/* ICH10-based - Consumer/corporate versions have different amount of GPIO */
static struct ichx_desc ich10_cons_desc = {
	.ngpio = 61,
	.have_blink = true,
	.regs = ichx_regs,
	.reglen = ichx_reglen,
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

/* LPT */
static struct ichx_desc lpt_desc = {
	.ngpio = 76,
	.regs = lpt_regs,
	.reglen = lpt_reglen,
	.use_outlvl_cache = true,
};

#include <linux/interrupt.h>

#define	PCI_ICHX_LPT_PMBASE_OFFSET	0x40
#define	PCI_DEVICE_ID_LPT_BRIDGE	0x8c54

static struct gpi_irq {
	u8	gpi_num;
	u8	_gpi_bank;
	u8	_rout_reg;
	u8	_rout_shift;
	u32	_gpi_bit;
	u8	_gpe0_shift;
	char *name;
} ichx_lpt_gpi_irq[64];
static struct pci_dev *ichx_lpt_pci_dev;
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

static struct ichx_gpi_irq_cb {
	irqreturn_t (*handler)(int, void *);
	struct	gpi_irq *gpi;
	void	*data;		/* Unused. */
} ichx_gpi_irq_cb[16];

static irqreturn_t ichx_gpi_interrupt(int irq, void *dev_id)
{
	static spinlock_t gpe0_lock;
	irqreturn_t ret = IRQ_HANDLED;
	struct ichx_gpi_irq_cb *cb = dev_id;
	int gpi_num = cb->gpi->gpi_num;
	int gpe0_sts_reg = 0x20 + gpe0_reg_offset(gpi_num);
	u32 gpe0_sts = ICHX_READ(gpe0_sts_reg, ichx_lpt_pm_base);

	spin_lock(&gpe0_lock);
	if (!(gpe0_sts & gpe0_modbit(gpi_num))) {
		spin_unlock(&gpe0_lock);
		return IRQ_NONE;
	}

	if (ichx_gpi_irq_cb[irq].handler)
		ret = ichx_gpi_irq_cb[irq].handler(irq, cb->data);
	ICHX_WRITE(gpe0_modbit(gpi_num), gpe0_sts_reg, ichx_lpt_pm_base);
	kobject_uevent(&ichx_priv.dev->dev.kobj, KOBJ_CHANGE);
	spin_unlock(&gpe0_lock);
	return ret;
}

static int acpi_irq(void)
{
	int irq;
	struct irq_desc *desc;

	for (irq = 0; irq < nr_irqs; irq++) {
		unsigned long flags;
		struct irqaction *action;

		if (!(desc = irq_to_desc(irq)))
			continue;
		raw_spin_lock_irqsave(&desc->lock, flags);
		for (action = desc->action; action; action = action->next)
			if (action->name && !strcmp(action->name, "acpi"))
				break;
		raw_spin_unlock_irqrestore(&desc->lock, flags);
		if (action) {
			pr_info("%s: acpi irq = %d\n", __func__, irq);
			return irq;
		}
	}
	pr_err("%s: acpi irq not found.\n", __func__);
	return -ENODEV;
}

static int lpt_control(struct gpio_chip *gc, unsigned int cmd, void *arg)
{
	struct ichx_priv *priv = container_of(gc, struct ichx_priv, chip);
	unsigned long flags;
	u32 inv;
	int inv_reg;
	int err = 0;

	if (IS_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd)) {
		/* cmd == IRQ_REGISTER | IRQ#
		   arg == {irqreturn_t(*handler)(int irq, void *dev_id), *arg}
		   Locking and superio_enter/exit is performed
		   out side of handler.
		   Do not call this in IRQ.
		 */
		int gpe0_en_reg;
		u32 rout, gpe0_en;
		int irq = IRQ_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd);
		int typ = TYP_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd);
		int gpi = GPI_ICHX_LPT_CMD_GPIO_IRQ_REGISTER(cmd);

		if (!irq)
			irq = acpi_irq();
		if (irq < 0)
			return irq;

		if (irq >= ARRAY_SIZE(ichx_gpi_irq_cb) ||
			!gpi_valid(gpi)	||
			!ICHX_LPT_TYP_GPIO_IRQ_GOOD(typ)) {
			pr_err("%s: invalid IRQ (%d) or GPI (%d) TYP (%d).\n",
				__func__, irq, gpi, typ);
			return -EINVAL;
		}

		spin_lock_irqsave(&priv->lock, flags);
		gpe0_en_reg = priv->desc->regs[GPI_IRQ_EN][gpi_bank(gpi)] +
					gpe0_reg_offset(gpi);
		gpe0_en = ICHX_READ(gpe0_en_reg, ichx_lpt_pm_base);

		pci_read_config_dword(ichx_lpt_pci_dev, rout_reg(gpi), &rout);
		rout &= ~rout_bit(gpi);

		if (arg) {
			if (!(ichx_lpt_gpi_irq[gpi].name =
						kmalloc(20, GFP_KERNEL))) {
				err = -ENOMEM;
				goto irq_err;
			}
			sprintf(ichx_lpt_gpi_irq[gpi].name,
						"%s:%u", DRV_NAME, gpi);
			if ((err = request_irq(irq,
						ichx_gpi_interrupt,
						IRQF_SHARED,
						ichx_lpt_gpi_irq[gpi].name,
						&ichx_gpi_irq_cb[irq]))) {
				kfree(ichx_lpt_gpi_irq[gpi].name);
				goto irq_err;
			}
			ichx_gpi_irq_cb[irq].handler = arg;
			ichx_gpi_irq_cb[irq].gpi = &ichx_lpt_gpi_irq[gpi];

			/* Set SCI */
			rout |= (typ << rout_shift(gpi));
			pci_write_config_dword(ichx_lpt_pci_dev,
						rout_reg(gpi),
						rout);
			ICHX_WRITE(gpe0_en | gpe0_modbit(gpi),
					gpe0_en_reg, ichx_lpt_pm_base);
		} else {
			/* Clear IRQ */
			pci_write_config_dword(ichx_lpt_pci_dev,
					rout_reg(gpi), rout);
			ICHX_WRITE(gpe0_en & ~gpe0_modbit(gpi),
					gpe0_en_reg, ichx_lpt_pm_base);
			free_irq(irq, &ichx_lpt_gpi_irq[gpi]);
			kfree(ichx_lpt_gpi_irq[gpi].name);
			ichx_lpt_gpi_irq[gpi].name = NULL;
			ichx_gpi_irq_cb[irq].handler = NULL;
			ichx_gpi_irq_cb[irq].gpi = NULL;
		}
irq_err:
		spin_unlock_irqrestore(&priv->lock, flags);
	} else if (IS_ICHX_LPT_POL_GPIO_IRQ(cmd)) {
		int gpi	= GPI_ICHX_LPT_POL_GPIO_IRQ(cmd);

		if (!gpi_valid(gpi))
			return -EINVAL;
		if (ichx_gpio_get_direction(gc, gpi) != GPIOF_DIR_IN)
			return -EPERM;

		spin_lock_irqsave(&priv->lock, flags);

		inv_reg	= priv->desc->regs[GPI_INV][gpi_bank(gpi)];
		inv	= ICHX_READ(inv_reg, priv->gpio_base);

		switch (ICHX_LPT_POL_GPIO_IRQ(cmd)) {
		case ICHX_LPT_POL_GPIO_IRQ_POS:
			inv &= ~gpi_bit(gpi);
			break;
		case ICHX_LPT_POL_GPIO_IRQ_NEG:
			inv |= gpi_bit(gpi);
			break;
		case ICHX_LPT_POL_GPIO_IRQ_INV:
			inv ^= gpi_bit(gpi);
			break;
		}
		ICHX_WRITE(inv, inv_reg, priv->gpio_base);

		spin_unlock_irqrestore(&priv->lock, flags);
	} else
		err = -EINVAL;

	return err;
}

static void lpt_irq_exit(struct ichx_priv *priv)
{
	if (ichx_lpt_pm_base != priv->pm_base)
		kfree(ichx_lpt_pm_base);
	ichx_lpt_pm_base = NULL;

	if (ichx_lpt_pci_dev)
		pci_dev_put(ichx_lpt_pci_dev);
	ichx_lpt_pci_dev = NULL;
}

static int lpt_irq_init(struct ichx_priv *priv)
{
	int i;

	if (!(ichx_lpt_pci_dev =
			pci_get_device(PCI_VENDOR_ID_INTEL,
					PCI_DEVICE_ID_LPT_BRIDGE, NULL))) {
		pr_err("%s: INTEL ISA bridge not found\n", __func__);
		return ichx_lpt_pci_dev ?
			PTR_ERR(ichx_lpt_pci_dev) : -ENODEV;
	}

	if (!(ichx_lpt_pm_base = priv->pm_base)) {
		u32 aba;
		int err = pci_read_config_dword(ichx_lpt_pci_dev,
						PCI_ICHX_LPT_PMBASE_OFFSET,
						&aba);
		if (err)
			return err;

		if (!(ichx_lpt_pm_base =
			kzalloc(sizeof *ichx_lpt_pm_base, GFP_KERNEL)))
			return -ENOMEM;
		
		ichx_lpt_pm_base->start = (aba & 0xffff) & ~0x7f;
		ichx_lpt_pm_base->end = ichx_lpt_pm_base->start + 0x7f;
		ichx_lpt_pm_base->name = "pmbase";
	}

	for (i = 0; i < ARRAY_SIZE(ichx_lpt_gpi_irq); i++) {
		ichx_lpt_gpi_irq[i].gpi_num = i;
		rout_reg(i) = 0xbc;	/* Assume GPI_ROUT2 */

		switch (i) {
		case 0 ... 12:
		case 14 ... 15:
			gpe0_shift(i)	= i + 16;
			rout_shift(i)	= i * 2;
			rout_reg(i)	= 0xb8;	/* GPI_ROUT */
			gpi_bank(i)	= 0;
			gpi_bit(i)	= 0x1 << (i);
			break;
		case 17:
			gpe0_shift(i)	= 56;
			rout_shift(i)	= i - 17;
			gpi_bank(i)	= 1;
			gpi_bit(i)	= 0x1 << (i - 16);
			break;
		case 19:
			gpe0_shift(i)	= 57;
			rout_shift(i)	= i - 17;
			gpi_bank(i)	= 1;
			gpi_bit(i)	= 0x1 << (i - 16);
			break;
		case 21:
			gpe0_shift(i)	= 58;
			rout_shift(i)	= i - 17;
			gpi_bank(i)	= 1;
			gpi_bit(i)	= 0x1 << (i - 16);
			break;
		case 22:
			gpe0_shift(i)	= 59;
			rout_shift(i)	= i - 18;
			gpi_bank(i)	= 1;
			gpi_bit(i)	= 0x1 << (i - 16);
			break;
		case 43:
			gpe0_shift(i)	= 60;
			rout_shift(i)	= i - 35;
			gpi_bank(i)	= 2;
			gpi_bit(i)	= 0x1 << (i - 32);
			break;
		case 57:
			gpe0_shift(i)	= 62;
			rout_shift(i)	= i - 45;
			gpi_bank(i)	= 2;
			gpi_bit(i)	= 0x1 << (i - 32);
			break;
		case 60:
			gpe0_shift(i)	= 63;
			rout_shift(i)	= i - 46;
			gpi_bank(i)	= 2;
			gpi_bit(i)	= 0x1 << (i - 32);
			break;
		default:
			ichx_lpt_gpi_irq[i].gpi_num = 0xff;
			break;
		}
	}

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
		ichx_priv.chip.control = lpt_control;
		if ((err = lpt_irq_init(&ichx_priv)))
			return err;
		break;

	default:
		return -ENODEV;
	}

	spin_lock_init(&ichx_priv.lock);
	res_base = platform_get_resource(pdev, IORESOURCE_IO, ICH_RES_GPIO);
	ichx_priv.use_gpio = ich_info->use_gpio;
	err = ichx_gpio_request_regions(res_base, pdev->name,
					ichx_priv.use_gpio);
	if (err)
		return err;

	ichx_priv.gpio_base = res_base;

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

	pr_info("GPIO from %d to %d on %s\n", ichx_priv.chip.base,
	       ichx_priv.chip.base + ichx_priv.chip.ngpio - 1, DRV_NAME);
	/*libolibo*/
	/* initialize outlvl_cache */
	for (i = 0; i < ARRAY_SIZE(ichx_priv.desc->regs[0]); i++) {
		ichx_priv.outlvl_cache[i] =
			ICHX_READ(ichx_priv.desc->regs[GPIO_LVL][i],
					ichx_priv.gpio_base);
		/*printk("libolibo yyy 0x%08x\n", ichx_priv.outlvl_cache[i]);*/
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

	if (ich_info->gpio_version == LPT_GPIO)
		lpt_irq_exit(&ichx_priv);

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
