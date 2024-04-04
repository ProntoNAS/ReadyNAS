/*
 *  GPIO interface for IT87xx Super I/O chips
 *
 *  Author: Diego Elio Pettenò <flameeyes@flameeyes.eu>
 *
 *  Based on it87_wdt.c     by Oliver Schuster
 *           gpio-it8761e.c by Denis Turischev
 *           gpio-stmpe.c   by Rabin Vincent
 *
 * IRQ handling by Hiro Sugawara (c) 2016,2017
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 2 as published
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>

/* Chip Id numbers */
#define NO_DEV_ID	0xffff
#define IT8613_ID	0x8613
#define IT8625_ID	0x8625
#define IT8728_ID	0x8728
#define IT8732_ID	0x8732
#define IT8761_ID	0x8761

/* IO Ports */
#define REG		0x2e
#define VAL		0x2f

/* Logical device Numbers LDN */
#define GPIO		0x07

/* Configuration Registers and Functions */
#define LDNREG		0x07
#define CHIPID		0x20
#define CHIPREV		0x22

/**
 * struct it87_gpio - it87-specific GPIO chip
 * @chip the underlying gpio_chip structure
 * @lock a lock to avoid races between operations
 * @io_base base address for gpio ports
 * @io_size size of the port rage starting from io_base.
 * @output_base Super I/O register address for Output Enable register
 * @simple_base Super I/O 'Simple I/O' Enable register
 * @simple_size Super IO 'Simple I/O' Enable register size; this is
 *	required because IT87xx chips might only provide Simple I/O
 *	switches on a subset of lines, whereas the others keep the
 *	same status all time.
 */
struct it87_gpio {
	struct gpio_chip chip;
	spinlock_t lock;
	u16 io_base;
	u16 io_size;
	u8 output_base;
	u8 simple_base;
	u8 simple_size;

	u16 chip_type;
	u8 polarity_base;
	u8 debounce_irq_level;
	u8 debounce_irq_map[2];
	u8 smi_status_reg2;

	u8 *valid_irq_mapping;	 /* Pointer to bool map of IRQ-possible GPIOs */
	u8 *valid_irq_levels;	 /* Pointer to bool map of possible IRQs */
	u16 irq_base;
	struct platform_device *pdev;
};

static struct it87_gpio it87_gpio_chip = {
};

static inline struct it87_gpio *to_it87_gpio(struct gpio_chip *chip)
{
	return container_of(chip, struct it87_gpio, chip);
}

/* Superio chip access functions; copied from wdt_it87 */

static inline void superio_select(int ldn)
{
	outb(LDNREG, REG);
	outb(ldn, VAL);
}

static inline int superio_enter(void)
{
	/*
	 * Try to reserve REG and REG + 1 for exclusive access.
	 */
	if (!request_muxed_region(REG, 2, KBUILD_MODNAME))
		return -EBUSY;

	outb(0x87, REG);
	outb(0x01, REG);
	outb(0x55, REG);
	outb(0x55, REG);
	superio_select(GPIO);
	return 0;
}

static inline void superio_exit(void)
{
	outb(0x02, REG);
	outb(0x02, VAL);
	release_region(REG, 2);
}

static inline int superio_inb(int reg)
{
	outb(reg, REG);
	return inb(VAL);
}

static inline void superio_outb(int val, int reg)
{
	outb(reg, REG);
	outb(val, VAL);
}

static inline int superio_inw(int reg)
{
	int val;

	outb(reg++, REG);
	val = inb(VAL) << 8;
	outb(reg, REG);
	val |= inb(VAL);
	return val;
}
#if 0
static inline void superio_outw(int val, int reg)
{
	outb(reg++, REG);
	outb(val >> 8, VAL);
	outb(reg, REG);
	outb(val, VAL);
}
#endif
static inline void superio_set_mask(int mask, int reg)
{
	u8 curr_val = superio_inb(reg);
	u8 new_val = curr_val | mask;

	if (curr_val != new_val)
		superio_outb(new_val, reg);
}

static inline void superio_clear_mask(int mask, int reg)
{
	u8 curr_val = superio_inb(reg);
	u8 new_val = curr_val & ~mask;

	if (curr_val != new_val)
		superio_outb(new_val, reg);
}

static int it87_debounce_irq_slot(struct it87_gpio *it87_gpio, unsigned int gpi)
{
	int i;

	gpi += 8;
	for (i = 0; i< ARRAY_SIZE(it87_gpio->debounce_irq_map); i++)
		if ((superio_inb(it87_gpio->debounce_irq_map[i]) & 0x3f) == gpi)
			return i;
	return -ENODEV;
}

static void simple_or_alternative(struct it87_gpio *it87_gpio,
					unsigned int gpio_num)
{
	u8 mask = BIT(gpio_num % 8);
	u8 group = gpio_num / 8;

        /* Panel button => ALTERNATE, otherwise => Simple IO */
        if (it87_debounce_irq_slot(it87_gpio, gpio_num) >= 0)
		superio_clear_mask(mask, group + it87_gpio->simple_base);
        else
                superio_set_mask(mask, group + it87_gpio->simple_base);

	/* Select GPIO on multi-function pin. */
	switch (gpio_num) {
#if 0	/* Manipulating Set 1 register reboots to BIOS on RR2304. */
	/* Set 1 (group 0) */
	case 0:		/* i.e. GP10 */
	case 2 ... 3:	/* i.e. GP12 ... GP13 */
		mask = BIT(gpio_num);
		break;
#endif
	/* Set 2 (group 1) */
	case 9 ... 11:	/* i.e. GP21 ... GP23 */
		mask = BIT(gpio_num - 8);
		break;
	/* Set 3 (group 2) */
	case 16 ... 19:	/* i.e. GP30 ... GP33 */
	case 22 ... 23:	/* i.e. GP36 ... GP37 */
		mask = BIT(gpio_num - 16);
		break;
	/* Set 4 (group 3) */
	case 24 ... 29:	/* i.e. GP40 ... GP45 */
		mask = BIT(gpio_num - 24);
		break;
	/* Set 5 (group 4) */
	case 32 ... 37:	/* i.e. GP50 ... GP55 */
		mask = BIT(gpio_num - 32);
		break;
	case 38 ... 39: /* i.e. GP56 ... GP57 */
		mask = BIT(7);
		break;
	case 40 ... 42: /* i.e. GP60 ... GP62 */
		mask = BIT(6);
		group = 4;
		break;
	default:
		mask = 0;
		break;
	}

	if (mask)
		superio_set_mask(mask, group + 0x25);
}

static int it87_gpio_request(struct gpio_chip *chip, unsigned gpio_num)
{
	u8 mask, group;
	int rc = 0;
	struct it87_gpio *it87_gpio = to_it87_gpio(chip);
	unsigned long flags;

	mask = BIT(gpio_num % 8);
	group = (gpio_num / 8);

	spin_lock_irqsave(&it87_gpio->lock, flags);

	rc = superio_enter();
	if (rc)
		goto exit;

	/* not all the IT87xx chips support Simple I/O and not all of
	 * them allow all the lines to be set/unset to Simple I/O.
	 */
	if (group < it87_gpio->simple_size)
		simple_or_alternative(it87_gpio, gpio_num);

	/* clear output enable, setting the pin to input, as all the
	 * newly-exported GPIO interfaces are set to input.
	 */
	superio_clear_mask(mask, group + it87_gpio->output_base);

	superio_exit();

exit:
	spin_unlock_irqrestore(&it87_gpio->lock, flags);
	return rc;
}

static int it87_gpio_get(struct gpio_chip *chip, unsigned gpio_num)
{
	u16 reg;
	u8 mask;
	struct it87_gpio *it87_gpio = to_it87_gpio(chip);

	mask = BIT(gpio_num % 8);
	reg = (gpio_num / 8) + it87_gpio->io_base;

	return !!(inb(reg) & mask);
}

static int it87_gpio_direction_in(struct gpio_chip *chip, unsigned gpio_num)
{
	u8 mask, group;
	int rc = 0;
	struct it87_gpio *it87_gpio = to_it87_gpio(chip);

	mask = BIT(gpio_num % 8);
	group = (gpio_num / 8);

	spin_lock(&it87_gpio->lock);

	rc = superio_enter();
	if (rc)
		goto exit;

	simple_or_alternative(it87_gpio, gpio_num);

	/* clear the output enable bit */
	superio_clear_mask(mask, group + it87_gpio->output_base);
	superio_exit();
exit:
	spin_unlock(&it87_gpio->lock);
	return rc;
}

static void __it87_gpio_set(struct it87_gpio *it87_gpio,
			  unsigned int gpio_num, int val)
{
	u8 mask, curr_vals;
	u16 reg;

	mask = BIT(gpio_num % 8);
	reg = (gpio_num / 8) + it87_gpio->io_base;

	curr_vals = inb(reg);
	if (val)
		outb(curr_vals | mask, reg);
	else
		outb(curr_vals & ~mask, reg);
}

static void it87_gpio_set(struct gpio_chip *chip,
			  unsigned int gpio_num, int val)
{
	struct it87_gpio *it87_gpio = to_it87_gpio(chip);
	unsigned long flags;

	spin_lock_irqsave(&it87_gpio->lock, flags);
	__it87_gpio_set(it87_gpio, gpio_num, val);
	spin_unlock_irqrestore(&it87_gpio->lock, flags);
}

static int it87_gpio_direction_out(struct gpio_chip *chip,
				   unsigned int gpio_num, int val)
{
	u8 mask, group;
	int rc = 0;
	struct it87_gpio *it87_gpio = to_it87_gpio(chip);

	mask = BIT(gpio_num % 8);
	group = (gpio_num / 8);

	spin_lock(&it87_gpio->lock);

	rc = superio_enter();
	if (rc)
		goto exit;

	simple_or_alternative(it87_gpio, gpio_num);

	/* set the output enable bit */
	superio_set_mask(mask, group + it87_gpio->output_base);

	__it87_gpio_set(it87_gpio, gpio_num, val);

	superio_exit();
exit:
	spin_unlock(&it87_gpio->lock);
	return rc;
}

static void it87_gpio_polarity(struct it87_gpio *it87_gpio,
				   unsigned int gpio_num, bool active_low)
{
	u8 mask = BIT(gpio_num % 8);
	u16 reg = it87_gpio->polarity_base + (gpio_num / 8);

	if (active_low)
		superio_set_mask(mask, reg);
	else
		superio_clear_mask(mask, reg);
}

/* Valid GPIO IRQ mapping using IT's GPxx port naming convention. */
#define	_P(x)	[(((x)/10)-1)*8 + ((x)%10)] = 1
static u8
it8613_valid_irq_mapping[] = {/* Table 8-10 */
	_P(10),_P(12),_P(13),_P(21),_P(22),_P(23),
	_P(30),_P(31),_P(32),_P(33),              _P(36),_P(37),
	_P(40),_P(41),_P(42),_P(43),_P(44),_P(45),
	_P(50),_P(51),_P(52),_P(53),_P(54),_P(55),_P(56),_P(57),
},
it8625_valid_irq_mapping[] = {/* Table 8-13 */
	_P(10),_P(11),_P(12),_P(13),_P(14),_P(15),_P(16),_P(17),
	_P(20),_P(21),_P(22),_P(23),_P(24),_P(25),_P(26),_P(27),
	_P(30),_P(31),_P(32),_P(32),_P(34),_P(35),_P(36),_P(37),
	_P(40),_P(41),_P(42),_P(43),_P(44),_P(45),_P(46),_P(47),
	_P(50),_P(51),_P(52),_P(53),_P(54),_P(55),_P(56),_P(57),
},
#if 0
it8702_valid_irq_mapping[] = {/* Note 4 */
	_P(10),_P(11),_P(12),_P(13),_P(14),_P(15),_P(16),_P(17),
	_P(20),_P(21),_P(22),_P(23),_P(24),_P(25),_P(26),_P(27),
	_P(30),_P(31),_P(32),_P(33),_P(34),_P(35),_P(36),_P(37),
	_P(40),_P(41),_P(42),_P(43),_P(44),_P(45),_P(46),_P(47),
	_P(50),_P(51),_P(52),_P(53),_P(54),_P(55),
},
#endif
it8728_valid_irq_mapping[] = {/* Table 8-14 */
	_P(10),_P(11),_P(12),       _P(14),_P(15),_P(16),_P(17),
	_P(20),_P(21),_P(22),_P(23),_P(24),_P(25),_P(26),_P(27),
	_P(30),_P(31),_P(32),_P(33),_P(34),_P(35),_P(36),_P(37),
	_P(40),_P(41),_P(42),_P(43),_P(44),_P(45),       _P(47),
	_P(50),_P(51),_P(52),_P(53),_P(54),_P(55),_P(56),_P(57),
},
it8732_valid_irq_mapping[] = {/* Table 8-17 */
	_P(10),_P(11),_P(12),       _P(14),_P(15),_P(16),_P(17),
	_P(20),_P(21),_P(22),_P(23),_P(24),_P(25),_P(26),_P(27),
	_P(30),_P(34),_P(35),_P(36),_P(37),
	_P(40),_P(42),_P(43),_P(44),_P(47),
	_P(50),_P(51),_P(52),_P(53),_P(54),_P(55),_P(56),_P(57),
};

#define _Q(x)	[(x)] = 1
static u8
it8613_valid_irq_levels[] = {/* Table 8-9 */
	       _Q(1),        _Q(3), _Q(4), _Q(5), _Q(6), _Q(7),
	_Q(8), _Q(9), _Q(10),_Q(11),_Q(12),
},
it8625_valid_irq_levels[] = {/* Table 8-11 */
	       _Q(1),        _Q(3), _Q(4), _Q(5), _Q(6), _Q(7),
	_Q(8), _Q(9), _Q(10),_Q(11),_Q(12),
},
it8728_valid_irq_levels[] = {/* Table 8-12 */
	       _Q(1),        _Q(3), _Q(12),
},
it8732_valid_irq_levels[] = {/* Table 8-16 */
	       _Q(1),        _Q(3), _Q(4), _Q(5), _Q(6), _Q(7),
	_Q(8), _Q(9), _Q(10),_Q(11),_Q(12),
};

#define	IT87_DEBIRQ_MASK(i)	BIT((i) + 6)

/* Clear IRQ status to accept next one. */
static void it87_gpio_debounce_irq_ack(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct it87_gpio *it87_gpio = to_it87_gpio(gc);
	u16 reg = it87_gpio->smi_status_reg2;
	int i;

	superio_enter();
	if ((i = it87_debounce_irq_slot(it87_gpio, d->hwirq)) >= 0) {
		u8 mask = IT87_DEBIRQ_MASK(i);

		if (superio_inb(reg) & mask)
			superio_outb(mask, reg);
	}
	superio_exit();
}

static void it87_gpio_irq_mask(struct irq_data *d) { }
static void it87_gpio_irq_unmask(struct irq_data *d) { }

/*
 * Set GPIO number to vacant De-bounce IRQ map register, or clear it.
 * Returns positive if such map register found.
 */
static int it87_goio_irq_set(struct it87_gpio *it87_gpio,
				unsigned int gpio, bool clear)
{
	int i = it87_debounce_irq_slot(it87_gpio, gpio);
	u8 setirq = (gpio + 8) | 0x40;

	/* Already set in either register? */
	if (i >= 0) {
		superio_outb(clear ? 0 : setirq,
				it87_gpio->debounce_irq_map[i]);
		return i;
	}
	/* If not and to clear, we are done. */
	if (clear)
		return 0;
	/* Look for a vacant register. */
	for (i = 0; i < ARRAY_SIZE(it87_gpio->debounce_irq_map); i++)
		if (!superio_inb(it87_gpio->debounce_irq_map[i])) {
			superio_outb(setirq, it87_gpio->debounce_irq_map[i]);
			return i;
		}
	pr_err("%s: No vacant IRQ slot register.\n", __func__);
	return -EBUSY;
}

#include <linux/delay.h>

/* Kernel thread to take care of initially active De-bounce GPIO's IRQ. */
static struct it87_sticky_debounce_work {
	struct work_struct worker;
	u8 gpio;
	u8 chan;
} it87_sticky_debounce_work[ARRAY_SIZE(it87_gpio_chip.debounce_irq_map)];

static void it87_sticky_debounce_checker(struct work_struct *work)
{
	unsigned long flags;
	struct it87_gpio *it87_gpio = &it87_gpio_chip;
	struct it87_sticky_debounce_work *sdw =
		container_of(work, struct it87_sticky_debounce_work, worker);

	do {
		msleep_interruptible(10);
	} while (!it87_gpio_get(&it87_gpio->chip, sdw->gpio));

	spin_lock_irqsave(&it87_gpio->lock, flags);
	if (!superio_enter()) {
		superio_outb(IT87_DEBIRQ_MASK(sdw->chan),
				it87_gpio->smi_status_reg2);
		superio_exit();
	}
	spin_unlock_irqrestore(&it87_gpio->lock, flags);
}

/* De-bounce IRQ responds to a negative pulse whose polarity is subject
 * to polarity control. To avoide confusion with GPIO polarity control,
 * we let de-bounce IRQ always respond to a negative pulse regardless of
 * irq_type.
 */
static int it87_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	unsigned long flags;
	int err = 0;
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	unsigned int gpio = irqd_to_hwirq(d);
	struct it87_gpio *it87_gpio = to_it87_gpio(gc);

	if (!it87_gpio->valid_irq_mapping[gpio]) {
		pr_err("%s: gpionum %d (a.k.a.GP%o) cannot be mapped "
			"for IRQ\n", __func__, gpio, gpio + 8);
		return -EINVAL;
	}

	spin_lock_irqsave(&it87_gpio->lock, flags);
	if ((err = superio_enter()))
		goto exit;

	switch (type) {
	case IRQ_TYPE_NONE:
		/* Dsiable IRQ for this GPIO. */
		err = it87_goio_irq_set(it87_gpio, gpio, true);
		goto exit2;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		err = it87_goio_irq_set(it87_gpio, gpio, false);
		break;
	default:
		err = -EINVAL;
		break;
	}
	if (err < 0)
		goto exit2;

	/* Select simple or alternative. */
	simple_or_alternative(it87_gpio, gpio);

	/* Set IRQ level register. */
	superio_outb(it87_gpio->irq_base, it87_gpio->debounce_irq_level);

	if (!work_busy(&it87_sticky_debounce_work[err].worker)) {
		it87_sticky_debounce_work[err].gpio = gpio;
		it87_sticky_debounce_work[err].chan = err;
		schedule_work(&it87_sticky_debounce_work[err].worker);
	}
exit2:
	superio_exit();
exit:
	spin_unlock_irqrestore(&it87_gpio->lock, flags);
	return err;
}

static struct irq_chip it87_gpio_irq_chip = {
	.irq_ack	= it87_gpio_debounce_irq_ack,
	.irq_mask	= it87_gpio_irq_mask,
	.irq_unmask	= it87_gpio_irq_unmask,
	.irq_set_type	= it87_gpio_irq_set_type,
};

static void it87_gpio_debounce_irq_handler(struct irq_desc *desc)
{
	struct irq_data *data	= irq_desc_get_irq_data(desc);
	struct gpio_chip *gc	= irq_desc_get_handler_data(desc);
	struct it87_gpio *it87_gpio = to_it87_gpio(gc);
	struct irq_chip *ichip	= irq_data_get_irq_chip(data);
	u8 smi_stat2;
	int i;

	superio_enter();
	smi_stat2 = superio_inb(it87_gpio->smi_status_reg2);

	for (i = 0; i < ARRAY_SIZE(it87_gpio->debounce_irq_map); i++) {
		const u8 mask = IT87_DEBIRQ_MASK(i);

		if (smi_stat2 & mask) {
			unsigned int gpio =
		(superio_inb(it87_gpio->debounce_irq_map[i]) & 0x3f) - 8;
			unsigned int virq =
				irq_find_mapping(gc->irqdomain, gpio);

			generic_handle_irq(virq);
		}
	}
	superio_exit();
	ichip->irq_eoi(data);
}

static int it87_gpio_irq_probe(struct platform_device *pdev)
{
	struct it87_gpio *it87_gpio = &it87_gpio_chip;
	struct resource *rsc = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	int err, i;

	if (!rsc || !rsc->start) {
		pr_err("%s: No IRQ resource.\n", __func__);
		return -ENODEV;
	}

	if (rsc->start >= 16 || !it87_gpio->valid_irq_levels[rsc->start]) {
		pr_err("%s: Bad IRQ level %u.\n",
			__func__, (unsigned int)rsc->start);
		return -EINVAL;
	}

	it87_gpio->chip.dev = &pdev->dev;

	for (i = 0; i < ARRAY_SIZE(it87_sticky_debounce_work); i++)
		INIT_WORK(&it87_sticky_debounce_work[i].worker,
					it87_sticky_debounce_checker);

	err = gpiochip_irqchip_add(&it87_gpio->chip, &it87_gpio_irq_chip, 0,
					handle_simple_irq, IRQ_TYPE_NONE);
	if (err) {
		pr_err("%s: failed in irqchip_add (%d)\n", __func__, err);
		return err;
	}

	it87_gpio->irq_base = rsc->start;
	gpiochip_set_chained_irqchip(&it87_gpio->chip,
					&it87_gpio_irq_chip,
					it87_gpio->irq_base,
					it87_gpio_debounce_irq_handler);

	superio_enter();
	for (i = 0; i < ARRAY_SIZE(it87_gpio->debounce_irq_map); i++)
		superio_outb(0, it87_gpio->debounce_irq_map[i]);
	superio_exit();
	return 0;
}

static const struct resource it87_gpio_resources[] = {
	{
		.name	= "gpio_it87_irq",
		.start	= 11,
		.flags	= IORESOURCE_IRQ,
	}
};

static struct platform_driver it87_gpio_irq_driver = {
	.probe	= it87_gpio_irq_probe,
	.driver	= {
		.name	= KBUILD_MODNAME,
	},
};

static int it87_gpio_irq_init(struct it87_gpio *it87_gpio)
{
	struct platform_device *pdev =
		platform_device_register_simple(
					it87_gpio_irq_driver.driver.name,
					0,
					it87_gpio_resources,
					ARRAY_SIZE(it87_gpio_resources));

	if (IS_ERR_OR_NULL(pdev)) {
		int err = pdev ? PTR_ERR(pdev) : -EIO;

		pr_err("%s: failed in platform_device_register (%d)\n",
			__func__, err);
		return err;
	}
	it87_gpio->pdev = pdev;
	return platform_driver_register(&it87_gpio_irq_driver);
}

static void it87_gpio_irq_exit(struct it87_gpio *it87_gpio)
{
	platform_device_unregister(it87_gpio->pdev);
	platform_driver_unregister(&it87_gpio_irq_driver);
}

static struct gpio_chip it87_template_chip = {
	.label			= KBUILD_MODNAME,
	.owner			= THIS_MODULE,
	.request		= it87_gpio_request,
	.get			= it87_gpio_get,
	.direction_input	= it87_gpio_direction_in,
	.set			= it87_gpio_set,
	.direction_output	= it87_gpio_direction_out,
	.base			= -1
};

static int __init it87_gpio_init(void)
{
	int rc = 0, i;
	u8 chip_rev, gpio_ba_reg;
	char *labels, **labels_table;

	struct it87_gpio *it87_gpio = &it87_gpio_chip;

	spin_lock_init(&it87_gpio->lock);
	rc = superio_enter();
	if (rc)
		return rc;

	it87_gpio->chip_type = superio_inw(CHIPID);
	chip_rev  = superio_inb(CHIPREV) & 0x0f;
	superio_exit();

	it87_gpio->chip = it87_template_chip;

	switch (it87_gpio->chip_type) {
	case IT8613_ID:
		if (!it87_gpio->valid_irq_mapping)
			it87_gpio->valid_irq_mapping = it8613_valid_irq_mapping;
		if (!it87_gpio->valid_irq_levels)
			it87_gpio->valid_irq_levels = it8613_valid_irq_levels;
		/* Falls through. */
	case IT8625_ID:
		if (!it87_gpio->valid_irq_mapping)
			it87_gpio->valid_irq_mapping = it8625_valid_irq_mapping;
		if (!it87_gpio->valid_irq_levels)
			it87_gpio->valid_irq_levels = it8625_valid_irq_levels;
		/* Falls through. */
	case IT8728_ID:
		if (!it87_gpio->valid_irq_mapping)
			it87_gpio->valid_irq_mapping = it8728_valid_irq_mapping;
		if (!it87_gpio->valid_irq_levels)
			it87_gpio->valid_irq_levels = it8728_valid_irq_levels;
		/* Falls through. */
	case IT8732_ID:
		if (!it87_gpio->valid_irq_mapping)
			it87_gpio->valid_irq_mapping = it8732_valid_irq_mapping;
		if (!it87_gpio->valid_irq_levels)
			it87_gpio->valid_irq_levels = it8732_valid_irq_levels;
		gpio_ba_reg = 0x62;
		it87_gpio->io_size = 8;
		it87_gpio->output_base = 0xc8;
		it87_gpio->simple_base = 0xc0;
		it87_gpio->simple_size = 5;
		if (!it87_gpio->chip.ngpio)
			it87_gpio->chip.ngpio = 64;

		it87_gpio->polarity_base	= 0xb0;
		it87_gpio->debounce_irq_level	= 0x70;
		it87_gpio->debounce_irq_map[0]	= 0xe0;
		it87_gpio->debounce_irq_map[1]	= 0xe1;
		it87_gpio->smi_status_reg2	= 0xf3;

		break;
	case IT8761_ID:
		gpio_ba_reg = 0x60;
		it87_gpio->io_size = 4;
		it87_gpio->output_base = 0xf0;
		it87_gpio->simple_size = 0;
		it87_gpio->chip.ngpio = 16;
		break;
	case NO_DEV_ID:
		pr_err("no device\n");
		return -ENODEV;
	default:
		pr_err("Unknown Chip found, Chip %04x Revision %x\n",
		       it87_gpio->chip_type, chip_rev);
		return -ENODEV;
	}

	rc = superio_enter();
	if (rc)
		return rc;

	/* fetch GPIO base address */
	it87_gpio->io_base = superio_inw(gpio_ba_reg);

	/* Clear regs explicitly. SIO may remain active during PM.*/
	for (i = 0; i < it87_gpio->chip.ngpio; i++)
		it87_gpio_polarity(it87_gpio, i, false);

	superio_exit();

	pr_info("Found Chip IT%04x rev %x. %u GPIO lines starting at %04xh\n",
		it87_gpio->chip_type, chip_rev, it87_gpio->chip.ngpio,
		it87_gpio->io_base);

	if (!request_region(it87_gpio->io_base, it87_gpio->io_size,
							KBUILD_MODNAME))
		return -EBUSY;

	/* Set up aliases for the GPIO connection.
	 *
	 * ITE documentation for recent chips such as the IT8728F
	 * refers to the GPIO lines as GPxy, with a coordinates system
	 * where x is the GPIO group (starting from 1) and y is the
	 * bit within the group.
	 *
	 * By creating these aliases, we make it easier to understand
	 * to which GPIO pin we're referring to.
	 */
	labels = kcalloc(it87_gpio->chip.ngpio, sizeof("it87_gpXY"),
								GFP_KERNEL);
	labels_table = kcalloc(it87_gpio->chip.ngpio, sizeof(const char *),
								GFP_KERNEL);

	if (!labels || !labels_table) {
		rc = -ENOMEM;
		goto labels_free;
	}

	for (i = 0; i < it87_gpio->chip.ngpio; i++) {
		char *label = &labels[i * sizeof("it87_gpXY")];

		sprintf(label, "it87_gp%u%u", 1+(i/8), i%8);
		labels_table[i] = label;
	}

	it87_gpio->chip.names = (const char *const*)labels_table;

	rc = gpiochip_add(&it87_gpio->chip);
	if (rc)
		goto labels_free;

	if ((rc = it87_gpio_irq_init(it87_gpio)))
		goto labels_free;

	return 0;

labels_free:
	kfree(labels_table);
	kfree(labels);
	release_region(it87_gpio->io_base, it87_gpio->io_size);
	return rc;
}

static void __exit it87_gpio_exit(void)
{
	struct it87_gpio *it87_gpio = &it87_gpio_chip;

	it87_gpio_irq_exit(it87_gpio);
	gpiochip_remove(&it87_gpio->chip);
	release_region(it87_gpio->io_base, it87_gpio->io_size);
	kfree(it87_gpio->chip.names[0]);
	kfree(it87_gpio->chip.names);
}

module_init(it87_gpio_init);
module_exit(it87_gpio_exit);

MODULE_AUTHOR("Diego Elio PettenÃ² <flameeyes@flameeyes.eu>");
MODULE_DESCRIPTION("GPIO interface for IT87xx Super I/O chips");
MODULE_LICENSE("GPL");
