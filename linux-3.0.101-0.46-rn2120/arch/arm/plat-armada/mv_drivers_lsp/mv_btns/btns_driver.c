/*******************************************************************************
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
*******************************************************************************/
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include "btns_dev.h"
#include "mvCommon.h"
#include "mvOs.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "btns_driver.h"
#include <linux/kthread.h>

#include <linux/netlink.h>
#define NETLINK_READYNAS       NETLINK_USERSOCK
#define NETLINK_CASTGROUP      1
#define RESET_BTN_ID           1
#define PWR_BTN_ID             2
#define ACTION_PRESS           1
#define ACTION_RELEASE         0
extern struct net init_net;
struct sock *nls;

/* MACROS */
#define GPP_GROUP(gpp) 	gpp/32
#define GPP_ID(gpp)   	gpp%32
#define GPP_BIT(gpp)	0x1 << GPP_ID(gpp)

#define SYS_LED_GROUP  1
#define SYSLEDGPP      MV_GPP6


/* waiting Q */
wait_queue_head_t btns_waitq;

/*
 * common debug for all
 */
#undef DEBUG

#ifdef DEBUG
#define dprintk   printk
#else
#define dprintk(a...)
#endif

/* At GPP initialization, this strucure is filled with what       
 * operation will be monitored for each button (Push and/or          
 * Release) */ 
BTN_OP 	btn_op_cfg[CONFIG_MV_GPP_MAX_PINS] = {BTN_NO_OP};

/* At GPP initialization, this strucure is filled with what       
 * operation will be monitored for each button (Push and/or          
 * Release) */ 
u32 	gpp_default_val_cfg[CONFIG_MV_GPP_MAX_PINS] = {-1};

/* This structures monitors how many time each button was 	  
 * Push/Released since the last time it was sampled */ 
BTN		btns_status[CONFIG_MV_GPP_MAX_PINS];

u32		is_opend = 0;
u32		gpp_changed = 0;

enum BUTTON_KEY {
	RESET       = 0x1,
	BACKUP      = 0x2,
	POWER       = 0x3,
	RESET_HOLD  = 0x11,
	BACKUP_HOLD = 0x12,
	POWER_HOLD  = 0x13,
};

//unsigned long jiffies_released = 0 , jiffies_pressed = 0;
static unsigned long jiffies_released = 0 , jiffies_pressed = 0;
static struct timer_list btns_timer;
static volatile int time_elapsed = 0;

static void key_send(u32 btn_number, int state, int duration)
{
#define MAX_NLMSG_LEN     64
	const u32 group = NETLINK_CASTGROUP;
	size_t size     = NLMSG_SPACE(MAX_NLMSG_LEN);
	struct nlmsghdr *nlh;
	struct sk_buff *skb;

	if (!netlink_has_listeners(nls, group))
		return;

	/* Each allocated skb will be "consumed" in netlink_broadcast()
	   so that it does not need to be freed */
	if (!(skb = alloc_skb(size, GFP_KERNEL))) {
		pr_err("%s: alloc_skb failed.\n", __func__);
		return;
	}
	nlh = NLMSG_PUT(skb, 0, 0, NLMSG_DONE, size - sizeof(*nlh));
	if (snprintf((char *)NLMSG_DATA(nlh), MAX_NLMSG_LEN,
		"Source: GPIO\nButton: %u\nAction: %s\nDuration: %d\n\n",
		btn_number,
		state ? "down" : "up",
		duration) >= MAX_NLMSG_LEN) {
		pr_err("%s Buffer overflowed.", __func__);
		goto nlmsg_failure;
	}
	NETLINK_CB(skb).dst_group = group;
	(void)netlink_broadcast(nls, skb, 0, group, GFP_KERNEL);
nlmsg_failure:
	return;
}

/*
 * Get GPP real value....
 * When Gpp is input:
 * Since reading GPP_DATA_IN_REG return the GPP real value after considering the active polarity
 * active low = 1, active high = 0
 * we will use: val^0 -> val, val^1->not(val)
 * when output don't consider the active polarity
 */
static unsigned int 
mv_gpp_value_real_get(unsigned int gpp_group, unsigned int mask)
{
        unsigned int temp, in_out, gpp_val;
        /* in ->1,  out -> 0 */
        in_out = MV_REG_READ(GPP_DATA_OUT_EN_REG(gpp_group)) & mask;

	gpp_val = MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & mask;

        /* outputs values */
        temp = (gpp_val & ~in_out);

        /* input */
        temp |= (( gpp_val ^ MV_REG_READ(GPP_DATA_IN_POL_REG(gpp_group)) ) & in_out) & mask;

        return temp;
}

static irqreturn_t
mv_btns_handler27(int irq , void *dev_id)
{
	MV_U32 intval,gpp_level,gppVal;
	int gpp = 27;
        intval = MV_REG_READ(GPP_INT_CAUSE_REG(GPP_GROUP(gpp)));
	if ( (intval & (1<<27)) == 0 ) 
		return IRQ_HANDLED;

        gpp_level = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
	BTN_OP btn_op;
	
	/* get gpp real val */
	gppVal = mv_gpp_value_real_get(GPP_GROUP(gpp), GPP_BIT(gpp));

	dprintk("Gpp value was changed: ");
        if (btn_op_cfg[gpp] != BTN_NO_OP) {
                dprintk("gpp %d has changed. now it's %x \n",gpp,
				mv_gpp_value_real_get(GPP_GROUP(gpp), GPP_BIT(gpp)) );
		
		/* Count button Pushes/Releases
		 * mv_gpp_value_real_get == gpp_default_val_cfg[gpp] --> Button push, 
		 * else --> Button release
		 */

		btn_op = (gppVal == gpp_default_val_cfg[gpp]) ? BTN_PUSH : BTN_RELEASE;

		if(btn_op == BTN_RELEASE || btns_status[gpp].btn_push_cntr > 0)
		{
			dprintk("button (of gpp %d) was released \n",gpp);
			key_send(PWR_BTN_ID, ACTION_RELEASE, time_elapsed);
			btns_status[gpp].btn_release_cntr++;
			jiffies_released = jiffies;
	
		} else {
			dprintk("button (of gpp %d) was pressed \n",gpp);
			key_send(PWR_BTN_ID, ACTION_PRESS, 0);
			btns_status[gpp].btn_push_cntr++;
			if (time_elapsed == 0)
			{
				time_elapsed = 1;
				btns_timer.expires = jiffies + 10;
				btns_timer.data = gpp;
				add_timer(&btns_timer);    //add_timer
			}
#if 0
		/*disable sys led blink */
			MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
			/*sys led on */
			MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);

#endif
		jiffies_pressed = jiffies;
			
		}
		
                /* change polarity */
                gpp_level = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
                gpp_level = gpp_level^GPP_BIT(gpp);
		MV_REG_WRITE(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)), gpp_level);

		/* Check if current botton operation should be monitored */
		if(btn_op_cfg[gpp] == btn_op || btn_op_cfg[gpp] == BTN_CHANGE)
		{
			if (btns_status[gpp].btn_release_cntr == btns_status[gpp].btn_push_cntr )
			{
				btns_status[gpp].btn_push_cntr = 0;
				btns_status[gpp].btn_release_cntr = 0;
				if (time_after(jiffies_released,jiffies_pressed+100*3))
				{
					gpp_changed = POWER_HOLD;
					wake_up_interruptible(&btns_waitq);
				}
				else
				{
					gpp_changed = POWER;
					wake_up_interruptible(&btns_waitq);
				}
			}
		}
	}

	return IRQ_HANDLED;
}


static irqreturn_t
mv_btns_handler41(int irq , void *dev_id)
{
	MV_U32 intval,gpp_level,gppVal;
	int gpp = 41;
        intval = MV_REG_READ(GPP_INT_CAUSE_REG(GPP_GROUP(gpp)));
	if ( (intval & (1<<9)) == 0 ) 
		return IRQ_HANDLED;

        gpp_level = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
	BTN_OP btn_op;
	
	/* get gpp real val */
	gppVal = mv_gpp_value_real_get(GPP_GROUP(gpp), GPP_BIT(gpp));

	dprintk("Gpp value was changed: ");
        if (btn_op_cfg[gpp] != BTN_NO_OP) {
                dprintk("gpp %d has changed. now it's %x \n",gpp,
				mv_gpp_value_real_get(GPP_GROUP(gpp), GPP_BIT(gpp)) );
		
		/* Count button Pushes/Releases
		 * mv_gpp_value_real_get == gpp_default_val_cfg[gpp] --> Button push, 
		 * else --> Button release
		 */
		btn_op = (gppVal == gpp_default_val_cfg[gpp]) ? BTN_PUSH : BTN_RELEASE;

		if(btn_op == BTN_RELEASE || btns_status[gpp].btn_push_cntr > 0)
		{
			dprintk("button (of gpp %d) was released \n",gpp);
			key_send(RESET_BTN_ID, ACTION_RELEASE, time_elapsed);
			btns_status[gpp].btn_release_cntr++;
			jiffies_released = jiffies;

		} else {
			dprintk("button (of gpp %d) was pressed \n",gpp);
			key_send(RESET_BTN_ID, ACTION_PRESS, 0);
			btns_status[gpp].btn_push_cntr++;
			if (time_elapsed == 0)
			{
				time_elapsed = 1;
				btns_timer.expires = jiffies + 10;
				btns_timer.data = gpp;
				add_timer(&btns_timer);    //add_timer
			}
#if 0
		/*disable sys led blink */
			MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
			/*sys led on */
			MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);

#endif
		jiffies_pressed = jiffies;
			
		}
		
                /* change polarity */
                gpp_level = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
                gpp_level = gpp_level^GPP_BIT(gpp);
		MV_REG_WRITE(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)), gpp_level);

		/* Check if current botton operation should be monitored */
		if(btn_op_cfg[gpp] == btn_op || btn_op_cfg[gpp] == BTN_CHANGE)
		{
			if (btns_status[gpp].btn_release_cntr == btns_status[gpp].btn_push_cntr )
			{
				btns_status[gpp].btn_push_cntr = 0;
				btns_status[gpp].btn_release_cntr = 0;
				smp_mb();
				if (time_after(jiffies_released,jiffies_pressed+100*3))
				{
					gpp_changed = RESET_HOLD;
					wake_up_interruptible(&btns_waitq);
				}
				else
				{
					gpp_changed = RESET;
					wake_up_interruptible(&btns_waitq);
				}
			}
		}
	}

	return IRQ_HANDLED;
}

#if 0
static int
btnsdev_ioctl(
        struct inode *inode,
        struct file *filp,
        unsigned int cmd,
        unsigned long arg)
{
	unsigned int btn_id;
	BTN btn_sts;
	BTN_PTR user_btn_sts_ptr;
	unsigned int error = 0;
	int i;	

        dprintk("%s()\n", __FUNCTION__);

        switch (cmd) {
        case CIOCWAIT_P:
		/* Haim - Is the condition here correct? */
            	error = wait_event_interruptible(btns_waitq, gpp_changed);
		/* Reset Wait Q condition */
		gpp_changed = 0;

		if(error < 0)
			dprintk("%s(CIOCWAIT_P) - got interrupted\n", __FUNCTION__);
		
		/* Set information for user*/
		btn_sts.btn_id = gpp_changed_id;
		btn_sts.btn_push_cntr   =btns_status[gpp_changed_id].btn_push_cntr;
		btn_sts.btn_release_cntr=btns_status[gpp_changed_id].btn_release_cntr;
		
		dprintk("Button ID %d was pressed %d and released %d\n",gpp_changed_id,
			btns_status[gpp_changed_id].btn_push_cntr,btns_status[gpp_changed_id].btn_release_cntr);

		user_btn_sts_ptr = &(((BTNS_STS_PTR)arg)->btns[0]);
		if ( copy_to_user((void*)user_btn_sts_ptr, &btn_sts,  sizeof(BTN)) ) {
                        dprintk("%s(CIOCWAIT_P) - bad copy\n", __FUNCTION__);
                        error = EFAULT;
                }

		/* Reset changed button operations counters*/
		btns_status[gpp_changed_id].btn_push_cntr = 0;
		btns_status[gpp_changed_id].btn_release_cntr = 0;

                break;
	case CIOCNOWAIT_P:
		/* Eventhough we don't monitor for a button status change, we need to 
 			reset the indication of a change in case it happend */
		gpp_changed = 0;

		dprintk("There are %d buttons to be checked\n", ((BTNS_STS_PTR)arg)->btns_number);

		/* Set information for user*/
		for (i=0; i<((BTNS_STS_PTR)arg)->btns_number; i++)
		{
			btn_id = ((BTNS_STS_PTR)arg)->btns[i].btn_id;

			/* initialize temp strucure which will be copied to user */
			btn_sts.btn_id = btn_id;
			btn_sts.btn_push_cntr = btns_status[btn_id].btn_push_cntr;
			btn_sts.btn_release_cntr = btns_status[btn_id].btn_release_cntr;

			/* Reset button's operations counters*/
			btns_status[btn_id].btn_push_cntr = 0;
			btns_status[btn_id].btn_release_cntr = 0;

			/* Copy temp structure to user */
			user_btn_sts_ptr = &(((BTNS_STS_PTR)arg)->btns[i]);

			if ( copy_to_user((void*)user_btn_sts_ptr, &btn_sts,  sizeof(BTN)) ) {
				dprintk("%s(CIOCNOWAIT_P) - bad copy\n", __FUNCTION__);
				error = EFAULT;
			}
		}
		
		break;
        default:
                dprintk("%s(unknown ioctl 0x%x)\n", __FUNCTION__, cmd);
                error = EINVAL;
                break;
        }
        return(-error);
}
#endif

/*
 * btn_gpp_init
 * initialize on button's GPP and registers its IRQ
 *
 */
static int  
btn_gpp_init(unsigned int gpp, unsigned int default_gpp_val, BTN_OP btn_op, char* btn_name)
{
	unsigned int val;
	if ( gpp == 41 )
	{
	/* Set Polarity bit */
	val = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
	val |= GPP_BIT(gpp);
	MV_REG_WRITE(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)), val);
	}
	if ( gpp == 27 )
	{
	/* Set Polarity bit */
	val = MV_REG_READ(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)));
	val &= ~GPP_BIT(gpp);
	MV_REG_WRITE(GPP_DATA_IN_POL_REG(GPP_GROUP(gpp)), val);
	}
	/* remove mask */
	val = MV_REG_READ(GPP_INT_LVL_REG(GPP_GROUP(gpp)));
	val |= GPP_BIT(gpp);
	MV_REG_WRITE(GPP_INT_LVL_REG(GPP_GROUP(gpp)), val);
	
	/* Set which button operation should be monitored */
	btn_op_cfg[gpp] = btn_op;

	/* Set GPP default value*/
	gpp_default_val_cfg[gpp] = default_gpp_val;
	if ( gpp == 41 )
	{
		/* Register IRQ */
		if( request_irq( IRQ_AURORA_GPIO_40_47, mv_btns_handler41,IRQF_DISABLED, btn_name,NULL) ) 
		{
			printk( KERN_ERR "btnsdev:  can't get irq for button %s (GPP %d)\n",btn_name,gpp );
			return -1;
		}
	}
	if (gpp == 27)
	{
		/* Register IRQ */
		if( request_irq( IRQ_AURORA_GPIO_24_31, mv_btns_handler27,IRQF_DISABLED, btn_name,NULL) ) 
		{
			printk( KERN_ERR "btnsdev:  can't get irq for button %s (GPP %d)\n",btn_name,gpp );
			return -1;
		}
	}

	return 0;
}

static int
btnsdev_open(struct inode *inode, struct file *filp)
{
        dprintk("%s()\n", __FUNCTION__);

	if(!is_opend) {
		is_opend = 1;
		
		/* Reset button operations counters*/
		memset(&btns_status,0,CONFIG_MV_GPP_MAX_PINS);
	}

        return(0);
}

static int
btnsdev_release(struct inode *inode, struct file *filp)
{
        dprintk("%s()\n", __FUNCTION__);
        return(0);
}

static ssize_t btnsdev_read(struct file *filp,char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned int error = 0;
	unsigned int key_ret = 0;
	if (filp->f_flags & O_NONBLOCK)
	{
		return -EAGAIN;
	}
	error = wait_event_interruptible(btns_waitq, gpp_changed);
	key_ret = gpp_changed;
	//printk(KERN_ALERT "----------%d---------- key: 0x%X\n", __LINE__, key_ret);
	if (copy_to_user(buffer, &key_ret, sizeof(unsigned int)))
		return -EFAULT;
	gpp_changed = 0;
	return sizeof(unsigned int);
}

static long btnsdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static struct file_operations btnsdev_fops = {
        .open = btnsdev_open,
        .release = btnsdev_release,
	.read = btnsdev_read,
	.unlocked_ioctl = btnsdev_ioctl,
};

static struct miscdevice btnsdev0 = {
        .minor = BTNSDEV_MINOR,
        .name = "buttons",
        .fops = &btnsdev_fops,
};

static struct miscdevice btnsdev1 = {
        .minor = BTNSDEV_MINOR+1,
        .name = "resetbtns",
        .fops = &btnsdev_fops,
};

static struct miscdevice btnsdev2 = {
        .minor = BTNSDEV_MINOR+2,
        .name = "powerbtns",
        .fops = &btnsdev_fops,
};

static int 
btns_probe(struct platform_device *pdev)
{
        struct btns_platform_data *btns_data = pdev->dev.platform_data;
        int ret, i;
	
	dprintk("%s\n", __FUNCTION__);
	dprintk(KERN_NOTICE "MV Buttons Driver Load\n");

        for (i = 0; i < btns_data->btns_num; i++) {
		ret = btn_gpp_init(btns_data->btns_data_arr[i].gpp_id, btns_data->btns_data_arr[i].default_gpp_val,btns_data->btns_data_arr[i].btn_op, btns_data->btns_data_arr[i].btn_name);

		if (ret != 0) {
			return ret;
		}
        }

        return 0;
}


static struct platform_driver btns_driver = {
	.probe          = btns_probe,
	.driver  = {
		.name		= MV_BTNS_NAME,
	},
};

#if 0
static struct task_struct *button_task;
static int button_thread(void *__unused)
{
	/* khubd needs to be freezable to avoid intefering with USB-PERSIST
	 * port handover.  Otherwise it might see that a full-speed device
	 * was gone before the EHCI controller had handed its port over to
	 * the companion full-speed controller.
	 */
	unsigned int error = 0;
	do {
		error = wait_event_interruptible(btns_waitq, gpp_changed);
		if( gpp_changed == RESET)
		{
			kobject_uevent( &btnsdev1.this_device->kobj,KOBJ_MOVE);
		}
		if( gpp_changed == RESET_HOLD)
		{
			kobject_uevent( &btnsdev1.this_device->kobj,KOBJ_CHANGE);
		}
		if( gpp_changed == POWER)
		{
			kobject_uevent( &btnsdev2.this_device->kobj,KOBJ_MOVE);
		}
		if( gpp_changed == POWER_HOLD)
		{
			kobject_uevent( &btnsdev2.this_device->kobj,KOBJ_CHANGE);
		}

		gpp_changed = 0;
	} while (!kthread_should_stop());

	return 0;
}
#endif

static void button_timer_callback(unsigned long data)
{
	u32 gppVal;
	BTN_OP btn_op;
	unsigned long gpp = data;
	gppVal = mv_gpp_value_real_get(GPP_GROUP(gpp), GPP_BIT(gpp));
	btn_op = (gppVal == gpp_default_val_cfg[gpp]) ? BTN_PUSH : BTN_RELEASE;
	if(btn_op == BTN_PUSH)
	{
		btns_timer.expires = jiffies + 10;
		add_timer(&btns_timer);    //add_timer
		dprintk("timer call:button is pressed,time_elapsed is %d\n",time_elapsed);
	}

	time_elapsed++;	
	//printk(KERN_ALERT " %d ", time_elapsed);
	if ( time_elapsed >= 100 )	//>10s
	{
#if 0
		/*disable sys led blink */
		MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
		/*sys led on */
		MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) & ~SYSLEDGPP);
#endif
	}
	else if ( time_elapsed >= 30 )	//>3s
	{
		if (time_elapsed == 30)
			printk(KERN_ALERT "\nhold button more than 3s\n");
#if 0
		/*enable sys led blink */
		MV_REG_WRITE(GPP_DATA_OUT_REG(SYS_LED_GROUP),MV_REG_READ(GPP_DATA_OUT_REG(SYS_LED_GROUP)) | SYSLEDGPP);
		/*sys led off */
		MV_REG_WRITE(GPP_BLINK_EN_REG(SYS_LED_GROUP),MV_REG_READ(GPP_BLINK_EN_REG(SYS_LED_GROUP)) | SYSLEDGPP);
#endif
	}

	if(btn_op == BTN_RELEASE)
	{
		time_elapsed = 0;
		dprintk("timer call:button is released,time_elapsed is %d\n",time_elapsed);
	}
	dprintk("timer call:time_elapsed is %d\n",time_elapsed);
}



static int __init
btnsdev_init(void)
{
	int rc;

	dprintk("%s\n", __FUNCTION__);

	/* Initialize Wait Q*/
	init_waitqueue_head(&btns_waitq);
	
	/* Register btns device */
	if (misc_register(&btnsdev0)) 
        {
            printk(KERN_ERR "btnsdev: registration of /dev/btnsdev failed\n");
            return -1;
        }
	if (misc_register(&btnsdev1)) 
        {
            printk(KERN_ERR "btnsdev: registration of /dev/btnsdev failed\n");
            return -1;
        }
	if (misc_register(&btnsdev2)) 
        {
            printk(KERN_ERR "btnsdev: registration of /dev/btnsdev failed\n");
            return -1;
        }


	/* Register platform driver*/
	rc = platform_driver_register(&btns_driver);
	if (rc) {
		printk(KERN_ERR "btnsdev: registration of platform driver failed\n");
		return rc;
	}
	//button_task = kthread_run(button_thread, NULL, "kbuttond");

        btns_timer.function = button_timer_callback;
	init_timer(&btns_timer);

	nls = netlink_kernel_create(&init_net, NETLINK_READYNAS, 0, NULL, NULL, THIS_MODULE);

        return 0;
}

static void __exit
btnsdev_exit(void)
{
	dprintk("%s() should never be called.\n", __FUNCTION__);
}

module_init(btnsdev_init);
module_exit(btnsdev_exit);

//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("Ronen Shitrit & Haim Boot");
//MODULE_DESCRIPTION("PH: Buttons press handling.");


