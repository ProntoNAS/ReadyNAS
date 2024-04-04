/*
 * readynas-io.c - NETGEAR ReadyNAS Intel platform I/O assortment
 *
 * Copyright (c) 2015-2017 NETGEAR, Inc.
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
#include <linux/i2c.h>

#include "readynas-io.h"

static struct proc_dir_entry *readynas_proc_root;

struct proc_dir_entry *readynas_proc_create(const char *name,
			umode_t mode,
			const struct file_operations *fops,
			void *data)
{
	return proc_create_data(name, mode, readynas_proc_root, fops, data);
}

/*
 * Generic GPIO and model name search functions.
 */
static int gpio_match_labels(struct gpio_chip *gc, void *data)
{
	return !strcmp(gc->label, (const char *)data);
}

struct gpio_chip *rn_gpiochip_find_by_label(const char *label)
{
	return gpiochip_find((void *)label, gpio_match_labels);
}

/*
 * List of supported NAS models.
 * First element of each internal array is the nominal model name followed
 * by up to 6 possible DMI name strings.
 */
static const struct compatible {
	const char *nominal;
	const char *models[12];
} compatibles[] = {
	{"ultra2", {"ReadyNAS-ProUltra2", NULL}},
	{"ultra4", {"ReadyNAS-NVX-V2", "Ultra4", NULL}},
	{"ultra6", {"FLAME6-2", "Ultra6", NULL}},
	{"pro6",   {"FLAME6-MB", "Pro6", NULL}},
	{"rn312",  {"ReadyNAS 312", "RN312", NULL}},
	{"rn314",  {"ReadyNAS 314", "RN314", NULL}},
	{"rn316",  {"ReadyNAS 316", "RN316", NULL}},
	{"rn422",  {"ReadyNAS 422", "RN422", NULL}},
	{"rn424",  {"ReadyNAS 424", "RN424", NULL}},
	{"rn426",  {"ReadyNAS 426", "RN426", NULL}},
	{"rn428",  {"ReadyNAS 428", "RN428", NULL}},
	{"rnx16",  {"ReadyNAS 516", "RN516", "ReadyNAS 716", "RN716", "ReadyDATA 516", "RDD516", NULL}},
	{"rnx24",  {"ReadyNAS 524", "RN524", "ReadyNAS 624", "RN624", NULL}},
	{"rnx26",  {"ReadyNAS 526", "RN526", "ReadyNAS 626", "RN626", NULL}},
	{"rnx28",  {"ReadyNAS 528", "RN528", "ReadyNAS 628", "RN628", NULL}},
	{"rr2304", {"ReadyNAS 2304", "RR2304", NULL}},
	{"rr2308", {"ReadyNAS 2308", "RR2308", NULL}},
	{"rr2312", {"ReadyNAS 2312", "RR2312", NULL}},
	{"rn3130", {"ReadyNAS 3130", "ReadyNAS3130", "RN3130", NULL}},
	{"rn3138", {"ReadyNAS 3138", "ReadyNAS3138", "RN3138", NULL}},
	{"rnx220", {"ReadyNAS 3220", "ReadyNAS3220", "RN3220",
	            "ReadyNAS 4220", "ReadyNAS4220", "RN4220", NULL}},
	{"rrx312", {"ReadyNAS 3312", "ReadyNAS3312", "RR3312",
	            "ReadyNAS 4312", "ReadyNAS4312", "RR4312",
		    "ReadyNAS 3312V2", "RR3312V2",
		    "ReadyNAS 4312V2", "RR4312SV2", "RR4312XV2", NULL}},
	{"rr4360", {"ReadyNAS 4360", "ReadyNAS4360", "RR4360", NULL}},
	{}
},
gcompatibles[] = {
	/* Group model names for submodel names. */
	{"rn42x",  {"rn422", "rn424", "rn426", "rn428", NULL}},
	{"rn422_4",  {"rn422", "rn424", NULL}},
	{"rn426_8",  {"rn426", "rn428", NULL}},
	{"rnx2x",  {"rnx24", "rnx26", "rnx28", NULL}},
	{"rrx312_60",  {"rrx312", "rr4360", NULL}},
	{"rr23xx",  {"rr2304", "rr2308", "rr2312", NULL}},
	{"rn313x",  {"rn3130", "rn3138", NULL}},
	{}
};

static bool model_compatible(char const *dmi, const struct compatible *model)
{
	const char * const *m = model->models;

	while (*m) {
		const char *p;

		if (!strcmp(dmi, *m) || !*m)
			return true;
		/* Some model names may have an 'X' suffix. */
		if ((p = strstr(dmi, *m))) {
			p += strlen(*m);
			if (!*p || isspace(*p) || *p == 'X' || *p == 'S')
				return true;
		}
		m++;
	}
	return false;
}

bool readynas_io_compatible(char const *nominal)
{
	const char *dmi[2];
	const struct compatible *model;
	int i;
	static bool inited;

	dmi[0] = dmi_get_system_info(DMI_PRODUCT_NAME);
	dmi[1] = dmi_get_system_info(DMI_PRODUCT_VERSION);

	if (!inited) {
		for (i = 0; i < ARRAY_SIZE(dmi); i++)
			if (dmi[i] &&
				strcmp(dmi[i], "ReadyNAS") &&
				strcmp(dmi[i], "ReadyNAS  ")) {
				pr_info("ReadyNAS model: %s\n", dmi[i]);
				break;
			}
		inited = true;
	}

	for (model = &compatibles[0]; model->nominal; model++)
		if (!strcmp(model->nominal, nominal))
			break;

	if (model->nominal) {
		for (i = 0; i < ARRAY_SIZE(dmi); i++)
			if (dmi[i] && model_compatible(dmi[i], model))
				return true;
		return false;
	}

	/* Submodel names? */
	for (model = &gcompatibles[0]; model->nominal; model++)
		if (!strcmp(model->nominal, nominal))
			break;
	if (!model->nominal) {
		WARN(1, "%s: '%s' not found", __func__, nominal);
		return false;
	}

	/* Recursive call for submodels. */
	for (i = 0; model->models[i]; i++)
		if (readynas_io_compatible(model->models[i]))
			return true;

	return false;
}

const char *rn_i2c_adapters_nct677x[] = { "NCT6775", "NCT6776", "NCT6779",
					"nct6775", "nct6776", "nct6779", NULL };
const char *rn_i2c_adapters_ipch[] = { "I801", "i801", NULL };

static int i2c_name_match(struct device *dev, void *data)
{
	const char *devname = to_i2c_adapter(dev)->name;
	const char **pp = data;

	while (*pp)
		if (strstr(devname, *pp++))
			return 1;
	return 0;
}

struct i2c_adapter *readynas_i2c_adapter_find(const char **i2c_names)
{
	struct device *i2c_dev = bus_find_device(&i2c_bus_type, NULL,
						i2c_names, i2c_name_match);
	return i2c_dev ? to_i2c_adapter(i2c_dev) : ERR_PTR(-ENODEV);
}

/* ACPI IRQ is hardcoded in each GPIO driver. */
#if 0
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

#define ACPI_IRQ(bw)	\
	do {					\
		if (!(bw)->irq) {		\
			int __irq = acpi_irq();	\
			if (__irq < 0)		\
				return __irq;	\
			(bw)->irq = __irq;	\
		}				\
	} while (0)
#endif

/*
 * Hitach HD44780 for RN314/Ultra4 LCD alphanumeric display
 */
static struct platform_device *rnx4_lcd_pdev;

static int __init rnx4_lcd_init(void)
{
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
	};

	static struct rnx4_lcd_info {
		const char *compatible;
		struct platform_driver pdriver;
		const struct gpio_lcd_map *map;
	} rnx4_lcd_info[] = {
		{
			"rn314",
			{ .driver = { .name	= "rn314-lcd", }, },
			&rn314_lcd_map
		}, {
			"ultra4",
			{ .driver = { .name	= "ultra4-lcd", }, },
			&ultra4_lcd_map
		},{}
	};

	int err;
	struct rnx4_lcd_info *lcd =
		__compatible_find(rnx4_lcd_info, compatible, 0, "");

	pr_info("%s: initializing %s LCD front display.\n",
		__func__, lcd->compatible);
	if ((err = platform_driver_register(&lcd->pdriver))) {
		pr_err("%s: failed to register %s LCD driver.\n", __func__,
			lcd->compatible);
		return err;
	}

	rnx4_lcd_pdev =
		platform_device_register_simple(lcd->pdriver.driver.name,
						-1, NULL, 0);
	if (IS_ERR(rnx4_lcd_pdev))
		err = PTR_ERR(rnx4_lcd_pdev);
	else if (!rnx4_lcd_pdev)
		err = -ENODEV;
	else
		platform_set_drvdata(rnx4_lcd_pdev, (void *)lcd->map);

	if (err) {
		pr_err("%s: failed to register %s LCD device.\n", __func__,
			lcd->compatible);
		platform_driver_unregister(&lcd->pdriver);
		rnx4_lcd_pdev = NULL;
	}

	return err;
}

static void rnx4_lcd_exit(void)
{
	if (rnx4_lcd_pdev) {
		struct device_driver *drv = rnx4_lcd_pdev->dev.driver;

		platform_device_unregister(rnx4_lcd_pdev);
		platform_driver_unregister(to_platform_driver(drv));
		rnx4_lcd_pdev = NULL;
	}
}

/*
 * Common button event and state handling.
 */
#define	BUTTON_ATTR_IRQ		(0<<16)
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
 * @key - input layer key code: KEY_*
 * @input_dev - relevant input device struct pointer
 * @input_dev_name - name string for input device (default: "rn_button")
 * @worker_to_set - button handling work function (default: button_worker)
 * @worker - button handling thread
 * @prev_button_state - for polling
 * @i2cfb_button_state - for FB button polling
 * @button - internal button number: BUTTON_BACKUP, BUTTON_RESET, ...
 * @type - event type: IRQ: spawns thread for each depress to poll for release,
 *	  THREAD: continues to poll button for release,
 *	  POLL: a common thread polls for press and release
 *	  IRQ: both rising and falling edges
 * @irq_type - IRQ_TYPE_*
 * @init, @exit - button-specific initializer and exiter
 * @depressed - depress sensor (default: button_gpio_depressed)
 * @irq - virtual IRQ number returned by gpio_to_irq()
 * @notifier - internal optional notifier
 * @private -  button-specific private item
 * @private2 - button-specific private item #2
 * @acpi_event_number - conflicting ACPI GPE number
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
	BUTTON_TYPE_NONE	= 0,
	BUTTON_TYPE_IRQ,
	BUTTON_TYPE_THREAD,
	BUTTON_TYPE_POLL,

	BUTTON_TYPE_I2CFB,

	BUTTON_TYPE_SUBWORK	= 0x10000,
	BUTTON_TYPE_NOBOUNCE	= 0x20000,
};

#define BUTTON_TYPE(bw)	((bw)->type & 0xffff)
#define BUTTON_TYPE_IS_IRQ(bw)	(BUTTON_TYPE(bw) == BUTTON_TYPE_IRQ)
#define	BUTTON_TYPE_IRQ_I2CFB	\
		(BUTTON_TYPE_IRQ | BUTTON_TYPE_SUBWORK | BUTTON_TYPE_NOBOUNCE)

static struct button_work {
	const char *name;
	const char *gpio_label;
	unsigned int gpio_num;
	bool	gpio_active_low;
	struct	gpio_desc *gpio_desc;
	unsigned int	key;
	struct	input_dev *input_dev;
	const char *input_dev_name;
	work_func_t	worker_to_set;
	struct	work_struct worker;
	unsigned int button;
	enum button_event_type type;
	unsigned int irq_type;
	bool busy;
	int  (*init)(struct button_work *);
	void (*exit)(struct button_work *);
	int  (*depressed)(struct button_work *);
	void (*irq_handler)(struct irq_desc *);
	int prev_button_state;
	u8 i2cfb_button_state;
	struct proc_dir_entry *status_proc;
	int  irq;
	void (*reporter)(struct button_work *, unsigned int);
	void (*notifier)(struct button_work *, bool);
	void *private;
	void *private2;
	const u32 acpi_event_number;
} *button_work;	/* Point to platform-specific button array. */

/*
 * acpi_ev_gpe_dispatch() checks this to see if the interrupt handling
 * for the GPE number conflicts current button_work's own and ACPI
 * should ignore the interrupt.
 */
bool readynas_ignore_acpi_event(u32 gpe_number)
{
	int i;

	if (!button_work)
		return false;
	for (i = 0; button_work[i].name; i++)
		if (button_work[i].acpi_event_number == gpe_number)
			return true;
	return false;
}

static inline struct button_work *work_to_button_work(struct work_struct *work)
{
	return container_of(work, struct button_work, worker);
}

static const char *button_type_str(struct button_work *bw)
{
	const char *p = "UNKWON";

	switch (BUTTON_TYPE(bw)) {
	case BUTTON_TYPE_IRQ:
		p = "IRQ"; break;
	case BUTTON_TYPE_POLL:
		p = "POLL"; break;
	case BUTTON_TYPE_THREAD:
		p = "THREAD"; break;
	case BUTTON_TYPE_I2CFB:
		p = "I2CFB"; break;
	default:
		break;
	}
	return p;
}

static inline int button_gpio_depressed(struct button_work *bw)
{
	return gpiod_get_value(bw->gpio_desc);
}

static inline int button_depressed(struct button_work *bw)
{
	return bw->depressed ?  bw->depressed(bw) : button_gpio_depressed(bw);
}

static void button_reporter(struct button_work *bw, unsigned int state)
{
	if (bw->reporter)
		bw->reporter(bw, state);
	else {
		if (bw->input_dev) {
			input_event(bw->input_dev, EV_KEY, bw->key, !!state);
			input_sync(bw->input_dev);
		}
		if (bw->notifier)
			bw->notifier(bw, !!state);
	}
}

static void button_worker(struct work_struct *work)
{
	struct button_work *bw = work_to_button_work(work);

	if (!(bw->type & BUTTON_TYPE_NOBOUNCE))
		msleep_interruptible(10);

	if (button_depressed(bw) > 0) {
		button_reporter(bw, 1);
		while (button_depressed(bw) > 0)
			msleep_interruptible(10);
		button_reporter(bw, 0);
	}
	bw->busy = false;
}

#if 0	/* Used only by retired ACPI handler. */
static void schedule_button_work(unsigned int button)
{
	struct button_work *bw;

	for (bw = button_work; bw->name; bw++) {
		if (bw->button != button || !bw->depressed)
			continue;
		else if (bw->busy || button_depressed(bw) <= 0)
			return;

		bw->busy = true;
		switch (BUTTON_TYPE(bw)) {
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
#endif

static int setup_gpio_input(struct button_work *bw)
{
	int err;
	struct gpio_desc *gd;
	struct gpio_chip *gc = rn_gpiochip_find_by_label(bw->gpio_label);

	if (IS_ERR_OR_NULL(gc)) {
		pr_err("%s: cannot find button GPIO %s.\n",
					__func__, bw->gpio_label);
		return gc ? PTR_ERR(gc) : -ENODEV;
	}

	gd = gpiochip_get_desc(gc, bw->gpio_num);
	if (IS_ERR_OR_NULL(gd)) {
		pr_err("%s: cannot find button GPIO %s:%d.\n",
			 __func__, bw->gpio_label, bw->gpio_num);
		return gd ? PTR_ERR(gd) : -ENODEV;
	}

	if (bw->gpio_active_low)
		set_bit(FLAG_ACTIVE_LOW, &gd->flags);
	err = gpiod_direction_input(gd);
	if (err) {
		pr_err("%s: failed to set GPIO %s:%d to input (%d).\n",
			 __func__, bw->gpio_label, bw->gpio_num, err);
		return err;
	}
	bw->gpio_desc = gd;
	return 0;
}

#if 0	/* Retired. */
/*
 * ACPI event based button handling.
 */
static void acpi_notify(struct acpi_device *device, u32 button)
{
	schedule_button_work(MK_BUTTON(ACPI, button));
}

static inline int acpi_add(struct acpi_device *device) { return 0; }
static inline int acpi_remove(struct acpi_device *device) { return 0; }
static const struct acpi_device_id button_device_ids[] = {
	{ "ACPI0006", 0 },
	{},
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

static int acpi_driver_registgered;
static int button_acpi_init(struct button_work *bw)
{
	int err = 0;

	pr_info("%s: installing ACPI button notifier for '%s'\n",
		__func__, bw->name);
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
#endif

/*
 * Interrupt-driven button worker
 */
static void button_worker_irq_ack(struct work_struct *work)
{
	struct button_work *bw = work_to_button_work(work);
	struct irq_data *d = irq_get_irq_data(bw->irq);

	button_worker(work);
	d->chip->irq_ack(d);
}

static void button_irq_handler(struct irq_desc *desc)
{
	struct irq_data *d	= irq_desc_get_irq_data(desc);
	struct button_work *bw	= irq_data_get_irq_handler_data(d);

	bw->busy = true;
	schedule_work(&bw->worker);
}

static void button_irq_handler_ack(struct irq_desc *desc)
{
	struct irq_data *d	= irq_desc_get_irq_data(desc);

	button_irq_handler(desc);
	d->chip->irq_ack(d);
}

static void button_irq_exit(struct button_work *bw)
{
	irq_set_irq_type(bw->irq, IRQ_TYPE_NONE);
	irq_set_chained_handler_and_data(bw->irq, NULL, NULL);
	bw->gpio_desc = NULL;
}

static int button_irq_init(struct button_work *bw)
{
	struct gpio_chip *gc = bw->gpio_desc->chip;
	int virq = gpio_to_irq(gc->base + bw->gpio_num);
	int err;

	if (virq < 0) {
		pr_err("%s: Bad GPIO# (%u+%u)\n",
			__func__, gc->base, bw->gpio_num);
		return virq;
	}

	irq_set_chained_handler_and_data(virq,
		bw->irq_handler ? bw->irq_handler : button_irq_handler, bw);
	if ((err = irq_set_irq_type(virq, bw->irq_type)))
		irq_set_chained_handler_and_data(virq, NULL, NULL);
	else
		bw->irq = virq;

	return err;
}

static struct button_work button_work_rnx220[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 1,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_IRQ,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
		.irq_handler	= button_irq_handler_ack,
		.acpi_event_number	= 0x11,
	},
#if 0	/* Not working */
	{
		.name = "uid",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 52,
		.gpio_active_low	= false,
		.type	= BUTTON_TYPE_POLL,
		.button = MK_BUTTON(ACPI, 177),
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
		.key	= KEY_COPY,
		.button	= BUTTON_BACKUP,
		.type	= BUTTON_TYPE_IRQ,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
		.irq_handler	= button_irq_handler_ack,
		.acpi_event_number	= 0x1b,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_IRQ,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
		.irq_handler	= button_irq_handler_ack,
		.acpi_event_number	= 0x15,
	}, {}
};

static struct button_work button_work_rn316[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_IRQ,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
		.irq_handler	= button_irq_handler_ack,
		.acpi_event_number	= 0x15,
	}, {}
};

static struct button_work button_work_rn516[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 1,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_IRQ,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
		.irq_handler	= button_irq_handler_ack,
	}, {}
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
		.key	= KEY_COPY,
		.button	= BUTTON_BACKUP,
		.type	= BUTTON_TYPE_POLL,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 5,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_POLL,
	}, {}
};

static struct button_work button_work_ultra6[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 15,
		.gpio_active_low	= 1,
		.key	= KEY_COPY,
		.button	= BUTTON_BACKUP,
		.type	= BUTTON_TYPE_POLL,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 8,
		.gpio_active_low	= 1,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_POLL,
	}, {}
};

static struct button_work button_work_pro6[] = {
	{
		.name		= "backup",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 15,
		.gpio_active_low	= 0,
		.key	= KEY_COPY,
		.button	= BUTTON_BACKUP,
		.type	= BUTTON_TYPE_POLL,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 8,
		.gpio_active_low	= 0,
		.key	= KEY_RESTART,
		.button	= BUTTON_RESET,
		.type	= BUTTON_TYPE_POLL,
	}, {}
};


static struct button_work button_work_rn313x[] = {
	{/* GP77 cannot generate IRQ. */
		.name 		= "uid",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 55,	/* a.k.a. "GP77" */
		.button		= BUTTON_UID,
		.type		= BUTTON_TYPE_POLL,
	},
	{
		.name		= "reset",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 21,	/* a.k.a. "GP35" */
		.gpio_active_low	= 1,
		.key		= KEY_RESTART,
		.type		= BUTTON_TYPE_IRQ | BUTTON_TYPE_NOBOUNCE,
		.irq_type	= IRQ_TYPE_EDGE_FALLING,
		.init		= button_irq_init,
		.exit		= button_irq_exit,
		.worker_to_set	= button_worker_irq_ack,
	}, {}
};

static const struct common_init_exit {
	char *name;
	enum button_event_type type;
	int  (*init)(struct button_work *);
	void (*exit)(struct button_work *);
} gpio_init_exit[] = {
	{
		.name	= "gpio_ich",
		.type	= BUTTON_TYPE_IRQ_I2CFB,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
	},
	{
		.name	= "gpio_dnv.0",
		.type	= BUTTON_TYPE_IRQ_I2CFB,
		.init	= button_irq_init,
		.exit	= button_irq_exit,
	}, {}
};

static const struct common_init_exit
*gpio_init_exit_find(const struct button_work *bw)
{
	const struct common_init_exit *gie = gpio_init_exit;

	for (; gie->name; gie++)
		if (!strcmp(gie->name, bw->gpio_label) && gie->type == bw->type)
			return gie;
	return NULL;
}

/*
 * RN42[2468]/[56]2[468] Front Board button handler.
 */
#define I2CFB_I2C_CHIPADDR	0x1c

#define I2CFB_ID		0x0
#define I2CFB_LED_STATE		0x1
#define I2CFB_LED_BRIGHTNESS	0x2
#define I2CFB_BUTTON_EVENT	0x3
#define I2CFB_BUTTON_STATE	0x4
#define I2CFB_BUTTON_EVENT_MASK	0x5

/*
 * Compare current I2c reading with previous one.
 */
static void i2cfb_reporter(struct button_work *bw, unsigned int state)
{
	u8 diff;
	struct button_work *sw;
	int i2cret = i2c_smbus_read_byte_data((struct i2c_client *)bw->private2,
						I2CFB_BUTTON_STATE);
	if (i2cret < 0)
		return;

	if (!(diff = bw->i2cfb_button_state ^ (i2cret & 0xff)))
		return;

	for (sw = bw->private; sw->name; sw++) {
		u8 button_mask = (1 << sw->gpio_num);

		if (diff & button_mask)
			button_reporter(sw, !!(i2cret & button_mask));
	}

	bw->i2cfb_button_state = i2cret;
}

/*
 * FB button processor scheduled by GPIO2 IRQ.
 */
static void button_worker_i2cfb(struct work_struct *work)
{
	button_reporter(work_to_button_work(work), 1);
}

static int i2cfb_button_depressed(struct button_work *sw)
{
	struct button_work *bw = sw->private;
	int i2cret = i2c_smbus_read_byte_data((struct i2c_client *)bw->private2,
						I2CFB_BUTTON_STATE);
	if (i2cret < 0)
		return i2cret;
	return !!(i2cret & (1 << sw->gpio_num));
}

/*
 * GPIO IRQ handler FB button events.
 * Accessing I2c may sleep. Schedule work for processing.
 * FB button events flip GPIO2 signal. Prepare for next event by inverting
 * GPIO IRQ polarity.
 */
static void i2cfb_exit(struct button_work *bw)
{
	const struct common_init_exit *gie = gpio_init_exit_find(bw);

	if (gie && gie->exit)
		gie->exit(bw);

	if (!IS_ERR_OR_NULL(bw->private2))
		i2c_unregister_device((struct i2c_client *)bw->private2);
	bw->private2 = NULL;
}

static int i2cfb_init(struct button_work *bw)
{
	int err;
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	const struct common_init_exit *gie;

	static struct i2c_board_info i2cfb_i2c_info = {
		I2C_BOARD_INFO("i2cfb", I2CFB_I2C_CHIPADDR),
	};

	pr_info("%s: initializing I2C FB buttons %s:%d (%s)\n",
		__func__, bw->gpio_label, bw->gpio_num, button_type_str(bw));

	err = setup_gpio_input(bw);
	if (err) {
		pr_err("%s: failed to set up GPIO input.\n", __func__);
		return err;
	}

	pr_info("%s: I2C adapter/chip addr %s/0x%x\n",
		__func__, rn_i2c_adapters_ipch[0], I2CFB_I2C_CHIPADDR);
	adapter = readynas_i2c_adapter_find(rn_i2c_adapters_ipch);
	if (IS_ERR_OR_NULL(adapter)) {
		pr_err("%s: failed to find I2C adapter.\n", __func__);
		return adapter ? PTR_ERR(adapter) : -ENODEV;
	}

	client = i2c_new_device(adapter, &i2cfb_i2c_info);
	if (!client) {
		pr_err("%s: failed to allocate I2CFB client.\n", __func__);
		return -ENOMEM;
	}

	bw->private2 = client;
	if ((err = i2c_smbus_read_byte_data(client, I2CFB_ID)) < 0) {
		pr_err("%s: failed to read a byte from I2C.\n", __func__);
		goto exit;
	}
	if (err != 'F') {
		pr_err("%s: ID='%02x' mismatches Front Board.", __func__, err);
		err = -ENODEV;
		goto exit;
	}

	if ((err = i2c_smbus_write_byte_data(client,
						I2CFB_LED_BRIGHTNESS, 15))) {
		pr_err("%s: failed to write a byte to I2C.\n", __func__);
		goto exit;
	}

	if ((err = i2c_smbus_read_byte_data(client, I2CFB_BUTTON_STATE)) < 0) {
		pr_err("%s: failed to read a byte from I2C.\n", __func__);
		goto exit;
	}
	bw->i2cfb_button_state = err & 0xff;
	if ((err = i2c_smbus_write_byte_data(client,
						I2CFB_BUTTON_EVENT_MASK, 0))) {
		pr_err("%s: failed to write a byte to I2C.\n", __func__);
		goto exit;
	}

	if ((gie = gpio_init_exit_find(bw)) && gie->init)
		err = gie->init(bw);
exit:
	if (err)
		i2cfb_exit(bw);
	return err;
}

static struct button_work button_work_i2cfb[] = {
	{
		.name		= "left",
		.gpio_num	= 0,	/* BUTTON_STATE bit number */
		.key		= KEY_LEFT,
		.type		= BUTTON_TYPE_I2CFB,
		.input_dev_name	= "fb_button",
		.depressed	= i2cfb_button_depressed,
	}, {
		.name		= "right",
		.gpio_num	= 1,	/* BUTTON_STATE bit number */
		.key		= KEY_RIGHT,
		.type		= BUTTON_TYPE_I2CFB,
		.input_dev_name	= "fb_button",
		.depressed	= i2cfb_button_depressed,
	}, {
		.name		= "up",
		.gpio_num	= 2,	/* BUTTON_STATE bit number */
		.key		= KEY_UP,
		.type		= BUTTON_TYPE_I2CFB,
		.input_dev_name	= "fb_button",
		.depressed	= i2cfb_button_depressed,
	}, {
		.name		= "down",
		.gpio_num	= 3,	/* BUTTON_STATE bit number */
		.key		= KEY_DOWN,
		.type		= BUTTON_TYPE_I2CFB,
		.input_dev_name	= "fb_button",
		.depressed	= i2cfb_button_depressed,
	}, {
		.name		= "ok",
		.gpio_num	= 4,	/* BUTTON_STATE bit number */
		.key		= KEY_OK,
		.type		= BUTTON_TYPE_I2CFB,
		.input_dev_name	= "fb_button",
		.depressed	= i2cfb_button_depressed,
	}, {}
};

static struct button_work button_work_rn42x[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 9,	/* a.k.a. "GP21" */
		.gpio_active_low	= 1,
		.key		= KEY_RESTART,
		.type		= BUTTON_TYPE_IRQ | BUTTON_TYPE_NOBOUNCE,
		.irq_type	= IRQ_TYPE_EDGE_FALLING,
		.init		= button_irq_init,
		.exit		= button_irq_exit,
		.worker_to_set	= button_worker_irq_ack,
	}, {
		.name		= "front-board",
		.gpio_label	= "gpio_dnv.0",
		.gpio_num	= 5,
		.type		= BUTTON_TYPE_IRQ_I2CFB,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init		= i2cfb_init,
		.exit		= i2cfb_exit,
		.irq_handler	= button_irq_handler_ack,
		.reporter	= i2cfb_reporter,
		.private	= &button_work_i2cfb,
		.worker_to_set	= button_worker_i2cfb,
	}, {}
};

static struct button_work button_work_rn526[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 23,	/* a.k.a. "GP37" */
		.gpio_active_low	= 1,
		.key		= KEY_RESTART,
		.type		= BUTTON_TYPE_IRQ | BUTTON_TYPE_NOBOUNCE,
		.irq_type	= IRQ_TYPE_EDGE_FALLING,
		.init		= button_irq_init,
		.exit		= button_irq_exit,
		.worker_to_set	= button_worker_irq_ack,
	}, {
		.name		= "front-board",
		.gpio_label	= "gpio_ich",
		.gpio_num	= 2,
		.type		= BUTTON_TYPE_IRQ_I2CFB,
		.irq_type	= IRQ_TYPE_EDGE_BOTH,
		.init		= i2cfb_init,
		.exit		= i2cfb_exit,
		.irq_handler	= button_irq_handler_ack,
		.reporter	= i2cfb_reporter,
		.private	= &button_work_i2cfb,
		.worker_to_set	= button_worker_i2cfb,
	}, {}
};

static struct button_work button_work_rr23xx[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 9,	/* a.k.a. "GP21" */
		.gpio_active_low	= 1,
		.key		= KEY_RESTART,
		.type		= BUTTON_TYPE_IRQ | BUTTON_TYPE_NOBOUNCE,
		.irq_type	= IRQ_TYPE_EDGE_FALLING,
		.init		= button_irq_init,
		.exit		= button_irq_exit,
		.worker_to_set	= button_worker_irq_ack,
	}, {}
};

/*
 * RR[34]312 buttons
 */
#ifdef CONFIG_RRx312_60_UID_BUTTON
/* RRx312/4360 have no UID button installed, Not tested. */
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
#endif

static struct button_work button_work_rrx312_60[] = {
	{
		.name		= "reset",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 31,	/* a.k.a. "GP47" */
		.gpio_active_low	= 1,
		.key		= KEY_RESTART,
		.type		= BUTTON_TYPE_IRQ | BUTTON_TYPE_NOBOUNCE,
		.irq_type	= IRQ_TYPE_EDGE_FALLING,
		.init		= button_irq_init,
		.exit		= button_irq_exit,
		.worker_to_set	= button_worker_irq_ack,
	},
#ifdef	CONFIG_RRx312_60_UID_BUTTON
	/* RRx312/4360 have no UID button installed, Not tested. */
	{
		.name		= "uid",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 49,	/* a.k.a. "GP71" */
		.gpio_active_low	= 0,
		.key		= KEY_FIND,
		.type		= BUTTON_TYPE_POLL,
		.notifier	= notifier_rnx312_uid_led_toggle,
	},
#endif
	{}
};

/*
 * Array of compatible button handlers.
 * Generic button state API.
 */
static struct button_set {
	const char *compatible;
	struct button_work *button_work;
} button_set[] = {
	{ "ultra2",	button_work_ultra2_4 },
	{ "ultra4",	button_work_ultra2_4 },
	{ "ultra6",	button_work_ultra6 },
	{ "pro6",	button_work_pro6 },
	{ "rn312",	button_work_rn314 },
	{ "rn314",	button_work_rn314 },
	{ "rn316",	button_work_rn316 },
	{ "rn42x",	button_work_rn42x },
	{ "rnx16",	button_work_rn516 },
	{ "rnx2x",	button_work_rn526 },
	{ "rr23xx",	button_work_rr23xx },
	{ "rn313x",	button_work_rn313x },
	{ "rnx220",	button_work_rnx220 },
	{ "rrx312_60",	button_work_rrx312_60 },
	{}
};

static int button_state_show(struct seq_file *s, void *v)
{
	struct button_work *bw = s->private;

	seq_printf(s, "%d\n", button_depressed(bw));
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
	bw->status_proc = readynas_proc_create(name, 0444,
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
static struct task_struct	*poller_kthread;

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

			if (BUTTON_TYPE(bw) != BUTTON_TYPE_POLL ||
				(state = button_depressed(bw)) ==
					bw->prev_button_state)
				continue;
			if (!(bw->type & BUTTON_TYPE_NOBOUNCE)) {
				msleep_interruptible(10);
				if ((state = button_depressed(bw)) ==
						bw->prev_button_state)
					continue;
			}
			button_reporter(bw, state);
			bw->prev_button_state = state;
		}
		msleep_interruptible(poll_interval);
	}
	return 0;
}

static int button_poller_start(void)
{
	int err = 0;

	mutex_lock(&poller_lock);
	if (!poller_kthread) {
		poller_kthread = kthread_run(button_poller, button_work,
						"readynas-button");
		if (IS_ERR(poller_kthread)) {
			err = -ENOMEM;
			poller_kthread = NULL;
		}
	}
	mutex_unlock(&poller_lock);
	return err;
}

static void button_poller_stop(void)
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
		button_poller_start();
	} else
		button_poller_stop();
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

	poller_proc = readynas_proc_create("button-poll", 0666,
						&button_thread_proc_fop, NULL);
	err = poller_proc ? 0 : -EIO;
	if (err)
		pr_err("%s: failed to create procfs button-poll\n", __func__);
	return err;
}

/*
 * Common button handler initializer/exiter
 */
static struct rn_input_dev {
	struct input_dev *dev;
	const char *name;
	const u16 bustype;
} rn_input_devs[] = {
	{
		.name		= "rn_button",
		.bustype	= BUS_HOST,
	}, {
		.name		= "fb_button",
		.bustype	= BUS_HOST,
	}, {}
};

static const char *button_input_dev_name(struct button_work *bw)
{
	return (bw->input_dev && bw->input_dev->name) ?
					bw->input_dev->name : "button";
}

static void __button_exit(struct button_work *bw);
static void __button_exit_one(struct button_work *bw)
{
	if (!bw)
		return;
	if (bw->status_proc) {
		proc_remove(bw->status_proc);
		bw->status_proc = NULL;
	}
	if (BUTTON_TYPE_IS_IRQ(bw))
		cancel_work_sync(&bw->worker);
	if (bw->type & BUTTON_TYPE_SUBWORK)
		__button_exit((struct button_work *)bw->private);

	if (bw->exit)
		bw->exit(bw);
}

static void __button_exit(struct button_work *bw)
{
	while (bw->name)
		__button_exit_one(bw++);
}

static void button_exit(void)
{
	int i;

	if (!button_work)
		return;

	if (poller_proc) {
		proc_remove(poller_proc);
		poller_proc = NULL;
	}

	button_poller_stop();

	__button_exit(button_work);

	for (i = 0; rn_input_devs[i].name; i++)
		if (rn_input_devs[i].dev) {
			input_unregister_device(rn_input_devs[i].dev);
			input_free_device(rn_input_devs[i].dev);
			rn_input_devs[i].dev = NULL;
		}
}

static int rn_set_button(struct button_work *bw)
{
	const char *name = bw->input_dev_name ?
				bw->input_dev_name : "rn_button";
	struct rn_input_dev *idev;

	for (idev = rn_input_devs;
		idev->name && strcmp(idev->name, name); idev++)
		;

	if (!idev->name) {
		pr_err("%s: Bad button input device name %s.\n", __func__,name);
		WARN_ON(1);
		return -ENODEV;
	}

	if (!idev->dev) {
		idev->dev = input_allocate_device();
		if (!idev->dev) {
			pr_err("%s: failed to allocate an input device.\n",
				__func__);
			return -ENOMEM;
		}
		idev->dev->name = idev->name;
		idev->dev->id.bustype = idev->bustype;
	}

	bw->input_dev = idev->dev;
	input_set_capability(bw->input_dev, EV_KEY, bw->key);
	return 0;
}

static int __button_init(struct button_work *base, struct button_work *parent)
{
	int err = 0, success = 0;
	struct button_work *bw;
	bool start_poller = false;

	for (bw = base; bw->name; bw++) {
		if (parent)
			bw->private = parent;

		if (bw->gpio_label) {
			pr_info("%s: %s '%s' %s:%d%s (%s)\n", __func__,
				button_input_dev_name(bw),
				bw->name,
				bw->gpio_label, bw->gpio_num,
				bw->gpio_active_low ? "n" : "",
				button_type_str(bw));
			err = setup_gpio_input(bw);
			if (err) {
				pr_err("%s: cannot find button GPIO %s:%d\n",
					 __func__,
					bw->gpio_label, bw->gpio_num);
				continue;
			}
		} else
			pr_info("%s: %s '%s' (%s)\n", __func__,
				button_input_dev_name(bw),
				bw->name, button_type_str(bw));

		if (BUTTON_TYPE_IS_IRQ(bw))
			INIT_WORK(&bw->worker,
				bw->worker_to_set ?
					bw->worker_to_set : button_worker);

		/* This must come after GPIO attaching and worker init
		 * to avoid race.
		 */
		if (bw->init && (err = bw->init(bw))) {
			pr_err("%s: failed to initialize button '%s' (%d).\n",
				__func__, bw->name, err);
			continue;
		}

		bw->prev_button_state = button_depressed(bw);

		/* If button has subwork(s), call recursively. */
		if (bw->type & BUTTON_TYPE_SUBWORK)
			err = __button_init(
				(struct button_work *)bw->private, bw);
		else {
			if ((err = create_button_state_proc(bw)))
				goto exit;
			if ((err = rn_set_button(bw)))
				goto exit;
		}
exit:
		if (err) {
			pr_err("%s: failed to set up button worker %s (%d).\n",
				__func__, bw->name, err);
			continue;
		}

			/* Must be delayed until all buttons get ready. */
		if (BUTTON_TYPE(bw) == BUTTON_TYPE_POLL)
			start_poller = true;
		else if (BUTTON_TYPE_IS_IRQ(bw))
			pr_info("%s '%s' IRQ#%d\n",
				button_input_dev_name(bw), bw->name, bw->irq);
		success++;
	}

	if (!success)
		return err;

	/* Delayed starting of button poller thread - avoid race. */
	if (start_poller && (err = button_poller_start()))
		return err;

	if ((err = create_button_thread_proc())) {
		proc_remove(poller_proc);
		poller_proc = NULL;
		button_poller_stop();
	}

	return err;
}

static int button_init(void)
{
	int err, i;
	struct button_set *bs =
			__compatible_find(button_set, compatible, 0, "");

	pr_info("%s: initializing ReadyNAS button set.\n", __func__);
	button_work = &bs->button_work[0];

	if ((err = __button_init(button_work, NULL)))
		return err;

	/* Delayed input_dev registration after all buttons added. */
	for (i = 0; rn_input_devs[i].name; i++)
		if (rn_input_devs[i].dev &&
			(err = input_register_device(rn_input_devs[i].dev))) {
			pr_err("%s: failed to install button input class device"
				" %s.\n", __func__, rn_input_devs[i].name);
			input_free_device(rn_input_devs[i].dev);
			rn_input_devs[i].dev = NULL;
			button_exit();
			return err;
		}

	return 0;
}

/*
 * PWR button status procfs handling
 */
#define	MK_ADDR_BIT_GPIO_NUM(a, b)	(((a)<<16)|(b))
#define	BK_ADDR_GPIO_NUM(bw)		((bw)->gpio_num >>16)
#define	BK_BIT_GPIO_NUM(bw)		BIT((bw)->gpio_num & 0xffff)

static int locate_acpi_devs(struct button_work *bw,
				struct platform_device **pdevp,
				struct pci_dev **pci_devp)
{
	struct device *dev;
	struct gpio_chip *gc = rn_gpiochip_find_by_label(bw->gpio_label);

	if (!gc || !(dev = gc->dev)) {
		pr_err("%s: failed to find GPIO %s.\n",
			__func__, bw->gpio_label);
		return -ENODEV;
	}

	if (pdevp)
		*pdevp = container_of(dev, struct platform_device, dev);
	if (pci_devp)
		*pci_devp = container_of(dev->parent, struct pci_dev, dev);
	return ((*pci_devp)->vendor && (*pci_devp)->device) ? 0 : -ENODEV;
}

static int pwr_button_depressed_mmio(struct button_work *bw)
{
	unsigned long offset = BK_ADDR_GPIO_NUM(bw);
	unsigned long base = (unsigned long)bw->private + (offset & PAGE_MASK);
	void __iomem *p = ioremap(base, PAGE_SIZE);
	int pressed =
		!!(*(u32 *)(p + (offset % PAGE_SIZE)) & BK_BIT_GPIO_NUM(bw));

	iounmap(p);
	return bw->gpio_active_low ? !pressed : pressed;
}

static int __init pwr_button_init_mmio_by_pci(struct button_work *bw)
{
	struct pci_dev *pci_dev;
	u32 pm_base;
	int err = locate_acpi_devs(bw, NULL, &pci_dev);

	if (err)
		return err;

	/* Get PM base address */
	pci_read_config_dword(pci_dev, (unsigned long)bw->private2, &pm_base);
	pm_base &= PCI_BASE_ADDRESS_IO_MASK;
	if (!pm_base) {
		pr_err("%s: failed to get pm base.", __func__);
		return -ENODEV;
	}

	bw->private = (void *)(unsigned long)pm_base;
	return 0;
}

static int pwr_button_depressed_pci(struct button_work *bw)
{
	struct pci_dev *pci_dev = bw->private;
	u32 pmcon;
	int pressed;

	pci_read_config_dword(pci_dev, BK_ADDR_GPIO_NUM(bw), &pmcon);
	pressed = !!(pmcon & BK_BIT_GPIO_NUM(bw));
	return bw->gpio_active_low ? !pressed : pressed;
}

static int __init pwr_button_init_pci(struct button_work *bw)
{
	struct pci_dev *pci_dev = NULL;
	int err = -ENODEV;

	if (bw->gpio_label)
		err = locate_acpi_devs(bw, NULL, &pci_dev);
	else if (bw->private2) {
		pci_dev = pci_get_device(PCI_VENDOR_ID_INTEL,
					(unsigned long)bw->private2, NULL);
		if (pci_dev)
			err = 0;
	}

	if (err)
		return err;

	bw->private = pci_dev;
	return 0;
}

/* Here's the modified device information convention:
 * gpio_label: if present, search for the PCI device for this GPIO.
 * private: 1) MMIO base address set by GPIO search
 *          2) preset MMIO base address if not for gpio_label or PCI ID
 *          3) or PCI device pointer if register is in PCI space
 * private2: 1) preset PCI device ID if not for gpio_label
 *           2) or preset PCI config BAR offset if register is in PCI space
 * gpio_num: combined PMC registger offset and bit number
 */
static struct button_work button_work_pwr[] = {
	{
#define	C2388_PMCON2		0x24
#define	C2388_PWRBTN_LVL_BIT	9
#define	C2388_PBASE_ADDR	0x44
		.name		= "pwr",
		.input_dev_name	= "rn313x",
		.gpio_label	= "gpio_ich",
		.gpio_num	= MK_ADDR_BIT_GPIO_NUM(C2388_PMCON2,
						C2388_PWRBTN_LVL_BIT),
		.private2	= (void *)C2388_PBASE_ADDR,
		.init		= pwr_button_init_mmio_by_pci,
		.depressed	= pwr_button_depressed_mmio,
		.gpio_active_low	= 1,
	}, {
		.name		= "pwr",
		.input_dev_name	= "rrx312_60",
		.gpio_label	= "gpio_it87",
		.gpio_num	= 27,	/* a.k.a. GP43 */
		.gpio_active_low	= 1,
		.init		= setup_gpio_input,
	}, {
#define PCI_DEVICE_ID_DENVERTON	0x19de
#define PCI_PMCON_A_DNV		0xa0
#define PCI_PWRBTN_LVL_BIT_DNV	9
		.name		= "pwr",
		.input_dev_name	= "rn42x",
		.gpio_num	= MK_ADDR_BIT_GPIO_NUM(PCI_PMCON_A_DNV,
						PCI_PWRBTN_LVL_BIT_DNV),
		.private2	= (void *)PCI_DEVICE_ID_DENVERTON,
		.init		= pwr_button_init_pci,
		.depressed	= pwr_button_depressed_pci,
		.gpio_active_low	= 1,
	}, {
#define MMIO_PMCON2_APL		0x1024
#define MMIO_PWRBTN_LVL_BIT_APL	9
		.name		= "pwr",
		.input_dev_name	= "rr2304",
		.gpio_num	= MK_ADDR_BIT_GPIO_NUM(MMIO_PMCON2_APL,
						MMIO_PWRBTN_LVL_BIT_APL),
		.private	= (void *)0xfe042000,
		.depressed	= pwr_button_depressed_mmio,
		.gpio_active_low	= 1,
	}, {/* Same Denverton as RN42x */
		.name		= "pwr",
		.input_dev_name	= "rr2312",
		.gpio_num	= MK_ADDR_BIT_GPIO_NUM(PCI_PMCON_A_DNV,
						PCI_PWRBTN_LVL_BIT_DNV),
		.private2	= (void *)PCI_DEVICE_ID_DENVERTON,
		.init		= pwr_button_init_pci,
		.depressed	= pwr_button_depressed_pci,
		.gpio_active_low	= 1,
	}, {
#define PCI_ICH_GEN_PMCON_1	0xa0
#define ICH_PWRBTN_LVL_BIT	9
		.name		= "pwr",
		.input_dev_name	= "",	/* Wildcard for all others. */
		.gpio_label	= "gpio_ich",
		.gpio_num	= MK_ADDR_BIT_GPIO_NUM(PCI_ICH_GEN_PMCON_1,
						ICH_PWRBTN_LVL_BIT),
		.init		= pwr_button_init_pci,
		.depressed	= pwr_button_depressed_pci,
		.gpio_active_low	= 1,
	}, {}
}, *bw_pwr;

static int __init pwr_button_state_init(void)
{
	int err = 0;
	struct button_work *bw_pwr =
		__compatible_find(button_work_pwr, input_dev_name, -ENODEV, "");

	pr_info("%s: initializing ReadyNAS PWR button state handler %s%s.\n",
		__func__, *bw_pwr->input_dev_name ? "for " : "",
			bw_pwr->input_dev_name);

	if (bw_pwr->init)
		err = bw_pwr->init(bw_pwr);

	if (err == -ENODEV) {
		pr_warn("%s: No PWR button found.\n", __func__);
		return 0;
	} else if (err) {
		pr_err("%s: failed for GPIO %s\n", __func__,
			bw_pwr->gpio_label ? bw_pwr->gpio_label
					: bw_pwr->input_dev_name);
		return err;
	}
	return create_button_state_proc(bw_pwr);
}

static void pwr_button_state_exit(void)
{
	__button_exit_one(bw_pwr);
}

#include "sx8635.h"

static int rn316_x16_local_init(void *d)
{
	struct i2c_adapter *adapter =
			readynas_i2c_adapter_find((const char **)d);

	if (IS_ERR_OR_NULL(adapter)) {
		pr_err("%s: failed to find I2C adapter.\n", __func__);
		return adapter ? PTR_ERR(adapter) : -ENODEV;
	}
	return sx8635_i2c_init(adapter);
}

static void rn316_x16_local_exit(void *d)
{
	sx8635_i2c_exit();
}


static struct local_init_exit {
	const char *compatible;
	int  (*init)(void *data);
	void (*exit)(void *data);
	void *data;
}  local_init_exit_pre[] = {
	{}
}, local_init_exit_post[] = {
	{	"rn316", rn316_x16_local_init, rn316_x16_local_exit,
		rn_i2c_adapters_ipch
	}, {	"rnx16", rn316_x16_local_init, rn316_x16_local_exit,
		rn_i2c_adapters_ipch
	}, {}
};

static int local_init(struct local_init_exit *lie)
{
	int rv = 0;

	while (lie->compatible) {
		if (readynas_io_compatible(lie->compatible) && lie->init)
			rv |= lie->init(lie->data);
		lie++;
	}
	return rv;
}

static void local_exit(struct local_init_exit *lie)
{
	while (lie->compatible) {
		if (readynas_io_compatible(lie->compatible) && lie->exit)
			lie->exit(lie->data);
		lie++;
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
		pr_err("%s: cannot find DMI device\n", __func__);
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

static int model_show(struct seq_file *s, void *p)
{
	const char *dmi[2];
	int i;
	bool printed = false;

	dmi[0] = dmi_get_system_info(DMI_PRODUCT_NAME);
	dmi[1] = dmi_get_system_info(DMI_PRODUCT_VERSION);

	for (i = 0; i < ARRAY_SIZE(dmi); i++)
		if (dmi[i] && dmi[i][0]) {
			seq_printf(s, "%s\n", dmi[i]);
			printed = true;
		}
	return printed ? 0 : -EIO;
}

static int model_open(struct inode *inode, struct file *file)
{
	return single_open(file, model_show, NULL);
}

static const struct file_operations model_fop = {
	.open	= model_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release= single_release,
};

#if 0	/* PCI register reading for debugging. */
static int __pci_config_read(struct seq_file *s, void *p)
{
	int i = 0;
	struct pci_dev *pci_dev = s->private;
	static const char *__pci_reg[] = {
		"Status:Command__Device ID:Vendor ID",
		"BIST.Hdr Type.Lst Timer.Cache Line S__Class Code.Rev ID",
		"BAR__BAR", "BAR__BAR", "BAR__BAR",
		"Subsystem ID:Subsystem Vendor ID__Card bus CIS Ptr",
		"Rsvd.Cap. Ptr__Expnsn ROM BAR",
		"Max Lst.Min Gnt.Intrpt Pin.Intrpt Line__Rsvd",
	};

	seq_printf(s, "%02x.%02x:%x\n", pci_dev->bus->number,
			PCI_SLOT(pci_dev->devfn), PCI_FUNC(pci_dev->devfn));
	while (i < 256) {
		u32 x, y;

		pci_read_config_dword(pci_dev, i, &x);
		pci_read_config_dword(pci_dev, i + 4, &y);
		seq_printf(s, "[%02x]=%08x_%08x %s\n", i, y, x,
			i/8 < ARRAY_SIZE(__pci_reg) ? __pci_reg[i/8] : "");
		i += 8;
	}
	return 0;
}

static int __pci_config_read_open(struct inode *inode, struct file *file)
{
	return single_open(file, __pci_config_read, PDE_DATA(inode));
}

static const struct file_operations __pci_config_read_fop = {
	.open	= __pci_config_read_open,
	.read	= seq_read,
	.llseek	= seq_lseek,
	.release= single_release,
};

static struct proc_dir_entry **__pci_config_read_procs, *__pci_config_dir_proc;

static void __pci_config_read_exit(void)
{
	int i = 0;

	if (!__pci_config_read_procs)
		return;
	while (__pci_config_read_procs[i])
		proc_remove(__pci_config_read_procs[i++]);
	kfree(__pci_config_read_procs);
	__pci_config_read_procs = NULL;
	proc_remove(__pci_config_dir_proc);
	__pci_config_dir_proc = NULL;
}

static int __pci_config_read_init(void)
{
	struct pci_dev *pci_dev = NULL;
	int count = 0;

        if (!proc_mkdir("pci", readynas_proc_root))
		return -EIO;

	for_each_pci_dev(pci_dev) {
		char name[16];
		struct proc_dir_entry *proc;

		sprintf(name, "pci/%04x:%04x",
				pci_dev->device, pci_dev->vendor);
		if ((proc = readynas_proc_create(name, 0444,
				&__pci_config_read_fop, (void *)pci_dev)))
			if (!(__pci_config_read_procs =
				krealloc(__pci_config_read_procs,
		(++count + 1) * sizeof(*__pci_config_read_procs), GFP_KERNEL)))
				return -ENOMEM;
		__pci_config_read_procs[count - 1] = proc;
		__pci_config_read_procs[count] = NULL;
	}
	return 0;
}
#else
static void __pci_config_read_exit(void) {}
static int __pci_config_read_init(void) {return 0;}
#endif

static struct proc_dir_entry
	*proc_bootmenu_enable, *proc_bootmenu_option, *proc_model;

static void procfs_exit(void)
{
	__pci_config_read_exit();
	if (proc_model);
		proc_remove(proc_model);
	if (proc_bootmenu_option);
		proc_remove(proc_bootmenu_option);
	if (proc_bootmenu_enable);
		proc_remove(proc_bootmenu_enable);
	if (readynas_proc_root);
		proc_remove(readynas_proc_root);
	proc_model		= NULL;
	proc_bootmenu_option	= NULL;
	proc_bootmenu_enable	= NULL;
	readynas_proc_root	= NULL;
}

static int __init procfs_init(void)
{
	int err = -EIO;
	pr_info("%s: initializing ReadyNAS procfs.\n", __func__);

	readynas_proc_root = proc_mkdir("readynas", NULL);
	if (!readynas_proc_root) {
		pr_err("%s: failed to create /proc/readynas\n", __func__);
		return -EIO;
	}
	if (!(proc_bootmenu_enable =
			readynas_proc_create("bootmenu-enable", 0444,
				&bootmenu_fop, (void *)"enable")) ||
		!(proc_bootmenu_option =
			readynas_proc_create("bootmenu-option", 0444,
				&bootmenu_fop, (void *)"option")) ||
		!(proc_model = readynas_proc_create("model", 0444,
				&model_fop, NULL)) ||
		(err = __pci_config_read_init())) {
		pr_err("%s: failed to install procfs.\n", __func__);
		procfs_exit();
		return err;
	}
	return 0;
}

static void readynas_io_exit(void)
{
	local_exit(local_init_exit_post);
	button_exit();
	pwr_button_state_exit();
	rnx4_lcd_exit();
	readynas_gpio_exit();
	local_exit(local_init_exit_pre);
	procfs_exit();
}

static int __init readynas_io_init(void)
{
	int ret;

	pr_info("%s: initializing ReadyNAS I/O.\n", __func__);
	mutex_init(&poller_lock);
	if ((ret = procfs_init()))
		return ret;
	if ((ret = local_init(local_init_exit_pre)))
		return ret;
	if ((ret = readynas_gpio_init()))
		goto fail;
	ret |= rnx4_lcd_init();
	ret |= pwr_button_state_init();
	ret |= button_init();
	ret |= local_init(local_init_exit_post);
	pr_info("%s: initialization %s completed.\n", __func__,
		ret ? "partially" : "successfully");
	return 0;

fail:
	readynas_io_exit();
	pr_err("%s: initialization failed (%d).\n", __func__, ret);
	return ret;
}

/* This module must be loaded after GPIO and I2C. */
device_initcall_sync(readynas_io_init);
module_exit(readynas_io_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NETGEAR Intel I/O driver v3");
MODULE_AUTHOR("hiro.sugawara@netgear.com");
