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

static struct proc_dir_entry *usi_ACPOWER_fail_recorder_proc;

static int usi_ACPOWER_fail_recorder_proc_read(char *page, char **start,
						off_t off, int count,
						int *eof, void *data) {
	/* MAX output length */					
	MV_32 len = 20;

	MV_U8 value;
    	if ((off > 0) || (count < len))
    	{
		*eof = 1;
		return 0;
	}

	usiRtcCharRead(RTC_CONTROL_1, &value);
	if (value & BIT5)
	    len = sprintf(page, "flag is set\n");
	else
	    len = sprintf(page, "flag is clean\n");

	*eof = 1;
	return len;
}

static int usi_ACPOWER_fail_recorder_proc_write(struct file *file, 
						const char __user *buffer,
						unsigned long count,
						void *data) {
	
	char temp_cmd[10], *ptr;
	char cmd[10];
	MV_U8 value;

	if(!buffer || (count == 0))
	    return 0;
	if(count > 10)
	    return count;

	memset(temp_cmd, 0, sizeof(temp_cmd)/sizeof(temp_cmd[0]));
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
	/* Using SL2(BIT5) bit of Control register 1 as AC Power fail recorder */
	usiRtcCharRead(RTC_CONTROL_1, &value);
        //printk(KERN_INFO "libo ...... ctrl 1 0x%x\n", value);
        //printk(KERN_INFO "libo ...... %s\n", ptr);

	/* cmd example: echo set > /proc/ACPOWER_fail_recorder */
	/* cmd example: echo clean > /proc/ACPOWER_fail_recorder */
	if (strncmp(ptr, "set", strlen("set")) == 0)
	{
	    value |= BIT5;
	} else if (strncmp(ptr, "clean", strlen("clean")) == 0)
	{
	    value &= ~BIT5;
	} else
	{
	    return count;
	}

        //printk(KERN_INFO "libo ...... ctrl 1 0x%x\n", value);
	usiRtcCharWrite(RTC_CONTROL_1, value);

        return count;
}

#define USI_ACPOWER_FAIL_RECORDER_NAME	"ACPOWER_fail_recorder"

int __init usi_ACPOWER_fail_recorder_init(MV_VOID)
{
    usi_ACPOWER_fail_recorder_proc = create_proc_entry(USI_ACPOWER_FAIL_RECORDER_NAME, 0666, NULL);

    if(NULL == usi_ACPOWER_fail_recorder_proc) {
	return 0;
    }

    strcpy(usi_ACPOWER_fail_recorder_proc->name, USI_ACPOWER_FAIL_RECORDER_NAME);
    usi_ACPOWER_fail_recorder_proc->read_proc = usi_ACPOWER_fail_recorder_proc_read;
    usi_ACPOWER_fail_recorder_proc->write_proc = usi_ACPOWER_fail_recorder_proc_write;
    usi_ACPOWER_fail_recorder_proc->nlink = 1;
    return 1;
}

void __exit usi_ACPOWER_fail_recorder_exit(void)
{
    remove_proc_entry(USI_ACPOWER_FAIL_RECORDER_NAME, NULL);
}

module_init(usi_ACPOWER_fail_recorder_init);
module_exit(usi_ACPOWER_fail_recorder_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("libo");
MODULE_DESCRIPTION("usi ACPOWER fail recorder using RS5C372A one bit.");
