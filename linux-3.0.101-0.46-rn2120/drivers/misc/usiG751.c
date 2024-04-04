/*******************************************************************************
Copyright (C) USI

*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/i2c.h>

#include <linux/proc_fs.h>

enum {
        G751_REG_TEMP,
        G751_REG_CONFIG,
        G751_REG_THYST,
        G751_REG_TOS,
};

static unsigned short G751_get_reg(struct device *dev,int reg_offset)
{
        int ret;
	struct i2c_client *client = to_i2c_client(dev);
	struct i2c_msg msg;
	unsigned char reg_val[2]= {0,0};
	unsigned short regval = 0;

	if ( reg_offset > 3 )
	{
		printk("G751 has only 4 registers, please select correct register\n");
		return 0;
	}
	/* write register offset to G751 point register */
	reg_val[0] = reg_offset;
	
        msg.addr = client->addr;
        msg.flags = client->flags;
        msg.len = sizeof(reg_val[0]);
        msg.buf = (char *)(reg_val);

	/* i2c_master_send i2c_master_recv is for 10bit address */
	ret = i2c_transfer(client->adapter, &msg, 1);
        if (ret < 0)
	{
                printk("i2c transfer fail !\n");
		return 0;
	}
	/* read register value which poinrt register figure out*/
        msg.addr = client->addr;
        msg.flags = client->flags | I2C_M_RD;
        msg.buf = (char *)(reg_val);
	/*G751_REG_CONFIG only has 8bit */
	if ( reg_offset == G751_REG_CONFIG )
        	msg.len = sizeof(reg_val[0]);
	else 
        	msg.len = sizeof(reg_val);

	ret = i2c_transfer(client->adapter, &msg, 1);
        if (ret < 0)
	{
                printk("i2c transfer fail !\n");
		return 0;
	}
	/* handle the return value form G751, reverse MSB and LSB for short type  */
	unsigned short tmp = * ((unsigned short *) reg_val );
        if ( reg_offset == G751_REG_CONFIG )
		regval = tmp & 0xff ;
	else 
		regval = ((tmp & 0xff00) >> 8) | ((tmp & 0xff) << 8) ;
	
	return regval;
}

int G751_set_reg(struct device *dev,int reg_offset, unsigned short val)
{
        int ret;
	struct i2c_client *client = to_i2c_client(dev);
	struct i2c_msg msg;
	unsigned char reg_val[3]= {0,0,0};

	if ( reg_offset > 3 )
	{
		printk("G751 has only 4 registers, please select correct register\n");
		return 0;
	}
	else if ( reg_offset == 0 )
	{
		printk("G751 temperature register is Read Only!\n");
		return 0;
	}
	/* write register value */
	reg_val[0] = reg_offset;
	
        msg.addr = client->addr;
        msg.flags = client->flags;
        msg.buf = (char *)(reg_val);
	if (reg_offset == G751_REG_CONFIG)
	{
		reg_val[1] = val;
	        msg.len = 2;
	}
	else 
	{
		reg_val[1] = (val & 0xff00) >> 8 ;
		reg_val[2] = val & 0x00ff;
		msg.len = 3;
	}

	/* i2c_master_send i2c_master_recv is for 10bit address */
	ret = i2c_transfer(client->adapter, &msg, 1);
        if (ret < 0)
	{
                printk("i2c transfer fail !\n");
		return 0;
	}
	
	return 1;
}


static ssize_t G751_reg_temp_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	unsigned short val;
	val = G751_get_reg(dev,G751_REG_TEMP);

	return sprintf(buf, "0x%04x\n", val);
}
static ssize_t G751_temp_input_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	unsigned short val;
	val = G751_get_reg(dev,G751_REG_TEMP);
	/* FIXME convert binary to integer which G751 datasheet indicate */
	/* Now it cannot show temperature below zero and after the point */
	/* Now it works when temperature is above zero, it doesn't include point */
	val = val >> 8;
	
	return sprintf(buf, "%d\n", val);
}
static ssize_t G751_reg_config_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	unsigned short val;
	val = G751_get_reg(dev,G751_REG_CONFIG);
	return sprintf(buf, "0x%02x\n", val);
}
static ssize_t G751_reg_thyst_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	unsigned short val;
	val = G751_get_reg(dev,G751_REG_THYST);
	return sprintf(buf, "0x%04x\n", val);
}
static ssize_t G751_reg_tos_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	unsigned short val;
	val = G751_get_reg(dev,G751_REG_TOS);
	return sprintf(buf, "0x%04x\n", val);
}

static ssize_t G751_reg_config_store(struct device *dev,
                struct device_attribute *attr, const  char *buf, size_t count)
{
	unsigned long val;
	int ret = 0;

	if (strict_strtoul(buf, 16, &val))
		return -EINVAL;
		
	ret = G751_set_reg(dev,G751_REG_CONFIG,val);
	if (ret == 0)
		printk("G751 set register fail\n");
		
	return count;
}

static ssize_t G751_reg_thyst_store(struct device *dev,
                struct device_attribute *attr, const  char *buf, size_t count)
{
	unsigned long val;
	int ret = 0;

	if (strict_strtoul(buf, 16, &val))
		return -EINVAL;
		
	ret = G751_set_reg(dev,G751_REG_THYST,val);
	if (ret == 0)
		printk("G751 set register fail\n");
		
	return count;
}

static ssize_t G751_reg_tos_store(struct device *dev,
                struct device_attribute *attr, const  char *buf, size_t count)
{
	unsigned long val;
	int ret = 0;

	if (strict_strtoul(buf, 16, &val))
		return -EINVAL;
		
	ret = G751_set_reg(dev,G751_REG_TOS,val);
	if (ret == 0)
		printk("G751 set register fail\n");
		
	return count;
}



static DEVICE_ATTR(reg_temp, S_IRUGO, G751_reg_temp_show, NULL);
static DEVICE_ATTR(temp_input, S_IRUGO, G751_temp_input_show, NULL);
static DEVICE_ATTR(reg_config, S_IRUGO | S_IWUGO, G751_reg_config_show, G751_reg_config_store);
static DEVICE_ATTR(reg_thyst, S_IRUGO | S_IWUGO, G751_reg_thyst_show, G751_reg_thyst_store);
static DEVICE_ATTR(reg_tos, S_IRUGO | S_IWUGO, G751_reg_tos_show, G751_reg_tos_store);

static struct attribute *G751_attrs[] = {
//	&dev_attr_lux0_sensor_range.attr,
	&dev_attr_reg_temp.attr,
	&dev_attr_reg_config.attr,
	&dev_attr_reg_thyst.attr,
	&dev_attr_reg_tos.attr,
	&dev_attr_temp_input.attr,
	NULL
};

static struct attribute_group G751_attr_group = {
	.name = "G751",
	.attrs = G751_attrs, 
};



static int  usiG751_probe(struct i2c_client *client,const struct i2c_device_id *id)
{

	int ret;

	ret = sysfs_create_group(&client->dev.kobj, &G751_attr_group);
	if (ret) {
		dev_err(&client->dev, "G751: device create file failed\n");
		return ret;
	}
	dev_info(&client->dev, "%s G751: chip found\n", client->name);
	return ret;


}

static int usiG751_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &G751_attr_group);
	return 0;
}


static struct i2c_device_id usiG751_ids[] = {
        { "usiG751-2", 0 },
        { }
};

static struct i2c_driver usiG751_driver = {
        .driver = {
                .name = "usiG751",
                .owner = THIS_MODULE,
        },
        .probe = usiG751_probe,
        .remove = __devexit_p(usiG751_remove),
        .id_table = usiG751_ids,
};

static int __init usiG751_init(MV_VOID)
{

        return i2c_add_driver(&usiG751_driver);
}

void __exit usiG751_exit(void)
{
        return i2c_del_driver(&usiG751_driver);
}

module_init(usiG751_init);
module_exit(usiG751_exit);

MODULE_AUTHOR("andrew hou");
MODULE_DESCRIPTION("G751 Temperature Driver");
MODULE_LICENSE("GPL v2");
