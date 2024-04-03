/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

*******************************************************************************/
#include "mvTypes.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include <linux/proc_fs.h>
#include "twsi/mvTwsi.h"

#include "gpp/mvGpp.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include <linux/delay.h>

static struct proc_dir_entry *LCD_proc;
static struct proc_dir_entry *LCDstatus_proc;

static char LCD_init_tbl[] = {
0x30, 0x30, 0x30,
0x20, 0x28,
0x08,
0x01, 0x07,
0xA0,
};

#define LCD_BL_GPP_GROUP        0
#define LCD_BL_BIT              BIT12
#define LCD_BL_BIT_SHIFT        12

#define LCD_RS_GPP_GROUP        0
#define LCD_RS_BIT              BIT14
#define LCD_RS_BIT_SHIFT        14

#define LCD_RW_GPP_GROUP        0
#define LCD_RW_BIT              BIT15
#define LCD_RW_BIT_SHIFT        15

#define LCD_E_GPP_GROUP         0
#define LCD_E_BIT               BIT16
#define LCD_E_BIT_SHIFT         16

#define LCD_INSTRUCTION         0
#define LCD_DATA                1
#define LCD_WR                  0
#define LCD_RD                  1
static void LCD_set_BL(MV_U32 on_off)
{
    /* BL is low active on Netgear NV+ */
    MV_U32 val = on_off ^ 1;

    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
    gppData &= ~LCD_BL_BIT;
    gppData |= ((val << LCD_BL_BIT_SHIFT) & LCD_BL_BIT);
    MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP), gppData);
}

static MV_U32 LCD_get_BL_status(void)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
    gppData &= LCD_BL_BIT;
    gppData >>= LCD_BL_BIT_SHIFT;
    return (gppData ^ 1);
}

static void LCD_set_RS(MV_U32 val)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_RS_GPP_GROUP));
    gppData &= ~LCD_RS_BIT;
    gppData |= ((val << LCD_RS_BIT_SHIFT) & LCD_RS_BIT);
    MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_RS_GPP_GROUP), gppData);
}

static void LCD_set_RW(MV_U32 val)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_RW_GPP_GROUP));
    gppData &= ~LCD_RW_BIT;
    gppData |= ((val << LCD_RW_BIT_SHIFT) & LCD_RW_BIT);
    MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_RW_GPP_GROUP), gppData);
}

static void LCD_set_E(MV_U32 val)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_E_GPP_GROUP));
    gppData &= ~LCD_E_BIT;
    gppData |= ((val << LCD_E_BIT_SHIFT) & LCD_E_BIT);
    MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_E_GPP_GROUP), gppData);
}

#define LCD_DB4                 BIT17
#define LCD_DB4_GPP_GROUP       0
#define LCD_DB4_SHIFT           17

#define LCD_DB5                 BIT1
#define LCD_DB5_GPP_GROUP       1
#define LCD_DB5_SHIFT           1

#define LCD_DB6                 BIT3
#define LCD_DB6_GPP_GROUP       1
#define LCD_DB6_SHIFT           3

#define LCD_DB7                 BIT17
#define LCD_DB7_GPP_GROUP       1
#define LCD_DB7_SHIFT           17

static void LCD_set_onedb(MV_U32 group, MV_U32 mask, MV_U32 shift, MV_U32 val)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(group));
    gppData &= ~mask;
    gppData |= ((val << shift) & mask);
    MV_REG_WRITE(GPP_DATA_OUT_REG(group), gppData);
}

static void LCD_fill_db(MV_U32 val)
{
    LCD_set_onedb(LCD_DB4_GPP_GROUP, LCD_DB4, LCD_DB4_SHIFT, (val & 0x1));
    LCD_set_onedb(LCD_DB5_GPP_GROUP, LCD_DB5, LCD_DB5_SHIFT, (val & 0x2) >> 1);
    LCD_set_onedb(LCD_DB6_GPP_GROUP, LCD_DB6, LCD_DB6_SHIFT, (val & 0x4) >> 2);
    LCD_set_onedb(LCD_DB7_GPP_GROUP, LCD_DB7, LCD_DB7_SHIFT, (val & 0x8) >> 3);
}

static void LCD_send_data(MV_U8 val, MV_U32 flag)
{
    MV_U32 data_h = val >> 4;
    MV_U32 data_l = val;

    LCD_set_RW(LCD_WR);
    LCD_set_RS(flag);
    LCD_set_E(1);
    //mdelay(1);

    LCD_fill_db(data_h);
    mdelay(1);
    LCD_set_E(0);
    mdelay(1);

    LCD_set_E(1);
    //mdelay(1);
    LCD_fill_db(data_l);
    mdelay(1);
    LCD_set_E(0);
    mdelay(1);
}

static void LCD_select_line(int line)
{
    if (1 == line)
        LCD_send_data(0x80, LCD_INSTRUCTION);
    else if (2 == line)
        LCD_send_data(0xc0, LCD_INSTRUCTION);
}

static int LCD_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char temp_cmd[100], *ptr_h, *ptr_e;
	int k = count, i;
	int value;
	MV_U8 tmp_data;
	
	memset(temp_cmd, 0, sizeof(temp_cmd));
	if (!buffer || (count == 0))
	    return 0;
	
	if (count > sizeof(temp_cmd)/sizeof(char))
	    k = sizeof(temp_cmd)/sizeof(char);

	copy_from_user(temp_cmd, buffer, k);
	
	ptr_h = temp_cmd;
	
	i = 0;
	value = simple_strtoul(ptr_h, NULL, 0);
	switch(value) {
	    case 1:
	    case 2:
		while ((*ptr_h != '"') && (i < k))
		{
		    ptr_h++;
		    i++;
		}
		if (*ptr_h == '"' && (i < k))
		{
		    ptr_h++;
		    ptr_e = ptr_h;
		    i++;
		    
		} else
		    break;

		while ((*ptr_e != '"') && (i < k))
		{
		    ptr_e++;
		    i++;
		}
		if (*ptr_e == '"' && (i < k))
		{
		    //off
	            LCD_select_line(value);
		    while(ptr_h < ptr_e)
		    {
			tmp_data = *ptr_h++;
			LCD_send_data(tmp_data, LCD_DATA);
		    }
		}
	        break;
	    case 3:
		/* Turn on BL */
		LCD_set_BL(1);
		break;
	    case 4:
		/* Turn off BL */
		LCD_set_BL(0);
		break;
	    default:
	        break;
	}
	
	return count;
}

static int LCDstatus_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data) {

    MV_32 len = 10;

    if ((off > 0) || (count < len))
    {
        *eof = 1;
        return 0;
    }

    len = sprintf(page, "%d\n", LCD_get_BL_status());
    *eof = 1;
    return len;
}

static void LCD_init(void)
{
        int i;

        for(i = 0; i < sizeof(LCD_init_tbl)/sizeof(char); i++)
        {
            mdelay(5);
            LCD_send_data(LCD_init_tbl[i], LCD_INSTRUCTION);
        }
//FIXME
        LCD_set_E(1);
        LCD_set_RS(LCD_DATA);
        mdelay(1);

        LCD_set_E(0);
}
//i2c

static int LCD_backlight_status = 1;
static void W83601G_init(void)
{
        MV_TWSI_SLAVE twsiSlave;
        MV_U8 data;

        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x1f;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.moreThen256 = MV_FALSE;

        twsiSlave.offset = 0x14;
        data = 0x40;
        if (mvTwsiWrite (0, &twsiSlave, &data, 1) != MV_OK)
        {
            printk("<0>""%s i2c rw error!\n", __func__);
        }
        udelay(1);

        twsiSlave.offset = 0x3;
        data = 0x0;
        if (mvTwsiWrite (0, &twsiSlave, &data, 1) != MV_OK)
        {
            printk("<0>""%s i2c rw error!\n", __func__);
        }
        udelay(1);

        twsiSlave.offset = 0x2;
        data = 0x0;
        mvTwsiWrite (0, &twsiSlave, &data, 1);
        udelay(1);

        twsiSlave.offset = 0x1;
        data = 0x0;
        mvTwsiWrite (0, &twsiSlave, &data, 1);
        udelay(1);
}

static char LCD_init_tbl_i2c[] = {
#if 1
    0x00, 0x38, 0x30,

    0x00, 0x08, 0x00,

    0x00, 0x38, 0x30,

    0x00, 0x08, 0x00,

    0x00, 0x38, 0x30,

    0x00, 0x08, 0x00,
#endif
    0x00, 0x28, 0x20,

    0x00, 0x08, 0x00,

    0x00, 0x28, 0x20,

    0x00, 0x88, 0x80,
#if 1
    0x00, 0x08, 0x00,

    0x00, 0x88, 0x80,
#endif
    0x00, 0x08, 0x00,

    0x00, 0x18, 0x10,

    0x00, 0x08, 0x00,

    0x00, 0x78, 0x70,
#if 1
    0x00, 0xA8, 0xA0,

    0x00, 0x08, 0x00,

    0x0a, 0x02,
#endif
};

static void LCD_init_i2c(void)
{
        MV_TWSI_SLAVE twsiSlave;
        int i;
        printk("<0>""%s\n", __func__);
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x1f;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.moreThen256 = MV_FALSE;
        twsiSlave.offset = 0x1;

        for(i = 0; i < sizeof(LCD_init_tbl_i2c)/sizeof(char); i++)
        {
            mvTwsiWrite (0, &twsiSlave, &LCD_init_tbl_i2c[i], 1);
            mdelay(5);
        }
}

static int LCDstatus_proc_read_i2c(char *page, char **start, off_t off, int count, int *eof, void *data) {
    MV_32 len = 10;

    if ((off > 0) || (count < len))
    {
        *eof = 1;
        return 0;
    }

    len = sprintf(page, "%d\n", LCD_backlight_status);
    *eof = 1;
    return len;
}

static void LCD_BL(int on_off)
{

    MV_TWSI_SLAVE twsiSlave;
    int value = on_off ^ 1;

    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.slaveAddr.address = 0x1f;
    twsiSlave.validOffset = MV_TRUE;
    twsiSlave.offset = 0x1;
    twsiSlave.moreThen256 = MV_FALSE;

    mvTwsiWrite (0, &twsiSlave, &value, 1);
}

static char LCD_line_cfg[2][6] = {
    {0x08, 0x88, 0x80, 0x08, 0x08, 0x00,},
    {0x08, 0xC8, 0xC0, 0x08, 0x08, 0x00,},
};

static void LCD_select_line_i2c(int line)
{
    MV_TWSI_SLAVE twsiSlave;
    int i;

    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.slaveAddr.address = 0x1f;
    twsiSlave.validOffset = MV_TRUE;
    twsiSlave.offset = 0x1;
    twsiSlave.moreThen256 = MV_FALSE;

    if(1 == line || 2 == line)
    {
        for(i = 0; i < sizeof(LCD_line_cfg[line-1])/sizeof(char); i++)
        {
            mvTwsiWrite (0, &twsiSlave, &LCD_line_cfg[line-1][i], 1);
            mdelay(5);
        }
    }
}

void LCD_write_data(MV_U8 data)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U8 tmp_data;

        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x1f;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = 0x1;
        twsiSlave.moreThen256 = MV_FALSE;
#if 0
	tmp_data = 0x02;
	//printk("<0>""tmp_data1 = 0x%x\n", tmp_data);
	mvTwsiWrite (0, &twsiSlave, &tmp_data, 1);
	udelay(1);

	tmp_data = 0x0a;
	//printk("<0>""tmp_data2 = 0x%x\n", tmp_data);
	mvTwsiWrite (0, &twsiSlave, &tmp_data, 1);
	udelay(1);
#endif	
	tmp_data = data | 0xa;
	//printk("<0>""tmp_data3 = 0x%x\n", tmp_data);
        mvTwsiWrite (0, &twsiSlave, &tmp_data, 1);
	udelay(1);
	
	tmp_data = data | 0x2;
	//printk("<0>""tmp_data4 = 0x%x\n", tmp_data);
        mvTwsiWrite (0, &twsiSlave, &tmp_data, 1);

	return;
}

static int LCD_proc_write_i2c(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char temp_cmd[100], *ptr_h, *ptr_e;
	int k = count, i;
	int value;
	MV_U8 tmp_data, data_l, data_h;
	
	memset(temp_cmd, 0, sizeof(temp_cmd));
	if (!buffer || (count == 0))
	    return 0;
	
	if (count > sizeof(temp_cmd)/sizeof(char))
	    k = sizeof(temp_cmd)/sizeof(char);

	copy_from_user(temp_cmd, buffer, k);
	
	ptr_h = temp_cmd;
	
	i = 0;
	value = simple_strtoul(ptr_h, NULL, 0);
	switch(value) {
	    case 1:
	    case 2:
		while ((*ptr_h != '"') && (i < k))
		{
		    ptr_h++;
		    i++;
		}
		if (*ptr_h == '"' && (i < k))
		{
		    ptr_h++;
		    ptr_e = ptr_h;
		    i++;
		    
		} else
		    break;

		while ((*ptr_e != '"') && (i < k))
		{
		    ptr_e++;
		    i++;
		}
		if (*ptr_e == '"' && (i < k))
		{
		    //off
	            LCD_select_line_i2c(value);
		    while(ptr_h < ptr_e)
		    {
			tmp_data = *ptr_h++;
			data_h = tmp_data & 0xF0;
			LCD_write_data(data_h);
			data_l = (tmp_data & 0x0F) << 4;
			LCD_write_data(data_l);
		    }
		    LCD_backlight_status = 1;//BL is on.
		}
	        break;
	    case 3:
		LCD_backlight_status = 1;//BL is on.
		LCD_BL(LCD_backlight_status);//Turn on BL
		break;
	    case 4:
		LCD_backlight_status = 0;//BL is off.
		LCD_BL(LCD_backlight_status);//Turn off BL
		break;
	    default:
	        break;
	}
	
	return count;
}

//
int __init usiLCD_init(void)
{
        MV_TWSI_SLAVE twsiSlave;
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x1f;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.moreThen256 = MV_FALSE;
        twsiSlave.offset = 0x1;
	MV_U8 flag;

	LCD_proc = create_proc_entry("LCD", 0222, NULL);
	LCDstatus_proc = create_proc_entry("LCDstatus", 0111, NULL);
	if(NULL == LCD_proc || NULL == LCDstatus_proc) {
		return 0;
	}

	strcpy(LCD_proc->name, "LCD");
	strcpy(LCDstatus_proc->name, "LCDstatus");

        if (MV_OK != mvTwsiRead (0, &twsiSlave, &flag, 1))
	{
	    //gpio
	    LCD_proc->write_proc = LCD_proc_write;
	    LCDstatus_proc->read_proc = LCDstatus_proc_read;
	    LCD_init();
	} else
	{
	    //i2c
	    LCD_proc->write_proc = LCD_proc_write_i2c;
	    LCDstatus_proc->read_proc = LCDstatus_proc_read_i2c;
	    W83601G_init();
	    mdelay(50);
	    LCD_init_i2c();
	}
	//LCD_proc->owner = THIS_MODULE;
	//LCD_proc->read_proc = LCD_proc_read;
	LCD_proc->nlink = 1;
	LCDstatus_proc->nlink = 1;
	
	return 1;
}

void __exit usiLCD_exit(void)
{
	remove_proc_entry("LCD", NULL);
}

module_init(usiLCD_init);
module_exit(usiLCD_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("libo");
MODULE_DESCRIPTION("WH1602D-TTI-JT#000 LCM driver.");
