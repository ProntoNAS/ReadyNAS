/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

*******************************************************************************/
#include "mvTypes.h"
#include "twsi/mvTwsi.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include <linux/proc_fs.h>
	
MV_VOID usiFanG760aSpeedSet(MV_U32 percnt);
MV_U8 usiFanG760aSpeedGet(MV_VOID);

static struct proc_dir_entry *FanG760a_proc;
static struct proc_dir_entry *fan0_pwm_proc;
static struct proc_dir_entry *fan0_rpm_proc;

/*******************************************************************************
* usiFanG760aSpeedSet - Set the Fan Speed Register.
*
* DESCRIPTION:
*       This function sets the Fan Speed Register which determines the Fan Speed 
*       when the FAN is on. and the FAN register value(N) is given by :
*          N = (clk * 30) / (rpm * P), here P=4, and clk = 32768
*
* INPUT:
*       percent : which determine the fan rotation's percent.
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID usiFanG760aSpeedSet(MV_U32 percent)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 data;
	MV_U8 SpeedN = 0;

	//	data = 300 * percent;

	//SpeedN = (32768 * 30) / data / 4;
	
	//	SpeedN = (MV_U8)(16384 / (10 * percent));
	
	//SpeedN = 220;

	//printk("martin : percent = %d \n", percent);

        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x3E;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = 0x0; //0x0 is the Speed Register Offset
        twsiSlave.moreThen256 = MV_FALSE;
	/* 0 : full speed; 0xff : stop */
	SpeedN = 0xFF*(100 - percent)/100;
        mvTwsiWrite (0, &twsiSlave, &SpeedN, 1);

	return;
}

/*******************************************************************************
* usiFanG760aSpeedGet - Get the FAN Speed.
*
* DESCRIPTION:
*       This function reads the actual fan speed register to get the fan speed 
*
* INPUT:
*       NULL;
*
* OUTPUT:
*       NULL;
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U8 usiFanG760aSpeedGet(MV_VOID)
{
	MV_TWSI_SLAVE   twsiSlave;
	MV_U8 ActSpeed = 0, CtrlReg;
#if 0
	twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x3E;
        twsiSlave.validOffset = MV_TRUE; //offset is valid
        twsiSlave.offset = 0x0; //the register offset
        twsiSlave.moreThen256 = MV_FALSE; //whether the address is over 256
        mvTwsiRead (&twsiSlave, &ActSpeed, 1);
	
	printk("set speed:%d\n", ActSpeed);

	twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x1E; //a dummy read
        twsiSlave.validOffset = MV_TRUE; //offset is valid
        twsiSlave.offset = 0x0; //the register offset
        twsiSlave.moreThen256 = MV_FALSE; //whether the address is over 256
        mvTwsiRead (&twsiSlave, &ActSpeed, 1);

#endif
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x3E;
        twsiSlave.validOffset = MV_TRUE; //offset is valid
        twsiSlave.offset = 0x1; //the register offset
        twsiSlave.moreThen256 = MV_FALSE; //whether the address is over 256
        mvTwsiRead (0, &twsiSlave, &ActSpeed, 1);

	//printk("FanSpeed=%d\n", ActSpeed);
#if 0


	twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x3E;
        twsiSlave.validOffset = MV_TRUE; //offset is valid
        twsiSlave.offset = 0x2; //the register offset
        twsiSlave.moreThen256 = MV_FALSE; //whether the address is over 256
        mvTwsiRead (&twsiSlave, &CtrlReg, 1);

        printk("CtrlReg=%d\n", CtrlReg);
#endif

        return ActSpeed;
}


/*
  A : Martin 2007-10-09
  Description : read the actual fan speed
*/
static int FanG760a_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
#if 0
	MV_U8 ActSpeed, len;
	
	ActSpeed = usiFanG760aSpeedGet();
	
	len = sprintf(page, "FanSpeed=%d \n", ActSpeed);
	
	return 0;
#else
	MV_U8 ActSpeed, len;
	MV_U32 RPM = 0;
	if (off > 0)
	{
            *eof = 1;
            return 0;
        }
	ActSpeed = usiFanG760aSpeedGet();
        // can't divided by 0
        if ( ActSpeed == 0 )
           ActSpeed = 1;
	/* N = (CLK x 30) / (rpm xP) */
#define CLK 8192
	RPM = (CLK * 30) / 2 / ActSpeed;
	ActSpeed = 100 - (ActSpeed * 100) / 0xFF;
	
	len = sprintf(page, "FanSpeed=%d%(RPM=%d) \n", ActSpeed, (ActSpeed == 0)?0:RPM);
	
	return len;
#endif
}

static char G760a_stoc(char *s) 
{
	MV_U8 i=0;

	while (*s) {
		i = i*10 + ((*s) - '0');
		s++;
	}
	return i;
}

static int FanG760a_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char temp_cmd[5], *ptr;
	char cmd[5];
	int k;
	MV_U8 SpeedPercent;

	if(NULL == buffer) 
		return 1;
	
	//mvOsPrintf("\n Format : echo speed_persent_value > FanG760a\n");
	
	copy_from_user(temp_cmd, buffer, 5);
	
	ptr = temp_cmd;
	
	/* chew the "SPACE" before the data string*/
	for(;;) {
		if((*ptr) == ' ') {
			ptr++;
		} else
			break;
	}

	k = 0;
	/* now get the data string, and appended '\0' at the end of the string */
	while(((*ptr) >= '0') && ((*ptr) <= '9')) {
		cmd[k++] = (*(ptr));
		ptr++;
	}
	cmd[k] = '\0';
	
	SpeedPercent = G760a_stoc(cmd);

	
	if(SpeedPercent >= 100)
		SpeedPercent = 100;

	if(SpeedPercent < 0)
		return 0;

	//mvOsPrintf("FanSpeed=%d\n", SpeedPercent);
	
	usiFanG760aSpeedSet(SpeedPercent);

	return 10;
}

static int fan0_pwm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	MV_U8 pwm = 0;
  MV_U8 len = 0;

	if (off > 0)
	{
            *eof = 1;
            return 0;
        }
	pwm = usiFanG760aSpeedGet();
    pwm = 255 - pwm; // our x86 platform is revert, so we follow

	len = sprintf(page, "%d\n", pwm);
	
	return len;

}

MV_VOID fan_pwm_set(MV_U8 pwm_value)
{
	MV_TWSI_SLAVE twsiSlave;
  MV_U8 pwm = pwm_value;
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.slaveAddr.address = 0x3E;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = 0x0; //0x0 is the Speed Register Offset
        twsiSlave.moreThen256 = MV_FALSE;

        mvTwsiWrite (0, &twsiSlave, &pwm, 1);

	return;
}

static int fan0_pwm_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char temp_cmd[5], *ptr;
	char cmd[5];
	int k;
	MV_U8 pwm;

	if(NULL == buffer) 
		return 1;
		
	copy_from_user(temp_cmd, buffer, 5);
	
	ptr = temp_cmd;
	
	/* chew the "SPACE" before the data string*/
	for(;;) {
		if((*ptr) == ' ') {
			ptr++;
		} else
			break;
	}

	k = 0;
	/* now get the data string, and appended '\0' at the end of the string */
	while(((*ptr) >= '0') && ((*ptr) <= '9')) {
		cmd[k++] = (*(ptr));
		ptr++;
	}
	cmd[k] = '\0';
	
	pwm = G760a_stoc(cmd);

	
	if(pwm >= 255)
		pwm = 255;

	if(pwm < 0)
		return 0;

    pwm = 255 - pwm; // our x86 platform is revert, so we follow	
	fan_pwm_set(pwm);

	return 10;
}

static int fan0_rpm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	MV_U8 pwm = 0;
        MV_U8 len = 0;
	MV_U32 RPM = 0;

	if (off > 0)
	{
            *eof = 1;
            return 0;
        }
	pwm = usiFanG760aSpeedGet();
        if ( 255 == pwm )
        {
          RPM = 0;
        }
        else
       	{
	// can't divided by 0  
	if ( pwm == 0 )
            pwm = 1;
        /* N = (CLK x 30) / (rpm xP) */
        #define CLK 8192
	RPM = (CLK * 30) / 2 / pwm;
	}
	len = sprintf(page, "%d\n",RPM);
	
	return len;

}

int __init usiFanG760a_init(MV_VOID)
{
	FanG760a_proc = create_proc_entry("FanG760a", 0777, NULL);
	
	if(NULL == FanG760a_proc) {
		return 0;
	}

	strcpy(FanG760a_proc->name, "FanG760a");

	//FanG760a_proc->owner = THIS_MODULE;
	FanG760a_proc->read_proc = FanG760a_proc_read;
	FanG760a_proc->write_proc = FanG760a_proc_write;
	FanG760a_proc->nlink = 1;
	
	// pwm entry
  fan0_pwm_proc = create_proc_entry("fan0_pwm", 0600, NULL);
	if(NULL == fan0_pwm_proc) {
		return 0;
	}
	strcpy(fan0_pwm_proc->name, "fan0_pwm");
	fan0_pwm_proc->read_proc = fan0_pwm_proc_read;
	fan0_pwm_proc->write_proc = fan0_pwm_proc_write;
	fan0_pwm_proc->nlink = 1;
  
	// rpm entry
  fan0_rpm_proc = create_proc_entry("fan0_rpm", 0444, NULL);
	if(NULL == fan0_rpm_proc) {
		return 0;
	}
	strcpy(fan0_rpm_proc->name, "fan0_rpm");
	fan0_rpm_proc->read_proc = fan0_rpm_proc_read;
	fan0_rpm_proc->nlink = 1;	
	
	return 1;
}

void __exit usiFanG760a_exit(void)
{
	remove_proc_entry("FanG760a", NULL);
	remove_proc_entry("fan0_pwm", NULL);
	remove_proc_entry("fan0_rpm", NULL);
}

module_init(usiFanG760a_init);
module_exit(usiFanG760a_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Martin Chi");
MODULE_DESCRIPTION("FanG760a for fan");

