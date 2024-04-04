/*! \file sx8635.c
 * \brief  SX8635 Driver
 *
 * Driver for the SX8635.
 *
 * smtc_wheel_input contains code for performing input with the
 * capacitive wheel.
 * sx86xx contains more generic code to process more global items
 * such as the code to process interrupts.
 *
 * Copyright (c) 2012 Semtech Corp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
//#define DEBUG
//#define REPORT_TO_INFO

#define DRIVER_NAME "sx8635"
#define MAX_WRITE_ARRAY_SIZE 32
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/acpi.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <net/netlink.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#define MAX_NUM_STATUS_BITS (8)

#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#include <linux/earlysuspend.h>
#include <linux/suspend.h>
#endif

typedef struct sx86XX sx86XX_t, *psx86XX_t;
struct sx86XX
{
  void * bus; /* either i2c_client or spi_client */

  struct device *pdev; /* common device struction for linux */

  void *pDevice; /* device specific struct pointer */

  /* Function Pointers */
  int (*init)(psx86XX_t this); /* (re)initialize device */

  /* since we are trying to avoid knowing registers, create a pointer to a
   * common read register which would be to read what the interrupt source
   * is from
   */
  int (*refreshStatus)(psx86XX_t this); /* read register status */

  int (*get_nirq_low)(void); /* get whether nirq is low (platform data) */

  /* array of functions to call for corresponding status bit */
  void (*statusFunc[MAX_NUM_STATUS_BITS])(psx86XX_t this,int fullStatus);

  struct mutex mutex;
  int irq; /* irq number used */

  /* whether irq should be ignored.. cases if enable/disable irq is not used
   * or does not work properly */
  char irq_disabled;

  u8 useIrqTimer; /* older models need irq timer for pen up cases */

  int irqTimeout; /* msecs only set if useIrqTimer is true */

  /* struct workqueue_struct	*ts_workq;  */  /* if want to use non default */
	struct delayed_work dworker; /* work struct for worker function */

#ifdef CONFIG_HAS_WAKELOCK
  struct early_suspend early_suspend;  /* early suspend data  */
#endif
};

#include <linux/input/smtc/misc/sx8635_i2c_reg.h>
#include <linux/input/smtc/misc/sx8635_spm_cfg.h>
#include <linux/input/smtc/misc/sx8635_platform_data.h>  /* platform data */

//#define NETLINK_READYNAS	NETLINK_USERSOCK
#define NETLINK_READYNAS	22
#define NETLINK_CASTGROUP       1

#define WATCHDOG_MSECS		60000UL

extern struct net init_net;

//static char newInterrupt = 0;
/********************************************************/
/*** Functions ***/
static int smtc_processWheelState(psx86XX_t this, touchInformation_t *pWheelInformation, int position);
static int smtc_InitWheelInformation(psx86XX_t this,
				     touchInformation_t *pWheelInformation,
				     struct input_dev *input);
static int smtc_processWheelReleasedState(psx86XX_t this, touchInformation_t *pWheelInformation);
static int smtc_processButtonReleasedState(psx86XX_t this, touchInformation_t *pButtonInformation);
static int smtc_processButtonState(psx86XX_t this, touchInformation_t *pButtonInformation, int data);

static int smtc_hasWheelMoved(touchInformation_t *pWheelInformation,
			      touchSection_t *phistoryData,
			      touchSection_t *pcurrentData);
static void delayUpdateIntensities(psx86XX_t this, struct _ledInfo *led,
				   int size, u8 immediate);
static void updateIntensities(psx86XX_t this, struct _ledInfo *led, int size);
static int retrieve_spm_single(psx86XX_t this, unsigned char spmAddress, unsigned char *data);
static signed short read_cap_adc_value(psx86XX_t this, unsigned char spm_base);

static int reinitialize(psx86XX_t this);
/********************************************************/

static char isMonitorMode = 0;

/****************************************************************************/


static void hw_init(psx86XX_t this);

/* Uncomment this to report button messages to the console */
//#define REPORT_TO_INFO

/*! \fn static int write_register(psx86XX_t this, u8 address, u8 value)
 * \brief Sends a write register to the device
 * \param this Pointer to main parent struct
 * \param address 8-bit register address
 * \param value   8-bit register value to write to address
 * \return Value from i2c_master_send
 */
static int write_register(psx86XX_t this, u8 address, u8 value)
{
	struct i2c_client *i2c = NULL;
	char buffer[2];
	int returnValue = 0;

	buffer[0] = address;
	buffer[1] = value;
	returnValue = -ENOMEM;
	if (this && this->bus) {
		i2c = this->bus;
		returnValue = i2c_smbus_write_byte_data(i2c, address, value);
		//dev_info(&i2c->dev,"write_register Address: 0x%x Value: 0x%x Return: %d\n",
		//address,value,returnValue);
	}
	return returnValue;
}

static int irq_test = 9;

//this is used to record only key of direction and ok
//not involve the clock-wise and counter clock-wise;
static volatile int press_key = 0;

/*! \struct sx8635
 * Specialized struct containing number of channels to read and touch / proximity
 * structs.
 */
struct sx8635 {
	unsigned char lastIrqSrc; /* updated when reading interrupt */

	/* Updated whenever their respective flags in irqsrc occur */
	unsigned char gpistat_Value;
	unsigned char compopmode_Value;
	unsigned char spmstat_Value;

	psx8635_platform_data_t hw; /* specific platform data settings */
	struct input_dev *input;

	touchInformation_t *touchInformation;
};
typedef struct sx8635 sx8635_t;
typedef struct sx8635 *psx8635_t;

static psx86XX_t sthis = NULL;

#define ACPI_TOUCHKEY_CLASS "touchkey"

static void sx86XX_process_interrupt(psx86XX_t this, u8 nirqlow)
{
	unsigned char i = 0;
	char foundstatus = 0;
	int status = 0;
	int counter = 0;
	signed short raw[64];
	signed short diff[64];
	signed short avg[64];

	dev_dbg(this->pdev, "sx86XX_process_interrupt()\n");
	if (!this) {
		printk(KERN_ERR "sx86XX_worker_func, NULL sx86XX_t\n");
		return;
	}
	/* since we are not in an interrupt don't need to disable irq. */
	status = this->refreshStatus(this);
	counter = -1;

	dev_dbg(this->pdev, "Worker - Refresh Status %d\n", status);
	while ((++counter) < MAX_NUM_STATUS_BITS) { /* counter start from MSB */
		dev_dbg(this->pdev, "Looping Counter %d\n", counter);
		if (((status >> counter) & 0x01) && this->statusFunc[counter]) {
			dev_dbg(this->pdev, "Function Pointer Found. Calling\n");
			this->statusFunc[counter](this, status);
			foundstatus = 1;
		}
	}

	if (/*(foundstatus != 1) &&*/ (isMonitorMode != 0)) {
		// If Monitior Mode is enabled, lets read some raw data for debugging..
		for (i = 0; i <= 0; i += 2) {
			counter = i / 2;
			raw[counter] = read_cap_adc_value(this, SX863X_CAPRAW_0_MSB_SPM + i);
			avg[counter] = read_cap_adc_value(this, SX863X_CAPAVG_0_MSB_SPM + i);
			diff[counter] = read_cap_adc_value(this, SX863X_CAPDIFF_0_MSB_SPM + i);
		}
		//dev_err(this->pdev, "%d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d ||  %d  %d  %d \n",raw[0],avg[0],diff[0],raw[1],avg[1],diff[1],raw[2],avg[2],diff[2],raw[3],avg[3],diff[3],raw[4],avg[4],diff[4],raw[5],avg[5],diff[5],raw[6],avg[6],diff[6],raw[7],avg[7],diff[7],raw[8],avg[8],diff[8],raw[9],avg[9],diff[9]);
		dev_err(this->pdev, "%d  %d  %d \n", raw[0], avg[0], diff[0]);

		//if (newInterrupt == 1) {
		if (cancel_delayed_work(&this->dworker) == 0) {
			// If still running wait till finished
			flush_delayed_work(&this->dworker);
		}

		schedule_delayed_work(&this->dworker, msecs_to_jiffies(150));
		// }
	} else if (foundstatus == 0) {
		// Just to be safe, lets reinitialize the device
		retrieve_spm_single(this, SX863X_CAPTHRESH0_SPM, &i);
		if (i != 0x80)
			reinitialize(this);
	}

	// If button interrupt and not wheel interrupt
	//  Process as button interrupt
	// If wheel interrupt and not button interrupt
	//   Process as wheel interrupt
	// If both interrupts
	//   If wheel has nothing touched, process button interrupt
	//   If wheel has something touched, process wheel interrupt
	if (unlikely(this->useIrqTimer && nirqlow)) {
		/* In case we need to send a timer for example on a touchscreen
		 * checking penup, perform this here
		 */
		if (cancel_delayed_work(&this->dworker) == 0) {
			// If still running wait till finished
			flush_delayed_work(&this->dworker);
		}

		schedule_delayed_work(&this->dworker, msecs_to_jiffies(this->irqTimeout));
	}
	else
		schedule_delayed_work(&this->dworker, msecs_to_jiffies(WATCHDOG_MSECS));
}


static void sx86XX_worker_func(struct work_struct *work)
{
	psx86XX_t this = container_of(work, sx86XX_t, dworker.work);

	if (isMonitorMode || !this->get_nirq_low || !this->get_nirq_low()) {
		mutex_lock(&this->mutex);
		/* only run if nirq is high */
		sx86XX_process_interrupt(this, 1);
		mutex_unlock(&this->mutex);
	}
}

#if defined(USE_KERNEL_SUSPEND)
static void sx86XX_suspend(psx86XX_t this)
{
	printk(KERN_INFO "sx86XX_suspend()\n");
	if (this)
		disable_irq(this->irq);
}
static void sx86XX_resume(psx86XX_t this)
{
	printk(KERN_INFO "sx86XX_resume()\n");
	if (this) {
		mutex_lock(&this->mutex);
		/* Just in case need to reset any uncaught interrupts */
		sx86XX_process_interrupt(this, 0);
		mutex_unlock(&this->mutex);
		if (this->init)
			this->init(this);
		enable_irq(this->irq);
	}
}
#endif

#ifdef CONFIG_HAS_WAKELOCK
extern suspend_state_t get_suspend_state(void);
static void sx86XX_early_suspend(struct early_suspend *h)
{
	psx86XX_t this = NULL;

	dev_dbg(this->pdev, "inside sx86XX_early_suspend()\n");
	this = container_of(h, sx86XX_t, early_suspend);
	sx86XX_suspend(this);
	dev_dbg(this->pdev, "exit sx86XX_early_suspend()\n");
}

static void sx86XX_late_resume(struct early_suspend *h)
{
	psx86XX_t this = NULL;

	dev_dbg(this->pdev, "inside sx86XX_late_resume()\n");
	this = container_of(h, sx86XX_t, early_suspend);
	sx86XX_resume(this);
	dev_dbg(this->pdev, "exit sx86XX_late_resume()\n");
}
#endif

static int sx86XX_init(psx86XX_t this)
{
	printk(KERN_INFO "sx86XX_init()\n");

	/* initialize mutex */
	mutex_init(&this->mutex);

	/* initialize worker function */
	INIT_DELAYED_WORK(&this->dworker, sx86XX_worker_func);
#ifdef CONFIG_HAS_WAKELOCK
	this->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	this->early_suspend.suspend = sx86XX_early_suspend;
	this->early_suspend.resume = sx86XX_late_resume;
	register_early_suspend(&this->early_suspend);
	if (has_wake_lock(WAKE_LOCK_SUSPEND) == 0 &&
	    get_suspend_state() == PM_SUSPEND_ON)
		sx86XX_early_suspend(&this->early_suspend);
#endif          //CONFIG_HAS_WAKELOCK
	/* call init function pointer (this should initialize all registers */
	return this->init ? this->init(this) : 0;
}

static void sx86XX_remove(psx86XX_t this)
{
	printk(KERN_INFO "sx86XX_remove()\n");

	if (!cancel_delayed_work_sync(&this->dworker))
		flush_delayed_work(&this->dworker);
	/*destroy_workqueue(this->workq); */
#ifdef CONFIG_HAS_WAKELOCK
	unregister_early_suspend(&this->early_suspend);
#endif
}
/*! \fn static int read_register(psx86XX_t this, u8 address, u8 *value)
 * \brief Reads a register's value from the device
 * \param this Pointer to main parent struct
 * \param address 8-Bit address to read from
 * \param value Pointer to 8-bit value to save register value to
 * \return Value from i2c_smbus_read_byte_data if < 0. else 0
 */
static int read_register(psx86XX_t this, u8 address, u8 *value)
{
	struct i2c_client *i2c = NULL;
	s32 returnValue = 0;

	if (this && value && this->bus) {
		i2c = this->bus;
		returnValue = i2c_smbus_read_byte_data(i2c, address);
		dev_dbg(&i2c->dev, "read_register Address: 0x%x Return: 0x%x\n", address, returnValue);
		if (returnValue >= 0) {
			*value = returnValue;
			return 0;
		} else
			return returnValue;
	}
	return -ENOMEM;
}
/*! \brief Sends a write register range to the device
 * \param this Pointer to main parent struct
 * \param reg 8-bit register address (base address)
 * \param data pointer to 8-bit register values
 * \param size size of the data pointer
 * \return Value from i2c_master_send
 */
static int write_registerEx(psx86XX_t this, unsigned char reg,
			    unsigned char *data, int size)
{
	struct i2c_client *i2c = NULL;
	int ret = 0;

	if (this && (i2c = this->bus) && data && size <= MAX_WRITE_ARRAY_SIZE) {
		dev_dbg(this->pdev, "inside sx863x_i2c_writeEx()\n");
		ret = i2c_smbus_write_i2c_block_data(i2c, reg, size, data);
	}
	if (ret < 0)
		dev_err(this->pdev, "I2C write error\n");
	dev_dbg(this->pdev, "leaving sx863x_i2c_write()\n");

	return ret;
}
/*! \brief Reads a group of registers from the device
 * \param this Pointer to main parent struct
 * \param reg 8-Bit address to read from (base address)
 * \param data Pointer to 8-bit value array to save registers to
 * \param size size of array
 * \return Value from i2c_smbus_read_byte_data if < 0. else 0
 */
static int read_registerEx(psx86XX_t this, unsigned char reg,
			   unsigned char *data, int size)
{
	struct i2c_client *i2c = NULL;
	int ret = 0;

	if (this && (i2c = this->bus) && data && size <= MAX_WRITE_ARRAY_SIZE) {
		dev_dbg(this->pdev, "inside read_registerEx()\n");
		ret = i2c_smbus_read_i2c_block_data(i2c, reg, size, data);
	}
	if (unlikely(ret < 0))
		dev_err(this->pdev, "I2C read error\n");
	dev_dbg(this->pdev, "leaving sx863x_i2c_readEx()\n");
	return ret;
}
/*! \brief Update SPM memory in an 8 byte block
 * \details Data must already by 8 byte block aligned.
 * \param this Pointer to main parent struct
 * \param baseAddress the base address to start the 8 byte block
 * \param pSpmBuffer array of data
 * \return Value return value from last read or write register
 */
static int update_spm_8block(psx86XX_t this,
			     unsigned char baseAddress,
			     unsigned char *pSpmBuffer)
{
	u8 reg_value = 0;
	psx8635_t pDevice = NULL;
	int ret;

	if (!this || !(pDevice = this->pDevice))
		return 0;
	dev_dbg(this->pdev, "inside sx863x_update_spm_block()\n");

	/* reset in case no interrupt */
	pDevice->lastIrqSrc &= ~SX863X_IRQSRC_SPM_WRITE_FLAG;
	//read_register(this, SX863X_COMPOPMODE_REG, &pDevice->compopmode_Value);
	ret = read_register(this, SX863X_SPMCFG_REG, &reg_value);
	if (unlikely(ret < 0))
		return ret;
	reg_value &= (~SX863X_SPMCFG_I2CSPM_MSK);
	ret = write_register(this,
			SX863X_SPMCFG_REG, reg_value | SX863X_SPMCFG_I2CSPM_ON);
	msleep(30);
	if (unlikely(ret < 0))
		return ret;
	ret = write_register(this, SX863X_SPMBASEADDR_REG, baseAddress);
	if (unlikely(ret < 0))
		return ret;
	ret = write_registerEx(this, 0x00, pSpmBuffer, 8);
	if (unlikely(ret < 0))
		return ret;
	ret = write_register(this, SX863X_SPMCFG_REG, SX863X_SPMCFG_I2CSPM_OFF);
	if (unlikely(ret < 0))
		return ret;

	/* instead of looking at interrupt, just wait 300ms. Since this is only
	 * done on startup, this shouldn't affect performance
	 */
	dev_dbg(this->pdev, "going to wait 50ms for programing complete\n");
	msleep(50);
	dev_dbg(this->pdev, "leaving sx863x_update_spm_block()\n");

	return 0;
}

/*! \brief Retrieve SPM memory in an 8 byte block
 * \details Data must already by 8 byte block aligned.
 * \param this Pointer to main parent struct
 * \param baseAddress the base address to start the 8 byte block read
 * \param pSpmBuffer array of data
 * \return Value return value from last read or write register
 */
static int retrieve_spm_8block(psx86XX_t this,
			       unsigned char baseAddress,
			       unsigned char *pSpmBuffer)
{
	int ret = 0;
	u8 reg_value = 0;

	if (!this)
		return 0;

	dev_dbg(this->pdev, "inside sx863x_retrieve_spm_8block()\n");
	ret = read_register(this, SX863X_SPMCFG_REG, &reg_value);
	if (unlikely(ret < 0)) return ret;  /* ERROR! */
	reg_value &= (~(SX863X_SPMCFG_I2CSPM_MSK | SX863X_SPMCFG_RWSPM_MSK));
	ret = write_register(this, SX863X_SPMCFG_REG,
		reg_value | SX863X_SPMCFG_I2CSPM_ON | SX863X_SPMCFG_RWSPM_READ);
	msleep(30);
	if (unlikely(ret < 0))
		return ret;
	ret = write_register(this, SX863X_SPMBASEADDR_REG, baseAddress);
	if (unlikely(ret < 0))
		return ret;
	ret = read_registerEx(this, 0x00, pSpmBuffer, 8);
	//if (unlikely(ret<0)) return ret; /* ERROR! */
	ret = write_register(this, SX863X_SPMCFG_REG, SX863X_SPMCFG_I2CSPM_OFF);
	if (unlikely(ret < 0))
		return ret;
	msleep(30);
	dev_dbg(this->pdev, "leaving sx863x_retrieve_spm_8block()\n");

	return 0;
}
static int update_spm_single(psx86XX_t this, unsigned char spmAddress, unsigned char data)
{
	int ret;
	unsigned char offset = 0;
	unsigned char spmDataBuf[8];
	unsigned char spmBaseAddress;

	spmBaseAddress = 0xF8 & spmAddress;
	offset = spmAddress - spmBaseAddress;

	/* Read 8 bytes data from spmBaseAddress */
	ret = retrieve_spm_8block(this, spmBaseAddress, spmDataBuf);
	if (ret < 0)
		return ret;

	spmDataBuf[offset] = data;

	return update_spm_8block(this, spmBaseAddress, spmDataBuf);
}


static int retrieve_spm_single(psx86XX_t this, unsigned char spmAddress, unsigned char *data)
{
	int ret;
	unsigned char offset = 0;
	unsigned char spmDataBuf[8];
	unsigned char spmBaseAddress;

	spmBaseAddress = 0xF8 & spmAddress;
	offset = spmAddress - spmBaseAddress;

	/* Read 8 bytes data from spmBaseAddress */
	ret = retrieve_spm_8block(this, spmBaseAddress, spmDataBuf);
	if (ret < 0)
		return ret;

	*data = spmDataBuf[offset];

	return ret;
}

/*
 *  * Read the CapX adc value
 *  */
static signed short read_cap_adc_value(psx86XX_t this, unsigned char spm_base)
{
	unsigned char HighCapValue, LowCapValue;
	signed short CapValue;

	retrieve_spm_single(this, spm_base, &HighCapValue);
	retrieve_spm_single(this, spm_base + 1, &LowCapValue);
	CapValue = ((HighCapValue << 8) | LowCapValue);

	return CapValue;
}

/*! \fn static int read_regStat(psx86XX_t this)
 * \brief Shortcut to read what caused interrupt.
 * \details This is to keep the drivers a unified
 * function that will read whatever register(s)
 * provide information on why the interrupt was caused.
 * \param this Pointer to main parent struct
 * \return If successful, Value of bit(s) that cause interrupt, else 0
 */
static int read_regStat(psx86XX_t this)
{
	u8 data = 0;

	//printk(KERN_INFO "read_regStat()\n");
	if (this && !read_register(this, SX863X_IRQSRC_REG, &data))
		return (data & 0x00FF);
	return 0;
}

/*! \brief  Initialize SPM config from platform data
 * \param this Pointer to main parent struct
 */
static void sx863x_hw_spm_cfg_init(psx86XX_t this)
{
	unsigned char data = 0;
	unsigned char spmBlock[I2C_SMBUS_BLOCK_MAX];
	unsigned char spmVerifyBlock[I2C_SMBUS_BLOCK_MAX];
	unsigned char spmBaseAddress = 0;
	int i = 0;
	int j = 0;
	int retry = 0;
	psx8635_t pDevice = NULL;
	psx8635_platform_data_t pdata = NULL;

	if (!this || !(pDevice = this->pDevice) || !(pdata = pDevice->hw))
		return;

	/* Program device to go into sleep mode */
	read_register(this, SX863X_COMPOPMODE_REG, &data);
	data = data & ~SX863X_COMPOPMODE_OPERATINGMODE_MSK;
	data = data | SX863X_COMPOPMODE_OPERATINGMODE_SLEEP;
	write_register(this, SX863X_COMPOPMODE_REG, data);
	msleep(50);

	while (i < pdata->spm_cfg_num) {
		spmBaseAddress = 0xF8 & pdata->spm_cfg[i].reg;
		dev_dbg(this->pdev, "Going to Read Spm Block Start: 0x%x\n",
				 spmBaseAddress);
		retrieve_spm_8block(this, spmBaseAddress, &spmBlock[0]);
		dev_dbg(this->pdev, "Read Spm Block StartReg: 0x%x\n", spmBaseAddress);
		for (j = (pdata->spm_cfg[i].reg - spmBaseAddress); j < 8; j++) {
			/* write out as many config parameters that belong to current block */
			if ((spmBaseAddress + j) == pdata->spm_cfg[i].reg) {
				spmBlock[j] = pdata->spm_cfg[i].val;
				dev_dbg(this->pdev, "\t SPM Address: 0x%x Value: 0x%x\n",
					 pdata->spm_cfg[i].reg, pdata->spm_cfg[i].val);
				i++;
			} else
				break;
		}
		retry = 0; // Reset retry counter
		do {
			dev_dbg(this->pdev, "Going to Write Spm Block Start: 0x%x\n", spmBaseAddress);
			update_spm_8block(this, spmBaseAddress, spmBlock);
			dev_dbg(this->pdev, "Wrote Spm Block\n");
			/*****************************************************/
			/* Verify the Write was correct                      */
			/*****************************************************/
			retrieve_spm_8block(this, spmBaseAddress, &spmVerifyBlock[0]);
			for (j = 0; j < 8; j++) {
				if (unlikely(spmBlock[j] != spmVerifyBlock[j])) {
					dev_dbg(this->pdev, "Going to rewrite SPM block!\n");
					break;
				}
			}
			// If all data was verified correctly, break out of this do/while
			// and continue
			if (likely(j == 8)) {
				dev_dbg(this->pdev, "SPM write looks good!\n");
				break;
			}
			/*****************************************************/
			/*****************************************************/
			retry++;
		} while (retry < 10);
	}

#if 0
	/********************************************************************/
	/* Enable this to perform a read on all SPM (could be used to verify data)  */
	/********************************************************************/
	i = 0;
	while (i < pdata->spm_cfg_num) {
		spmBaseAddress = 0xF8 & pdata->spm_cfg[i].reg;
		dev_dbg(this->pdev, "Going to Read Spm Block Start: 0x%x\n",
				 spmBaseAddress);
		retrieve_spm_8block(this, spmBaseAddress, &spmBlock[0]);
		dev_dbg(this->pdev, "Read Spm Block StartReg: 0x%x\n", spmBaseAddress);
		for (j = pdata->spm_cfg[i].reg - spmBaseAddress; j < 8; j++) {
			dev_err(this->pdev, "\t VERIFY SPM Address: 0x%x Value: 0x%x\n",
				 spmBaseAddress + j, spmBlock[j]);

			/* write out as many config parameters that belong to current block */
			if ((spmBaseAddress + j) == pdata->spm_cfg[i].reg) {
				i++;
			}
		}
	}
#endif
	/* Program device to go into active mode */
	read_register(this, SX863X_COMPOPMODE_REG, &data);
	data = data & ~SX863X_COMPOPMODE_OPERATINGMODE_MSK;
	data = data | SX863X_COMPOPMODE_OPERATINGMODE_ACTIVE;
	write_register(this, SX863X_COMPOPMODE_REG, data);
	msleep(50);
}

/*! \brief  Initialize SPM and I2C config from platform data
 * \param this Pointer to main parent struct
 */
static void hw_init(psx86XX_t this)
{
	psx8635_t pDevice = NULL;
	psx8635_platform_data_t pdata = NULL;
	int i = 0;

	/* configure device */
	/* First make sure SPM is setup */
	sx863x_hw_spm_cfg_init(this);
	dev_dbg(this->pdev, "Going to Setup I2C Registers\n");
	if (!this || !(pDevice = this->pDevice) || !(pdata = pDevice->hw)) {
		dev_dbg(this->pdev, "ERROR! platform data 0x%p\n", pDevice->hw);
		return;
	}
	while (i < pdata->i2c_reg_num) {
		/* Write all registers/values contained in i2c_reg */
		dev_dbg(this->pdev, "Going to Write Reg: 0x%x Value: 0x%x\n",
				pdata->i2c_reg[i].reg, pdata->i2c_reg[i].val);
		write_register(this, pdata->i2c_reg[i].reg,
				pdata->i2c_reg[i].val);
		i++;
	}
}

/*********************************************************************/
/*! \brief Perform a manual offset calibration
 * \param this Pointer to main parent struct
 * \return Value return value from the write register
 */
static int manual_offset_calibration(psx86XX_t this)
{
	return write_register(this, SX863X_COMPOPMODE_REG,
			      SX863X_COMPOPMODE_COMPENSATION);
}

/*! \fn static int initialize(psx86XX_t this)
 * \brief Performs all initialization needed to configure the device
 * \param this Pointer to main parent struct
 * \return Last used command's return value (negative if error)
 */
static int initialize(psx86XX_t this)
{
	if (!this)
		return -ENOMEM;

	/* prepare reset by disabling any irq handling */
	this->irq_disabled = 1;
	/* perform a reset */
	write_register(this, SX863X_SOFTRESET_REG, SX863X_SOFTRESET_COMMAND1);
	write_register(this, SX863X_SOFTRESET_REG, SX863X_SOFTRESET_COMMAND2);
	msleep(400);
	/* wait until the reset has finished by monitoring NIRQ */
	//while(this->get_nirq_low && this->get_nirq_low());
	/* re-enable interrupt handling */
	hw_init(this);
	read_regStat(this);
	msleep(400);
	manual_offset_calibration(this);
	this->irq_disabled = 0;
	return 0;
}


/***************************************************************************/
// LED delay structure and static variable
/***************************************************************************/
#define CURRENT_LED_MAX_SIZE 16
struct led_delay {
	/// LED array used for delays to keep track of changes
	struct _ledInfo currentLED[CURRENT_LED_MAX_SIZE];
	/// Size of the current LED array
	int nCurrentLEDSize;
	/// Final LED array we want to use
	struct _ledInfo *finalLED;
	/// Size of the final LED array
	int nFinalLEDSize;
	/// The amount of steps left in the fade process
	int stepsLeft;
	/// This is set in platform data on how many steps we want to have
	int startAmountSteps;
	/// flag to indicate if we set everything off or not
	u8 allOff;
	/// Set in platform data for the delay between each fade event
	int delay;
	/// Set the delay for the proximity timer (in platform data)
	int timeoutDelay;
	/// mutex used for making sure we don't access variable at the same time
	struct mutex mutex;
	struct delayed_work dworker; /* work struct for worker function */
	/// Pointer to parent, used when setting intensities
	psx86XX_t parent;
};
typedef struct led_delay led_delay_t;
static led_delay_t ledDelayInfo;

/*! \brief Handle what to do when a compensation occurs
 * \param this Pointer to main parent struct
 * \param fullStatus The original value of the status register
 */
static void compensationProcess(psx86XX_t this, int fullStatus)
{
	psx8635_t pDevice = NULL;

	if (!this || !(pDevice = this->pDevice))
		return;

	dev_dbg(this->pdev, "Inside compensationProcess()\n");
	mutex_lock(&ledDelayInfo.mutex);
	if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
		// If still running wait till finished
		flush_delayed_work(&ledDelayInfo.dworker);
	}
	mutex_unlock(&ledDelayInfo.mutex);
	smtc_processButtonReleasedState(this, pDevice->touchInformation);
	dev_dbg(this->pdev, "Leaving compensationProcess()\n");
}

/*! \brief Handle what to do when a touch occurs
 * \param this Pointer to main parent struct
 * \param fullStatus The original value of the status register
 */
static void touchProcess(psx86XX_t this, int fullStatus)
{
	u8 i = 0;
	psx8635_t pDevice = NULL;

	if (!this || !(pDevice = this->pDevice))
		return;

	dev_dbg(this->pdev, "Inside touchProcess()\n");
	read_register(this, SX863X_CAPSTATMSB_REG, &i);
	// If we also had a wheel flag, check if wheel was released.
	// If it was released, we want to process a wheel released when
	// no buttons are touched, otherwise process button state
	if ((SX863X_IRQSRC_WHEEL_FLAG & fullStatus) != 0) {
		if ((i & SX863X_CAPSTATMSB_WHL_TOUCHED) == 0) {
			read_register(this, SX863X_CAPSTATLSB_REG, &i);
			if (i == 0) {
				mutex_lock(&ledDelayInfo.mutex);
				if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
					// If still running wait till finished
					flush_delayed_work(&ledDelayInfo.dworker);
				}
				mutex_unlock(&ledDelayInfo.mutex);
				smtc_processWheelReleasedState(this, pDevice->touchInformation);
			} else {
				mutex_lock(&ledDelayInfo.mutex);
				if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
					// If still running wait till finished
					flush_delayed_work(&ledDelayInfo.dworker);
				}
				mutex_unlock(&ledDelayInfo.mutex);
				smtc_processButtonState(this, pDevice->touchInformation, (int)i);
			}
		}
	} else {
		// If no wheel flag, process button state if something is touched, otherwise
		// process release
		read_register(this, SX863X_CAPSTATLSB_REG, &i);
		if (i == 0) {
			mutex_lock(&ledDelayInfo.mutex);
			if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
				// If still running wait till finished
				flush_delayed_work(&ledDelayInfo.dworker);
			}
			mutex_unlock(&ledDelayInfo.mutex);
			smtc_processButtonReleasedState(this, pDevice->touchInformation);
		} else {
			mutex_lock(&ledDelayInfo.mutex);
			if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
				// If still running wait till finished
				flush_delayed_work(&ledDelayInfo.dworker);
			}
			mutex_unlock(&ledDelayInfo.mutex);
			smtc_processButtonState(this, pDevice->touchInformation, (int)i);
		}
	}
	dev_dbg(this->pdev, "Leaving touchProcess()\n");
}

/*! \brief Handle what to do when a wheel interrupt occurs
 * \param this Pointer to main parent struct
 * \param fullStatus The original value of the status register
 */
static void wheelProcess(psx86XX_t this, int fullStatus)
{
	u8 i = 0;
	u8 regValue = 0;
	int position = 0;
	psx8635_t pDevice = NULL;

	if (!this || !(pDevice = this->pDevice))
		return;

	dev_dbg(this->pdev, "Inside wheelProcess()\n");

	read_register(this, SX863X_CAPSTATMSB_REG, &regValue);

	if ((regValue & SX863X_CAPSTATMSB_WHL_TOUCHED) == 0) {
		// If wheel is not touched, find out if we had a button flag
		// If we did not, check to make sure no buttons are touched.
		// If none or touched, process released state
		// Otherwise, process button state
		if ((SX863X_IRQSRC_BUTTON_FLAG & fullStatus) == 0) {
			read_register(this, SX863X_CAPSTATLSB_REG, &i);
			mutex_lock(&ledDelayInfo.mutex);
			if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
				// If still running wait till finished
				flush_delayed_work(&ledDelayInfo.dworker);
			}
			mutex_unlock(&ledDelayInfo.mutex);
			if (i == 0) {
				// Send a release state on the wheel
				smtc_processWheelReleasedState(this, pDevice->touchInformation);
			} else {
				smtc_processButtonState(this, pDevice->touchInformation, (int)i);
			}
		}
	} else {
		mutex_lock(&ledDelayInfo.mutex);
		if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
			// If still running wait till finished
			flush_delayed_work(&ledDelayInfo.dworker);
		}
		mutex_unlock(&ledDelayInfo.mutex);
		// If wheel is touched, process wheel state
		read_register(this, SX863X_WHLPOSMSB_REG, &regValue);
		position = ((u16)(0x0FF & regValue)) << 8;
		read_register(this, SX863X_WHLPOSLSB_REG, &regValue);
		position |= (0x0FF & regValue);
		smtc_processWheelState(this, pDevice->touchInformation, position);
	}
	dev_dbg(this->pdev, "Leaving wheelProcess()\n");
}

/*********************************************************************/
static ssize_t monitor_mode_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	unsigned char data = 0;
	int ret = 0;

	mutex_lock(&sthis->mutex);
	ret = read_register(sthis, 0x0D, &data);
	if (likely(ret >= 0)) {
		if ((data & 0x04) != 0)
			isMonitorMode = 1;
		else
			isMonitorMode = 0;
		mutex_unlock(&sthis->mutex);
		return sprintf(buf, "Monitor Mode: %d\n", (int)isMonitorMode);
	} else {
		mutex_unlock(&sthis->mutex);
		return sprintf(buf, "ERROR Reading Register! (%d)\n", ret);
	}
}

static ssize_t monitor_mode_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	unsigned char data = 0;
	int ret = 0;
	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	mutex_lock(&sthis->mutex);
	isMonitorMode = (val != 0 ? 1 : 0);

	// Set Bit 2 of I2C register SPMCFG (address 0x0D) to 1 for enable)
	// Forces scan period to ~195ms
	// No flag is set in IrqSrc
	// Values are coded 16 bits signed 2 complement format
	// CapxDiff starts at 0xBA  (2 bytes)
	// CapxRaw starts at 0x80  (2 bytes)
	// CapxAvg starts at 0x9A  (2 bytes)
	ret = read_register(sthis, 0x0D, &data);
	if (unlikely(ret < 0)) {
		dev_err(sthis->pdev, "ERROR Reading Register! (%d)\n", ret);
		mutex_unlock(&sthis->mutex);
		return count;
	}
	if (isMonitorMode != 0)
		data = data | 0x04;
	else
		data = data & 0xFB;
	ret = write_register(sthis, 0x0D, data);
	if (unlikely(ret < 0)) {
		dev_err(sthis->pdev, "ERROR Writing Register! (%d)\n", ret);
		mutex_unlock(&sthis->mutex);
		return count;
	}
	// n * 15ms
	ret = update_spm_single(sthis, 0xF9, 10);
	if (unlikely(ret < 0)) {
		dev_err(sthis->pdev, "ERROR Writing Monitor Mode Period! (%d)\n", ret);
		mutex_unlock(&sthis->mutex);
		return count;
	}

	ret = write_register(sthis, 0x0D, data);
	if (unlikely(ret < 0)) {
		dev_err(sthis->pdev, "ERROR Writing Register! (%d)\n", ret);
		mutex_unlock(&sthis->mutex);
		return count;
	}

	mutex_unlock(&sthis->mutex);
	return count;
}

static DEVICE_ATTR(monitormode, 0664, monitor_mode_show, monitor_mode_store);

/*********************************************************************/
static int nvm_program(psx86XX_t this)
{
	int ret = 0;

	printk(KERN_INFO "start - NVM BURN!!!\n");
	ret = write_register(this, 0xAC, 0x62);
	if (unlikely(ret < 0)) return ret;      /* ERROR! */
	ret = write_register(this, 0xAD, 0x9D);
	if (unlikely(ret < 0)) return ret;      /* ERROR! */
	ret = write_register(this, 0x0E, 0xA5);
	if (unlikely(ret < 0)) return ret;      /* ERROR! */
	ret = write_register(this, 0x0E, 0x5A);
	if (unlikely(ret < 0)) return ret;      /* ERROR! */
	printk(KERN_INFO "end   - NVM BURN!!!\n");

	return ret;
}

static ssize_t nvm_program_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	mutex_lock(&sthis->mutex);
	if (val) {
		dev_info(sthis->pdev, "Performing nvm_program()\n");
		nvm_program(sthis);
	} else {
		dev_info(sthis->pdev, "not Performing nvm_program(), passed in 0\n");
	}
	mutex_unlock(&sthis->mutex);
	return count;
}

static DEVICE_ATTR(nvmburn, 0200, NULL, nvm_program_store);

static int reinitialize(psx86XX_t this)
{
	initialize(this);
	return 1;
}

static ssize_t reinitialize_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	mutex_lock(&sthis->mutex);
	if (val) {
		dev_info(sthis->pdev, "Performing reinitialize()\n");
		reinitialize(sthis);
	}
	mutex_unlock(&sthis->mutex);
	dev_dbg(sthis->pdev, "Exiting reinitialize()\n");
	return count;
}

static DEVICE_ATTR(reinitial, 0200, NULL,
		   reinitialize_store);

/*********************************************************************/
/*! \brief sysfs show function for manual calibration which currently just
 * returns register value.
 */
static ssize_t manual_offset_calibration_show(struct device *dev,
					      struct device_attribute *attr, char *buf)
{
	u8 reg_value = 0;

	printk(KERN_INFO "manual_offset_calibration_show()\n");
	mutex_lock(&sthis->mutex);
	dev_info(sthis->pdev, "Reading COMPOPMODE_REG\n");
	read_register(sthis, SX863X_COMPOPMODE_REG, &reg_value);
	mutex_unlock(&sthis->mutex);
	return sprintf(buf, "%d\n", reg_value);
}

/*! \brief sysfs store function for manual calibration
 */
static ssize_t manual_offset_calibration_store(struct device *dev,
					       struct device_attribute *attr,
					       const char *buf, size_t count)
{
	unsigned long val;

	printk(KERN_INFO "manual_offset_calibration_store()\n");
	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	mutex_lock(&sthis->mutex);
	if (val) {
		dev_info(sthis->pdev, "Performing manual_offset_calibration()\n");
		manual_offset_calibration(sthis);
	}
	mutex_unlock(&sthis->mutex);
	return count;
}

static DEVICE_ATTR(calibrate, 0664, manual_offset_calibration_show,
		   manual_offset_calibration_store);

static ssize_t pressed_key_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", press_key);
}

static DEVICE_ATTR(pressed_key, 0444, pressed_key_show, NULL);

/*********************************************************************/
/*! \brief set/get logo led (gpio5) offIntensity. If set, also change intensity.
 * returns current value
 */
static inline int getIntensity(psx86XX_t this, u8 io, u8 *intensity);
static inline int setIntensity(psx86XX_t this, u8 io, u8 intensity);

static ssize_t logoled_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	psx8635_t pDevice = sthis->pDevice;

	return sprintf(buf, "%d\n",
		       pDevice->touchInformation->offSection->pLEDState[5].intensity);
}

static ssize_t logoled_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t count)
{
	unsigned long val;
	psx8635_t pDevice = sthis->pDevice;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	if (val > 255)
		val = 255;
	mutex_lock(&sthis->mutex);

	setIntensity(sthis, 5, (u8)val);
	pDevice->touchInformation->offSection->pLEDState[5].intensity = val;

	mutex_unlock(&sthis->mutex);
	return count;
}

static DEVICE_ATTR(logoled, 0664, logoled_show, logoled_store);

/*********************************************************************/
static struct attribute *sx8635_attributes[] = {
	&dev_attr_calibrate.attr,
	&dev_attr_reinitial.attr,
	&dev_attr_nvmburn.attr,
	&dev_attr_monitormode.attr,
	&dev_attr_pressed_key.attr,
	&dev_attr_logoled.attr,
	NULL,
};
static struct attribute_group sx8635_attr_group = {
	.attrs	= sx8635_attributes,
};
/*********************************************************************/

/***************************************************************************/
static void acpi_touchkey_notify(struct acpi_device *device, u32 event)
{
	if (sthis->irq_disabled)
		return;

	mutex_lock(&sthis->mutex);
	if (cancel_delayed_work(&sthis->dworker) == 0) {
		// If still running wait till finished
		flush_delayed_work(&sthis->dworker);
	}

	// No need to check if nirq_low because this is only called when low
	//if ((!sthis->get_nirq_low) || sthis->get_nirq_low())
	sx86XX_process_interrupt(sthis, 1);
	mutex_unlock(&sthis->mutex);
}

static int acpi_touchkey_add(struct acpi_device *device)
{
	return 0;
}

static int acpi_touchkey_remove(struct acpi_device *device)
{
	return 0;
}

static const struct acpi_device_id touchkey_device_ids[] = {
	{ "ACPI0005", 0 },
	{ "",	      0 },
};

MODULE_DEVICE_TABLE(acpi, touchkey_device_ids);

static struct acpi_driver acpi_touchkey_driver = {
	.name		= "Touch key ACPI driver",
	.class		= ACPI_TOUCHKEY_CLASS,
	.ids		= touchkey_device_ids,
	.ops		= {
		.add	= acpi_touchkey_add,
		.remove = acpi_touchkey_remove,
		.notify = acpi_touchkey_notify,
	},
};
/***************************************************************************/
/***************************************************************************/

/*! \fn static int sx8635_remove(struct i2c_client *client)
 * \brief Called when device is to be removed
 * \param client Pointer to i2c_client struct
 * \return Value from sx86XX_remove()
 */
static int sx8635_remove(struct i2c_client *client)
{
	psx8635_platform_data_t pplatData = NULL;
	psx86XX_t this = i2c_get_clientdata(client);
	psx8635_t pDevice = NULL;

	printk(KERN_INFO "sx8635_remove()\n");
	acpi_bus_unregister_driver(&acpi_touchkey_driver);
	if (this && (pDevice = this->pDevice)) {
		sysfs_remove_group(&client->dev.kobj, &sx8635_attr_group);
		pplatData = client->dev.platform_data;
		if (pplatData && pplatData->exit_platform_hw)
			pplatData->exit_platform_hw();
		input_unregister_device(pDevice->input);
		kfree(this->pDevice);
		this->pDevice = NULL;
	}
	sx86XX_remove(this);
	if (!cancel_delayed_work(&ledDelayInfo.dworker))
		flush_delayed_work(&ledDelayInfo.dworker);
	kfree(this);
	sthis = NULL;
	i2c_set_clientdata(client, NULL);
	return 0;
}

/*! \fn static int sx8635_probe(struct i2c_client *client, const struct i2c_device_id *id)
 * \brief Probe function
 * \param client pointer to i2c_client
 * \param id pointer to i2c_device_id
 * \return Whether probe was successful
 */
static int sx8635_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;

	// psx86XX_t this = 0;
	psx8635_t pDevice = NULL;
	psx8635_platform_data_t pplatData = NULL;

	pr_info("sx8635_probe()\n");
	dev_info(&client->dev, "probe()\n");

	pplatData = client->dev.platform_data;
	if (!pplatData) {
		dev_err(&client->dev, "platform data is required!\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_READ_WORD_DATA)) {
		dev_err(&client->dev, "I2C_FUNC_SMBUS_READ_WORD_DATA not found!!\n");
		return -EIO;
	}

	sthis = kzalloc(sizeof(sx86XX_t), GFP_KERNEL); /* create memory for main struct */
	if (!sthis)
		return -ENOMEM;
	dev_dbg(&client->dev, "\t Initialized Main Memory: 0x%p\n", sthis);

	sthis->irq_disabled = 1;
	/* In case we need to reinitialize data
	 * (e.q. if suspend reset device) */
	sthis->init = initialize;
	/* shortcut to read status of interrupt */
	sthis->refreshStatus = read_regStat;
	/* pointer to function from platform data to get pendown
	 * (1->NIRQ=0, 0->NIRQ=1) */
	/* We don't currently enable GPIOLIB, so calling its functions
	** results in a WARN_ON().  We can probably remove this once we
	** enable GPIOLIB. */
	sthis->get_nirq_low = NULL; /* pplatData->get_is_nirq_low; */
	client->irq = irq_test;
	/* save irq in case we need to reference it */
	sthis->irq = client->irq;
	/* do we need to create an irq timer after interrupt ? */
	sthis->useIrqTimer = 0;

	/* Setup function to call on corresponding reg irq source bit */
	if (MAX_NUM_STATUS_BITS >= 8) {
		sthis->statusFunc[0] = NULL;                    /* SX863X_IRQSRC_OPERATING_MODE_FLAG */
		sthis->statusFunc[1] = compensationProcess;     /* SX863X_IRQSRC_COMPENSATION_FLAG   */
		sthis->statusFunc[2] = touchProcess;            /* SX863X_IRQSRC_BUTTON_FLAG */
		sthis->statusFunc[3] = wheelProcess;            /* SX863X_IRQSRC_WHEEL_FLAG  */
		sthis->statusFunc[4] = NULL;                    /* SX863X_IRQSRC_GPI_FLAG            */
		sthis->statusFunc[5] = NULL;                    /* SX863X_IRQSRC_SPM_WRITE_FLAG      */
		sthis->statusFunc[6] = NULL;                    /* SX863X_IRQSRC_NVM_BURN_FLAG       */
		sthis->statusFunc[7] = NULL;                    /* SX863X_IRQSRC_RESERVED_0x80       */
	}

	/* setup i2c communication */
	sthis->bus = client;

	/* record device struct */
	sthis->pdev = &client->dev;

	/* create memory for device specific struct */
	sthis->pDevice = pDevice = kzalloc(sizeof(sx8635_t), GFP_KERNEL);
	if (!pDevice) {
		kfree(sthis);
		return -ENOMEM;
	}

	dev_dbg(&client->dev, "\t Initialized Device Specific Memory: 0x%p\n", pDevice);

	if (pplatData->init_platform_hw)
		pplatData->init_platform_hw();

	pDevice->hw = pplatData;

	pDevice->input = input_allocate_device();
	pDevice->touchInformation = pplatData->touchInformation;
	// Iniitialize further data for wheel information
	smtc_InitWheelInformation(sthis, pDevice->touchInformation, pDevice->input);

	pDevice->input->name = "Semtech Cap Button";
	pDevice->input->id.bustype = BUS_I2C;
	pDevice->input->dev.parent = sthis->pdev;

	if (input_register_device(pDevice->input)) {
		sx8635_remove(client);
		return -ENOMEM;
	}
	dev_dbg(&client->dev, "\t Going to call Main Init\n");
	sx86XX_init(sthis);
	i2c_set_clientdata(client, sthis);

	err = acpi_bus_register_driver(&acpi_touchkey_driver);
	if (err)
		sx8635_remove(client);
	else
		sysfs_create_group(&client->dev.kobj, &sx8635_attr_group);

	return err;
}

/*====================================================*/
#if defined(USE_KERNEL_SUSPEND)
/***** Kernel Suspend *****/
static int sx8635_suspend(struct i2c_client *client)
{
	unsigned char data = 0;
	psx86XX_t this = i2c_get_clientdata(client);

	printk(KERN_INFO "sx8635_suspend()\n");
	sx86XX_suspend(this);
	read_register(this, SX863X_COMPOPMODE_REG, &data);
	data = data & ~SX863X_COMPOPMODE_OPERATINGMODE_MSK;
	data = data | SX863X_COMPOPMODE_OPERATINGMODE_SLEEP;
	write_register(this, SX863X_COMPOPMODE_REG, data);
	return 0;
}
/***** Kernel Resume *****/
static int sx8635_resume(struct i2c_client *client)
{
	unsigned char data = 0;
	psx86XX_t this = i2c_get_clientdata(client);

	printk(KERN_INFO "sx86XX_resume()\n");
	read_register(this, SX863X_COMPOPMODE_REG, &data);
	data = data & ~SX863X_COMPOPMODE_OPERATINGMODE_MSK;
	data = data | SX863X_COMPOPMODE_OPERATINGMODE_ACTIVE;
	write_register(this, SX863X_COMPOPMODE_REG, data);
	sx86XX_resume(this);
	return 0;
}
#endif
/*====================================================*/

// Set the intensity of the specified io
/*! \brief  Set the current intensity for the specified LED
 * \param this Pointer to main parent struct for LED intensity changes
 * \param io The GPIO used for the LED
 * \param intensity The intensity to set the LED to
 * \return return of last write_register
 * \sa write_register()
 */
static inline int setIntensity(psx86XX_t this, u8 io, u8 intensity)
{
	int registerReturn = 0;

	write_register(this, SX863X_GPPID_REG, io);
	registerReturn = write_register(this, SX863X_GPPINTENSITY_REG, intensity);

	return registerReturn;
}
/*! \brief  Get the current intensity for the specified LED
 * \details Here in case later on want to find the intensity last sent
 * to the device without keeping track of a variable.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param io The GPIO used for the LED
 * \param intensity The intensity that the LED is currently set to (return)
 * \return -1 if unsuccessful, otherwise the return of read_register
 * \sa write_register(), read_register()
 */
static inline int getIntensity(psx86XX_t this, u8 io, u8 *intensity)
{
	int registerReturn = -1;

	if (intensity != NULL) {
		write_register(this, SX863X_GPPID_REG, io);
		registerReturn = read_register(this, SX863X_GPPINTENSITY_REG, intensity);
	}

	return registerReturn;
}

/*! \brief  Updates LED Intensities
 * \details This first checks if a delayed intensity update is running and
 * stops it immediately. This then will set all intensities to the passed in
 * led array.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param led Pointer to the led array to use as the intensities
 * \param size The size of this array
 * \sa mutex_lock(), mutex_unlock(), cancel_delayed_work(), setIntensity()
 */
static void updateIntensities(psx86XX_t this, struct _ledInfo *led, int size)
{
	int i = 0;

	mutex_lock(&ledDelayInfo.mutex);
	if (ledDelayInfo.stepsLeft > 0) {
		// If this was called when a delay is occuring, clear the work queue and
		// set intensities past in to this function
		ledDelayInfo.stepsLeft = 0;

	}
	// Set the currentLED array to what we are passing in
	ledDelayInfo.nCurrentLEDSize = size;
	if (size > CURRENT_LED_MAX_SIZE) {
		// Only use the maximum size so we do not have array overflows
		ledDelayInfo.nCurrentLEDSize = CURRENT_LED_MAX_SIZE;
	}
	for (i = 0; i < ledDelayInfo.nCurrentLEDSize; i++) {
		ledDelayInfo.currentLED[i].io = led[i].io;
		ledDelayInfo.currentLED[i].intensity = led[i].intensity;
		setIntensity(this, led[i].io,
			     led[i].intensity);
	}
	// clear lock
	mutex_unlock(&ledDelayInfo.mutex);
}

/*! \brief  Work Queue Function for Fading of LEDs
 * \details This performs the small LED intensity changes when performing
 * a fade in or fade out.
 * \param work Work struct item
 * \sa mutex_lock(), mutex_unlock(), cancel_delayed_work(), setIntensity(),
                                 updateIntensities(), schedule_delayed_work()
 */
static void workerUpdateIntensities(struct work_struct *work)
{
	int inc = 0;
	int i = 0;
	led_delay_t *this = NULL;

	//printk(KERN_DEBUG "inside workerUpdateIntensities\n");
	if (unlikely(work == NULL)) {
		printk(KERN_ERR "leaving workerUpdateIntensities (work queue NULL)\n");
		return; // ERROR!!
	}
	this = container_of(work, led_delay_t, dworker.work);

	if (this->stepsLeft > 1) {
		/*printk(KERN_DEBUG "leaving workerUpdateIntensities (step: %d)\n",
			this->stepsLeft);*/
		// Calculate the next step in intensity based on how many steps left
		for (i = 0; i < this->nFinalLEDSize; i++) {
			inc = (this->finalLED[i].intensity - this->currentLED[i].intensity);
			if (this->stepsLeft > 0) {
				inc = inc / this->stepsLeft;
			} else {
				printk(KERN_ERR "leaving workerUpdateIntensities stepsLeft 0!!!\n");
				return;
			}
			this->currentLED[i].intensity += inc;
			setIntensity(this->parent, this->currentLED[i].io,
				     this->currentLED[i].intensity);
		}
		// remove step for next time in queue
		this->stepsLeft--;
		// If we still have steps left, requeue this work queue at the delay specified
		if (this->stepsLeft > 0) {
			// requeue work item
			schedule_delayed_work(&this->dworker, this->delay);
		}
	} else {
		// Since we are using integers, just set  the last step's intensity to
		// whatever the final is
		this->stepsLeft = 0;
		//printk(KERN_DEBUG
		//      "leaving workerUpdateIntensities (last step immediately sending)\n");
		updateIntensities(this->parent, this->finalLED, this->nFinalLEDSize);
	}
}

/*! \brief Start a delayed update intensity change.
 * \details This will start the process of fading in the LEDs through a
 * worker queue. If it is currently running and the same final intensities
 * should be used, then ignore this request and let the other complete. If
 * it is not the same LED, go directly to this new LED intensity.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param led Pointer to the led array to use as the final intensity
 * \param size The size of this array
 * \param immediate Whether to start the fading immediately or delay the fade.
 * \sa mutex_lock(), mutex_unlock(), cancel_delayed_work(),
                                 updateIntensities(), schedule_delayed_work()
 */
static void delayUpdateIntensities(psx86XX_t this, struct _ledInfo *led,
				   int size, u8 immediate)
{
	//printk(KERN_DEBUG "inside delayUpdateIntensities\n");
	mutex_lock(&ledDelayInfo.mutex);
	if (ledDelayInfo.stepsLeft > 0) {
		// If we still have steps left from an existing and the led we want to set
		// to is the same as what the existing will go to, just ignore this and
		// let the other continue running.
		if (led == ledDelayInfo.finalLED) {
			//printk(KERN_DEBUG
//"leaving delayUpdateIntensities (delay led is same final. Let original delay take place) \n");
			mutex_unlock(&ledDelayInfo.mutex);
			return;
		}
		// make sure to reset any pending queues if for some reason there are any.
		if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
			// If still running wait till finished
			flush_delayed_work(&ledDelayInfo.dworker);
		}

		// Since we want to go to a different intensity, stop the currently running
		// one and immediately set the intensities
		ledDelayInfo.stepsLeft = 0;

		mutex_unlock(&ledDelayInfo.mutex);
		updateIntensities(this, led, size);
		//printk(KERN_DEBUG
		//     "leaving delayUpdateIntensities (immediately set intensity) \n");
		return;
	}
	// make sure to reset any pending queues if for there are any.
	if (cancel_delayed_work(&ledDelayInfo.dworker) == 0) {
		// If still running wait till finished
		flush_delayed_work(&ledDelayInfo.dworker);
	}

	// start timer to change intensities
	// set the final led to what we want to have for the intensities
	ledDelayInfo.finalLED = led;
	ledDelayInfo.nFinalLEDSize = size;

	// initialize the steps left
	ledDelayInfo.stepsLeft = ledDelayInfo.startAmountSteps;
	// Start workqueue. If we are setting proximity timer (immediate==0), then
	// set a long delay
	if (immediate != 0) {
		schedule_delayed_work(&ledDelayInfo.dworker, 0);
	} else {
		schedule_delayed_work(&ledDelayInfo.dworker, ledDelayInfo.timeoutDelay);
	}
	// clear lock
	mutex_unlock(&ledDelayInfo.mutex);
	//printk(KERN_DEBUG "leaving delayUpdateIntensities started workqueue \n");
}

/*! \brief Process event
 * \details  Since wheel and button both send a pressed/released
 * immediately, we can keep same function.
 * \param input The input to use for sending events
 * \param pInputEvent Pointer to item containing key to send
 * \sa input_report_key(), input_sync()
 */
static inline void smtc_processWheelEvent(struct input_dev *input, inputEvent_t *pInputEvent)
{
	if (unlikely((input == NULL) || (pInputEvent == NULL)))
		return;
	dev_dbg(input->dev.parent, "\t\t\t\tSent Key: %d\n", pInputEvent->key);
	// Send pressed
	press_key = pInputEvent->key; //record in sysfs
	//key_send(pInputEvent->key, 1);
	input_report_key(input, pInputEvent->key, 1);
	input_sync(input);
	// Send release

	//key_send(pInputEvent->key, 0);
	input_report_key(input, pInputEvent->key, 0);
	input_sync(input);
	//press_key = 0;
}
/*! \brief Determine the section ID of the passed in button index data.
 * \details Searches for the button pressed (this requires that it only be
 * called for a touch and not a release as interrupts can detect the
 * difference. This only returns on button section and takes priority over the
 * ones with a higher cap number. For this driver we only have one button and
 * proximity. By construction we are able to show button touch vs proximity
 * touch by using this priority.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pButtonInformation Touch information with settings
 * \param data The button index data used when checking for the proper section.
 * \return unknown section if error, otherwise section
 */
static inline touchSection_t smtc_processButtonIndex(psx86XX_t this,
						     touchInformation_t *pButtonInformation, int data)
{
	int i = 0;
	touchSection_t unknownSection = { 0 };

	if (unlikely((this == NULL) || (pButtonInformation == NULL)))
		return unknownSection;

	for (i = pButtonInformation->nButtonSectionGroupSize - 1; i > 0; i--) {
		if (((1 << i) & data) != 0) {
			//  found index
			break;
		}
	}
	// Return index 0 if not found.. Should never get to this state
	// but this implementation has prox for 0 so it is ok
	return pButtonInformation->buttonSectionGroup[i];
}
/*! \brief Determine the section ID of the passed in position.
 * \details Searches for the position based on the sections available
 * on the wheel.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pWheelInformation Touch information with settings
 * \param position The position to use when checking for the proper section.
 * \return unknown section if error, otherwise section with position updated
 */
static inline touchSection_t smtc_processWheelPosition(psx86XX_t this,
						       touchInformation_t *pWheelInformation, int position)
{
	int i = 0;
	touchSection_t returningSection = { 0 };
	touchSection_t *pSectionArray = NULL;

	if (unlikely((this == NULL) || (pWheelInformation == NULL)))
		return returningSection;
	// Initialize to the start
	pSectionArray = &pWheelInformation->wheelSectionGroup[0];
	// Loop through each section checking the min and max calculated before
	for (i = 0; i < pWheelInformation->nWheelSectionGroupSize; i++) {
		if (pSectionArray == NULL) {
			// ERROR!!!!
			dev_err(pWheelInformation->input->dev.parent,
				"ERROR! While looking for Wheel section, we got null!!\n");
			return returningSection;
		}
		dev_dbg(pWheelInformation->input->dev.parent,
			"\t Checking Section: %d (%d <=  %d  => %d)\n",
			pSectionArray->sectionID, pSectionArray->minSection, position,
			pSectionArray->maxSection);
		if ((position >= pSectionArray->minSection) &&
		    (position <= pSectionArray->maxSection)) {
			// Found correct section!!
			dev_dbg(pWheelInformation->input->dev.parent, "\t\t FOUND SECTION!!!\n");
			break;
		}
		pSectionArray++;
	}
	// As the wheel value wraps around, this case (max_pos <-> min_section)
	// won't be caught.  So  always assume index 0 in the array is this case.
	if ((i >= pWheelInformation->nWheelSectionGroupSize) || (pSectionArray == NULL)) {
		pSectionArray = &pWheelInformation->wheelSectionGroup[0];
		dev_dbg(pWheelInformation->input->dev.parent,
			"\t\t Wraparound.. Using Section: %d (%d <=  %d  => %d)\n",
			pSectionArray->sectionID, pSectionArray->minSection, position,
			pSectionArray->maxSection);
	}
	// Make copy of the section and set the position accordingly
	returningSection = *pSectionArray;
	returningSection.position = position;
	return returningSection;
}
/*! \brief Check if wheel moved
 * \details Compares two sets of wheel data to determine if a move has
 * occurred.  If a move occurred, the correct move code is returned otherwise
 * the STOPPED code is returned. This also checks if the position was a
 * possible wraparound and correctly adjusts for this.
 * \param pWheelInformation Touch information with settings
 * \param phistoryData The previous wheel data to compare with
 * \param pcurrentData The current wheel data to compare with
 * \return UNKNOWN if unsuccessful, otherwise the correct event code
 */
static int smtc_hasWheelMoved(
	touchInformation_t *pWheelInformation, touchSection_t *phistoryData,
	touchSection_t *pcurrentData)
{
	int largeChange = 0;
	int diff = 0;

	if (unlikely((pWheelInformation == NULL) || (phistoryData == NULL) ||
		     (pcurrentData == NULL)))
		return UNKNOWN;
	dev_dbg(pWheelInformation->input->dev.parent,
		"\t Searching for movement before:%d now:%d threshold: %d\n",
		phistoryData->position, pcurrentData->position,
		pWheelInformation->move_threshold);
	// Calculate out what a large change would be for wraparounds
	largeChange = pWheelInformation->max_value
		      - (pWheelInformation->max_value >> 1);
	// check if we went clock wise
	diff = (phistoryData->position - pcurrentData->position);
	if (diff > pWheelInformation->move_threshold) {
		// If there is a large change, could be wraparound, else call it a move
		if (diff < largeChange) {
			// We had a movement clockwise
			dev_dbg(pWheelInformation->input->dev.parent,
				"\t\t Found Clockwise Movement diff: %d\n", diff);
			return MOVE_CWISE;
		}
		// make sure if this is a large change that we check for wrap around
		diff = (pcurrentData->position + pWheelInformation->max_value)
		       - phistoryData->position;
		if (diff > pWheelInformation->move_threshold) {
			// We had a movement clockwise
			dev_dbg(pWheelInformation->input->dev.parent,
				"\t\t Found Counter-Clockwise Movement on wraparound diff: %d\n", diff);
			return MOVE_CCWISE;
		}
		dev_dbg(pWheelInformation->input->dev.parent,
			"\t\t Found Clockwise Movement a diff: %d\n", diff);
		return MOVE_CWISE;
	}
	// check if we went counter clock wise
	diff = diff * (-1);
	if (diff > pWheelInformation->move_threshold) {
		// If there is a large change, could be wraparound, else call it a move
		if (diff < largeChange) {
			// We had a movement clockwise
			dev_dbg(pWheelInformation->input->dev.parent,
				"\t\t Found Counter-Clockwise Movement diff: %d\n", diff);
			return MOVE_CCWISE;
		}
		// make sure if this is a large change that we check for wrap around
		diff = (phistoryData->position + pWheelInformation->max_value)
		       - pcurrentData->position;
		if (diff > pWheelInformation->move_threshold) {
			// We had a movement counter-clockwise
			dev_dbg(pWheelInformation->input->dev.parent,
				"\t\t Found Clockwise Movement on wraparound diff: %d\n", diff);
			return MOVE_CWISE;
		}
		dev_dbg(pWheelInformation->input->dev.parent,
			"\t\t Found Counter-Clockwise Movement a diff: %d\n", diff);
		return MOVE_CCWISE;
	}
	dev_dbg(pWheelInformation->input->dev.parent,
		"\t\t Found Stopped Movement.. diff: %d\n", diff);
	return STOPPED;
}

/*! \brief Initialize wheel information.
 * \details Some data is already initialized from platform data in probe.
 * This performs calculations and other final setups needed to make sure data
 * is ready.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pWheelInformation Touch information with settings to be updated
 * \param input Pre Allocated input event to be used
 * \return 1 for Successful, otherwise -1
 * \sa retrieve_spm_8block(),__set_bit(),mutex_init(),INIT_DELAYED_WORK()
 */
static int smtc_InitWheelInformation(psx86XX_t this,
				     touchInformation_t *pWheelInformation,
				     struct input_dev *input)
{
	unsigned char spmBlock[I2C_SMBUS_BLOCK_MAX];
	unsigned char spmBaseAddress = 0;
	int i = 0;
	int nSectionSize = 0;
	int iSectionPosition = 0;
	u16 wheel_norm = 0;
	u8 regValue = 0;

	if (unlikely((this == NULL) || (pWheelInformation == NULL) ||
		     (pWheelInformation->wheelSectionGroup == NULL)))
		return -1;

	// Read in the Wheel Norm value by accessing SPM
	spmBaseAddress = 0xF8 & SX863X_WHLNORMMSB_SPM;
	retrieve_spm_8block(this, spmBaseAddress, &spmBlock[0]);
	regValue = spmBlock[SX863X_WHLNORMMSB_SPM - spmBaseAddress];
	wheel_norm = ((u16)(0x0FF & regValue)) << 8;
	if (likely((SX863X_WHLNORMLSB_SPM - spmBaseAddress) < 8)) {
		regValue = spmBlock[SX863X_WHLNORMLSB_SPM - spmBaseAddress];
		wheel_norm |= (0x0FF & regValue);
	} else {
		// Should never be here but in case something changes where we
		// are at different 8 byte blocks
		spmBaseAddress = 0xF8 & SX863X_WHLNORMLSB_SPM;
		retrieve_spm_8block(this, spmBaseAddress, &spmBlock[0]);
		regValue = spmBlock[SX863X_WHLNORMLSB_SPM - spmBaseAddress];
		wheel_norm |= (0x0FF & regValue);
	}
	// Calculate the max_value through the wheel_norm and number of sensors
	pWheelInformation->max_value = (wheel_norm >> 5) * pWheelInformation->nSensors;
	// Set the move threshold..
	pWheelInformation->move_threshold = pWheelInformation->max_value >>
					    pWheelInformation->move_threshold_factor;
	// Initialize variable to size of section
	nSectionSize = pWheelInformation->max_value /
		       pWheelInformation->nWheelSectionGroupSize;
	dev_dbg(input->dev.parent,
		"max_value: %d wheel_norm %d sensors: %d move_threhsold: %d sectionSize: %d\n",
		pWheelInformation->max_value, wheel_norm, pWheelInformation->nSensors,
		pWheelInformation->move_threshold, nSectionSize);
	__set_bit(EV_KEY, input->evbit);
	// Set all of the minSection/maxSection items in the wheel section array
	// as well as the input events
	for (i = 0; i < pWheelInformation->nWheelSectionGroupSize; i++) {
		pWheelInformation->wheelSectionGroup[i].minSection = iSectionPosition;
		iSectionPosition += nSectionSize;
		// Subtract one except for last so each section does not overlap the next
		if (i < (pWheelInformation->nWheelSectionGroupSize - 1)) {
			pWheelInformation->wheelSectionGroup[i].maxSection = iSectionPosition - 1;
		} else {
			pWheelInformation->wheelSectionGroup[i].maxSection = iSectionPosition;
		}
		// Set the correct keybits for the wheel input
		__set_bit(pWheelInformation->wheelSectionGroup[i].pInputState->key,
			  input->keybit);
	}
	// Set the correct keybits for the button inputs
	for (i = 0; i < pWheelInformation->nButtonSectionGroupSize; i++) {
		if (pWheelInformation->buttonSectionGroup[i].pInputState != NULL)
			__set_bit(pWheelInformation->buttonSectionGroup[i].pInputState->key,
				  input->keybit);
	}
	// Set input bits for CWISE and CCWISE
	__set_bit(pWheelInformation->moveCWISE_InputState->key, input->keybit);
	__set_bit(pWheelInformation->moveCCWISE_InputState->key, input->keybit);
	// Set the pointer to the correct input for the wheel
	pWheelInformation->input = input;
	// Clear out the currentLED array so we don't have unexpected results
	for (i = 0; i < CURRENT_LED_MAX_SIZE; i++) {
		ledDelayInfo.currentLED[i].io = i;
		ledDelayInfo.currentLED[i].intensity = 0;
	}
	// Set flags in ledDelayInfo (this is static as it is used in the work
	// queue) Also set some of the variables from the platform data
	ledDelayInfo.allOff = 1;
	ledDelayInfo.stepsLeft = 0;
	ledDelayInfo.parent = this;
	ledDelayInfo.startAmountSteps = pWheelInformation->nLEDIncrementSteps;
	ledDelayInfo.delay = pWheelInformation->nLEDDelayBetweenSteps;
	ledDelayInfo.timeoutDelay  = pWheelInformation->nLEDTimeoutOffDelay;
	/* initialize mutex */
	mutex_init(&ledDelayInfo.mutex);
	/* initialize worker function */
	INIT_DELAYED_WORK(&ledDelayInfo.dworker, workerUpdateIntensities);

	return 1;
}

/*! \brief Process the released event for a button
 * \details This processes a release specific for buttons.
 * If the last event was a touch vs proximity or even a wheel move, we need
 * to send an input event for that event.  We then perform a delay in updating
 * LEDs which will eventually fade out.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pButtonInformation Touch information with settings and previous data
 * \return 1 for Successful, otherwise -1
 * \sa smtc_processWheelEvent(), delayUpdateIntensities()
 */
static int smtc_processButtonReleasedState(psx86XX_t this,
					   touchInformation_t *pButtonInformation)
{
	if (unlikely(!this))
		return -1;
	// If the last section was a touch make sure to send this event
	// Proximity does not have an input key so this will just be ignored
	if (pButtonInformation->lastSentSection.state == TOUCH) {

		press_key = 0;
		dev_dbg(pButtonInformation->input->dev.parent,
			"RELEASED.. sending TOUCH\n");
		smtc_processWheelEvent(pButtonInformation->input,
				       pButtonInformation->lastSentSection.pInputState);
		// First immediately set it to the proximity state to be sure we do a delay
		// with proximity LEDs
		pButtonInformation->lastSentSection = *(pButtonInformation->proxSection);
		updateIntensities(this, pButtonInformation->lastSentSection.pLEDState,
				  pButtonInformation->lastSentSection.nLEDStateSize);
	}
	// Since this is a button release event and there is a check before to make
	// sure wheel is also released, we don't need to worry about resetting the
	// LEDs to proximity as it will already have been done
	pButtonInformation->lastSentSection = *(pButtonInformation->offSection);
	// Make sure to delay the change and also set the proximity timer

	if (pButtonInformation->bLEDFadeOutEnable == 1) {
		delayUpdateIntensities(this, pButtonInformation->lastSentSection.pLEDState,
				       pButtonInformation->lastSentSection.nLEDStateSize, 0);
	} else {
		updateIntensities(this, pButtonInformation->lastSentSection.pLEDState,
				  pButtonInformation->lastSentSection.nLEDStateSize);
	}

	// Make sure to set the last action as RELEASED
	pButtonInformation->lastSentSection.state = RELEASED;
	// Make sure to set the allOff flag
	ledDelayInfo.allOff = 1;
	return 1;
}
/*! \brief Process the released event for a wheel
 * \details This processes a release specific for the wheel.
 * If the last event was a wheel touch or even a button touch, we need
 * to send an input event for that event.  We then perform a delay in updating
 * LEDs which will eventually fade out. In case the user removed the touch fast
 * enough to not catch proximity leaving, we make sure LEDs are first forced to
 * the proximity LED intensity before starting the delay.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pWheelInformation Touch information with settings and previous data
 * \return 1 for Successful, otherwise -1
 * \sa smtc_processWheelEvent(), updateIntensities(), delayUpdateIntensities()
 */
static int smtc_processWheelReleasedState(psx86XX_t this,
					  touchInformation_t *pWheelInformation)
{
	if (unlikely(!this))
		return -1;

	// If the last section was a touch (NOT MOVE) make sure to send this event
	if (pWheelInformation->lastSentSection.state == TOUCH) {
		press_key = 0;
		dev_dbg(pWheelInformation->input->dev.parent,
			"RELEASED.. sending TOUCH\n");
		smtc_processWheelEvent(pWheelInformation->input,
				       pWheelInformation->lastSentSection.pInputState);
	}
	// First make sure we are at proximity intensities (if user removed hand
	// fast enough, proximity may be already cleared)
	pWheelInformation->lastSentSection = *(pWheelInformation->proxSection);
	updateIntensities(this, pWheelInformation->lastSentSection.pLEDState,
			  pWheelInformation->lastSentSection.nLEDStateSize);
	// Set to the off section and perform a delay update intensities
	// (use proximity timer as well)
	pWheelInformation->lastSentSection = *(pWheelInformation->offSection);

	if (pWheelInformation->bLEDFadeOutEnable == 1)
		delayUpdateIntensities(this, pWheelInformation->lastSentSection.pLEDState,
				       pWheelInformation->lastSentSection.nLEDStateSize, 0);
	else
		updateIntensities(this, pWheelInformation->lastSentSection.pLEDState,
				  pWheelInformation->lastSentSection.nLEDStateSize);

	// Make sure to set the last state as released
	pWheelInformation->lastSentSection.state = RELEASED;
	// Make sure to set the allOff flag
	ledDelayInfo.allOff = 1;
	return 1;
}

/*! \brief Process the event specific for wheel events
 * \details This determines what section of the wheel has been pressed.
 * After the section has been found, a check is determined to see if it has
 * moved from the previous section's position enough to be qualified as a
 * movement. If it has, move specific information is set and the input event
 * according to that movement is processed (normal touch is performed on the
 * release). Then the LEDs are updated according to the position on the wheel.
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pWheelInformation Touch information with settings and previous data
 * \param position Position of the touch
 * \return 1 for Successful, otherwise -1
 * \sa smtc_processWheelPosition(), smtc_hasWheelMoved(),
 * smtc_processWheelEvent(), updateIntensities()
 */
static int smtc_processWheelState(psx86XX_t this,
				  touchInformation_t *pWheelInformation, int position)
{
	touchSection_t newSection = { 0 };
	int foundState = 0;

	if (unlikely(!this))
		return -1;
	// First determine the section (UP/DOWN/LEFT/RIGHT WHEEL)....
	newSection = smtc_processWheelPosition(this, pWheelInformation, position);
	switch (pWheelInformation->lastSentSection.state) {
	default:
	case RELEASED:
		// If the last section was off or released, then start with touch
		pWheelInformation->lastSentSection = newSection;
		pWheelInformation->lastSentSection.state = TOUCH;
		dev_dbg(pWheelInformation->input->dev.parent,
			"Setting state to TOUCH.. From RELEASED or default\n");
		break;
	case TOUCH:
		// make sure we are coming from a wheel event and not button
		if (pWheelInformation->lastSentSection.sectionID < MAX_WHEEL_STATES) {

			// Determine if we have a move. If we don't, leave things the same as
			// we only send input events if a move or release
			foundState = smtc_hasWheelMoved(pWheelInformation, &pWheelInformation->lastSentSection, &newSection);
			switch (foundState) {
			case MOVE_CWISE:
				pWheelInformation->lastSentSection = newSection;
				pWheelInformation->lastSentSection.state = MOVE_CWISE;
				pWheelInformation->lastSentSection.pInputState = pWheelInformation->moveCWISE_InputState;
				dev_dbg(pWheelInformation->input->dev.parent,
					"Setting state to MOVE_CWISE.. From TOUCH\n");
				break;
			case MOVE_CCWISE:
				pWheelInformation->lastSentSection = newSection;
				pWheelInformation->lastSentSection.state = MOVE_CCWISE;
				pWheelInformation->lastSentSection.pInputState = pWheelInformation->moveCCWISE_InputState;
				dev_dbg(pWheelInformation->input->dev.parent,
					"Setting state to MOVE_CCWISE.. From TOUCH\n");
				break;
			default:
				// No movement, keep as TOUCH so that when a release occurs, we know
				// to send the event.
				dev_dbg(pWheelInformation->input->dev.parent,
					"Keeping as TOUCH\n");
				break;
			}
		} else {
			// If we are coming from a button touch, just reset it to this
			pWheelInformation->lastSentSection = newSection;
			pWheelInformation->lastSentSection.state = TOUCH;
			dev_dbg(pWheelInformation->input->dev.parent,
				"Last was a Button touch, Setting state to TOUCH for wheel..\n");
		}
		break;
	case MOVE_CWISE:
	case MOVE_CCWISE:
	case STOPPED:
		// If we moved at all process similarly to touch BUT we don't have to
		// check if a button TOUCH as buttons do not set MOVE or STOPPED
		foundState = smtc_hasWheelMoved(pWheelInformation, &pWheelInformation->lastSentSection, &newSection);
		switch (foundState) {
		case MOVE_CWISE:
			pWheelInformation->lastSentSection = newSection;
			pWheelInformation->lastSentSection.state = MOVE_CWISE;
			pWheelInformation->lastSentSection.pInputState = pWheelInformation->moveCWISE_InputState;
			dev_dbg(pWheelInformation->input->dev.parent,
				"Setting state to MOVE_CWISE.. \n");
			break;
		case MOVE_CCWISE:
			pWheelInformation->lastSentSection = newSection;
			pWheelInformation->lastSentSection.state = MOVE_CCWISE;
			pWheelInformation->lastSentSection.pInputState = pWheelInformation->moveCCWISE_InputState;
			dev_dbg(pWheelInformation->input->dev.parent,
				"Setting state to MOVE_CCWISE.. \n");
			break;
		default:
			// No movement
			pWheelInformation->lastSentSection.state = STOPPED;
			pWheelInformation->lastSentSection.pInputState = NULL;
			dev_dbg(pWheelInformation->input->dev.parent,
				"Setting state to STOPPED.. \n");
			break;
		}
		;
		break;
	}
	;

	// If we have a touch, don't send event until release.
	if (pWheelInformation->lastSentSection.state != TOUCH)
		smtc_processWheelEvent(pWheelInformation->input, pWheelInformation->lastSentSection.pInputState);
	// Update the LEDs immediately on any wheel event
	updateIntensities(this, pWheelInformation->lastSentSection.pLEDState, pWheelInformation->lastSentSection.nLEDStateSize);
	// Make sure the allOff flag is cleared
	ledDelayInfo.allOff = 0;
	return 1;
}

/*! \brief Processes a button event
 * \details This determines what button has been pressed (only one is
 * accepted and the higher CAP number takes priority). This
 * will then update intensities according to the section or
 * button found.  This also will check if the button is
 * proximity and if it is with no previous touch (user moves
 * hand over sensors) it will perform a fade in effect by
 * calling delayUpdateIntensities().
 * \param this Pointer to main parent struct for LED intensity changes
 * \param pButtonInformation Touch information with settings and  previous data
 * \param data Touch data in the form of index for each button pressed
 * \return 1 for Successful, otherwise -1
 * \sa smtc_processButtonIndex(), smtc_processWheelEvent(),
 * delayUpdateIntensities(),updateIntensities()
 */
static int smtc_processButtonState(psx86XX_t this,
			touchInformation_t *pButtonInformation, int data)
{
	touchSection_t newSection = { 0 };

	if (unlikely(!this) || !pButtonInformation)
		return -1;
	// First determine the section (MIDDLE or PROX button)....
	newSection = smtc_processButtonIndex(this, pButtonInformation, data);
	switch (pButtonInformation->lastSentSection.state) {
	default:
	case RELEASED:
		// If the last section was off or released, start with touch
		pButtonInformation->lastSentSection = newSection;
		pButtonInformation->lastSentSection.state = TOUCH;
		dev_dbg(pButtonInformation->input->dev.parent,
		"button Setting state to TOUCH.. From RELEASED or default\n");
		break;
	case TOUCH:
		// If we are getting a proximity and not middle button,
		// this msut mean we lost the middle button touch
		if (newSection.sectionID != BUTTON_PROX) {
			pButtonInformation->lastSentSection = newSection;
			pButtonInformation->lastSentSection.state = TOUCH;
			dev_dbg(pButtonInformation->input->dev.parent,
				"button ReSetting state to TOUCH..\n");
		} else {
			// Since we released the last touch, process the previous event first
			smtc_processWheelEvent(pButtonInformation->input,
					       pButtonInformation->lastSentSection.pInputState);
			pButtonInformation->lastSentSection = newSection;
			pButtonInformation->lastSentSection.state = TOUCH;
			dev_dbg(pButtonInformation->input->dev.parent,
				"button a ReSetting state to TOUCH..\n");
		}
		break;
	}

	// IF we are getting a proximity event and the leds are off (allOff==1),
	// then we want to fade in, otherwise, set the leds immediately
	if ((pButtonInformation->lastSentSection.sectionID == BUTTON_PROX)  &&
	     (ledDelayInfo.allOff == 1) &&
	     (pButtonInformation->bLEDFadeInEnable == 1)) {
		delayUpdateIntensities(this,
			pButtonInformation->lastSentSection.pLEDState,
			pButtonInformation->lastSentSection.nLEDStateSize, 1);
	} else {
		updateIntensities(this,
			pButtonInformation->lastSentSection.pLEDState,
			pButtonInformation->lastSentSection.nLEDStateSize);
	}
	// Since we had some touch, make sure we clear the all off flag
	ledDelayInfo.allOff = 0;
	return 1;
}
/*===========================================================================*/


/*====================================================*/
static struct i2c_device_id sx8635_idtable[] = {
	{ DRIVER_NAME, 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, sx8635_idtable);

static struct i2c_driver sx8635_driver = {
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= DRIVER_NAME
	},
	.id_table	= sx8635_idtable,
	.probe		= sx8635_probe,
	.remove		= sx8635_remove,
#if defined(USE_KERNEL_SUSPEND)
	.suspend	= sx8635_suspend,
	.resume		= sx8635_resume,
#endif
};

static int __init sx8635_init(void)
{
	return i2c_add_driver(&sx8635_driver);
}

static void __exit sx8635_exit(void)
{
	i2c_del_driver(&sx8635_driver);
}

module_init(sx8635_init);
module_exit(sx8635_exit);
module_param(irq_test, int, 0644);
MODULE_PARM_DESC(irq_test, "sx8635 irq number");

MODULE_AUTHOR("Semtech Corp. (http://www.semtech.com/)");
MODULE_DESCRIPTION("SX8635 Capacitive Touch Controller Driver");
MODULE_LICENSE("GPL");
