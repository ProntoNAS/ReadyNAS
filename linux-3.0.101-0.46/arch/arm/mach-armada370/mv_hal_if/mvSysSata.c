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

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "sata/CoreDriver/mvSata.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

MV_STATUS mvSysSataWinInit(MV_VOID)
{
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status == MV_OK)
		status = mvSataWinInit(addrWinMap);

	return status;
}


#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include "twsi/mvTwsi.h"
#include "boardEnv/mvBoardEnvSpec.h"
/*
 * This driver is only used for Netgear Rn102,Rn104 and S2000.
 * Another project please modify the board related part.
 */

/* Global parameter */ 
#define HDD_POWERON_PERIOD	5000
#define HDD_POWERON_PERIOD_S	HDD_POWERON_PERIOD/1000
#define MV_INT_SET_EN_REG	0x20A30
#define MV_INT_CLAER_EN_REG	0x20A34
typedef struct {
	MV_U32 prt_group;
	MV_U32 prt_bit;
	MV_U32 pr_on_group;
	MV_U32 pr_on_bit;
	MV_U32 prt_irq;
	MV_U32 position;
}hdd_info_t;

static hdd_info_t	*hdd_info;
static MV_U32		sata_int_id;
static MV_U32		hdd_num;
static hdd_info_t	hdd_info_rn104 [] = {
	{1, BIT16, 0, BIT4, (IRQ_GPP_START + 48), 1},
	{1, BIT17, 0, BIT5, (IRQ_GPP_START + 49), 2},
	{1, BIT18, 0, BIT6, (IRQ_GPP_START + 50), 3},
	{1, BIT19, 0, BIT7, (IRQ_GPP_START + 51), 4},
};

static hdd_info_t	hdd_info_rn102_s2000 [] = {
	{0, BIT12, 0, BIT13, (IRQ_GPP_START + 12), 1},
	{0, BIT10, 0, BIT11, (IRQ_GPP_START + 10), 2},
};

static struct _usiHDDpwrctrl_wl {
    struct list_head list;
    spinlock_t lock;
} usiHDDpwrctrl_wl;

struct _usiHDDpwrctrl_record {
    struct list_head node;
    MV_U32 ID;
};

static struct _power_ing {
    MV_U32 ID;
    MV_U32 power_on_time;
} power_ing;

static struct timer_list usiHDDpwrctrl_timer;

struct _usiHDDpwrctrl_record * rec_ptr;

#include <linux/kthread.h>
extern void pca9554_read_reg(int offset, uint16_t * reg_val);
extern void pca9554_write_reg(int offset, uint16_t reg_val);
wait_queue_head_t pca9554_waitq;
static struct task_struct *pca9554_task;
/* Default status come from Uboot */
static MV_U32 hdd_changed = 0xF0;
static MV_U32 hdd_changed_back = 0xF0;

#define PCA9554_RB_LEN 64
typedef struct {
        MV_U32 off;
        MV_U32 flag;
}hdd_power_info_t;
static hdd_power_info_t pca9554_rb[PCA9554_RB_LEN];
static MV_U32 pca9554_rb_in, pca9554_rb_out;

static int pca9554_thread(void *__unused)
{
	unsigned int error = 0;

        do {
                error = wait_event_interruptible(pca9554_waitq, hdd_changed);
		hdd_changed = 0;
		
		while (pca9554_rb_in != pca9554_rb_out)
		{
			pca9554_write_reg(pca9554_rb[pca9554_rb_out].off + 3, pca9554_rb[pca9554_rb_out].flag);
			pca9554_rb_out++;
			pca9554_rb_out %= PCA9554_RB_LEN;
		}
        } while (!kthread_should_stop());

        return 0;
}

static void power_on_off(MV_U32 HDD_ID, MV_U32 flag)
{

    MV_U32 hdd_id = HDD_ID - 1;//Mapping

    if (flag == 1)//power on
    {
	if (get_board_type() & NETGEAR_BD_TYPE_RN104)
	{
		hdd_changed = hdd_changed_back;
		hdd_changed |= hdd_info[hdd_id].pr_on_bit;
		hdd_changed_back = hdd_changed;
		pca9554_rb[pca9554_rb_in].off = HDD_ID;
		pca9554_rb[pca9554_rb_in++].flag = 1;
		pca9554_rb_in %= PCA9554_RB_LEN;
		wake_up_interruptible(&pca9554_waitq);
	} else {
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(hdd_info[hdd_id].pr_on_group), hdd_info[hdd_id].pr_on_bit);
	}
    } else {
	if (get_board_type() & NETGEAR_BD_TYPE_RN104)
	{
		hdd_changed = hdd_changed_back;
		hdd_changed &= ~(hdd_info[hdd_id].pr_on_bit);
		/* If hdd_changed == 0, pca9554_waitq will not be wake up, we will miss one slot to power off */
		if (hdd_changed == 0)
			hdd_changed |= BIT31;
		hdd_changed_back = hdd_changed;
		pca9554_rb[pca9554_rb_in].off = HDD_ID;
		pca9554_rb[pca9554_rb_in++].flag = 0;
		pca9554_rb_in %= PCA9554_RB_LEN;
		wake_up_interruptible(&pca9554_waitq);
	} else {
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(hdd_info[hdd_id].pr_on_group), hdd_info[hdd_id].pr_on_bit);
	}
    }
}
void usiHDDpwrctrl_timer_func(unsigned long data) {

    struct _usiHDDpwrctrl_record * ptr;
    unsigned long flags;

    printk("<0>""In %s, power_ing.ID = %d.\n", __func__, power_ing.ID);
    memset(&power_ing, 0, sizeof(struct _power_ing));
    spin_lock_irqsave(&usiHDDpwrctrl_wl.lock, flags);
    if (!list_empty(&(usiHDDpwrctrl_wl.list)))
    {
        ptr = list_entry(usiHDDpwrctrl_wl.list.next, struct _usiHDDpwrctrl_record, node);
        power_ing.ID = ptr->ID;
        power_ing.power_on_time = jiffies;
        list_del(usiHDDpwrctrl_wl.list.next);
        //kfree(ptr);
        printk("<0>""In %s, power on ing HDD(%d).\n", __func__, power_ing.ID);
        power_on_off(power_ing.ID, 1);
	
        usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
        add_timer(&usiHDDpwrctrl_timer);
    }

    spin_unlock_irqrestore(&usiHDDpwrctrl_wl.lock, flags);
}

/* Scan each slot at init time, the empty slot will be power off when interrupt is open.
 * Every slots have been power on under Uboot.
 */
static void hdd_detect(void)
{
	MV_U32 tmpVal = 0;
	MV_U32 i = 0;
	
	for (i = 0; i < hdd_num; i++)
	{
		if ((MV_REG_READ(GPP_DATA_IN_REG(hdd_info[i].prt_group)) & hdd_info[i].prt_bit) == 0)
		{
			tmpVal |= (1 << i);
			printk(KERN_INFO "SATA%d is detected.\n", i + 1);
		} else {
			printk(KERN_INFO "SATA%d is not detected.\n", i + 1);
		}
	}
}

static void gpio_init(void)
{
	MV_U32 i = 0;
	/* Init INT LVL */
	for (i = 0; i < hdd_num; i++)
	{
		MV_REG_BIT_SET(GPP_INT_LVL_REG(hdd_info[i].prt_group), hdd_info[i].prt_bit);
	}
}

irqreturn_t HDDpwrctrl_irq_handle(int irq, void *dev_id)
{
    unsigned long irq_flags;
    MV_U32 wait_flag = 0;
    MV_U32 tmpVal;
    struct _usiHDDpwrctrl_record * ptr;
    MV_U32 HDD_ID = 0;
    MV_U32 hdd_id, i;

    local_irq_save(irq_flags);

    for (i = 0; i < hdd_num; i++)
    {
	if (irq == hdd_info[i].prt_irq)
	{
		HDD_ID = hdd_info[i].position;
		break;
	}
    }
    if (i == hdd_num)
    {
	local_irq_restore(irq_flags);
	return IRQ_NONE;
    }

    hdd_id = HDD_ID - 1;//Mapping
    /* Disable SATA present interrupt */
    MV_REG_WRITE(MV_INT_CLAER_EN_REG, sata_int_id);
    
    tmpVal = MV_REG_READ(GPP_DATA_IN_REG(hdd_info[hdd_id].prt_group));
    if (MV_REG_READ(GPP_DATA_IN_POL_REG(hdd_info[hdd_id].prt_group)) & hdd_info[hdd_id].prt_bit)
       tmpVal = ~tmpVal;

    if (tmpVal & hdd_info[hdd_id].prt_bit) {
//poweroff
        list_for_each_entry(ptr, &usiHDDpwrctrl_wl.list, node)
        {
            if (ptr->ID == HDD_ID)
            {
                list_del(&(ptr->node));
                printk(KERN_INFO "Delete HDD(%d) from power on wait list.\n", HDD_ID);
                //break;
            }
	    
        }

        if (power_ing.ID == HDD_ID) {
            del_timer(&usiHDDpwrctrl_timer);
            memset(&power_ing, 0, sizeof(struct _power_ing));
            //poweroff it.
	    power_on_off(HDD_ID, 0);
            printk("<0>""Power off HDD(%d) after power on it less than %d s", HDD_ID, HDD_POWERON_PERIOD_S);

            spin_lock(&usiHDDpwrctrl_wl.lock);
            //find a HDD form wait list and power on it.
            if (!list_empty(&(usiHDDpwrctrl_wl.list)))
            {
                ptr = list_entry(usiHDDpwrctrl_wl.list.next, struct _usiHDDpwrctrl_record, node);
                power_ing.ID = ptr->ID;
                power_ing.power_on_time = jiffies;
                list_del(usiHDDpwrctrl_wl.list.next);
                //kfree(ptr);
                printk(KERN_INFO "Power on HDD(%d) from power on wait list.\n", power_ing.ID);
                power_on_off(power_ing.ID, 1);
                usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
                add_timer(&usiHDDpwrctrl_timer);
            }
    
            spin_unlock(&usiHDDpwrctrl_wl.lock);

        } else {
            //poweroff
	    power_on_off(HDD_ID, 0);
            printk(KERN_INFO "Just power off HDD(%d).\n", HDD_ID);
        }

    } else {
    //Power on.
        if(power_ing.ID == 0) {
            usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
            add_timer(&usiHDDpwrctrl_timer);
            //poweron
            printk(KERN_INFO "Just power on HDD(%d).\n", HDD_ID);
	    power_on_off(HDD_ID, 1);
            power_ing.ID =HDD_ID;
            power_ing.power_on_time = jiffies;
        } else
        {
	    /* If already wait on the list? */
            spin_lock(&usiHDDpwrctrl_wl.lock);
	    list_for_each_entry(ptr, &usiHDDpwrctrl_wl.list, node)
	    {
		if (ptr->ID == HDD_ID)
                {
                    wait_flag = 1;
                }
	    }
            spin_unlock(&usiHDDpwrctrl_wl.lock);

	    if (wait_flag || power_ing.ID == HDD_ID)
	    {
		printk(KERN_INFO "HDD(%d) already in poweron wait list or on powering.\n", HDD_ID);
		goto exit;
	    }
            spin_lock(&usiHDDpwrctrl_wl.lock);
            printk(KERN_INFO "Add HDD(%d) to the wait list.\n", HDD_ID);
            list_add_tail(&(rec_ptr[HDD_ID -1].node), &(usiHDDpwrctrl_wl.list));
            spin_unlock(&usiHDDpwrctrl_wl.lock);
        }
    }

    tmpVal = MV_REG_READ(GPP_DATA_IN_POL_REG(hdd_info[hdd_id].prt_group));
    if (tmpVal & hdd_info[hdd_id].prt_bit)
    {
        MV_REG_WRITE(GPP_DATA_IN_POL_REG(hdd_info[hdd_id].prt_group), (tmpVal & (~(hdd_info[hdd_id].prt_bit))));
    } else
    {
        MV_REG_WRITE(GPP_DATA_IN_POL_REG(hdd_info[hdd_id].prt_group), tmpVal | hdd_info[hdd_id].prt_bit);
    }
exit:
    /* eliminate jitter */
    mdelay(500);

    /* Enable SATA present interrupt */
    MV_REG_WRITE(MV_INT_SET_EN_REG, sata_int_id);
    local_irq_restore(irq_flags);
    return IRQ_HANDLED;
}

static int __init HDD_pwrctl_init(void)
{
    int result;
    int i = 0;
    printk(KERN_INFO "%s\n", __func__);

    if (get_board_type() & NETGEAR_BD_TYPE_RN104)
    {
	hdd_info = hdd_info_rn104;
	sata_int_id = IRQ_AURORA_GPIO_48_55;
	hdd_num = 4;
	init_waitqueue_head(&pca9554_waitq);
	pca9554_task = kthread_run(pca9554_thread, NULL, "kpca9554d");
    } else {
	hdd_info = hdd_info_rn102_s2000;
	sata_int_id = IRQ_AURORA_GPIO_8_15;
	hdd_num = 2;
    }

    hdd_detect();

    /* Disable SATA present interrupt */
    MV_REG_WRITE(MV_INT_CLAER_EN_REG, sata_int_id);
    gpio_init();

    spin_lock_init(&usiHDDpwrctrl_wl.lock);
    INIT_LIST_HEAD(&usiHDDpwrctrl_wl.list);
	
    init_timer(&usiHDDpwrctrl_timer);
    usiHDDpwrctrl_timer.expires = jiffies + 1*HZ;
    usiHDDpwrctrl_timer.data = 0;
    usiHDDpwrctrl_timer.function = usiHDDpwrctrl_timer_func;
	
    rec_ptr = kmalloc(sizeof(struct _usiHDDpwrctrl_record) * hdd_num, GFP_ATOMIC);
    if (!rec_ptr)
    {
        printk("<0>""kmalloc failed!\n");
        goto earlyfail;
    }

    /* Request irq */
    (rec_ptr + 0)->ID = 1;
    result = request_irq(hdd_info[0].prt_irq, HDDpwrctrl_irq_handle, (IRQF_DISABLED), "HDDpwrctrl-1", NULL);
    if (result)
    {
	printk("<0>""Request the irq : HDDpwrctrl(HDD 1) failed!\n");
	i = 1;
	goto fail;
    }

    (rec_ptr + 1)->ID = 2;
    result = request_irq(hdd_info[1].prt_irq, HDDpwrctrl_irq_handle, (IRQF_DISABLED), "HDDpwrctrl-2", NULL);
    if (result)
    {
	printk("<0>""Request the irq : HDDpwrctrl(HDD 2) failed!\n");
	i = 2;
	goto fail;
    }
    if (hdd_num > 2)
    {
	(rec_ptr + 2)->ID = 3;
	result = request_irq(hdd_info[2].prt_irq, HDDpwrctrl_irq_handle, (IRQF_DISABLED), "HDDpwrctrl-3", NULL);
	if (result)
	{
	    printk("<0>""Request the irq : HDDpwrctrl(HDD 3) failed!\n");
	    i = 3;
	    goto fail;
        }

	(rec_ptr + 3)->ID = 4;
        result = request_irq(hdd_info[3].prt_irq, HDDpwrctrl_irq_handle, (IRQF_DISABLED), "HDDpwrctrl-4", NULL);
        if (result)
        {
	    printk("<0>""Request the irq : HDDpwrctrl(HDD 4) failed!\n");
	    i = 4;
	    goto fail;
        }
    }
fail:
    if (i)
    {
	i = i - 1;
	while(i)
	    free_irq(hdd_info[--i].prt_irq, NULL);
		
            kfree(rec_ptr);
    } else
    {
        printk(KERN_INFO "Request the irq HDDpwrctrl success.\n");
    }
earlyfail:
    /* Enable SATA present interrupt */
    MV_REG_WRITE(MV_INT_SET_EN_REG, sata_int_id);
    return 0;
}

static void __exit usi_pwrctl_exit(void)
{
    MV_U32 i;
    for(i = 0; i < hdd_num; i++)
        free_irq(hdd_info[i].prt_irq, NULL);
	
    kfree(rec_ptr);
}
MODULE_AUTHOR("Libo <bo_li@usish.com>");
MODULE_DESCRIPTION("HDD power control driver");
MODULE_LICENSE("GPL");
/* NOTICE: HDD_pwrctl_init depends on pca953x init complete at Rn104 project.
 * So the HDD_pwrctl_init must be declared as late_initcall.
 */
late_initcall(HDD_pwrctl_init);
