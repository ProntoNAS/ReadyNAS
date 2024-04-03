/*
    usiHDDpwrctl.c - Used for HDD(s) power on/off control of Netgear
            DUOV3 and NV+V4 projects.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "gpp/mvGpp.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/irq.h>

/*
 * This driver only handles the HDD(s) power on/off control of Netgear
 * DUOV3 and NV+V4 projects. If used for another project please
 * modify the gpio.
 */
 
#define HDD_POWERON_PERIOD 5000
#define HDD_POWERON_PERIOD_S HDD_POWERON_PERIOD/1000


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

#ifdef CONFIG_USI_BOARD_DUO_V3
#define HDD_NUM 2
#else
#define HDD_NUM 4
#endif

static unsigned int HDD_irq_tbl[HDD_NUM] = {
    IRQ_GPP_25,
    IRQ_GPP_27,
#if (HDD_NUM > 2)
    IRQ_GPP_39,
    IRQ_GPP_41,
#endif
};

void poweron(MV_U32 ID) {
    MV_U32 tmpVal;
    unsigned int gpp_group = 0;
    unsigned int gpp_in = 0;
    unsigned int gpp_out = 0;

    switch (ID) {
    case 1:
        gpp_group = 0;
        gpp_in = BIT25;
        gpp_out = BIT26;
        break;
    case 2:
        gpp_group = 0;
        gpp_in = BIT27;
        gpp_out = BIT28;
        break;
#if (HDD_NUM > 2)
    case 3:
        gpp_group = 1;
        gpp_in = BIT7;
        gpp_out = BIT8;
        break;
    case 4:
        gpp_group = 1;
        gpp_in = BIT9;
        gpp_out = BIT10;
        break;
#endif
    default:
        return;
    }
	
    tmpVal = MV_REG_READ(GPP_DATA_IN_REG(gpp_group));
    if (MV_REG_READ(GPP_DATA_IN_POL_REG(gpp_group)) & gpp_in)
        tmpVal = ~tmpVal;
    if ((tmpVal & gpp_in) == 0) {
        //poweron
        MV_REG_WRITE(GPP_DATA_OUT_REG(gpp_group), MV_REG_READ(GPP_DATA_OUT_REG(gpp_group)) | gpp_out);
        printk(KERN_INFO "In %s, poweron HDD(%d).\n", __func__, ID);
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
        poweron(power_ing.ID);
        usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
        add_timer(&usiHDDpwrctrl_timer);
    }

    spin_unlock_irqrestore(&usiHDDpwrctrl_wl.lock, flags);
}

/* sequence HDD(s) power on/off */
static void pretestHDD(void)
{
    MV_U32 tmpVal;
    MV_U32 hdd_poweron_started = 0;
    /* MPPx Low means power off */
    /* MPPx High means power on */
	
    if ((MV_REG_READ(GPP_DATA_IN_REG(0)) & BIT25) == 0)
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(0));
	printk(KERN_INFO "MPP26 High.\n");
    } else
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(0));
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), tmpVal & (~BIT26));
	printk(KERN_INFO "MPP26 Low.\n");
    }
    if ((MV_REG_READ(GPP_DATA_IN_REG(0)) & BIT27) == 0)
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(0));
	printk(KERN_INFO "MPP28 High.\n");
    } else
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(0));
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), tmpVal & (~BIT28));
	printk(KERN_INFO "MPP28 Low.\n");
    }
#if (HDD_NUM > 2)
    /* MPP39 MPP40 */
    if ((MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT7) == 0)
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(1));
	printk(KERN_INFO "MPP40 High.\n");
    } else
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(1));
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), tmpVal & (~BIT8));
	printk(KERN_INFO "MPP40 Low.\n");
    }
    /* MPP41 MPP42*/
    if ((MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT9) == 0)
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(1));
	printk(KERN_INFO "MPP42 High.\n");
    } else
    {
        tmpVal = MV_REG_READ(GPP_DATA_OUT_REG(1));
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), tmpVal & (~BIT10));
	printk(KERN_INFO "MPP42 Low.\n");
    }
#endif
}

static void usi_gpio_init(void)
{
    MV_U32 tmpVal;

    tmpVal = MV_REG_READ(GPP_INT_LVL_REG(0));
    MV_REG_WRITE(GPP_INT_LVL_REG(0), tmpVal | BIT25 | BIT27);
#if (HDD_NUM > 2)
    /* MPP39 MPP41*/
    tmpVal = MV_REG_READ(GPP_INT_LVL_REG(1));
    MV_REG_WRITE(GPP_INT_LVL_REG(1), tmpVal | BIT7 | BIT9);
#endif
}

irqreturn_t HDDpwrctrl_irq_handle(int irq, void *dev_id)
{
    unsigned long irq_flags;
    unsigned long wait_flag = 0;
    MV_U32 tmpVal;
    struct _usiHDDpwrctrl_record * ptr;

    unsigned int main_irq_mask = 0;
    unsigned int gpp_group = 0;
    unsigned int gpp_in = 0;
    unsigned int gpp_out = 0;
    unsigned int HDD_ID = 0;

    local_irq_save(irq_flags);

    switch (irq) {
    case IRQ_GPP_25:
        main_irq_mask = BIT6;
        gpp_group = 0;
        gpp_in = BIT25;
        gpp_out = BIT26;
        HDD_ID = 1;
        break;
    case IRQ_GPP_27:
        main_irq_mask = BIT6;
        gpp_group = 0;
        gpp_in = BIT27;
        gpp_out = BIT28;
        HDD_ID = 2;
        break;
#if (HDD_NUM > 2)
    case IRQ_GPP_39:
        main_irq_mask = BIT7;
        gpp_group = 1;
        gpp_in = BIT7;
        gpp_out = BIT8;
        HDD_ID = 3;
        break;
    case IRQ_GPP_41:
        main_irq_mask = BIT8;
        gpp_group = 1;
        gpp_in = BIT9;
        gpp_out = BIT10;
        HDD_ID = 4;
        break;
#endif
    default:
        local_irq_restore(irq_flags);
        return IRQ_NONE;
    }
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & (~main_irq_mask)));
    
    tmpVal = MV_REG_READ(GPP_DATA_IN_REG(gpp_group));
    if (MV_REG_READ(GPP_DATA_IN_POL_REG(gpp_group)) & gpp_in)
       tmpVal = ~tmpVal;

    if (tmpVal & gpp_in) {
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
            MV_REG_WRITE(GPP_DATA_OUT_REG(gpp_group), MV_REG_READ(GPP_DATA_OUT_REG(gpp_group)) & (~gpp_out));
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
                poweron(power_ing.ID);
                usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
                add_timer(&usiHDDpwrctrl_timer);
            }
    
            spin_unlock(&usiHDDpwrctrl_wl.lock);

        }else
        {
            //poweroff
            MV_REG_WRITE(GPP_DATA_OUT_REG(gpp_group), MV_REG_READ(GPP_DATA_OUT_REG(gpp_group)) & (~gpp_out));
            printk(KERN_INFO "Just power off HDD(%d).\n", HDD_ID);
        }

    } else {
    //Power on.
        if(power_ing.ID == 0) {
            usiHDDpwrctrl_timer.expires = jiffies + HDD_POWERON_PERIOD_S*HZ;
            add_timer(&usiHDDpwrctrl_timer);
            //poweron
            printk(KERN_INFO "Just power on HDD(%d).\n", HDD_ID);
            MV_REG_WRITE(GPP_DATA_OUT_REG(gpp_group), MV_REG_READ(GPP_DATA_OUT_REG(gpp_group)) | gpp_out);
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
		printk(KERN_INFO "Already in poweron wait list or on powering.\n", HDD_ID);
		goto exit;
	    }
            spin_lock(&usiHDDpwrctrl_wl.lock);
            printk(KERN_INFO "Add HDD(%d) to the wait list.\n", HDD_ID);
            list_add_tail(&(rec_ptr[HDD_ID -1].node), &(usiHDDpwrctrl_wl.list));
            spin_unlock(&usiHDDpwrctrl_wl.lock);
        }
    }

    tmpVal = MV_REG_READ(GPP_DATA_IN_POL_REG(gpp_group));
    if (tmpVal & gpp_in)
    {
        MV_REG_WRITE(GPP_DATA_IN_POL_REG(gpp_group), (tmpVal & ~gpp_in));
    } else
    {
        MV_REG_WRITE(GPP_DATA_IN_POL_REG(gpp_group), tmpVal | gpp_in);
    }
exit:
    /* eliminate jitter */
    mdelay(500);

    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | main_irq_mask ));
    local_irq_restore(irq_flags);
    return IRQ_HANDLED;
}

static int __init HDD_pwrctl_init(void)
{
    int result;
    int i;
    int errflag = 0;

    printk(KERN_INFO "%s\n", __func__);
    pretestHDD();
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & ~(BIT6 | BIT7 | BIT8)));

    usi_gpio_init();

    spin_lock_init(&usiHDDpwrctrl_wl.lock);
    INIT_LIST_HEAD(&usiHDDpwrctrl_wl.list);
	
    init_timer(&usiHDDpwrctrl_timer);
    usiHDDpwrctrl_timer.expires = jiffies + 1*HZ;
    usiHDDpwrctrl_timer.data = 0;
    usiHDDpwrctrl_timer.function = usiHDDpwrctrl_timer_func;
	
    rec_ptr = kmalloc(sizeof(struct _usiHDDpwrctrl_record) * HDD_NUM, GFP_ATOMIC);
    if (!rec_ptr)
    {
        printk("<0>""kmalloc failed!\n");
        goto earlyfail;
    }
    for(i = 0; i < HDD_NUM; i++)
    {
        (rec_ptr +i)->ID = i + 1;

        result = request_irq(HDD_irq_tbl[i], HDDpwrctrl_irq_handle, (IRQF_DISABLED), "HDDpwrctrl", NULL);
        if (result)
        {
            printk("<0>""Request the irq : HDDpwrctrl(HDD %d) failed!\n", i + 1);
            errflag = 1;
        }    
    }
    if (errflag)
    {
        for(i = 0; i < HDD_NUM; i++)
            free_irq(HDD_irq_tbl[i], NULL);
		
            kfree(rec_ptr);
    } else
    {
        printk(KERN_INFO "Request the irq HDDpwrctrl success.\n");
    }
earlyfail:
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT6 | BIT7 | BIT8));
    return 0;
}

static void __exit usi_pwrctl_exit(void)
{
    int i;
    for(i = 0; i < HDD_NUM; i++)
        free_irq(HDD_irq_tbl[i], NULL);
	
    kfree(rec_ptr);
}

MODULE_AUTHOR("Libo <bo_li@usish.com>");
MODULE_DESCRIPTION("HDD power control driver");
MODULE_LICENSE("GPL");

__initcall(HDD_pwrctl_init);
