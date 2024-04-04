#include <linux/module.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "mvCommon.h"
#include "mvOs.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include <linux/delay.h>
#include "boardEnv/mvBoardEnvSpec.h"

#include <linux/netlink.h>
#include <linux/sysdev.h>

#define NETLINK_READYNAS	NETLINK_USERSOCK
#define NETLINK_CASTGROUP	1
extern struct net init_net;
struct sock *nls;
static int backup_key = 0;
static int reset_key = 0;

#define BUF_CLEAR _IO(0xFF, 0)

#define MAX_KEY_BUF 16
#define KEYSTATUS_DOWNX 2
#define KEYSTATUS_DOWN 1
#define KEYSTATUS_UP 0

#define BUTTON_MAJOR 0
unsigned int usi_button_major = BUTTON_MAJOR;
#define DEVICE_NAME "buttons"

#define KEY_TIME_DELAY (HZ/10)      // 100ms
#define KEY_TIME_DELAY1 (HZ/100)    // 10ms

#define BUF_HEAD (usi_button_dev.buf[usi_button_dev.head])
#define BUF_TAIL (usi_button_dev.buf[usi_button_dev.tail])

#define INCBUF(x, mod) (((x) + 1) & ((mod) - 1))

#define MV_INT_SET_EN_REG	0x20A30
#define MV_INT_CLAER_EN_REG	0x20A34

enum BUTTON_KEY {
	RESET,
	BACKUP,
	POWER,
	BTN_NUM,
};

typedef struct {
        MV_U32 group;
        MV_U32 bit;
        MV_U32 irq;
        MV_U32 int_id;
	MV_U32 key_code;
}btn_info_t;

#define BACKUP_BTN_ID		0
#define BACKUP_BTN_IRQ_RN104	(IRQ_GPP_START + 52)
#define BACKUP_BTN_IRQ_RN102	(IRQ_GPP_START + 58)
#define BACKUP_BTN_IRQ_S2000	BACKUP_BTN_IRQ_RN102
#define RESET_BTN_ID		1
#define RESET_BTN_IRQ_RN104	(IRQ_GPP_START + 65)
#define RESET_BTN_IRQ_RN102	(IRQ_GPP_START + 6)
#define RESET_BTN_IRQ_S2000	RESET_BTN_IRQ_RN102
#define PWR_BTN_ID		2
#define PWR_BTN_IRQ_RN104	(IRQ_GPP_START + 62)
#define PWR_BTN_IRQ_RN102	PWR_BTN_IRQ_RN104
#define PWR_BTN_IRQ_S2000	PWR_BTN_IRQ_RN102

static btn_info_t	*btn_info;
static btn_info_t	btn_info_rn102 [] = {
        {0, BIT6,  RESET_BTN_IRQ_RN102,  IRQ_AURORA_GPIO_0_7,   RESET_BTN_ID},
        {1, BIT26, BACKUP_BTN_IRQ_RN102, IRQ_AURORA_GPIO_56_63, BACKUP_BTN_ID},
        {1, BIT30, PWR_BTN_IRQ_RN102,    IRQ_AURORA_GPIO_56_63, PWR_BTN_ID},
};

static btn_info_t	btn_info_rn104 [] = {
        {2, BIT1,  RESET_BTN_IRQ_RN104,  IRQ_AURORA_GPIO_64_66, RESET_BTN_ID},
        {1, BIT20, BACKUP_BTN_IRQ_RN104, IRQ_AURORA_GPIO_48_55, BACKUP_BTN_ID},
        {1, BIT30, PWR_BTN_IRQ_RN104,    IRQ_AURORA_GPIO_56_63, PWR_BTN_ID},
};

static btn_info_t	btn_info_s2000 [] = {
        {0, BIT6,  RESET_BTN_IRQ_S2000,  IRQ_AURORA_GPIO_0_7,   RESET_BTN_ID},
        {1, BIT26, BACKUP_BTN_IRQ_S2000, IRQ_AURORA_GPIO_56_63, BACKUP_BTN_ID},
        {1, BIT30, PWR_BTN_IRQ_S2000,    IRQ_AURORA_GPIO_56_63, PWR_BTN_ID},
};

struct class *usi_button_class;

struct Button_Dev
{
    struct cdev cdev;
    unsigned int key_status[BTN_NUM];
    unsigned int key_timer_count[BTN_NUM];
    unsigned int buf[MAX_KEY_BUF];
    unsigned int head,tail;
    wait_queue_head_t wq;
}usi_button_dev,*usi_button_devp = NULL;

struct timer_list key_timer[BTN_NUM];

static struct sysdev_class rn_button_sysclass = {
	.name = "rn_button",
};

static struct sys_device rn_button_0 = {
	.id	= 0,
	.cls	= &rn_button_sysclass,
};

static ssize_t rn_show_backup_btn(struct sys_device *dev,
			struct sysdev_attribute *attr, char *buf)
{

	int val = (backup_key == 0)?0:1;
	
	return sprintf(buf, "%d\n", val);
}

static SYSDEV_ATTR(backup_button, S_IRUGO, rn_show_backup_btn, NULL);


static void btn_int_clear(void)
{
	MV_REG_WRITE(MV_INT_CLAER_EN_REG, btn_info[RESET].int_id);
	MV_REG_WRITE(MV_INT_CLAER_EN_REG, btn_info[BACKUP].int_id);
	MV_REG_WRITE(MV_INT_CLAER_EN_REG, btn_info[POWER].int_id);
}

static void btn_int_set(void)
{
	MV_REG_WRITE(MV_INT_SET_EN_REG, btn_info[RESET].int_id);
	MV_REG_WRITE(MV_INT_SET_EN_REG, btn_info[BACKUP].int_id);
	MV_REG_WRITE(MV_INT_SET_EN_REG, btn_info[POWER].int_id);
}

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

static void keyEvent(unsigned key)
{
    int duration;
    if (key & 0x10)
        BUF_HEAD = btn_info[(key & 0xF)].key_code | 0x10;//press and hold on
    else if (key & 0x20)
        BUF_HEAD = btn_info[(key & 0xF)].key_code | 0x20;//pressed
    else
        BUF_HEAD = btn_info[key].key_code;

    usi_button_dev.head = INCBUF(usi_button_dev.head, MAX_KEY_BUF);

    if (key & 0x10)
        duration = 60;
    else if (key & 0x20)
        duration = 0;
    else
        duration = 10;
    key_send(btn_info[(key & 0xF)].key_code, !duration, duration);

    //check if the queue is empty
    if (waitqueue_active(&(usi_button_dev.wq)))
        wake_up_interruptible(&(usi_button_dev.wq));
}

static void free_irqs(void)
{
    int i;
    for (i = 0; i < BTN_NUM; i++)
    {
        free_irq(btn_info[i].irq, (void *)i);
    }
}

static irqreturn_t usi_button_irq(int irq, void *dev_id)
{
    int key = (int)dev_id;
    int i;
    int found = 0;
    unsigned long irq_flags;

    local_irq_save(irq_flags);

    for (i = 0; i < BTN_NUM; i++)
    {
        if (btn_info[i].irq == irq) 
	{
            found = 1;
            break;
        }
    }
    if (!found)
    {
        printk(KERN_NOTICE"bad irq %d in button\n", irq);
        return IRQ_NONE;
    }

    btn_int_clear();

    usi_button_dev.key_status[key] = KEYSTATUS_DOWNX;
    //key_timer[key].expires = jiffies + KEY_TIME_DELAY1;
    mod_timer(&key_timer[key], jiffies + KEY_TIME_DELAY1);

    local_irq_restore(irq_flags);
    return IRQ_HANDLED;
}

static int request_irqs(void)
{
    int ret = 0;
    if (request_irq(btn_info[RESET].irq, usi_button_irq, IRQF_DISABLED, "reset_btn", (void *)RESET))
    {
	printk(KERN_WARNING "buttons:can't get irq no.%d\n", btn_info[RESET].irq);
	return ret | BIT0;
    }
    if (request_irq(btn_info[BACKUP].irq, usi_button_irq, IRQF_DISABLED, "backup_btn", (void *)BACKUP))
    {
	printk(KERN_WARNING "buttons:can't get irq no.%d\n", btn_info[BACKUP].irq);
	return ret | BIT1;
    }
    if (request_irq(btn_info[POWER].irq, usi_button_irq, IRQF_DISABLED, "power_btn", (void *)POWER))
    {
	printk(KERN_WARNING "buttons:can't get irq no.%d\n", btn_info[POWER].irq);
	return ret | BIT2;
    }
    return ret;
}

static void usi_button_timer_callback(unsigned long data)
{
    int key = data;
    int isdown = 0;
    switch (key) {
        case RESET:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(btn_info[RESET].group)) & btn_info[RESET].bit;
		reset_key = isdown;
	    break;
	case BACKUP:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(btn_info[BACKUP].group)) & btn_info[BACKUP].bit;
		backup_key = isdown;
	    break;
	case POWER:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(btn_info[POWER].group)) & btn_info[POWER].bit;
	    break;
	default:
	    break;
    }
    if (isdown)
    {
        if (usi_button_dev.key_status[key] == KEYSTATUS_DOWNX)
        {
            keyEvent(key | 0x20); /* Initial button press */
            usi_button_dev.key_status[key] = KEYSTATUS_DOWN;
            //key_timer[key].expires = jiffies + KEY_TIME_DELAY;
	    
	    usi_button_dev.key_timer_count[key] += 1;
            mod_timer(&key_timer[key], jiffies + KEY_TIME_DELAY);
        }else
        {
	    if (usi_button_dev.key_timer_count[key] == 30)
	    {
	        /* press and hold on more than 3s. key | 0x10 */
		keyEvent(key | 0x10);
		switch (key) {
		    case RESET:
			MV_REG_BIT_RESET(GPP_INT_LVL_REG(btn_info[RESET].group), btn_info[RESET].bit);
			break;
		    case BACKUP:
			MV_REG_BIT_RESET(GPP_INT_LVL_REG(btn_info[BACKUP].group), btn_info[BACKUP].bit);
			break;
		    case POWER:
			MV_REG_BIT_RESET(GPP_INT_LVL_REG(btn_info[POWER].group), btn_info[POWER].bit);
			break;
		    default:
			break;
		}
		btn_int_set();
	    }
            //key_timer[key].expires = jiffies + KEY_TIME_DELAY;
	    usi_button_dev.key_timer_count[key] += 1;
            mod_timer(&key_timer[key], jiffies + KEY_TIME_DELAY);
        }
    }else
    {
    	if (usi_button_dev.key_timer_count[key] > 30)
	{
	    /* press and hold on more than 3s. key | 0x10 */
	    //keyEvent(key | 0x10);
    	    switch (key) {
                case RESET:
		    MV_REG_BIT_SET(GPP_INT_LVL_REG(btn_info[RESET].group), btn_info[RESET].bit);
	    	    break;
	        case BACKUP:
		    MV_REG_BIT_SET(GPP_INT_LVL_REG(btn_info[BACKUP].group), btn_info[BACKUP].bit);
	    	    break;
	        case POWER:
		    MV_REG_BIT_SET(GPP_INT_LVL_REG(btn_info[POWER].group), btn_info[POWER].bit);
	    	    break;
	        default:
	            break;
	    }
	}
	else if (usi_button_dev.key_timer_count[key] >= 0)
	{
	    keyEvent(key);
	    btn_int_set();
	}

        usi_button_dev.key_status[key] = KEYSTATUS_UP;
	usi_button_dev.key_timer_count[key] = 0;
    }
}

//static int usi_button_ioctl(struct inode *nodep, struct file *filp, unsigned
static long usi_button_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    unsigned long flags;
    
    switch (cmd)
    {
	case BUF_CLEAR:
	    local_irq_save(flags);
	    usi_button_dev.head = usi_button_dev.tail = 0;
	    local_irq_restore(flags);
	    break;
	default:
            return -EINVAL;
    }
    return 0;
}

static ssize_t usi_button_read(struct file *filp,char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned int key_ret = 0,press = 0;
    unsigned long flags;
    //FIXME
    while(1)
    {
    if (usi_button_dev.head != usi_button_dev.tail)
    {
        local_irq_save(flags);
        key_ret = BUF_TAIL;
        usi_button_dev.tail = INCBUF(usi_button_dev.tail, MAX_KEY_BUF);
        local_irq_restore(flags);
        press = 1;
    }
    else
    {
        if (filp->f_flags & O_NONBLOCK)
        {
            return -EAGAIN;
        }
	//FIXME
        interruptible_sleep_on(&(usi_button_dev.wq));
        //interruptible_sleep_on_timeout(&(usi_button_dev.wq),5);
        if (signal_pending(current))
        {
            return -ERESTARTSYS;
        }
    }
    if (press)
        break;
    }
    if (copy_to_user(buffer, &key_ret, sizeof(unsigned int)))
	return -EFAULT;
    
    return sizeof(unsigned int);
}


static int usi_button_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int usi_button_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations usi_button_fops =
{
    .owner = THIS_MODULE,
    .read  = usi_button_read,
    .unlocked_ioctl = usi_button_ioctl,
    //.ioctl = usi_button_ioctl,
    .open  = usi_button_open,
    .release = usi_button_release,
};

static int usi_button_setup_cdev(void)
{
    int err = 0;
    dev_t devno;
    devno = MKDEV(usi_button_major, 0);
    cdev_init(&usi_button_dev.cdev,&usi_button_fops);
    usi_button_dev.cdev.owner = THIS_MODULE;
    usi_button_dev.cdev.ops = &usi_button_fops;
    err = cdev_add(&usi_button_dev.cdev, devno, 1);
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding button", err);
	return err;
    }
    return err;
}

static int __init usi_button_init(void)
{
    int result, i;
    dev_t devno;

    if (get_board_type() & NETGEAR_BD_TYPE_RN102)
	btn_info = btn_info_rn102;
    else if (get_board_type() & NETGEAR_BD_TYPE_RN104)
	btn_info = btn_info_rn104;
    else if (get_board_type() & NETGEAR_BD_TYPE_S2000)
	btn_info = btn_info_s2000;

    btn_int_clear();	

    result = request_irqs();
    if (result) {
        goto fail_request_irqs;
    }

    devno = MKDEV(usi_button_major, 0);
    /* request device number */
    if (usi_button_major)
        result = register_chrdev_region(devno, 1, DEVICE_NAME);
    else
    {
        result = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
        usi_button_major = MAJOR(devno);
        printk(KERN_INFO "Todo: mknod /dev/%s c %d 0\n", DEVICE_NAME, usi_button_major);
    }
    if (result < 0)
        goto fail_request_chrdev_region;
	
    usi_button_devp = kmalloc(sizeof(struct Button_Dev), GFP_KERNEL);
    if (!usi_button_devp)
    {
        result = -ENOMEM;
        goto fail_malloc;
    }
    
    /* Create directory at /sys/class/x */
    usi_button_class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(usi_button_class))
    {
	printk("Err: failed in creating usi_button_class class.\n");
	result = IS_ERR(usi_button_class);
        goto fail_class_create;
    }
    device_create(usi_button_class, NULL, devno, NULL, DEVICE_NAME);
	
    memset(usi_button_devp, 0, sizeof(struct Button_Dev));
    result = usi_button_setup_cdev();
    if (result)
        goto fail_cdev;

    nls = netlink_kernel_create(&init_net, NETLINK_READYNAS,
                                0, NULL, NULL, THIS_MODULE);

    init_waitqueue_head(&(usi_button_dev.wq));
    usi_button_dev.head = usi_button_dev.tail = 0;
	
    for(i = 0; i < BTN_NUM; i++)
    {
        usi_button_dev.key_status[i] = KEYSTATUS_UP;
    }
	
    for(i = 0; i < BTN_NUM; i++)
    {        
        key_timer[i].function = usi_button_timer_callback;
        key_timer[i].data = i;
        init_timer(&key_timer[i]);
    }
    btn_int_set();

	result  = sysdev_class_register(&rn_button_sysclass);

	if(result)
		goto failed_create_sysfs;	
		
	result = sysdev_register(&rn_button_0);

	if(result)
		goto failed_create_sysfs;
	
	sysdev_create_file(&rn_button_0, &attr_backup_button);
	
    return 0;

fail_cdev:
    device_destroy(usi_button_class,devno);
fail_class_create:
    kfree(usi_button_devp);
fail_malloc: 
    unregister_chrdev_region(devno, 1);
fail_request_chrdev_region:
    free_irqs();
fail_request_irqs:
    btn_int_set();
failed_create_sysfs:
	sysdev_remove_file(&rn_button_0, &attr_backup_button);
	sysdev_unregister(&rn_button_0);
	sysdev_class_unregister(&rn_button_sysclass);
    return result;
}

static void __exit usi_button_exit(void)
{
    int i;
    device_destroy(usi_button_class, MKDEV(usi_button_major, 0));
    cdev_del(&usi_button_dev.cdev);
    kfree(usi_button_devp);
    netlink_kernel_release(nls);
    unregister_chrdev_region(MKDEV(usi_button_major, 0), 1);
    free_irqs();
    for(i = 0; i < BTN_NUM; i++)
    {
        del_timer(&key_timer[i]);
    }
	sysdev_remove_file(&rn_button_0, &attr_backup_button);
	sysdev_unregister(&rn_button_0);
	sysdev_class_unregister(&rn_button_sysclass);
}

MODULE_AUTHOR("usi nas team");
MODULE_LICENSE("GPL");

module_init(usi_button_init);
module_exit(usi_button_exit); 
