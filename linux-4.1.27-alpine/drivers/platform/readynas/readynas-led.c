/*
 * LEDs driver for NETGEAR ReadyNAS x86_64 platforms
 *
 * Copyright (C) 2015 Justin Maggard <justin.maggard@netgear.com>
 * Copyright (C) 2015-2016 Hiro.Sugawara <hiro.sugawara@netgear.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This driver provides all x86 platforms with a unified API using
 * /sys/class/leds/readynas:color:led/func.
 */

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/dmi.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/syscalls.h>
#include <linux/spinlock.h>
#include <linux/libata.h>
#include <scsi/scsi_host.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <../drivers/gpio/gpiolib.h>

struct rnled_info {
	const	char *name;
	const	int  num;		/* GPIO port number */
	const	struct rnled_proc *proc;
	const	unsigned int active_low; /* Bit#0: primary, Bit#1: secondary */
	const	int  init_state;	/* -1 for off, 1 for on, 0 for as-is. */
	const	char *trigger;
	void	*desc;
	void	*desc2;			/* Secondary GPIO for reading */
	struct	led_classdev cdev;
	bool	registered;
	int	uid_busy;	/* Wait time in ms. */
	struct timer_list timer;
};

struct rnled_proc {
	const	char *label;	/* GPIO label */
	void (*init)(struct rnled_info *, const char *);
	void (*exit)(struct rnled_info *);
	enum led_brightness (*get)(struct led_classdev *);
	/* setter returns 0 for success, 1 for busy, -1 for error. */
	int (*set)(struct rnled_info *, int);
};

static enum led_brightness led_get_gpio(struct led_classdev *led_cdev)
{
	struct rnled_info *led =
		container_of(led_cdev, struct rnled_info, cdev);
	return gpiod_get_value(led->desc);
}

/*
 * Under led_servicer, all LED accesses (except UID) are serialized
 * and no need for locking.
 */
static int led_set_gpio(struct rnled_info *led, int val)
{
	gpiod_set_value(led->desc, val);
	return 0;
}

static int gpio_match(struct gpio_chip *chip, void *data)
{
	return !strcmp(chip->label, (const char *)data);
}

static void led_init_gpio(struct rnled_info *led, const char *label)
{
	int state, err;
	struct gpio_chip *chip = gpiochip_find((void *)label, gpio_match);

	if (!chip) {
		pr_err("%s GPIO chip %s not found\n", __func__, label);
		return;
	}

	led->desc = gpio_to_desc(led->num + chip->base);
	if (IS_ERR_OR_NULL(led->desc))
		return;

	if (led->active_low & 0x1)
		set_bit(FLAG_ACTIVE_LOW,
			&((struct gpio_desc *)led->desc)->flags);

	if (led->init_state < 0)
		state = 0;
	else if (led->init_state > 0)
		state = 1;
	else if (led->proc->get)
		state = led->proc->get(&led->cdev);
	else
		state = 0;

	if ((err = gpiod_direction_output(led->desc, state)) && err != -EIO) {
		/* EIO is returned if no direction_output. */
		led->desc = ERR_PTR(err);
		return;
	}

	led->cdev.brightness = state;
}

static const struct rnled_proc led_gpio_ich = {
	.label	= "gpio_ich",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

#define IBPI_PATTERN_NORMAL		0x00000000
#define IBPI_PATTERN_DEGRADED		0x00200000
#define IBPI_PATTERN_REBUILD		0x00480000
#define IBPI_PATTERN_FAILED_DRIVE	0x00400000
#define IBPI_PATTERN_LOCATE		0x00080000
static int led_set_scsi(struct rnled_info *led, int val)
{ 
	struct Scsi_Host *shost;
	struct ata_port *ap;
	u32 message;

	/* get disk index */
	if (led->num < 0)
		return -1;

	shost = scsi_host_lookup(led->num);
	if (!shost)
		return -1;
	ap = ata_shost_to_port(shost);
	if (!ap || !ap->ops->transmit_led_message)
		return -1;
	message = val ? IBPI_PATTERN_FAILED_DRIVE : IBPI_PATTERN_NORMAL;
	return ap->ops->transmit_led_message(ap, message, 4) > 0 ? 0 : -1;
}

static const struct rnled_proc led_scsi = {
	.set	= led_set_scsi,
};

/* IT8732 SuperIO GPIO */

static const struct rnled_proc led_gpio_it87 = {
	.label	= "gpio_it8732",
	.init	= led_init_gpio,
	.get	= led_get_gpio,
	.set	= led_set_gpio,
};

static enum led_brightness led_get_gpio2(struct led_classdev *led_cdev)
{
	struct rnled_info *led =
		container_of(led_cdev, struct rnled_info, cdev);
	return gpiod_get_value(led->desc2);
}

/*
 * The LED class driver implements blinking and one-shot LED control
 * using kernel timers. This causes a problem if the underlying LED
 * driver (GPIO) could sleep in msleep or mutex_lock because a timer callback
 * runs in a softirq context. A dedicated kernel thread isolates the
 * sleeping LED driver from LED class driver's preference.
 *
 * UID LED is turned on and off with a long (200ms) pulse in a flip-flop
 * manner. A timer delay release the pulse after the delay, then another
 * timer delay disables subsequent LED operation requests until a recovery
 * period (another 200ms) gets elapsed.
 */
static struct led_service {
	const char *name;
	struct task_struct *kthread;
	wait_queue_head_t wq;
	spinlock_t lock;
	int count;
	struct led_set_request {
		struct list_head list;
		struct rnled_info *led;
		int val;
	} req;
} led_service = {.name = "readynas-led", };

/* UID LED is set with a flip-flop */
/* This is a timer callback (a.k.a. softirq). */
static void uid_delay(unsigned long d)
{
	struct rnled_info *led = (struct rnled_info *)d;

	if (gpiod_get_value(led->desc)) {
		/* Flip-flop pulse is active. Request servicer to deactivate
		   it and wait for recovery time.
		 */
		struct led_service *service = &led_service;
		struct led_set_request *req = kmalloc(sizeof *req, GFP_ATOMIC);

		if (!req) {
			pr_err("%s: kmalloc failed.\n", __func__);
			return;
		}

		req->led = led;
		req->val = -1;	/* UID pulse release */
		spin_lock(&service->lock);
		list_add_tail(&req->list, &service->req.list);
		service->count++;
		spin_unlock(&service->lock);
		wake_up(&service->wq);
	} else
		led->uid_busy = 0;
}

#define UID_PULSE_MS		200
#define UID_PULSE_WAIT_TICK	20
static int led_set_gpio_uid(struct rnled_info *led, int val)
{
	if (val >= 0) {
		/* No change. */
		if (val == gpiod_get_value(led->desc2))
			return 0;
		if (led->uid_busy) {
			/* Previous operation is pending - retry. */
			msleep(UID_PULSE_WAIT_TICK);
			/* Overkilling caution for timeout. */
			led->uid_busy -= UID_PULSE_WAIT_TICK;
			if (led->uid_busy < 0)
				led->uid_busy = 0;
			return 1;
		}

		/* Flip it -- initiate a pulse. */
		gpiod_set_value(led->desc, 1);
		/* Prepare for timeout. */
		led->uid_busy = UID_PULSE_MS * 10;
	} else {
		/* Release the pulse */
		gpiod_set_value(led->desc, 0);
	}

	/* Wait so long as HW debouncer requires. */
	mod_timer(&led->timer, jiffies + msecs_to_jiffies(UID_PULSE_MS));
	return 0;
}

static void led_init_gpio_uid(struct rnled_info *led, const char *label)
{
	int state, err;

	led_init_gpio(led, label);
	if (IS_ERR_OR_NULL(led->desc))
		return;

	if (!led->desc2)
		return;

	led->desc2 = gpio_to_desc((unsigned long)led->desc2 +
				((struct gpio_desc *)led->desc)->chip->base);
	if (IS_ERR_OR_NULL(led->desc2)) {
		led->desc = ERR_PTR(-ENODEV);
		return;
	}

	if (led->active_low & 0x2)
		set_bit(FLAG_ACTIVE_LOW,
			&((struct gpio_desc *)led->desc2)->flags);

	if ((err = gpiod_direction_input(led->desc2)) && err != -EIO) {
		led->desc = ERR_PTR(err);
		return;
	}
	state = gpiod_get_value(led->desc2);
	led->cdev.brightness = state;
	led->cdev.brightness_get = led_get_gpio2;
	init_timer(&led->timer);
	setup_timer(&led->timer, uid_delay, (unsigned long)led);
}

static void led_exit_gpio_uid(struct rnled_info *led)
{
	del_timer(&led->timer);
	led->uid_busy = false;
}

static const struct rnled_proc led_gpio_it87_rn3130_uid = {
	.label	= "gpio_it8732",
	.init	= led_init_gpio_uid,
	.exit	= led_exit_gpio_uid,
	.set	= led_set_gpio_uid,
};

static const char *compatible_ultra2[] = { "ReadyNAS-ProUltra2", NULL};
static struct rnled_info leds_ultra2[] = {
	{ "readynas:blue:pwr",     28, &led_gpio_ich, 1, 0, "timer" },
	{ "readynas:green:backup", 22, &led_gpio_ich, 1, },
	{ "readynas:green:sata1",  6,  &led_gpio_ich, 1, },
	{ "readynas:green:sata2",  7,  &led_gpio_ich, 1, },
	{}
};

static const char *compatible_pro6[]   = { "FLAME6-MB", "Pro6", NULL};
static const char *compatible_ultra6[] = { "FLAME6-2", "Ultra6", NULL };
static struct rnled_info leds_pro6_ultra6[] = {
	{ "readynas:green:backup", 20, &led_gpio_ich, 0, },
	{},
};

static const char *compatible_ultra4[] = { "ReadyNAS-NVX-V2", "Ultra4", NULL};
static const char *compatible_rn312[] = { "ReadyNAS 312", "RN312", NULL};
static const char *compatible_rn314[] = { "ReadyNAS 314", "RN314", NULL};
static struct rnled_info leds_ultra4_rn312_rn314[] = {
	{ "readynas:blue:pwr",     28, &led_gpio_ich, 1, 0, "timer" },
	{ "readynas:green:backup", 22, &led_gpio_ich, 1, },
	{ "readynas:green:sata1",  16, &led_gpio_ich, 1, },
	{ "readynas:green:sata2",  20, &led_gpio_ich, 1, },
	{ "readynas:green:sata3",  6,  &led_gpio_ich, 1, },
	{ "readynas:green:sata4",  7,  &led_gpio_ich, 1, },
	{}
};

static const char *compatible_rnx16[] = { "ReadyNAS 316", "RN316",
					  "ReadyNAS 516", "RN516",
					  "ReadyNAS 716", "RN716", NULL};
static const char *compatible_rnx26[] = { "ReadyNAS 526", "RN526",
					  "ReadyNAS 626", "RN626", NULL};
static struct rnled_info leds_rnx16[] = {
	{ "readynas:red:sata1",	 0, &led_scsi, },
	{ "readynas:red:sata2",	 1, &led_scsi, },
	{ "readynas:red:sata3",	 2, &led_scsi, },
	{ "readynas:red:sata4",	 3, &led_scsi, },
	{ "readynas:red:sata5",	 4, &led_scsi, },
	{ "readynas:red:sata6",	 5, &led_scsi, },
	{}
};

static const char *compatible_rn3130[] =
		{ "ReadyNAS 3130", "ReadyNAS3130", "RN3130", NULL};
#define	SUS(n)	((n)+32)
static struct rnled_info leds_rn3130[] = {
	{ "readynas:green:pwr",	 39,	&led_gpio_it87, 1, 0, "timer" },
	{ "readynas:red:err",	 24,	&led_gpio_it87, 1, },
	{ "readynas:blue:uid",	 53,	&led_gpio_it87_rn3130_uid, 1, 0,
					NULL, NULL, (void*)55},
	/* HDD power control (prototype HW only) */
	{ ".readynas::sata1_pwr", 11,	&led_gpio_ich, 0, 1, },
	{ ".readynas::sata2_pwr", 15,	&led_gpio_ich, 0, 1, },
	{ ".readynas::sata3_pwr", SUS(17),	&led_gpio_ich, 0, 1, },
	{ ".readynas::sata4_pwr", 18,	&led_gpio_ich, 0, 1, },

	{ "readynas:green:sata1", 12,	&led_gpio_ich, },
	{ "readynas:green:sata2", 20,	&led_gpio_ich, },
	{ "readynas:green:sata3", 19,	&led_gpio_ich, },
	{ "readynas:green:sata4", 17,	&led_gpio_ich, },
	{ "readynas:red:sata1", SUS(21),	&led_gpio_ich, },
	{ "readynas:red:sata2", SUS(19),	&led_gpio_ich, },
	{ "readynas:red:sata3", SUS(22),	&led_gpio_ich, },
	{ "readynas:red:sata4", 27,	&led_gpio_ich, },
	{}
};

static const struct rnled_proc led_gpio_nct6775 = {
	.label	= "gpio_nct6775",
	.init	= led_init_gpio,
	.get	= led_get_gpio,
	.set	= led_set_gpio,
};

static const struct rnled_proc led_gpio_rnx220pic = {
	.label	= "gpio_rnx220pic",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

static const struct rnled_proc led_gpio_nct6775_rnx220_uid = {
	.label	= "gpio_nct6775",
	.init	= led_init_gpio_uid,
	.set	= led_set_gpio_uid,
};

static const struct rnled_proc led_gpio_rnx220marvell = {
	.label	= "gpio_rnx220marvell",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

static const char *compatible_rnx220[] =
		{ "ReadyNAS 3220", "ReadyNAS3220", "RN3220",
		  "ReadyNAS 4220", "ReadyNAS4220", "RN4220", NULL};
static struct rnled_info leds_rnx220[] = {
	{ "readynas:green:pwr",	 72,	&led_gpio_nct6775, 0, 0, "timer" },
	{ "readynas:red:err",	 5,	&led_gpio_ich, 1},
	{ "readynas:blue:uid",	 8,	&led_gpio_nct6775_rnx220_uid, 3, 0,
					NULL, NULL, (void*)9},
	{ "readynas:green:sata1",  0,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata2",  1,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata3",  0,	&led_gpio_rnx220marvell, },
	{ "readynas:green:sata4",  1,	&led_gpio_rnx220marvell, },
	{ "readynas:green:sata5",  4,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata6",  5,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata7",  6,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata8",  7,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata9",  8,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata10", 9,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata11",10,	&led_gpio_rnx220pic, },
	{ "readynas:green:sata12",11,	&led_gpio_rnx220pic, },
	{}
};

/* PCA9505 I2C GPIO */
static const struct rnled_proc led_gpio_pca9505 = {
	.label	= "pca9505",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

/* Intel surisepoint PCH-H GPIO */
static const struct rnled_proc led_gpio_spth = {
	.label	= "INT345D:00",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

static const char *compatible_rrx312[] = 
		 { "ReadyNAS 3312", "ReadyNAS3312", "RR3312",
		   "ReadyNAS 4312", "ReadyNAS4312", "RR4312", NULL};
static struct rnled_info leds_rrx312[] = {
	{ "readynas:green:pwr",	 14,	&led_gpio_it87, 0, 1, "timer" },
	{ "readynas:red:err",	 82,	&led_gpio_spth, 1, },
	{ "readynas:blue:uid",	 24,	&led_gpio_it87, 0, },

	{ ".readynas::sata1_pwr", 12,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata2_pwr", 13,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata3_pwr", 14,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata4_pwr", 15,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata5_pwr", 16,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata6_pwr", 17,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata7_pwr", 18,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata8_pwr", 19,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata9_pwr", 20,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata10_pwr",21,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata11_pwr",22,	&led_gpio_pca9505, 0, 1 },
	{ ".readynas::sata12_pwr",23,	&led_gpio_pca9505, 0, 1 },

	{ "readynas:red:sata1",   24,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata2",   25,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata3",   26,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata4",   27,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata5",   28,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata6",   29,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata7",   30,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata8",   31,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata9",   32,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata10",  33,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata11",  34,	&led_gpio_pca9505, 1 },
	{ "readynas:red:sata12",  35,	&led_gpio_pca9505, 1 },
	{}
};

/*
 * Compatibility table for all models
 */
static const struct model_info {
	const char **compatible;
	struct rnled_info *leds;
	int   num_leds;
} rnmodels[] = {
	{ compatible_pro6,	leds_pro6_ultra6},
	{ compatible_ultra2,	leds_ultra2},
	{ compatible_ultra4,	leds_ultra4_rn312_rn314},
	{ compatible_ultra6,	leds_pro6_ultra6},
	{ compatible_rn312,	leds_ultra4_rn312_rn314, 4},
	{ compatible_rn314,	leds_ultra4_rn312_rn314},
	{ compatible_rnx16,	leds_rnx16},
	{ compatible_rnx26,	leds_rnx16},
	{ compatible_rn3130,	leds_rn3130},
	{ compatible_rnx220,	leds_rnx220},
	{ compatible_rrx312,	leds_rrx312},
	{}
},
*rnmodel;

/* Kernel thread to serialize LED accesses in process context. */
static int led_servicer(void *data)
{
	struct led_service *service = data;

	while (!kthread_should_stop()) {
		struct led_set_request *req, *q;
		unsigned long flags;

		wait_event_interruptible(service->wq,
					!list_empty(&service->req.list));

		spin_lock_irqsave(&service->lock, flags);
		/* Release UID pulse ASAP to avoid busy waiters. */
		list_for_each_entry_safe(req, q, &service->req.list, list) {
			if (req->val < 0 ||
				req->led->proc->set != led_set_gpio_uid)
				break;
		}
		if (!req || req->val >= 0)
			req = list_first_entry_or_null(&service->req.list,
						struct led_set_request, list);
		if (req) {
			list_del(&req->list);
			service->count--;
		}
		spin_unlock_irqrestore(&service->lock, flags);

		if (req) {
			/* If UID LED is busy. Retry after a short sleep. */
			if (req->led->proc->set(req->led, req->val) > 0) {
				spin_lock_irqsave(&service->lock, flags);
				list_add(&req->list, &service->req.list);
				service->count++;
				spin_unlock_irqrestore(&service->lock, flags);
				continue;
			}

			kfree(req);
		}
	}
	return 0;
}

/* This common LED setter may be called in softirq (a.k.a. timer callback). */
static void led_set(struct led_classdev *led_cdev, enum led_brightness val)
{
	unsigned long flags;
	struct rnled_info *led = container_of(led_cdev, struct rnled_info,cdev);
	struct led_service *service = &led_service;
	struct led_set_request *req = kmalloc(sizeof *req, GFP_ATOMIC);

	if (!req) {
		pr_err("%s: malloc failed (%lu bytes)\n", __func__,sizeof *req);
		return;
	}
	req->led = led;
	req->val = !!val;

	spin_lock_irqsave(&service->lock, flags);
	if (service->count < 500) {
		list_add_tail(&req->list, &service->req.list);
		service->count++;
	} else
		pr_warn("%s: LED request queue full.\n", __func__);
	spin_unlock_irqrestore(&service->lock, flags);
	wake_up(&service->wq);
}

static int register_led(struct device *parent, struct rnled_info *led)
{
	pr_info("%s: registering led \"%s\"\n", __func__, led->name);

	led->cdev.name = led->name;
	led->cdev.brightness_get = led->proc->get;
	led->cdev.brightness_set = led_set;

	if (led->proc->init) {
		led->proc->init(led, led->proc->label);
		/* Errors happen only after an explicit init. */
		if (!led->desc)
			return -ENODEV;
		else if (IS_ERR(led->desc))
			return PTR_ERR(led->desc);
	}

	led->cdev.flags |= LED_CORE_SUSPENDRESUME;
	led->cdev.default_trigger = led->trigger;

	return led_classdev_register(parent, &led->cdev);
}

extern bool readynas_io_compatible(char const **model);

static int readynas_led_probe(struct platform_device *pdev)
{
	int i;
	struct model_info const *mp;

	for (rnmodel = rnmodels; rnmodel->compatible; rnmodel++)
		if (readynas_io_compatible(rnmodel->compatible))
			break;

	if (!rnmodel->compatible)
		return -ENODEV;
	mp = rnmodel;

	init_waitqueue_head(&led_service.wq);
	spin_lock_init(&led_service.lock);
	INIT_LIST_HEAD(&led_service.req.list);
	led_service.kthread = kthread_run(led_servicer, &led_service,
						led_service.name);
	if (!led_service.kthread) {
		pr_err("%s: failed to start %s service kthread.\n",
			__func__, led_service.name);
		return -ENOMEM;
	}

	for (i = 0; (!mp->num_leds || i < mp->num_leds) && mp->leds[i].name;
								i++) {
		int ret = register_led(&pdev->dev, &mp->leds[i]);

		if (ret) {
			kthread_stop(led_service.kthread);
			led_service.kthread = NULL;
			while (--i >= 0) {
				led_classdev_unregister(&mp->leds[i].cdev);
				mp->leds[i].registered = false;
			}
			return ret;
		}
		mp->leds[i].registered = true;
	}
	return 0;
}

static int readynas_led_remove(struct platform_device *pdev)
{
	int i;
	struct model_info const *mp = rnmodel;

	for (i = 0; i < mp->num_leds && mp->leds[i].name; i++)
		if (mp->leds[i].registered) {
			if (mp->leds[i].proc->exit)
				mp->leds[i].proc->exit(&mp->leds[i]);
			led_classdev_unregister(&mp->leds[i].cdev);
			mp->leds[i].registered = false;
		}

	if (led_service.kthread) {
		kthread_stop(led_service.kthread);
		led_service.kthread = NULL;
	}
	return 0;
}

static struct platform_driver readynas_led_driver = {
	.probe		= readynas_led_probe,
	.remove		= readynas_led_remove,
	.driver		= {
		.name	= "readynas-led",
	},
};

static struct platform_device *readynas_led_pdev;

static int __init readynas_led_init(void)
{
	int err = platform_driver_register(&readynas_led_driver);

	pr_info("%s: installing ReadyNAS LED driver.\n", __func__);
	if (err)
		return err;

	readynas_led_pdev =
		platform_device_register_simple(readynas_led_driver.driver.name,
						 -1, NULL, 0);
	if (IS_ERR(readynas_led_pdev))
		err = PTR_ERR(readynas_led_pdev);
	else if (!readynas_led_pdev)
		err = -ENODEV;
	if (err)
		platform_driver_unregister(&readynas_led_driver);
	return err;
}

static void __exit readynas_led_exit(void)
{
	platform_device_unregister(readynas_led_pdev);
	platform_driver_unregister(&readynas_led_driver);
}

/* This module must be loaded after readynas-io. */
device_initcall_sync(readynas_led_init);
module_exit(readynas_led_exit);

/*
 * For software UID button/switch toggling simulation
 */
struct rnled_info *rnled_get_info(const char *name)
{
	int i;

	if (!rnmodel)
		return NULL;
	for (i = 0; (!rnmodel->num_leds || i < rnmodel->num_leds) &&
			rnmodel->leds[i].name; i++) {
		if (!strcmp(rnmodel->leds[i].name, name))
			return &rnmodel->leds[i];
	}
	return NULL;
}
EXPORT_SYMBOL_GPL(rnled_get_info);

int rnled_toggle(struct rnled_info *led)
{
	int state;

	if (!led->proc->get)
		return -1;
	state = led->proc->get(&led->cdev);
	state = !state;
	led->proc->set(led, state);
	return state;
}
EXPORT_SYMBOL_GPL(rnled_toggle);

MODULE_AUTHOR("Justin Maggard <justin.maggard@netgear.com>");
MODULE_DESCRIPTION("NETGEAR ReadyNAS LED driver");
MODULE_LICENSE("GPL");
