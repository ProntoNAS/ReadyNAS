#include <linux/module.h>
#include <linux/cdev.h>
/*
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
*/
#include <linux/delay.h>
#include <linux/interrupt.h>
/*
#include <linux/workqueue.h>
*/
#include "gpp/mvGpp.h"
#include "twsi/mvTwsi.h"
#include "pwrctl.h"
#include "rtc/integ_rtc/mvRtcReg.h"

#define PWRCTL_HDD1_GROUP       1
#define PWRCTL_HDD1_LINE        MV_GPP4

#define PWRCTL_HDD2_GROUP       1
#define PWRCTL_HDD2_LINE        MV_GPP5

#define SYS_LED_GROUP  1
#define SYSLEDGPP      MV_GPP6
#define SYSLED_ON              0
#define SYSLED_OFF             1
#define SYSLED_BLINK_EN        2

#define OPTLEDGPP      MV_GPP7
#define OPTLED_ON              0
#define OPTLED_OFF             1
#define OPTLED_BLINK_EN        2

#define SYSFAILGPP      MV_GPP5
#define SYSFAIL_ON              0
#define SYSFAIL_OFF             1
#define SYSFAIL_BLINK_EN        2

#define DHCPACTGPP      MV_GPP16
#define DHCPACT_ON              0
#define DHCPACT_OFF             1
#define DHCPACT_BLINK_EN        2

#define POWER_INT_CAUSE  BIT8
#define POWER_OFF_IRQ   IRQ_GPP_46
#define POWER_INT_GROUP  1
#define POWER_INT_LINE   MV_GPP14

#define POWER_INT_CAUSE1  BIT8
#define POWER_OFF_IRQ1   IRQ_GPP_44
#define POWER_INT_GROUP1  1
#define POWER_INT_LINE1   MV_GPP12


#define PWRCTL_NAME ("pwrctl")

/*for power button define*/
#define POWER_OFF_STATUS 1
#define SWITCH_STATUS_OPTIONAL 2

enum COPY_STATUS{
	COPY_STAGE_1_NO_BTN 	= 0,
	COPY_STAGE_2_BTN_PUSH 	= 1,
	COPY_STAGE_3_BTN_POP	= 2,
	COPY_STAGE_4_BTN_PUSH	= 3,
	COPY_STAGE_5_BTN_ACT	= 4,
};

dev_t usi_pwrctl_no;
struct cdev *usi_pwrctl_cdev = NULL;

static volatile unsigned char copy_btn_status = 0;
static volatile unsigned char power_btn_status = 0;
void i2c_init(MV_U8 chanNum, int speed, int slaveaddr)
{
	MV_TWSI_ADDR slave;
	slave.type = ADDR7_BIT;
	slave.address = slaveaddr;
	mvTwsiInit(chanNum, speed, mvBoardTclkGet(), &slave, 0);
}

int i2c_write(MV_U8 chanNum, unsigned char dev_addr, unsigned int offset, int alen, unsigned char* data, int len)
{
	MV_TWSI_SLAVE twsiSlave;
	//	unsigned int i2cFreq = CFG_I2C_SPEED;
	unsigned int i2cFreq = 100000;
	//	DP(puts("i2c_write\n"));
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.slaveAddr.address = dev_addr;
	if(alen != 0){
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = offset;
		if(alen == 2)
		{
			twsiSlave.moreThen256 = MV_TRUE;
		}
		else
		{
			twsiSlave.moreThen256 = MV_FALSE;
		}
	}

	i2c_init(chanNum, i2cFreq,0); /* set the i2c frequency */

	return mvTwsiWrite (chanNum, &twsiSlave, data, len);
}

static void usiSysLedCtl(int Ctl_Sts)
{
	if (Ctl_Sts == SYSLED_ON)
	{
		//disable blink
		MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
	}
	else if (Ctl_Sts == SYSLED_OFF)
	{
		//led off
		MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) | SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);

		//disable blink
		MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
	}
	else if (Ctl_Sts == SYSLED_BLINK_EN)
	{
		//led off
		MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) | SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);

		MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) | SYSLEDGPP);
		MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
	}
	else
		printk("the led opration isn't supported in the kernel");

}
static void usiSysFailCtl(int Ctl_Sts)
{

       if (Ctl_Sts == SYSFAIL_ON)
       {
       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);

               MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
	}
       else if (Ctl_Sts == SYSFAIL_OFF)
       {
       		//led off
               MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) | SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);

       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
	}
       else if (Ctl_Sts == SYSFAIL_BLINK_EN)
       {
       		//led off
               MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) | SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);

               MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) | SYSFAILGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_EN_REG(SYS_LED_GROUP)) & ~SYSFAILGPP);
	}
	else
		printk("the led opration isn't supported in the kernel");
}
static void usiOptLedCtl(int Ctl_Sts)
{
       if (Ctl_Sts == OPTLED_ON)
       {
       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);

               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);
	}
       else if (Ctl_Sts == OPTLED_OFF)
       {
       		//led off 
               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);

       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);
	}
       else if (Ctl_Sts == OPTLED_BLINK_EN)
       {
       		//led off 
               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);

               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) | OPTLEDGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~OPTLEDGPP);
	}
	else
		printk("the led opration isn't supported in the kernel");

}
static void usiDhcpActCtl(int Ctl_Sts)
{
       if (Ctl_Sts == DHCPACT_ON)
       {
       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);

               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);
	}
       else if (Ctl_Sts == DHCPACT_OFF)
       {
       		//led off 
               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);

       		//disable blink
               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);
	}
       else if (Ctl_Sts == DHCPACT_BLINK_EN)
       {
       		//led off 
               MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);

               MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) | DHCPACTGPP);
               MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~DHCPACTGPP);
	}
	else
		printk("the led opration isn't supported in the kernel");

}
static void usiSetRtcSts(void)
{
	MV_U32 tmpval;
	tmpval = MV_REG_READ(RTC_DATE_REG);
	tmpval |= (0x1 << 22);
	MV_REG_WRITE(RTC_DATE_REG,tmpval);
}
unsigned char usi_pwrctl_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned char c_ret = 0;
	switch(cmd)
	{
	case 302:
		usiDhcpActCtl(DHCPACT_ON);
		break;

	case 303:
		usiDhcpActCtl(DHCPACT_OFF);
		break;
	case 304:
		usiDhcpActCtl(DHCPACT_BLINK_EN);
		break;

	case 305:
		c_ret = copy_btn_status;
		copy_btn_status = COPY_STAGE_1_NO_BTN;
		break;
	case 501:
		c_ret = power_btn_status;
		power_btn_status = 0;
		break;
	case 502:
		usiSysLedCtl(SYSLED_ON);
		break;
	case 503:
		usiSysLedCtl(SYSLED_OFF);
		break;
	case 504:
		usiSysLedCtl(SYSLED_BLINK_EN);
		break;
	case 602:
		usiSysFailCtl(SYSFAIL_ON);
		break;
	case 603:
		usiSysFailCtl(SYSFAIL_OFF);
		break;
	case 604:
		usiSysFailCtl(SYSFAIL_BLINK_EN);
		break;
	case 702:
		usiOptLedCtl(OPTLED_ON);
		break;
	case 703:
		usiOptLedCtl(OPTLED_OFF);
		break;
	case 704:
		usiOptLedCtl(OPTLED_BLINK_EN);
		break;
	case 801:
		usiSetRtcSts();
		break;

	default:
		printk("%s: ioctl wrong input command(0x%x)\n",PWRCTL_NAME ,cmd);
		return -1;
	}
	return c_ret;
}
static void usi_gpio_init(void)
{
//	MV_U32 tmpVal;
//	mvGppTypeSet(PWRCTL_HDD1_GROUP, PWRCTL_HDD1_LINE, (MV_GPP_OUT & PWRCTL_HDD1_LINE));
//	mvGppTypeSet(PWRCTL_HDD2_GROUP, PWRCTL_HDD2_LINE, (MV_GPP_OUT & PWRCTL_HDD2_LINE));

//	tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(PWRCTL_HDD1_GROUP));
	//MV_REG_WRITE(GPP_DATA_OUT_REG(PWRCTL_HDD1_GROUP), tmpVal | PWRCTL_HDD1_LINE);

//	tmpVal = MV_REG_READ(GPP_DATA_OUT_EN_REG(PWRCTL_HDD1_GROUP));
//	MV_REG_WRITE(GPP_DATA_OUT_EN_REG(PWRCTL_HDD1_GROUP), tmpVal & (~PWRCTL_HDD1_LINE));

}

static void usi_power_btn_interrupt_init(void)
{
	MV_U32 RegValue;

	/* Enable GPIO power IO in */
	mvGppTypeSet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN & POWER_INT_LINE));
	mvGppPolaritySet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN_INVERT & POWER_INT_LINE));

	/* Enable GPIO power level sensitive interrupt by high level mask reg*/
	RegValue = MV_REG_READ(GPP_INT_LVL_REG(POWER_INT_GROUP));
	RegValue |= POWER_INT_LINE;
	MV_REG_WRITE(GPP_INT_LVL_REG(POWER_INT_GROUP), RegValue);

	//mvGppPolaritySet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN_INVERT & POWER_INT_LINE) );

	/* Enable GPP power(low 31-24) IRQ interrupt by high IRQ mask reg*/
	RegValue = MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG);
	RegValue |= POWER_INT_CAUSE;
	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, RegValue);


	/* Enable GPIO power IO in */
	mvGppTypeSet(POWER_INT_GROUP1, POWER_INT_LINE1, (MV_GPP_IN & POWER_INT_LINE1));
	mvGppPolaritySet(POWER_INT_GROUP1, POWER_INT_LINE1, (MV_GPP_IN_INVERT & POWER_INT_LINE1));

	/* Enable GPIO power level sensitive interrupt by high level mask reg*/
	RegValue = MV_REG_READ(GPP_INT_LVL_REG(POWER_INT_GROUP1));
	RegValue |= POWER_INT_LINE1;
	MV_REG_WRITE(GPP_INT_LVL_REG(POWER_INT_GROUP1), RegValue);

	//mvGppPolaritySet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN_INVERT & POWER_INT_LINE) );

	/* Enable GPP power(low 31-24) IRQ interrupt by high IRQ mask reg*/
	RegValue = MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG);
	RegValue |= POWER_INT_CAUSE1;
	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, RegValue);

}
static void usi_interrupt_init(void)
{
	/* clear all int */
	MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);

	usi_power_btn_interrupt_init();
}

irqreturn_t power_off_irq_handle(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long irq_flags;
	int i;

	local_irq_save(irq_flags);

	MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
	/* we mask this interrupt */
	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & (~POWER_INT_CAUSE)));

	if(!mvGppPolarityGet(POWER_INT_GROUP, POWER_INT_LINE))
		goto POWER_BTN_EXIT;
	if(!(MV_REG_READ(GPP_DATA_IN_REG(POWER_INT_GROUP)) & POWER_INT_LINE))
		goto POWER_BTN_EXIT;
	
	//MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
	//waiting 4 seconds to poweroff; because power off , the irq delay seconds has no influstion
	for (i=0;i<2000;i++)
	{
		mdelay(1);
		if(!(MV_REG_READ(GPP_DATA_IN_REG(POWER_INT_GROUP)) & POWER_INT_LINE))
			goto POWER_BTN_EXIT;
	}
	MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) | SYSLEDGPP);

	power_btn_status = POWER_OFF_STATUS;
	MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) | SYSLEDGPP);

//	MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
//	MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);

	//CPU power down
//	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~(0x1 << 30));

POWER_BTN_EXIT:

	if(mvGppPolarityGet(POWER_INT_GROUP, POWER_INT_LINE))
	{
		mvGppPolaritySet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN_ORIGIN & POWER_INT_LINE) );
	}else
	{
		mvGppPolaritySet(POWER_INT_GROUP, POWER_INT_LINE, (MV_GPP_IN_INVERT & POWER_INT_LINE) );
	}

	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | (POWER_INT_CAUSE)));
	local_irq_restore(irq_flags);

	return IRQ_HANDLED;
}

irqreturn_t power_off_irq_handle1(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long irq_flags;
	int i;
	static unsigned long flag = 0;

	local_irq_save(irq_flags);

	MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
	/* we mask this interrupt */
	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & (~POWER_INT_CAUSE1)));

	if(!mvGppPolarityGet(POWER_INT_GROUP1, POWER_INT_LINE1))
		goto POWER_BTN_EXIT;
	if(!(MV_REG_READ(GPP_DATA_IN_REG(POWER_INT_GROUP1)) & POWER_INT_LINE1))
		goto POWER_BTN_EXIT;
	
#if 0
	/* if DHCP_ON led on */
	if((MV_REG_READ(GPP_DATA_OUT_REG(1)) & DHCPACTGPP) == 0)
	{
		power_btn_status = SWITCH_STATUS_DHCPON;
	#if 1
		/* DHCP_ON led (MPP48) off */
		MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | DHCPACTGPP); 
		/* DHCP_OFF (MPP39) led on */
		MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~OPTLEDGPP);
	#endif
	}
	else
	{
		power_btn_status = SWITCH_STATUS_DHCPOFF;
	#if 1
		/* DHCP_OFF (MPP39) led off */
		MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | OPTLEDGPP);
		/* DHCP_ON led (MPP48) on */
		MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~DHCPACTGPP); 
	#endif
	}
#endif
	power_btn_status = SWITCH_STATUS_OPTIONAL;

POWER_BTN_EXIT:

	if(mvGppPolarityGet(POWER_INT_GROUP1, POWER_INT_LINE1))
	{
		mvGppPolaritySet(POWER_INT_GROUP1, POWER_INT_LINE1, (MV_GPP_IN_ORIGIN & POWER_INT_LINE1) );
	}else
	{
		mvGppPolaritySet(POWER_INT_GROUP1, POWER_INT_LINE1, (MV_GPP_IN_INVERT & POWER_INT_LINE1) );
	}

	MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | (POWER_INT_CAUSE1)));
	local_irq_restore(irq_flags);

	return IRQ_HANDLED;
}

struct file_operations usi_pwrctl_fops =
{
	ioctl:  usi_pwrctl_ioctl,
};

static int __init usi_pwrctl_init(void)
{
	int usi_pwrctl_major, result;
	/* now register this char device and get major number*/
	usi_pwrctl_major = 221;
	usi_pwrctl_no = usi_pwrctl_major << 20;
	result = register_chrdev_region(usi_pwrctl_no, 1, PWRCTL_NAME);
	if(result < 0)
	{
		printk("%s: can not allocate major number\n",PWRCTL_NAME);
		return -1;
	}
	usi_pwrctl_major = MAJOR(usi_pwrctl_no);
	printk("%s: dev_t_NO. = 0x%x, major = %d\n", PWRCTL_NAME, usi_pwrctl_no, usi_pwrctl_major);

	/* allocate this cdev space */
	usi_pwrctl_cdev = cdev_alloc();
	if(!usi_pwrctl_cdev)
	{
		printk("%s: allocate the cdev mem space fail\n",PWRCTL_NAME);
		return -1;
	}
	cdev_init(usi_pwrctl_cdev, &usi_pwrctl_fops);

	usi_pwrctl_cdev->owner = THIS_MODULE;

	result = cdev_add(usi_pwrctl_cdev, usi_pwrctl_no, 1);
	if(result)
	{
		printk("%s: fail to add cdev into the system\n",PWRCTL_NAME);
		cdev_del(usi_pwrctl_cdev);
		return -1;
	}

        MV_U32 tmpVal;

	//usi_gpio_init();
	//usi_interrupt_init(); #if 0 //libo 2011-03-22 used in 1281P2 before, not used in netgear.

	printk("USB 3.0 Port Power On, Waiting for 3 seconds\n");
	mdelay(3000);
	tmpVal = MV_REG_READ(GPP_DATA_OUT_EN_REG(1));
	MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1), tmpVal & ~(0x1 << 14));
	tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(1));
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), tmpVal | (0x1 << 14));

#if 0 //libo 2011-03-22 used in 1281P2 before, not used in netgear.
	result = request_irq(POWER_OFF_IRQ, power_off_irq_handle, (IRQF_DISABLED), "power off control", NULL);
	result = request_irq(POWER_OFF_IRQ1, power_off_irq_handle1, (IRQF_DISABLED), "power off control1", NULL);
	if(result)
	{
		printk("%s: request the irq power off failed. \n",PWRCTL_NAME);
		return -1;
	}
	else
	{
		printk("%s: request the irq power off registered. \n",PWRCTL_NAME);
	}
#endif
	return result;
}


static void __exit usi_pwrctl_exit(void)
{
	cdev_del(usi_pwrctl_cdev);
	unregister_chrdev_region(usi_pwrctl_no, 1);
}
module_init(usi_pwrctl_init);
module_exit(usi_pwrctl_exit);
