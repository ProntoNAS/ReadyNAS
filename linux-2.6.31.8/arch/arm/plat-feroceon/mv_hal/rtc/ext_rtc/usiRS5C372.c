/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
 
 
/* includes */
#include "usiRS5C372.h"
#include "usiRS5C372Reg.h"
#if 1 //libo 2011-02-21
#include <linux/bcd.h>
#include "mvTypes.h"
#include "twsi/mvTwsi.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include <linux/proc_fs.h>

#endif
static MV_VOID usiRtcCharWrite(MV_U32 offset, MV_U8 data);
static MV_VOID usiRtcCharRead(MV_U32 offset, MV_U8 *data);

/*******************************************************************************
* mvRtcDS1339TimeSet - Set the Alarm of the Real time clock
*
* DESCRIPTION:
*
* INPUT:
*       time - A pointer to a structure RTC_TIME (defined in mvDS1339.h).
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID usiRS5C372AlarmSet(MV_RTC_TIME* time, MV_U8 *enabled, MV_U8 * pending)
{
#if 1 //libo 2011-02-21
	int			i;
	MV_U8		buf[3];
#if 0
	/* only handle up to 24 hours in the future, like RTC_ALM_SET */
	if (t->time.tm_mday != -1
			|| t->time.tm_mon != -1
			|| t->time.tm_year != -1)
		return -EINVAL;
#endif
	/* REVISIT: round up tm_sec */

	/* if needed, disable irq (clears pending status) */
#define CTRL1_AALE		(1 << 7)
#define CT0			(1 << 0)
#define CT1			(1 << 1)
#define CT2			(1 << 2)
	usiRtcCharRead(RTC_CONTROL_1,&buf[0]);
	if (buf[0] & CTRL1_AALE) {
		
		buf[0] = buf[0] & (~CTRL1_AALE);
		usiRtcCharWrite(RTC_CONTROL_1,buf[0]);
	}

	usiRtcCharRead(RTC_CONTROL_1, &buf[0]);
	if (buf[0] & BIT5)
		buf[0] &= ~BIT5;
	else
		buf[0] |= BIT5;
	usiRtcCharWrite(RTC_CONTROL_1,buf[0]);
        //printk(KERN_INFO "libo ...... ctrl 1 0x%x\n", buf[0]);
	/* set alarm */
	buf[0] = bin2bcd(time->minutes);
        //printk(KERN_INFO "libo ...... MINUTE = %d\n", buf[0]);
	/* hours (24) */ 
	buf[1] = bin2bcd(time->hours);
        //printk(KERN_INFO "libo ...... HOURS = %d\n", buf[1]);
	//buf[2] = 0x7f;	/* any/all days */
	// day of the week.
  buf[2] = time->day;
	//printk(KERN_INFO "libo ...... DAYS = %d\n", buf[2]);

	for (i = 0; i < sizeof(buf); i++) {
		usiRtcCharWrite(((RTC_ALARM_A_MINUTE >> 4) + i) << 4,buf[i]);
	}

	/* ... and maybe enable its irq */
	if (*enabled)
	{
		usiRtcCharRead(RTC_CONTROL_1,&buf[0]);
		if ((buf[0] & CTRL1_AALE) == 0) {
		
			buf[0] = buf[0] | CTRL1_AALE;
			usiRtcCharWrite(RTC_CONTROL_1,buf[0]);
		}
	}

#endif //libo 2011-02-21
	return;
}
#if 1 //libo 2011-02-21
MV_32  usiRS5C372AlarmRead(MV_RTC_TIME* time, MV_U8 *enabled, MV_U8 * pending)
{

	MV_U8 tempValue;
#if 1 //libo 2011-03-08
	usiRtcCharRead(RTC_CONTROL_1, &tempValue);
	//printk("<0>""libo...... ctrl 1 0x%x\n", tempValue);
#endif
	/* report alarm time */
	time->seconds= 0;
	usiRtcCharRead(RTC_ALARM_A_MINUTE,&tempValue);
        //printk(KERN_INFO "libo ...... MINUTE = %d\n", tempValue);
	time->minutes= bcd2bin(tempValue & 0x7f);
	usiRtcCharRead(RTC_ALARM_A_HOUR,&tempValue);
        //printk(KERN_INFO "libo ...... HOUR = %d\n", tempValue);
	time->hours= bcd2bin(tempValue);
	usiRtcCharRead(RTC_ALARM_A_DAY,&tempValue);
        //printk(KERN_INFO "libo ...... HOUR = %d\n", tempValue);
	time->day= tempValue & 0x7f;	
#if 0
	t->time.tm_mday = -1;
	t->time.tm_mon = -1;
	t->time.tm_year = -1;
	t->time.tm_wday = -1;
	t->time.tm_yday = -1;
	t->time.tm_isdst = -1;
#endif
	/* ... and status */
	usiRtcCharRead(RTC_CONTROL_1,&tempValue);
	*enabled = !!(tempValue & (1 << 7));
	
	usiRtcCharRead(RTC_CONTROL_2,&tempValue);
	*pending = !!(tempValue & (1 << 1));

	return 0;
}

#endif
/*******************************************************************************
* mvRS5C372TimeSet - Update the Real Time Clock.
*
* DESCRIPTION:
*       This function sets a new time and date to the RTC from the given 
*       structure RTC_TIME . All fields within the structure must be assigned 
*		with a value prior to the use of this function.
*
* INPUT:
*       time - A pointer to a structure RTC_TIME (defined in mvDS1339.h).
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvRS5C372TimeSet(MV_RTC_TIME* time)
{
	MV_U8 tempValue;
	MV_U8 tens;
	MV_U8 single;

	//printk(KERN_INFO "libo ...... %s, %d.\n", __func__, time->year);
	/* seconds */
        tens = time->seconds / 10;
        single = time->seconds % 10;
	tempValue = ((tens  << RTC_CLOCK_10SECONDS_SHF )& RTC_CLOCK_10SECONDS_MSK )|
			      (( single << RTC_CLOCK_SECONDS_SHF)&RTC_CLOCK_SECONDS_MSK);
	usiRtcCharWrite(RTC_CLOCK_SECONDS,tempValue);

	/* minutes */
	tens = time->minutes / 10;
	single = time->minutes % 10;
	tempValue = ((tens  << RTC_CLOCK_10MINUTES_SHF )& RTC_CLOCK_10MINUTES_MSK )|
			    (( single << RTC_CLOCK_MINUTES_SHF)& RTC_CLOCK_MINUTES_MSK);
	usiRtcCharWrite(RTC_CLOCK_MINUTES,tempValue);

	/* hours (24) */ 
	tens = time->hours / 10;
	single = time->hours % 10;
	tempValue = ((tens << RTC_CLOCK_10HOURS_SHF) & RTC_CLOCK_10HOURS_MSK2 )|
				(( single << RTC_CLOCK_HOURS_SHF ) & RTC_CLOCK_HOURS_MSK);
	usiRtcCharWrite(RTC_CLOCK_HOUR,tempValue);

	/* day */ 
	single = time->day;
	tempValue = ((single << RTC_CLOCK_DAY_SHF ) & RTC_CLOCK_DAY_MSK);
	usiRtcCharWrite(RTC_CLOCK_DAY,tempValue);

	/* date */ 
	tens = time->date / 10;
	single = time->date % 10;
	tempValue = ((tens << RTC_CLOCK_10DATE_SHF ) & RTC_CLOCK_10DATE_MSK )|
				((single << RTC_CLOCK_DATE_SHF )& RTC_CLOCK_DATE_MSK);
	usiRtcCharWrite(RTC_CLOCK_DATE, tempValue);

	/* month */ 
	tens = time->month / 10;
	single = time->month % 10;
	tempValue = ((tens << RTC_CLOCK_10MONTH_SHF ) & RTC_CLOCK_10MONTH_MSK )|
				((single << RTC_CLOCK_MONTH_SHF)& RTC_CLOCK_MONTH_MSK);
	usiRtcCharWrite( RTC_CLOCK_MONTH_CENTURY,tempValue);

	/* year */ 
	tens = time->year / 10;
	single = time->year % 10;
	tempValue = ((tens << RTC_CLOCK_10YEAR_SHF) & RTC_CLOCK_10YEAR_MSK )|
				((single << RTC_CLOCK_YEAR_SHF) & RTC_CLOCK_YEAR_MSK);
	usiRtcCharWrite(RTC_CLOCK_YEAR,tempValue);	

	return;
}

/*******************************************************************************
* mvRS5C372TimeGet - Read the time from the RTC.
*
* DESCRIPTION:
*       This function reads the current time and date from the RTC into the 
*       structure RTC_TIME (defined in mvDS1339.h).
*
* INPUT:
*       time - A pointer to a structure TIME (defined in mvDS1339.h).
*
* OUTPUT:
*       The structure RTC_TIME is updated with the time read from the RTC.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvRS5C372TimeGet(MV_RTC_TIME* time)
{
	MV_U8 tempValue;
	MV_U32 tens;
	MV_U32 single;

	/* seconds */ 
	usiRtcCharRead(RTC_CLOCK_SECONDS,&tempValue);
	tens = ( tempValue & RTC_CLOCK_10SECONDS_MSK) >> RTC_CLOCK_10SECONDS_SHF;   
	single = (tempValue & RTC_CLOCK_SECONDS_MSK) >> RTC_CLOCK_SECONDS_SHF ;
	time->seconds = 10*tens + single;

	/* minutes */ 
	usiRtcCharRead(RTC_CLOCK_MINUTES,&tempValue);
	tens = (tempValue & RTC_CLOCK_10MINUTES_MSK) >> RTC_CLOCK_10MINUTES_SHF;   
	single = (tempValue & RTC_CLOCK_MINUTES_MSK) >> RTC_CLOCK_MINUTES_SHF;
	time->minutes = 10*tens + single;		      

	/* hours */ 
	usiRtcCharRead(RTC_CLOCK_HOUR,&tempValue);
	tens = (tempValue & RTC_CLOCK_10HOURS_MSK2) >> RTC_CLOCK_10HOURS_SHF;   
	single = (tempValue & RTC_CLOCK_HOURS_MSK) >> RTC_CLOCK_HOURS_SHF;
	time->hours = 10*tens + single;

	/* day */ 
	usiRtcCharRead(RTC_CLOCK_DAY,&tempValue);
	time->day = (tempValue & RTC_CLOCK_DAY_MSK) >> RTC_CLOCK_DAY_SHF ;

	/* date */ 
	usiRtcCharRead(RTC_CLOCK_DATE,&tempValue);
	tens = (tempValue & RTC_CLOCK_10DATE_MSK) >> RTC_CLOCK_10DATE_SHF;   
	single = (tempValue & RTC_CLOCK_DATE_MSK) >> RTC_CLOCK_DATE_SHF;
	time->date = 10*tens + single;

	/* century/ month */  
	usiRtcCharRead(RTC_CLOCK_MONTH_CENTURY,&tempValue);
	//tempValue = pdata[5];
	tens = (tempValue & RTC_CLOCK_10MONTH_MSK) >> RTC_CLOCK_10MONTH_SHF;   
	single = (tempValue & RTC_CLOCK_MONTH_MSK) >> RTC_CLOCK_MONTH_SHF;
	time->month = 10*tens + single;
	time->century = (tempValue & RTC_CLOCK_CENTURY_MSK)>>RTC_CLOCK_CENTURY_SHF;

	/* year */ 
	usiRtcCharRead(RTC_CLOCK_YEAR,&tempValue);
	tens = (tempValue & RTC_CLOCK_10YEAR_MSK) >> RTC_CLOCK_10YEAR_SHF;   
	single = (tempValue & RTC_CLOCK_YEAR_MSK) >> RTC_CLOCK_YEAR_SHF;
	time->year = 10*tens + single;
#if 0
	printk("robinson : time->seconds = %d \n", time->seconds);
        printk("robinson : time->minutes = %d \n", time->minutes);
        printk("robinson : time->hours = %d \n", time->hours);
        printk("robinson : time->day = %d \n", time->day);
        printk("robinson : time->date = %d \n", time->date);
        printk("robinson : time->year = %d \n", time->year);
#endif
	return;	
}

/*******************************************************************************
* usiRtcDS1339Init - Initialize the clock.
*
* DESCRIPTION:
*       This function initialize the clock registers and read\write functions
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID usiRS5C272Init(MV_VOID)
{

	MV_U8 ucTemp;

	/* We do not use the alarm function .*/
	//usiRtcCharRead(RTC_CONTROL,&ucTemp);
	//ucTemp |= RTC_CONTROL_INTCN_BIT;
 	usiRtcCharWrite(RTC_CONTROL_1, 0x0);
       // printk("ok write\n");
        mvOsDelay(1);	
	usiRtcCharRead(RTC_CONTROL_1, &ucTemp);
       // printk("ok read\n");	
        
       // printk("get the control-1 value ucTemp = 0x%x\n",ucTemp);
	/* disable trickle */
	usiRtcCharWrite(RTC_CONTROL_2, 0x20);
	
	usiRtcCharRead(RTC_CONTROL_2, &ucTemp);
       // printk("get the control-2 value ucTemp = 0x%x\n",ucTemp);
       // printk("end rtc chip init. \n");
	
	return;
}


/* assumption twsi is initialized !!! */
/*******************************************************************************
* usiRtcCharRead - Read a char from the RTC.
*
* DESCRIPTION:
*       This function reads a char from the RTC offset.
*
* INPUT:
*       offset - offset
*
* OUTPUT:
*       data - char read from offset offset.
*
* RETURN:
*       None.
*
*******************************************************************************/
static MV_VOID	usiRtcCharRead(MV_U32 offset, MV_U8 *data)
{
	MV_TWSI_SLAVE   twsiSlave;
	
	twsiSlave.slaveAddr.type = mvBoardRtcTwsiAddrTypeGet(); 
	//get the addr type(7bits or 10bits)
	twsiSlave.slaveAddr.address = mvBoardRtcTwsiAddrGet();
	//get the (MV_U32)address 
        twsiSlave.validOffset = MV_TRUE; //offset is valid
	//twsiSlave.validOffset = MV_FALSE; //offset is valid
	twsiSlave.offset = offset; //the register offset
	twsiSlave.moreThen256 = MV_FALSE; //whether the address is over 256
	//mvTwsiRead (&twsiSlave, data, 1);
        //mvTwsiRead (RTC_I2C_CH, &twsiSlave, data, 1);
        mvTwsiRead (0x00, &twsiSlave, data, 1);
	return;
}

/*******************************************************************************
* usiRtcCharWrite - Write a char from the RTC.
*
* DESCRIPTION:
*       This function writes a char to the RTC offset.
*
* INPUT:
*       offset - offset
*
* OUTPUT:
*       data - char write to addr address.
*
* RETURN:
*       None.
*
*******************************************************************************/
static MV_VOID usiRtcCharWrite(MV_U32 offset, MV_U8 data)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_STATUS ret = 0;

	twsiSlave.slaveAddr.type = mvBoardRtcTwsiAddrTypeGet();
	twsiSlave.slaveAddr.address = mvBoardRtcTwsiAddrGet();
	twsiSlave.validOffset = MV_TRUE; //robinson change
//	twsiSlave.validOffset = MV_FALSE;
	twsiSlave.offset = offset;
	twsiSlave.moreThen256 = MV_FALSE;
	//mvTwsiWrite (&twsiSlave, &data, 1);
        //mvTwsiWrite (RTC_I2C_CH, &twsiSlave, &data, 1);
        ret = mvTwsiWrite (0x00, &twsiSlave, &data, 1);
	//printk(KERN_INFO "libo ...... %s, ret = %d\n", __func__, ret);
	return;
}


MV_STATUS usiNasTwsiInit()
{
        MV_TWSI_ADDR slave;
        MV_U32 tclk;

        tclk = mvBoardTclkGet();
        //printk("robinson : tclk =%d\n",tclk);
        /* Init TWSI first */
        slave.type = ADDR7_BIT;
        slave.address = 0x0;
        
        //MV_U32 mvTwsiInit(MV_U8 chanNum, MV_HZ frequancy, MV_U32 Tclk, MV_TWSI_ADDR *pTwsiAddr, MV_BOOL generalCallEnable)
        //mvTwsiInit(RTC_I2C_CH,100000, tclk, &slave, 0);
        mvTwsiInit(0x00,100000, tclk, &slave, 0);

        return MV_OK;
}

#if 1 //libo 2011-02-21

static struct proc_dir_entry *RS5C372a_alarm_proc;
static int RS5C372a_alarm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data) {
    MV_32 len = 100;
    MV_RTC_TIME time;
    MV_U8 enabled = 0;
    MV_U8 pending = 0;

    if ((off > 0) || (count < len))
    {
        *eof = 1;
	return 0;
    }

    memset(&time, 0, sizeof(time));

    usiRS5C372AlarmRead(&time, &enabled, &pending);
    //printk(KERN_INFO "libo ...... %d\n", enabled);
    if ( enabled == 0 )
      len = sprintf(page, "%s\n","disabled");
    else  
      len = sprintf(page, "1P%dD%dH%dM\n",time.day,time.hours, time.minutes);
    *eof = 1;
    return len;
}

MV_U32 str2int(char * p, char * q) {
    char * front = p;
    char * rear = q;
    MV_U32 i = 0;

    while(rear != front) {
        if (((*rear) >= '0') && ((*rear) <= '9'))
            i = i*10 + ((*rear) - '0');
	rear++;
    }
    return i;
}

MV_U8 process_time(char * cmd, MV_RTC_TIME * time) {

	char * p, * q;
	MV_U8 ret = 0;
	MV_U32 pattern = 0;
	MV_U32 hours = 0;
	MV_U32 minutes = 0;
	MV_U32 day = 0;

	p = q = cmd;
	if (p == NULL) {
	    return ret;
	}
	//printk(KERN_INFO "libo ...... %s\n", p);
	/* Pattern : 1PxxHyyM*/
	while ((*p != 'P') && (*p != '\0')) {
	    p++;
	}
	if (*p == '\0')
	    return ret;
	else
	    pattern = str2int(p, q);
	//printk(KERN_INFO "libo ...... %s\n", p);
	switch (pattern) {
	    case 1:
// michael add day
	        q = p;
		while ((*p != 'D') && (*p != '\0'))
		    p++;
		if (*p == '\0') {
		    ret = 0;
		    break;
		}
		day = str2int(p, q);
		if (day > 255) {
		    ret = 0;
		    break;
		}
	  //printk(KERN_INFO "libo ...... %s\n", p);

	        q = p;
		while ((*p != 'H') && (*p != '\0'))
		    p++;
		if (*p == '\0') {
		    ret = 0;
		    break;
		}
		hours = str2int(p, q);
		if (hours > 23) {
		    ret = 0;
		    break;
		}
	//printk(KERN_INFO "libo ...... %s\n", p);
		q = p;
		while ((*p != 'M') && (*p != '\0'))
		     p++;
		if (*p == '\0') {
		    ret = 0;
		    break;
		}
		minutes = str2int(p, q);
		if (hours > 59) {
		    ret = 0;
		    break;
		}
	//printk(KERN_INFO "libo ...... %s\n", p);
		time->day = day;
    time->hours = hours;
		time->minutes = minutes;
		ret = 1;
	        break;
	    defalut:
	        ret = 0;
	        break;
	}
	return ret;
}
static int RS5C372a_alarm_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data) {
	
	char temp_cmd[20], *ptr;
	char cmd[20];

	MV_RTC_TIME time;
	MV_U8 enable = 0;

	if(NULL == buffer) 
		return 1;
	
	memset(temp_cmd, 0, sizeof(temp_cmd));
	copy_from_user(temp_cmd, buffer, count);
	temp_cmd[count] = '\0';

	ptr = temp_cmd;
	
	/* chew the "SPACE" before the data string*/
	for(;;) {
		if((*ptr) == ' ') {
			ptr++;
		} else
			break;
	}

	memset(&time, 0, sizeof time);

	if (strcmp(ptr, "Disable") == 0)
	{
	    enable = 0;
	} else {
	    enable = process_time(ptr, &time);
	}
	usiRS5C372AlarmSet(&time, &enable, NULL);    

        return 20;
}
int __init usiRS5C372a_alarm_init(MV_VOID)
{
    RS5C372a_alarm_proc = create_proc_entry("RS5C372a_alarm", 0777, NULL);

    if(NULL == RS5C372a_alarm_proc) {
	return 0;
    }

    strcpy(RS5C372a_alarm_proc->name, "RS5C372a_alarm");
    RS5C372a_alarm_proc->read_proc = RS5C372a_alarm_proc_read;
    RS5C372a_alarm_proc->write_proc = RS5C372a_alarm_proc_write;
    RS5C372a_alarm_proc->nlink = 1;
    return 1;
}

void __exit usiRS5C372a_alarm_exit(void)
{
    remove_proc_entry("RS5C372a_alarm", NULL);
}

module_init(usiRS5C372a_alarm_init);
module_exit(usiRS5C372a_alarm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("libo");
MODULE_DESCRIPTION("RS5C372a Alarm.");
#endif
