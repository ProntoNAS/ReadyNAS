/*
 * Intersil ISL12057 rtc class driver
 *
 * Copyright 2012 <zouzhen@usish.com>
 * Based on Intersil ISL1208 rtc class driver
 * by Hebert Valerio Riedel <hvr@gnu.org>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/proc_fs.h>
#define DRV_VERSION "0.1"

/* Register map */
/* rtc section */
#define ISL12057_REG_SC  0x00
#define ISL12057_REG_MN  0x01
#define ISL12057_REG_HR  0x02
#define ISL12057_REG_HR_PM      (1<<5)	/* PM/AM bit in 12h mode */
#define ISL12057_REG_HR_MIL     (1<<6)	/* 24h/12h mode */
#define ISL12057_REG_DW  0x03
#define ISL12057_REG_DT  0x04
#define ISL12057_REG_MO  0x05
#define ISL12057_REG_YR  0x06
#define ISL12057_RTC_SECTION_LEN    7

/* alarm1 section */
#define ISL12057_REG_A1SC 0x07
#define ISL12057_REG_A1MN 0x08
#define ISL12057_REG_A1HR 0x09
#define ISL12057_REG_A1DT 0x0a
#define ISL12057_REG_A1DT_A1DW (1<<6)
#define ISL12057_ALARM1_SECTION_LEN 4
/* alarm2 section */
#define ISL12057_REG_A2MN 0x0b
#define ISL12057_REG_A2HR 0x0c
#define ISL12057_REG_A2DT 0x0d
#define ISL12057_REG_A2DT_A2DW (1<<6)
#define ISL12057_ALARM2_SECTION_LEN 3

/* control/status section */
#define ISL12057_REG_INT 0x0e
#define ISL12057_REG_INT_A1IE   (1<<0)  /* alarm1 interrupt enable */
#define ISL12057_REG_INT_A2IE   (1<<1)  /* alarm2 interrupt enable */
#define ISL12057_REG_INT_INTCN  (1<<2)  /* interrupt enable */
#define ISL12057_REG_INT_EOSC   (1<<7)  /* oscillator enable */
#define ISL12057_REG_SR  0x0f
#define ISL12057_REG_SR_A1F     (1<<0)	/* alarm1 interrupt bit */
#define ISL12057_REG_SR_A2F     (1<<1)	/* alarm2 interrupt bit */
#define ISL12057_REG_SR_OSF     (1<<7)	/* oscillator failure */

static struct i2c_driver isl12057_driver;

/* block read */
static int
isl12057_i2c_read_regs(struct i2c_client *client, u8 reg, u8 buf[],
		      unsigned len)
{
	u8 reg_addr[1] = { reg };
	struct i2c_msg msgs[2] = {
		{client->addr, 0, sizeof(reg_addr), reg_addr}
		,
		{client->addr, I2C_M_RD, len, buf}
	};
	int ret;

	BUG_ON(reg > ISL12057_REG_SR);
	BUG_ON(reg + len > ISL12057_REG_SR + 1);

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret > 0)
		ret = 0;
	return ret;
}

/* block write */
static int
isl12057_i2c_set_regs(struct i2c_client *client, u8 reg, u8 const buf[],
		     unsigned len)
{
	u8 i2c_buf[ISL12057_REG_SR + 2];
	struct i2c_msg msgs[1] = {
		{client->addr, 0, len + 1, i2c_buf}
	};
	int ret;

	BUG_ON(reg > ISL12057_REG_SR);
	BUG_ON(reg + len > ISL12057_REG_SR + 1);

	i2c_buf[0] = reg;
	memcpy(&i2c_buf[1], &buf[0], len);

	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret > 0)
		ret = 0;
	return ret;
}

/* simple check to see wether we have a isl12057 */
static int
isl12057_i2c_validate_client(struct i2c_client *client)
{
	u8 regs[ISL12057_RTC_SECTION_LEN] = { 0, };
	u8 zero_mask[ISL12057_RTC_SECTION_LEN] = {
		0x80, 0x80, 0x80, 0xf8, 0xc0, 0x60, 0x00
	};
	int i;
	int ret;

	ret = isl12057_i2c_read_regs(client, 0, regs, ISL12057_RTC_SECTION_LEN);
	if (ret < 0)
		return ret;

	for (i = 0; i < ISL12057_RTC_SECTION_LEN; ++i) {
		if (regs[i] & zero_mask[i])	/* check if bits are cleared */
			return -ENODEV;
	}

	return 0;
}

static int
isl12057_i2c_get_sr(struct i2c_client *client)
{
	int sr = i2c_smbus_read_byte_data(client, ISL12057_REG_SR);
	if (sr < 0)
		return -EIO;

	return sr;
}

static int
isl12057_rtc_toggle_alarm(struct i2c_client *client, int enable)
{
	int icr = i2c_smbus_read_byte_data(client, ISL12057_REG_INT);

	if (icr < 0) {
		dev_err(&client->dev, "%s: reading INT failed\n", __func__);
		return icr;
	}

	if (enable)
		icr |= ISL12057_REG_INT_A1IE;
	else
		icr &= ~ISL12057_REG_INT_A1IE;
	icr = i2c_smbus_write_byte_data(client, ISL12057_REG_INT, icr);
	if (icr < 0) {
		dev_err(&client->dev, "%s: writing INT failed\n", __func__);
		return icr;
	}

	return 0;
}

static int
isl12057_rtc_proc(struct device *dev, struct seq_file *seq)
{
	struct i2c_client *const client = to_i2c_client(dev);
	int sr, icr;

	sr = isl12057_i2c_get_sr(client);
	if (sr < 0) {
		dev_err(&client->dev, "%s: reading SR failed\n", __func__);
		return sr;
	}

	seq_printf(seq, "status_reg\t:%s%s%s (0x%.2x)\n",
		   (sr & ISL12057_REG_SR_OSF) ? " OSF" : "",
		   (sr & ISL12057_REG_SR_A1F) ? " A1F" : "",
		   (sr & ISL12057_REG_SR_A2F) ? " A2F" : "", sr);

	icr = i2c_smbus_read_byte_data(client, ISL12057_REG_INT);
	if (icr < 0) {
		dev_err(&client->dev, "%s: reading INT failed\n", __func__);
		return icr;
	}

	seq_printf(seq, "control_reg\t:%s%s%s%s (0x%.2x)\n",
		   (icr & ISL12057_REG_INT_A1IE) ? " A1IE" : "",
		   (icr & ISL12057_REG_INT_A2IE) ? " A2IE" : "",
		   (icr & ISL12057_REG_INT_INTCN) ? " INTCN" : "",
		   (icr & ISL12057_REG_INT_EOSC) ? " EOSC" : "", icr);

	return 0;
}

static int
isl12057_i2c_read_time(struct i2c_client *client, struct rtc_time *tm)
{
	int sr;
	u8 regs[ISL12057_RTC_SECTION_LEN] = { 0, };

	sr = isl12057_i2c_get_sr(client);
	if (sr < 0) {
		dev_err(&client->dev, "%s: reading SR failed\n", __func__);
		return -EIO;
	}

	sr = isl12057_i2c_read_regs(client, 0, regs, ISL12057_RTC_SECTION_LEN);
	if (sr < 0) {
		dev_err(&client->dev, "%s: reading RTC section failed\n",
			__func__);
		return sr;
	}

	tm->tm_sec = bcd2bin(regs[ISL12057_REG_SC]);
	tm->tm_min = bcd2bin(regs[ISL12057_REG_MN]);

	/* HR field has a more complex interpretation */
	{
		const u8 _hr = regs[ISL12057_REG_HR];
		if (_hr & ISL12057_REG_HR_MIL) {/* 12h format */
			tm->tm_hour = bcd2bin(_hr & 0x1f);
			if (_hr & ISL12057_REG_HR_PM)	/* PM flag set */
				tm->tm_hour += 12;
		} else {
			/* 24h format */
			tm->tm_hour = bcd2bin(_hr & 0x3f);
		}
	}

	tm->tm_mday = bcd2bin(regs[ISL12057_REG_DT]);
	tm->tm_mon = bcd2bin(regs[ISL12057_REG_MO]) - 1;	/* rtc starts at 1 */
	tm->tm_year = bcd2bin(regs[ISL12057_REG_YR]) + 100;
	tm->tm_wday = bcd2bin(regs[ISL12057_REG_DW]);

	return 0;
}

static int
isl12057_i2c_read_alarm(struct i2c_client *client, struct rtc_wkalrm *alarm)
{
	struct rtc_time *const tm = &alarm->time;
	u8 regs[ISL12057_ALARM1_SECTION_LEN] = { 0, };
	int icr, sr = isl12057_i2c_get_sr(client);

	if (sr < 0) {
		dev_err(&client->dev, "%s: reading SR failed\n", __func__);
		return sr;
	}

	sr = isl12057_i2c_read_regs(client, ISL12057_REG_A1SC, regs,
				   ISL12057_ALARM1_SECTION_LEN);
	if (sr < 0) {
		dev_err(&client->dev, "%s: reading alarm section failed\n",
			__func__);
		return sr;
	}

	/* MSB of each alarm register is an enable bit */
	tm->tm_sec = bcd2bin(regs[ISL12057_REG_A1SC - ISL12057_REG_A1SC] & 0x7f);
	tm->tm_min = bcd2bin(regs[ISL12057_REG_A1MN - ISL12057_REG_A1SC] & 0x7f);
	tm->tm_hour = bcd2bin(regs[ISL12057_REG_A1HR - ISL12057_REG_A1SC] & 0x3f);
	tm->tm_mday = bcd2bin(regs[ISL12057_REG_A1DT - ISL12057_REG_A1SC] & 0x3f);

	icr = i2c_smbus_read_byte_data(client, ISL12057_REG_INT);
	if (icr < 0) {
		dev_err(&client->dev, "%s: reading INT failed\n", __func__);
		return icr;
	}
	alarm->enabled = !!(icr & ISL12057_REG_INT_A1IE);


	sr = i2c_smbus_read_byte_data(client, ISL12057_REG_SR);
	if (sr < 0) {
		dev_err(&client->dev, "%s: reading INT failed\n", __func__);
		return sr;
	}
	alarm->pending = !!(sr & ISL12057_REG_SR_A1F);

	return 0;
}

static int
isl12057_i2c_set_alarm(struct i2c_client *client, struct rtc_wkalrm *alarm)
{
	struct rtc_time *alarm_tm = &alarm->time;
	u8 regs[ISL12057_ALARM1_SECTION_LEN] = { 0, };
	const int offs = ISL12057_REG_A1SC;
	unsigned long rtc_secs, alarm_secs;
	struct rtc_time rtc_tm;
	int err, disable, sr;

	err = isl12057_i2c_read_time(client, &rtc_tm);
	if (err)
		return err;
	err = rtc_tm_to_time(&rtc_tm, &rtc_secs);
	if (err)
		return err;
	err = rtc_tm_to_time(alarm_tm, &alarm_secs);
	if (err)
		return err;

	/* If the alarm time is before the current time disable the alarm */
	if (!alarm->enabled || alarm_secs <= rtc_secs)
		disable = 0x80;
	else
		disable = 0x00;

	sr = isl12057_i2c_get_sr(client);
	if (sr & ISL12057_REG_SR_A1F) {
		sr &= ~ISL12057_REG_SR_A1F;
		sr = i2c_smbus_write_byte_data(client, ISL12057_REG_SR, sr);
		if (sr < 0)
			dev_err(&client->dev, "%s: writing SR failed\n",
				__func__);
	}


	/* Program the alarm and enable it for each setting */
	regs[ISL12057_REG_A1SC - offs] = bin2bcd(alarm_tm->tm_sec) | disable;
	regs[ISL12057_REG_A1MN - offs] = bin2bcd(alarm_tm->tm_min) | disable;
	regs[ISL12057_REG_A1HR - offs] = bin2bcd(alarm_tm->tm_hour) | disable;
	regs[ISL12057_REG_A1DT - offs] = bin2bcd(alarm_tm->tm_mday) | disable;

	/* write ALARM registers */
	err = isl12057_i2c_set_regs(client, offs, regs,
				  ISL12057_ALARM1_SECTION_LEN);
	if (err < 0) {
		dev_err(&client->dev, "%s: writing ALARM section failed\n",
			__func__);
		return err;
	}

	err = isl12057_rtc_toggle_alarm(client, !disable);
	if (err)
		return err;

	return 0;
}

static int
isl12057_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return isl12057_i2c_read_time(to_i2c_client(dev), tm);
}

static int
isl12057_i2c_set_time(struct i2c_client *client, struct rtc_time const *tm)
{
	int sr;
	u8 regs[ISL12057_RTC_SECTION_LEN] = { 0, };

	/* The clock has an 8 bit wide bcd-coded register (they never learn)
	 * for the year. tm_year is an offset from 1900 and we are interested
	 * in the 2000-2099 range, so any value less than 100 is invalid.
	 */
	if (tm->tm_year < 100)
		return -EINVAL;

	regs[ISL12057_REG_SC] = bin2bcd(tm->tm_sec);
	regs[ISL12057_REG_MN] = bin2bcd(tm->tm_min);
	regs[ISL12057_REG_HR] = bin2bcd(tm->tm_hour);

	regs[ISL12057_REG_DT] = bin2bcd(tm->tm_mday);
	regs[ISL12057_REG_MO] = bin2bcd(tm->tm_mon + 1);
	regs[ISL12057_REG_YR] = bin2bcd(tm->tm_year - 100);

	regs[ISL12057_REG_DW] = bin2bcd(tm->tm_wday & 7);

	/* write RTC registers */
	sr = isl12057_i2c_set_regs(client, 0, regs, ISL12057_RTC_SECTION_LEN);
	if (sr < 0) {
		dev_err(&client->dev, "%s: writing RTC section failed\n",
			__func__);
		return sr;
	}

	return 0;
}


static int
isl12057_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return isl12057_i2c_set_time(to_i2c_client(dev), tm);
}

static int
isl12057_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	return isl12057_i2c_read_alarm(to_i2c_client(dev), alarm);
}

static int
isl12057_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	return isl12057_i2c_set_alarm(to_i2c_client(dev), alarm);
}

static int
isl12057_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	return isl12057_rtc_toggle_alarm(to_i2c_client(dev), enabled);
}

static irqreturn_t
isl12057_rtc_interrupt(int irq, void *data)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(1000);
	struct i2c_client *client = data;
	int handled = 0, sr, err;

	/*
	 * I2C reads get NAK'ed if we read straight away after an interrupt?
	 * Using a mdelay/msleep didn't seem to help either, so we work around
	 * this by continually trying to read the register for a short time.
	 */
	while (1) {
		sr = isl12057_i2c_get_sr(client);
		if (sr >= 0)
			break;

		if (time_after(jiffies, timeout)) {
			dev_err(&client->dev, "%s: reading SR failed\n",
				__func__);
			return sr;
		}
	}

	if (sr & ISL12057_REG_SR_A1F) {
		dev_dbg(&client->dev, "alarm!\n");

		/* Clear the alarm */
		sr &= ~ISL12057_REG_SR_A1F;
		sr = i2c_smbus_write_byte_data(client, ISL12057_REG_SR, sr);
		if (sr < 0)
			dev_err(&client->dev, "%s: writing SR failed\n",
				__func__);
		else
			handled = 1;

		/* Disable the alarm */
		err = isl12057_rtc_toggle_alarm(client, 0);
		if (err)
			return err;
	}

	return handled ? IRQ_HANDLED : IRQ_NONE;
}

static const struct rtc_class_ops isl12057_rtc_ops = {
	.proc = isl12057_rtc_proc,
	.read_time = isl12057_rtc_read_time,
	.set_time = isl12057_rtc_set_time,
	.read_alarm = isl12057_rtc_read_alarm,
	.set_alarm = isl12057_rtc_set_alarm,
	.alarm_irq_enable = isl12057_rtc_alarm_irq_enable,
};

/* Power off mode2(WOL Disabled) */
static struct i2c_client power_off_mode2_client;
int power_off_mode2_flag;
#define BIT1        0x00000002
#define BIT0        0x00000001
/* Power off mode2(WOL Disabled) */
static int
isl12057_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	struct rtc_device *rtc;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	if (isl12057_i2c_validate_client(client) < 0)
		return -ENODEV;

	dev_info(&client->dev,
		 "chip found, driver version " DRV_VERSION "\n");

	if (client->irq > 0) {
		rc = request_threaded_irq(client->irq, NULL,
					  isl12057_rtc_interrupt,
					  IRQF_SHARED,
					  isl12057_driver.driver.name, client);
		if (!rc) {
			device_init_wakeup(&client->dev, 1);
			enable_irq_wake(client->irq);
		} else {
			dev_err(&client->dev,
				"Unable to request irq %d, no alarm support\n",
				client->irq);
			client->irq = 0;
		}
	}

	rtc = rtc_device_register(isl12057_driver.driver.name,
				  &client->dev, &isl12057_rtc_ops,
				  THIS_MODULE);
	if (IS_ERR(rtc)) {
		rc = PTR_ERR(rtc);
		goto exit_free_irq;
	}

	i2c_set_clientdata(client, rtc);

	rc = isl12057_i2c_get_sr(client);
	if (rc < 0) {
		dev_err(&client->dev, "reading status failed\n");
		goto exit_unregister;
	}

	if (rc & ISL12057_REG_SR_OSF) {
		dev_warn(&client->dev, "rtc power failure detected, "
			 "please set clock.\n");
	}

	/* Clear alarm flags */
	if (rc & ISL12057_REG_SR_A1F) {
		rc &= ~ISL12057_REG_SR_A1F;
		rc = i2c_smbus_write_byte_data(client, ISL12057_REG_SR, rc);
		if (rc < 0)
			dev_err(&client->dev, "%s: writing SR failed\n",
				__func__);
	}

	/* Using this bit to record AC power fail */
	rc = i2c_smbus_read_byte_data(client, ISL12057_REG_A2MN);
	if (rc < 0) {
		dev_err(&client->dev, "%s: reading flag failed.\n", __func__);
		goto exit_unregister;
	}
	rc = i2c_smbus_write_byte_data(client, ISL12057_REG_A2MN, rc | BIT0);
	if (rc < 0) {
		dev_err(&client->dev, "%s: writing flag failed.\n", __func__);
		goto exit_unregister;
	}
	/* Power off mode2(WOL Disabled) */
	memcpy(&power_off_mode2_client, client, sizeof(power_off_mode2_client));
	/* Power off mode2(WOL Disabled) */
	return 0;

exit_unregister:
	rtc_device_unregister(rtc);
exit_free_irq:
	if (client->irq)
		free_irq(client->irq, client);

	return rc;
}

static int
isl12057_remove(struct i2c_client *client)
{
	struct rtc_device *rtc = i2c_get_clientdata(client);

	rtc_device_unregister(rtc);
	if (client->irq)
		free_irq(client->irq, client);

	return 0;
}

static const struct i2c_device_id isl12057_id[] = {
	{ "isl12057", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, isl12057_id);

static void
isl12057_shutdown(struct i2c_client *client)
{
	int rc = 0;
	/* Power off mode2(WOL Disabled) */
	power_off_mode2_flag = 0;
	rc = i2c_smbus_read_byte_data(client, ISL12057_REG_A2MN);
	if (rc < 0) {
		rc = 0;
		dev_err(&client->dev, "%s: reading flag failed.\n", __func__);
	}
	if ( rc & BIT1)
		power_off_mode2_flag = 0;
	else
		power_off_mode2_flag = 1;
	/* Power off mode2(WOL Disabled) */
	/* Using this bit to record AC power fail */
	rc = i2c_smbus_write_byte_data(client, ISL12057_REG_A2MN, rc & (~BIT0));
	if (rc < 0) {
		dev_err(&client->dev, "%s: writing flag failed.\n", __func__);
	}
	printk("Reset the power fail detect flag.\n");
}

static struct i2c_driver isl12057_driver = {
	.driver = {
		   .name = "rtc-isl12057",
		   },
	.probe = isl12057_probe,
	.remove = isl12057_remove,
	.id_table = isl12057_id,
	.shutdown = isl12057_shutdown,
};

/* Power off mode2(WOL Disabled) */
static int power_off_mode2_write(struct file *file, const char __user *buffer,
				unsigned long count, void *data)
{
	char temp_cmd[4];
	unsigned long value;
	int ori_value, rc;

	if(!buffer || (count == 0))
	    return 0;
	if((count > 4) || (power_off_mode2_client.addr == 0))
	    return count;

	memset(temp_cmd, 0, sizeof(temp_cmd)/sizeof(temp_cmd[0]));
	copy_from_user(temp_cmd, buffer, count);
	temp_cmd[count] = '\0';

	ori_value = i2c_smbus_read_byte_data(&power_off_mode2_client, ISL12057_REG_A2MN);
	if (ori_value < 0)
	{
		printk("Read ISL12057_REG_A2MN failed.\n");
		return count;
	}

	value = simple_strtoul(temp_cmd, NULL, 10);
	if (0 == value)
	{
		rc = i2c_smbus_write_byte_data(&power_off_mode2_client, ISL12057_REG_A2MN, ori_value | BIT1);
		if (rc < 0) {
			printk("Write ISL12057_REG_A2MN failed.\n");
			return count;
		}
	}
	else if (1 == value) {
		rc = i2c_smbus_write_byte_data(&power_off_mode2_client, ISL12057_REG_A2MN, ori_value & (~BIT1));
		if (rc < 0) {
			printk("Write ISL12057_REG_A2MN failed.\n");
			return count;
		}
	}

        return count;
}

static int power_off_mode2_read(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int value, len = 4;

    	if ((count < len) || (power_off_mode2_client.addr == 0))
    	{
		*eof = 1;
		return 0;
	}

	value = i2c_smbus_read_byte_data(&power_off_mode2_client, ISL12057_REG_A2MN);
	if (value < 0)
	{
		printk("Read ISL12057_REG_A2MN failed.\n");
		*eof = 1;
		return 0;
	}
	if (value & BIT1)
	    len = sprintf(page, "0\n");
	else
	    len = sprintf(page, "1\n");

	*eof = 1;
	return len;
}

/* Power off mode2(WOL Disabled) */
static struct proc_dir_entry * power_off_mode2_proc;
static int __init
isl12057_init(void)
{
	/* Power off mode2(WOL Disabled) */
	power_off_mode2_proc = create_proc_entry("power_off_mode2", 0644, NULL);
	if(NULL == power_off_mode2_proc)
	{
		printk("Failed to create power_off_mode2 node.\n");
	} else {
		power_off_mode2_proc->read_proc = power_off_mode2_read;
		power_off_mode2_proc->write_proc = power_off_mode2_write;
		power_off_mode2_proc->nlink = 1;
	}
	/* Power off mode2(WOL Disabled) */

	return i2c_add_driver(&isl12057_driver);
}

static void __exit
isl12057_exit(void)
{
	/* Power off mode2(WOL Disabled) */
	remove_proc_entry("power_off_mode2", NULL);
	/* Power off mode2(WOL Disabled) */
	i2c_del_driver(&isl12057_driver);
}

MODULE_AUTHOR("<zouzhen@usish.com>");
MODULE_DESCRIPTION("Intersil ISL12057 RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(isl12057_init);
module_exit(isl12057_exit);
