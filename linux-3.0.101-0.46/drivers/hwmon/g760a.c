/*
    g760a - Driver for the Global Mixed-mode Technology Inc. G760A
            fan speed PWM controller chip

    Copyright (C) 2007  Herbert Valerio Riedel <hvr@gnu.org>

    Complete datasheet is available at GMT's website:
      http://www.gmt.com.tw/product/datasheet/EDS-760A.pdf 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include "boardEnv/mvBoardEnvSpec.h"
#include "mvOs.h"
#include "gpp/mvGppRegs.h"
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
static const struct i2c_device_id g760a_id[] = {
	{ "g760a", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, g760a_id);

enum g760a_regs {
	G760A_REG_SET_CNT = 0x00,
	G760A_REG_ACT_CNT = 0x01,
	G760A_REG_FAN_STA = 0x02,
	G762_REG_FAN_CMD1 = 0x04,
};

#define G760A_REG_FAN_STA_RPM_OFF 0x1 /* +/-20% off */
#define G760A_REG_FAN_STA_RPM_LOW 0x2 /* below 1920rpm */

/* register data is read (and cached) at most once per second */
#define G760A_UPDATE_INTERVAL (HZ)

struct g760a_data {
	struct i2c_client *client;
	struct device *hwmon_dev;
	struct mutex update_lock;

	/* board specific parameters */
	u32 clk; /* default 32kHz */
	u16 fan_div; /* default P=2 */

	/* g760a register cache */
	unsigned int valid:1;
	unsigned long last_updated; /* In jiffies */

	u8 set_cnt; /* PWM (period) count number; 0xff stops fan */
	u8 act_cnt; /*   formula: cnt = (CLK * 30)/(rpm * P) */
	u8 fan_sta; /* bit 0: set when actual fan speed more than 20%
		     *   outside requested fan speed
		     * bit 1: set when fan speed below 1920 rpm */
        u8 fan_enable;
} * g760a_datap = NULL;

#define G760A_DEFAULT_CLK 8192
#define G760A_DEFAULT_FAN_DIV 2

#define PWM_FROM_CNT(cnt)	(0xff-(cnt))
#define PWM_TO_CNT(pwm)		(0xff-(pwm))

static inline unsigned int rpm_from_cnt(u8 val, u32 clk, u16 div)
{
	return ((val == 0x00) ? 0 : ((clk*30)/(val*div)));
}

/* new-style driver model */
static int g760a_probe(struct i2c_client *client,
			const struct i2c_device_id *id);
static int g760a_remove(struct i2c_client *client);

static struct i2c_driver g760a_driver = {
	.driver = {
		.name	= "g760a",
	},
	.probe	  = g760a_probe,
	.remove	  = g760a_remove,
	.id_table = g760a_id,
};

/* read/write wrappers */
static int g760a_read_value(struct i2c_client *client, enum g760a_regs reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

static int g760a_write_value(struct i2c_client *client, enum g760a_regs reg,
			     u16 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

/****************************************************************************
 * sysfs attributes
 */

static struct g760a_data *g760a_update_client(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct g760a_data *data = i2c_get_clientdata(client);

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + G760A_UPDATE_INTERVAL)
	    || !data->valid) {
		dev_dbg(&client->dev, "Starting g760a update\n");

		data->set_cnt = g760a_read_value(client, G760A_REG_SET_CNT);
		data->act_cnt = g760a_read_value(client, G760A_REG_ACT_CNT);
		data->fan_sta = g760a_read_value(client, G760A_REG_FAN_STA);

		data->last_updated = jiffies;
		data->valid = 1;
	}

	mutex_unlock(&data->update_lock);

	return data;
}

static ssize_t show_fan(struct device *dev, struct device_attribute *da,
			char *buf)
{
	struct g760a_data *data = g760a_update_client(dev);
	unsigned int rpm = 0;

	mutex_lock(&data->update_lock);
	if (!(data->fan_sta & G760A_REG_FAN_STA_RPM_LOW))
		rpm = rpm_from_cnt(data->act_cnt, data->clk, data->fan_div);
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", rpm);
}

static int g760a_present(void);
static ssize_t show_present(struct device *dev, struct device_attribute *da,
		char *buf)
{
	struct g760a_data *data = g760a_update_client(dev);
	unsigned int present = 0;

	mutex_lock(&data->update_lock);
	if (get_board_type() & NETGEAR_BD_TYPE_S2000)
		present = g760a_present();
	else
		present = 1;
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", present);
}

static ssize_t show_fan_alarm(struct device *dev, struct device_attribute *da,
			      char *buf)
{
	struct g760a_data *data = g760a_update_client(dev);

	int fan_alarm = (data->fan_sta & G760A_REG_FAN_STA_RPM_OFF) ? 1 : 0;

	return sprintf(buf, "%d\n", fan_alarm);
}

static ssize_t get_pwm(struct device *dev, struct device_attribute *da,
		       char *buf)
{
	struct g760a_data *data = g760a_update_client(dev);

	return sprintf(buf, "%d\n", PWM_FROM_CNT(data->set_cnt));
}

static ssize_t set_pwm(struct device *dev, struct device_attribute *da,
		       const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct g760a_data *data = g760a_update_client(dev);
	unsigned long val;

	if (strict_strtoul(buf, 10, &val))
		return -EINVAL;

	mutex_lock(&data->update_lock);
	data->set_cnt = PWM_TO_CNT(SENSORS_LIMIT(val, 0, 255));
	g760a_write_value(client, G760A_REG_SET_CNT, data->set_cnt);
	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t set_fan1_enable(struct device *dev, struct device_attribute *da,
		const char *buf, size_t count)
{
	struct g760a_data *data = g760a_update_client(dev);
	unsigned long val;
	int ret;
	struct i2c_client *client = to_i2c_client(dev);

	if (strict_strtoul(buf, 10, &val))
		return -EINVAL;

	mutex_lock(&data->update_lock);
	if (get_board_type() & NETGEAR_BD_TYPE_S2000)
	{
		data->fan_enable = val;
		if (val){
			MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), BIT7);
			ret = i2c_smbus_write_byte_data(client, G762_REG_FAN_CMD1, 0x10);	/* close loop */
		}
		else{
			MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), BIT7);
			ret = i2c_smbus_write_byte_data(client, G762_REG_FAN_CMD1, 0x0);	/* open loop */
		}
	} else {
		/* Fan will always be enabled */
		data->fan_enable = 1;
		ret = 0;
	}
	mutex_unlock(&data->update_lock);
	if (ret < 0)
		return ret;

	return count;
}

static ssize_t get_fan1_enable(struct device *dev, struct device_attribute *da,
		char *buf)
{
	struct g760a_data *data = g760a_update_client(dev);

	return sprintf(buf, "%d\n", data->fan_enable);
}

static DEVICE_ATTR(pwm1, S_IWUSR | S_IRUGO, get_pwm, set_pwm);
static DEVICE_ATTR(fan1_input, S_IRUGO, show_fan, NULL);
static DEVICE_ATTR(fan1_alarm, S_IRUGO, show_fan_alarm, NULL);
static DEVICE_ATTR(fan1_enable, S_IWUSR | S_IRUGO,
		get_fan1_enable, set_fan1_enable);
static DEVICE_ATTR(fan1_present, S_IRUGO, show_present, NULL);


static struct attribute *g760a_attributes[] = {
	&dev_attr_pwm1.attr,
	&dev_attr_fan1_input.attr,
	&dev_attr_fan1_alarm.attr,
	&dev_attr_fan1_enable.attr,
	&dev_attr_fan1_present.attr,
	NULL
};

static const struct attribute_group g760a_group = {
	.attrs = g760a_attributes,
};

/****************************************************************************
 * new-style driver model code
 */
static int g760a_init_rn25(struct g760a_data *data);
static int g760a_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct g760a_data *data;
	int err;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE_DATA))
		return -EIO;

	g760a_datap = data = kzalloc(sizeof(struct g760a_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);

	data->client = client;
	mutex_init(&data->update_lock);

	/* setup default configuration for now */
	data->fan_div = G760A_DEFAULT_FAN_DIV;
	data->clk = G760A_DEFAULT_CLK;

	/* Register sysfs hooks */
	err = sysfs_create_group(&client->dev.kobj, &g760a_group);
	if (err)
		goto error_sysfs_create_group;

	data->hwmon_dev = hwmon_device_register(&client->dev);
	if (IS_ERR(data->hwmon_dev)) {
		err = PTR_ERR(data->hwmon_dev);
		goto error_hwmon_device_register;
	}

	if (get_board_type() & NETGEAR_BD_TYPE_S2000)
	{
		err = g760a_init_rn25(data);
		if (err)
			return err;
	} else {
		data->fan_enable = 1;
	}
	return 0;

error_hwmon_device_register:
	sysfs_remove_group(&client->dev.kobj, &g760a_group);
error_sysfs_create_group:
	kfree(data);

	return err;
}

static int g760a_remove(struct i2c_client *client)
{
	struct g760a_data *data = i2c_get_clientdata(client);
	hwmon_device_unregister(data->hwmon_dev);
	sysfs_remove_group(&client->dev.kobj, &g760a_group);
	kfree(data);

	return 0;
}

#define MV_INT_SET_EN_REG       0x20A30
#define MV_INT_CLAER_EN_REG     0x20A34

wait_queue_head_t g760a_waitq;
static struct task_struct *g760a_task;
static unsigned int gpp_changed = 0;

enum RN25_FAN_GPP_STATUS {
	ADD	= 0x1,
	REMOVE	= 0x2,
};
static int g760a_thread(void *__unused)
{
	unsigned int error = 0;
	do {
		error = wait_event_interruptible(g760a_waitq, gpp_changed);
		if( gpp_changed == ADD)
		{
			kobject_uevent( &(g760a_datap->hwmon_dev->kobj), KOBJ_ADD);
		}
		if( gpp_changed == REMOVE)
		{
			kobject_uevent( &(g760a_datap->hwmon_dev->kobj), KOBJ_REMOVE);
		}
		gpp_changed = 0;
	} while (!kthread_should_stop());

	return 0;
}

static int g760a_present(void)
{
	int result;
	/* FAN present? MPP65# */
	if (((MV_REG_READ(GPP_DATA_IN_REG(2))) & BIT1) ^ ((MV_REG_READ(GPP_DATA_IN_POL_REG(2))) & BIT1)) { /* Not detected */
		result = 0;
	} else
		result = 1;
	return result;
}

irqreturn_t g760a_irq_handle(int irq, void *dev_id)
{
	unsigned long irq_flags;
	local_irq_save(irq_flags);
	if (irq == (IRQ_GPP_START + 65))
	{
		MV_REG_WRITE(MV_INT_CLAER_EN_REG, IRQ_AURORA_GPIO_64_66);	/* Close interrupt */
		if (g760a_present())
		{
			gpp_changed = ADD;
		}
		else {
			gpp_changed = REMOVE;
		}
		if (MV_REG_READ(GPP_DATA_IN_POL_REG(2)) & BIT1)
			MV_REG_BIT_RESET(GPP_DATA_IN_POL_REG(2), BIT1);
		else
			MV_REG_BIT_SET(GPP_DATA_IN_POL_REG(2), BIT1);
		wake_up_interruptible(&g760a_waitq);
		MV_REG_WRITE(MV_INT_SET_EN_REG, IRQ_AURORA_GPIO_64_66);         /* Open interrupt */

	} else {
		local_irq_restore(irq_flags);
		return IRQ_NONE;
	}
	local_irq_restore(irq_flags);
	return IRQ_HANDLED;
}
static int g760a_init_rn25(struct g760a_data *data)
{
	int result;

	init_waitqueue_head(&g760a_waitq);
	g760a_task = kthread_run(g760a_thread, NULL, "kg760ad");
	if (g760a_present())
	{
		/* FAN power control: FAN_PW_ON, MPP7 */
		/* Marvell's code cleared this bit during kernel early init. So we need to set it back. */
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), BIT7);
		data->fan_enable = 1;
	}
	else {
		MV_REG_BIT_SET(GPP_DATA_IN_POL_REG(2), BIT1);	/* protect no irq will be triggered during g760a_init_rn25 process */
		data->fan_enable = 0;
	}

	MV_REG_WRITE(MV_INT_CLAER_EN_REG, IRQ_AURORA_GPIO_64_66);	/* Close interrupt */
	MV_REG_BIT_SET(GPP_INT_LVL_REG(2), BIT1);	/* Set MPP65 Level interrupt */
	result = request_irq((IRQ_GPP_START + 65), g760a_irq_handle, (IRQF_DISABLED), "g760a", NULL);
	if (result)
	{
		printk("<0>""Request the irq : g760a failed!\n");
	}
	MV_REG_WRITE(MV_INT_SET_EN_REG, IRQ_AURORA_GPIO_64_66);		/* Open interrupt */
	return result;
}

/* module management */

static int __init g760a_init(void)
{
	return i2c_add_driver(&g760a_driver);
}

static void __exit g760a_exit(void)
{
	i2c_del_driver(&g760a_driver);
}

MODULE_AUTHOR("Herbert Valerio Riedel <hvr@gnu.org>");
MODULE_DESCRIPTION("GMT G760A driver");
MODULE_LICENSE("GPL");

module_init(g760a_init);
module_exit(g760a_exit);
