/*
 * LEDs driver for NETGEAR ReadyNAS x86_64 platforms
 *
 * Copyright (C) 2015 Justin Maggard <justin.maggard@netgear.com>
 * Copyright (C) 2015-2017 Hiro.Sugawara <hiro.sugawara@netgear.com>
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
#include "readynas-io.h"

struct rnled_proc;
static struct rnled_info {
	const	char *name;
		u16  num;		/* GPIO port number */
	const	struct rnled_proc *proc;
	const	u8   active_low;	/* Bit#0: primary, Bit#1: secondary */
	const	int  init_state;	/* -1 for off, 1 for on, 0 for as-is. */
	const	char *trigger;
	void	*desc;
	void	*desc2;			/* Secondary GPIO for reading */
	struct	led_classdev cdev;
	bool	registered;
	int	uid_busy;	/* Wait time in ms. */
	struct timer_list timer;
} *rnleds;

struct rnled_proc {
	const	char *label;	/* GPIO label */
	void (*init)(struct rnled_info *, const char *);
	void (*exit)(struct rnled_info *);
	enum led_brightness (*get)(struct led_classdev *);
	/* setter returns 0 for success, 1 for busy, -1 for error. */
	int (*set)(struct rnled_info *, int);
};

static inline struct rnled_info *led_cdev_to_info(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct rnled_info, cdev);
}

static enum led_brightness led_get_gpio(struct led_classdev *led_cdev)
{
	struct rnled_info *led = led_cdev_to_info(led_cdev);

	return gpiod_get_value_cansleep(led->desc);
}

/*
 * Under led_servicer, all LED accesses (except UID) are serialized
 * and no need for locking.
 */
static int led_set_gpio(struct rnled_info *led, int val)
{
	gpiod_set_value_cansleep(led->desc, val);
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
		led->desc = ERR_PTR(-ENODEV);
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

static const struct rnled_proc led_gpio_dnv = {
	.label	= "gpio_dnv.0",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

static const struct rnled_proc led_gpio_apl = {
	.label	= "gpio_apl",
	.init	= led_init_gpio,
	.get	= led_get_gpio,
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
	.label	= "gpio_it87",
	.init	= led_init_gpio,
	.get	= led_get_gpio,
	.set	= led_set_gpio,
};

static enum led_brightness led_get_gpio2(struct led_classdev *led_cdev)
{
	struct rnled_info *led = led_cdev_to_info(led_cdev);

	/* Use desc2 only if it appears a valid gpiod (or it may be GPIO2#. */
	return gpiod_get_value_cansleep((long)led->desc2 < 0
					? led->desc2 : led->desc);
}

/*
 * The LED class driver implements blinking and one-shot LED control
 * using kernel timers. This causes a problem if the underlying LED
 * driver (GPIO) could sleep in msleep or mutex_lock because a timer callback
 * runs in a softirq context. A dedicated kernel thread isolates the
 * sleeping LED driver from LED class driver's preference.
 *
 * UID LED is turned on and off with a long (200ms) pulse in a flip-flop
 * manner. A timer delay releases the pulse after the delay, then another
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

	if (gpiod_get_value_cansleep(led->desc)) {
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
		if (val == (led->proc->get ? led->proc->get(&led->cdev)
				    : gpiod_get_value_cansleep(led->desc2)))
			return 0;	/* No change. */
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
		gpiod_set_value_cansleep(led->desc, 1);
		/* Prepare for timeout. */
		led->uid_busy = UID_PULSE_MS * 10;
	} else {
		/* Release the pulse */
		gpiod_set_value_cansleep(led->desc, 0);
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

	/* desc2 has GPIO# for get. */
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
	state = gpiod_get_value_cansleep(led->desc2);
	led->cdev.brightness = state;
	led->cdev.brightness_get = led_get_gpio2;
	init_timer(&led->timer);
	setup_timer(&led->timer, uid_delay, (unsigned long)led);
}

static void led_exit_gpio_uid(struct rnled_info *led)
{
	del_timer(&led->timer);
	led->uid_busy = 0;
}

static const struct rnled_proc led_gpio_it87_uid_rn313x = {
	.label	= "gpio_it87",
	.init	= led_init_gpio_uid,
	.exit	= led_exit_gpio_uid,
	.set	= led_set_gpio_uid,
};

static struct rnled_info leds_ultra2[] = {
	{ "readynas:blue:pwr",     28, &led_gpio_ich, 1, 0, "timer" },
	{ "readynas:green:backup", 22, &led_gpio_ich, 1, },
	{ "readynas:green:sata1",  6,  &led_gpio_ich, 1, },
	{ "readynas:green:sata2",  7,  &led_gpio_ich, 1, },
	{}
};

static struct rnled_info leds_pro6_ultra6[] = {
	{ "readynas:green:backup", 20, &led_gpio_ich, 0, },
	{},
};

static struct rnled_info leds_ultra4_rn312_rn314[] = {
	{ "readynas:blue:pwr",     28, &led_gpio_ich, 1, 0, "timer" },
	{ "readynas:green:backup", 22, &led_gpio_ich, 1, },
	{ "readynas:green:sata1",  16, &led_gpio_ich, 1, },
	{ "readynas:green:sata2",  20, &led_gpio_ich, 1, },
	{ "readynas:green:sata3",  6,  &led_gpio_ich, 1, },
	{ "readynas:green:sata4",  7,  &led_gpio_ich, 1, },
	{}
};

static struct rnled_info leds_rn424[] = {
	{ "readynas:red:sata1",  90, &led_gpio_dnv, },
	{ "readynas:red:sata2",  91, &led_gpio_dnv, },
	{ "readynas:red:sata3",  92, &led_gpio_dnv, },
	{ "readynas:red:sata4",  93, &led_gpio_dnv, },
	{}
};

static struct rnled_info leds_rn428[] = {
	{ "readynas:red:sata1",  91, &led_gpio_dnv, },
	{ "readynas:red:sata2",  90, &led_gpio_dnv, },
	{ "readynas:red:sata3",  93, &led_gpio_dnv, },
	{ "readynas:red:sata4",  92, &led_gpio_dnv, },
	{ "readynas:red:sata5",  23, &led_gpio_dnv, },
	{ "readynas:red:sata6",  22, &led_gpio_dnv, },
	{ "readynas:red:sata7",  40, &led_gpio_dnv, },
	{ "readynas:red:sata8",  39, &led_gpio_dnv, },
	{}
};

static struct rnled_info leds_rnx16_rn316[] = {
	{ "readynas:red:sata1",	 0, &led_scsi, },
	{ "readynas:red:sata2",	 1, &led_scsi, },
	{ "readynas:red:sata3",	 2, &led_scsi, },
	{ "readynas:red:sata4",	 3, &led_scsi, },
	{ "readynas:red:sata5",	 4, &led_scsi, },
	{ "readynas:red:sata6",	 5, &led_scsi, },
	{}
};

static struct rnled_info leds_rnx28[] = {
	{ "readynas:red:sata1",	48, &led_gpio_ich, },
	{ "readynas:red:sata2",	39, &led_gpio_ich, },
	{ "readynas:red:sata3",	38, &led_gpio_ich, },
	{ "readynas:red:sata4",	22, &led_gpio_ich, },
	{ "readynas:red:sata5",	67, &led_gpio_ich, },
	{ "readynas:red:sata6",	37, &led_gpio_ich, },
	{ "readynas:red:sata7",	36, &led_gpio_ich, },
	{ "readynas:red:sata8",	21, &led_gpio_ich, },
	{}
};

static struct rnled_proc led_gpio_it87_uid_rr23xx = {
	.label	= "gpio_it87",
	.init	= led_init_gpio_uid,
	.set	= led_set_gpio_uid,
	.get	= led_get_gpio2,
	.exit	= led_exit_gpio_uid,
};

static struct rnled_info leds_rr2304[] = {
	{ "readynas:green:pwr",	 24, &led_gpio_it87, 1, 0, "timer" }, /* GP40*/
	{ "readynas:red:err",	 28, &led_gpio_it87, 1, },	/* GP44 */
	{ "readynas:blue:uid",	 27, &led_gpio_it87_uid_rr23xx, 1,
				.desc2 = (void *)11, },	/* GP43, GP23 */

	{ "readynas:red:sata1",  90, &led_gpio_apl, },	/* AVS_I2S3_WS_SYNC */
	{ "readynas:red:sata2",  91, &led_gpio_apl, },	/* AVS_I2S3_SDI */
	{ "readynas:red:sata3",  86, &led_gpio_apl, },	/* AVS_I2S2_WS_SYNC */
	{ "readynas:red:sata4",  89, &led_gpio_apl, },	/* AVS_I2S3_BCLK */
	{}
};

static struct rnled_info leds_rr2312[] = {
	{ "readynas:green:pwr",	 24, &led_gpio_it87, 0, 0, "timer" }, /* GP40*/
	{ "readynas:red:err",	 44, &led_gpio_it87, },	/* GP64 */
	{ "readynas:blue:uid",	 10, &led_gpio_it87_uid_rr23xx, 1,
				.desc2 = (void *)11, },	/* GP22, GP23 */

	{ "readynas:red:sata1",  91, &led_gpio_dnv, },
	{ "readynas:red:sata2",  90, &led_gpio_dnv, },
	{ "readynas:red:sata3",  93, &led_gpio_dnv, },
	{ "readynas:red:sata4",  92, &led_gpio_dnv, },
	{ "readynas:red:sata5",  23, &led_gpio_dnv, },
	{ "readynas:red:sata6",  22, &led_gpio_dnv, },
	{ "readynas:red:sata7",  40, &led_gpio_dnv, },
	{ "readynas:red:sata8",  39, &led_gpio_dnv, },
	{ "readynas:red:sata9",  31, &led_gpio_dnv, },
	{ "readynas:red:sata10", 28, &led_gpio_dnv, },
	{ "readynas:red:sata11", 30, &led_gpio_dnv, },
	{ "readynas:red:sata12", 36, &led_gpio_dnv, },
	{}
};

#define	SUS(n)	((n)+32)
static struct rnled_info leds_rn313x[] = {
	{ "readynas:green:pwr",	 39,	&led_gpio_it87, 1, 0, "timer" },
	{ "readynas:red:err",	 24,	&led_gpio_it87, 1, },
	{ "readynas:blue:uid",	 53,	&led_gpio_it87_uid_rn313x, 1, 0,
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
	{ "readynas:red:sata4",   27,	&led_gpio_ich, },
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

static struct rnled_info leds_rnx220[] = {
	{ "readynas:green:pwr",	  72,	&led_gpio_nct6775, 0, 0, "timer" },
	{ "readynas:red:err",	   5,	&led_gpio_ich, 1},
	{ "readynas:blue:uid",	   8,	&led_gpio_nct6775_rnx220_uid, 3, 0,
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

/* Intel sunrisepoint PCH-H GPIO */
static const struct rnled_proc led_gpio_spth = {
	.label	= "INT345D:00",
	.init	= led_init_gpio,
	.set	= led_set_gpio,
};

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
 * RR4360 uses SAS SES to control disk LEDs.
 *
 * leds_rr4360[] is a template for front panel LEDs.
 */
static struct rnled_info leds_rr4360[] = {
	{ "readynas:blue:pwr",	 14,	&led_gpio_it87, 0, 1, "timer" },
	{ "readynas:red:err",	 82,	&led_gpio_spth, 1, },
	{ "readynas:blue:uid",	 24,	&led_gpio_it87, 0, },
	{}
};

#include <linux/enclosure.h>

static enum led_brightness led_get_ses(struct led_classdev *led_cdev)
{
	struct rnled_info *led = led_cdev_to_info(led_cdev);
	struct enclosure_component *ec = led->desc;
	struct enclosure_device *edev;

	if (!ec)
		return 0;

	edev = to_enclosure_device(ec->cdev.parent);
	if (edev->cb && edev->cb->get_locate)
		edev->cb->get_locate(edev, ec);
	return ec->locate;
}

static int led_set_ses(struct rnled_info *led, int val)
{
	struct enclosure_component *ec = led->desc;
	struct enclosure_device *edev;

	if (!ec)
		return -1;

	edev = to_enclosure_device(ec->cdev.parent);
	if (edev->cb && edev->cb->set_locate)
		edev->cb->set_locate(edev, ec, val);
	return 0;
}

struct rnled_proc led_ses = {
	.get	= led_get_ses,
	.set	= led_set_ses,
};

static const char *ses_led_name_temp = "readynas:red:sas%u";
#define SES_LED_NAME_LEN	(strlen(ses_led_name_temp) + 4)

/* We count only internal SAS drives excluding those on JBOD extenders. */
static inline int is_internal_ses(struct enclosure_device *edev)
{
	/* TODO */
	return (edev->components == 20);
}

static int num_ses_leds;
static int ses_enclosure(struct enclosure_device *edev, void *p)
{
	if (is_internal_ses(edev))
		num_ses_leds += edev->components;
	return 0;
}

/* RR4360 houses 60 SAS HDDs. 3 SAS motherboards each of which accepts
 * 20 HDDs. 20 SAS ports are divided into 4 5-port bunks in a staggering
 * layout. "Slot numbers" should be uniquely assigned and corresponding to
 * the physical slots, but the 20-port motherboards may not be unique or
 * appropriately installed.
 * First, try using the slot numbers, and, if it results bad, use
 * hardcoded slot mapping.
 */
static int ses_enclosure2(struct enclosure_device *edev, void *p)
{
	struct rnled_info *sesleds = p;
	int i;

	if (!is_internal_ses(edev))
		return 0;

	for (i = 0; i < edev->components; i++) {
		struct enclosure_component *ec = &edev->component[i];
		/* Slot number is 1-based. */
		struct rnled_info *led = &sesleds[ec->slot - 1];

		if (ec->slot <= num_ses_leds &&
			!memcmp(dev_name(&ec->cdev), "Disk", 4)) {
			if (led->name[0])
				return -EIO;
			sprintf((char *)led->name, ses_led_name_temp, ec->slot);
			led->desc = ec;
		}
	}
	return 0;
}

/* Returns newly allocated rnled_info array with dynamically filled LEDs. */
static struct rnled_info *leds_ses_init(struct rnled_info *temp)
{
	char *led_names;
	struct rnled_info *leds;
	int i, fixed, err;

	num_ses_leds = 0;
	enclosure_for_each_device(ses_enclosure, NULL);
	if (num_ses_leds < 60) {
		pr_debug("%s: Not enough SES LEDs yet, deferring\n", __func__);
		return ERR_PTR(-EPROBE_DEFER);
	}

	/* Count fixed portion size. */
	for (fixed = 0; temp[fixed].name; fixed++)
		;

	if (!(leds = kzalloc(sizeof(*temp) * (fixed + num_ses_leds + 1),
					GFP_ATOMIC)))
		return ERR_PTR(-ENOMEM);
	if (!(led_names = kzalloc(SES_LED_NAME_LEN * num_ses_leds,
					GFP_ATOMIC))) {
		kfree(leds);
		return ERR_PTR(-ENOMEM);
	}

	memcpy(leds, temp, sizeof(*temp) * fixed);
	for (i = 0; i < num_ses_leds; i++) {
		struct rnled_info *led = &leds[fixed + i];

		led->name = led_names + i * SES_LED_NAME_LEN;
		led->proc = &led_ses;
	}

	if ((err = enclosure_for_each_device(ses_enclosure2, &leds[fixed]))) {
		kfree(leds);
		kfree(led_names);
		pr_err("%s: failed to set up SES LEDs (%d).\n",
			__func__, err);
		return ERR_PTR(err);
	}
	return leds;
}

static void leds_ses_exit(struct rnled_info *leds)
{
	kfree(leds[0].name);
	kfree(leds);
}

/*
 * Compatibility table for all models
 */
static const struct model_info {
	const char *compatible;
	struct rnled_info *leds;
	int	num_leds;
	struct rnled_info *(*init)(struct rnled_info *);
	void (*exit)(struct rnled_info *);
} rnmodels[] = {
	{ "pro6",	leds_pro6_ultra6},
	{ "ultra2",	leds_ultra2},
	{ "ultra4",	leds_ultra4_rn312_rn314},
	{ "ultra6",	leds_pro6_ultra6},
	{ "rn312",	leds_ultra4_rn312_rn314, 4},
	{ "rn314",	leds_ultra4_rn312_rn314},
	{ "rn316",	leds_rnx16_rn316},
	{ "rn422",	leds_rn424, 2},
	{ "rn424",	leds_rn424},
	{ "rn426",	leds_rn428, 6},
	{ "rn428",	leds_rn428},
	{ "rnx16",	leds_rnx16_rn316},
	{ "rnx24",	leds_rnx28, 4},
	{ "rnx26",	leds_rnx16_rn316},
	{ "rnx28",	leds_rnx28},
	{ "rr2304",	leds_rr2304},
	{ "rr2312",	leds_rr2312},
	{ "rn313x",	leds_rn313x},
	{ "rnx220",	leds_rnx220},
	{ "rrx312",	leds_rrx312},
	{ "rr4360",	leds_rr4360, 0, leds_ses_init, leds_ses_exit},
	{}
},
*rnmodel;

/* Kernel thread to serialize LED accesses in process context. */
static void __led_servicer(struct led_service *service)
{
	struct led_set_request *req = NULL;

	do {
		struct led_set_request *q;
		unsigned long flags;

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
	} while (req);
}

static int led_servicer(void *data)
{
	struct led_service *service = data;

	while (!kthread_should_stop()) {
		wait_event_interruptible_timeout(service->wq,
				!list_empty(&service->req.list), 2 * HZ);
		__led_servicer(service);
	}
	return 0;
}

/* This common LED setter may be called in softirq (a.k.a. timer callback).
 * So, do not sleep.
 */
static void led_set(struct led_classdev *led_cdev, enum led_brightness val)
{
	unsigned long flags;
	struct rnled_info *led = led_cdev_to_info(led_cdev);
	struct led_service *service = &led_service;
	struct led_set_request *req = kmalloc(sizeof *req, GFP_ATOMIC);

	if (!req) {
		pr_err("%s: kmalloc failed (%lu bytes)\n",
			__func__, sizeof *req);
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
	pr_info("%s: registering LED \"%s\"\n", __func__, led->name);

	led->cdev.name = led->name;
	led->cdev.brightness_get = led->proc->get;
	led->cdev.brightness_set = led_set;

	if (led->proc->init) {
		led->proc->init(led, led->proc->label);
		/* Errors happen only after an explicit init. */
		if (IS_ERR_OR_NULL(led->desc))
			return led->desc ? PTR_ERR(led->desc) : -ENODEV;
	}

	led->cdev.flags |= LED_CORE_SUSPENDRESUME;
	led->cdev.default_trigger = led->trigger;

	return led_classdev_register(parent, &led->cdev);
}

static int readynas_led_probe(struct platform_device *pdev)
{
	struct model_info const *mp;
	int i = 0, err = 0;

	mp  = __compatible_find(rnmodels, compatible, -ENODEV,
				"no compatible platform\n");
	rnleds = mp->leds;

	if (mp->init) {
		rnleds = mp->init(mp->leds);
		if (IS_ERR(rnleds))
			return PTR_ERR(rnleds);
		if (!rnleds)
			return -ENODEV;
	}

	init_waitqueue_head(&led_service.wq);
	spin_lock_init(&led_service.lock);
	INIT_LIST_HEAD(&led_service.req.list);
	led_service.kthread = kthread_run(led_servicer, &led_service,
						led_service.name);

	if (IS_ERR(led_service.kthread)) {
		led_service.kthread = NULL;
		rnleds = NULL;
		pr_err("%s: failed to start %s service kthread.\n",
			__func__, led_service.name);
		err = -ENOMEM;
		goto fail;
	}

	for (i = 0; (!mp->num_leds || i < mp->num_leds) && rnleds[i].name; i++)
		if (rnleds[i].name[0]) {/* Or skip unavailable drive. */
			if ((err = register_led(&pdev->dev, &rnleds[i])))
				pr_err("%s: %s registration failed.\n",
					__func__, rnleds[i].name);
			else
				rnleds[i].registered = true;
		}

	rnmodel = mp;
	return 0;

fail:
	if (mp->exit)
		mp->exit(rnleds);
	return err;
}

static int readynas_led_remove(struct platform_device *pdev)
{
	int i;
	struct model_info const *mp = rnmodel;

	if (led_service.kthread) {
		kthread_stop(led_service.kthread);
		led_service.kthread = NULL;
	}

	for (i = 0; (!mp->num_leds || i < mp->num_leds) && rnleds[i].name; i++)
		if (rnleds[i].registered) {
			if (rnleds[i].proc->exit)
				rnleds[i].proc->exit(&rnleds[i]);
			led_classdev_unregister(&rnleds[i].cdev);
			rnleds[i].registered = false;
		}

	if (mp->exit)
		mp->exit(rnleds);
	rnleds = NULL;
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
	else if (!rnleds) {
		platform_device_unregister(readynas_led_pdev);
		err = -ENODEV;
	}
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
			rnleds[i].name; i++) {
		if (!strcmp(rnleds[i].name, name))
			return &rnleds[i];
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
