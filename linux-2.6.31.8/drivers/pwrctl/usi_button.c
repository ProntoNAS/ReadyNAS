#include <linux/module.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "gpp/mvGpp.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include <linux/delay.h>

#define BUF_CLEAR _IO(0xFF, 0)

#define MAX_KEY_BUF 16
#define KEY_NUM  3
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

#define INCBUF(x,mod) ((++(x))&((mod)-1))

struct class *usi_button_class;

struct Button_Dev
{
    struct cdev cdev;
    unsigned int key_status[KEY_NUM];
    unsigned int key_timer_count[KEY_NUM];
    unsigned int buf[MAX_KEY_BUF];
    unsigned int head,tail;
    wait_queue_head_t wq;
}usi_button_dev,*usi_button_devp = NULL;

struct timer_list key_timer[KEY_NUM];

static struct key_info
{
    int irq_no;
    unsigned int key_code;
    char *name;
}key_info_tab[KEY_NUM] = 
{
    {IRQ_GPP_13, 0x1, "reset" },
    {IRQ_GPP_45, 0x2, "backup"},
    {IRQ_GPP_47, 0x3, "power"},
};

enum BUTTON_KEY {
	RESET,
	BACKUP,
	POWER,
};

static void keyEvent(unsigned key)
{
    if (key & 0x10)
        BUF_HEAD = key_info_tab[(key & 0xF)].key_code | 0x10;//press and hold on
    else
        BUF_HEAD = key_info_tab[key].key_code;
    usi_button_dev.head = INCBUF(usi_button_dev.head, MAX_KEY_BUF);
    //check if the queue is empty
    if (waitqueue_active(&(usi_button_dev.wq)))
        wake_up_interruptible(&(usi_button_dev.wq));
}

static void free_irqs(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(key_info_tab); i++)
    {
        free_irq(key_info_tab[i].irq_no, (void *)i);
    }
}

static irqreturn_t usi_button_irq(int irq, void *dev_id)
{
    int key = (int)dev_id;
    int i;
    int found = 0;
    unsigned long irq_flags;

    //printk(KERN_NOTICE"libo ...... %s\n", __func__);

    local_irq_save(irq_flags);
    //MV_REG_READ(GPP_INT_CAUSE_REG(0));//clear
    //MV_REG_READ(GPP_INT_CAUSE_REG(1));
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);

    //printk(KERN_NOTICE"libo ......1 %s\n", __func__);
    for (i = 0; i < ARRAY_SIZE(key_info_tab); i++)
    {
        if (key_info_tab[i].irq_no == irq) 
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
    //printk(KERN_NOTICE"libo ......2 %s, %s\n", __func__, key_info_tab[i].name);
    //disable_irq(key_info_tab[key].irq_no);
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & ~(BIT4 | BIT8)));

    usi_button_dev.key_status[key] = KEYSTATUS_DOWNX;
    key_timer[key].expires = jiffies + KEY_TIME_DELAY1;
    //printk(KERN_NOTICE"libo ......4 %s\n", __func__);
    add_timer(&key_timer[key]);

    local_irq_restore(irq_flags);
    return IRQ_HANDLED;
}

static int request_irqs(void)
{
    int i;
    MV_U32 tmp_val = 0;

    /* close main interrupt */
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
    //MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & ~(BIT4 | BIT8)));

    /* set POL */
    tmp_val = MV_REG_READ(GPP_DATA_IN_POL_REG(0)) | BIT13;
    MV_REG_WRITE(GPP_DATA_IN_POL_REG(0), tmp_val);
		
    tmp_val = MV_REG_READ(GPP_DATA_IN_POL_REG(1)) | BIT13 | BIT15;
    MV_REG_WRITE(GPP_DATA_IN_POL_REG(1), tmp_val);

    for (i = 0; i < ARRAY_SIZE(key_info_tab); i++)
    {
        if (request_irq(key_info_tab[i].irq_no, usi_button_irq,IRQF_DISABLED,DEVICE_NAME,(void *)i))
        {
            printk(KERN_WARNING "buttons:can't get irq no.%d\n", key_info_tab[i].irq_no);            
            return 1;
        }
	//printk("<0>""libo ...... request_irq %d\n", i);
    }

    /* Open main interrupt */
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
    //MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
    return 0;
}

static void usi_button_timer_callback(unsigned long data)
{
    int key = data;
    int isdown = 0;
    //printk("<0>" "libo ...... %s.\n", __func__);
    //MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));

    switch (key) {
        case RESET:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(0)) & BIT13;
	    //MV_REG_WRITE(GPP_INT_CAUSE_REG(0), MV_REG_READ(GPP_INT_CAUSE_REG(0)) & (~BIT13));//clear
	    break;
	case BACKUP:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT13;
	    //MV_REG_WRITE(GPP_INT_CAUSE_REG(1), MV_REG_READ(GPP_INT_CAUSE_REG(1)) & (~BIT13));//clear
	    break;
	case POWER:
	    isdown = MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT15;
	    //MV_REG_WRITE(GPP_INT_CAUSE_REG(1), MV_REG_READ(GPP_INT_CAUSE_REG(1)) & (~BIT15));//clear
	    break;
	default:
	    break;
    }
    if (isdown)
    {
        if (usi_button_dev.key_status[key] == KEYSTATUS_DOWNX)
        {
            usi_button_dev.key_status[key] = KEYSTATUS_DOWN;
            key_timer[key].expires = jiffies + KEY_TIME_DELAY;
	    
	    usi_button_dev.key_timer_count[key] += 1;
            add_timer(&key_timer[key]);
            //printk("<0>" "libo ......DOWNX %s.\n", __func__);
        }else
        {
	    if (usi_button_dev.key_timer_count[key] == 30)
	    {
	        /* press and hold on more than 3s. key | 0x10 */
		keyEvent(key | 0x10);
		switch (key) {
		    case RESET:
			MV_REG_WRITE(GPP_INT_LVL_REG(0), MV_REG_READ(GPP_INT_LVL_REG(0)) & (~BIT13));
			break;
		    case BACKUP:
			MV_REG_WRITE(GPP_INT_LVL_REG(1), MV_REG_READ(GPP_INT_LVL_REG(1)) & (~BIT13));
			break;
		    case POWER:
			MV_REG_WRITE(GPP_INT_LVL_REG(1), MV_REG_READ(GPP_INT_LVL_REG(1)) & (~BIT15));
			break;
		    default:
			break;
		}
		MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
	    }
            //printk("<0>" "libo ......DOWN %s.\n", __func__);
            key_timer[key].expires = jiffies + KEY_TIME_DELAY;
	    usi_button_dev.key_timer_count[key] += 1;
            add_timer(&key_timer[key]);
        }
    }else
    {
    	if (usi_button_dev.key_timer_count[key] > 30)
	{
	    /* press and hold on more than 3s. key | 0x10 */
	    //keyEvent(key | 0x10);
    	    switch (key) {
                case RESET:
		    MV_REG_WRITE(GPP_INT_LVL_REG(0), MV_REG_READ(GPP_INT_LVL_REG(0)) | BIT13);
	    	    break;
	        case BACKUP:
	            MV_REG_WRITE(GPP_INT_LVL_REG(1), MV_REG_READ(GPP_INT_LVL_REG(1)) | BIT13);
	    	    break;
	        case POWER:
	            MV_REG_WRITE(GPP_INT_LVL_REG(1), MV_REG_READ(GPP_INT_LVL_REG(1)) | BIT15);
	    	    break;
	        default:
	            break;
	    }
	}
	else if (usi_button_dev.key_timer_count[key] >= 0)
	{
	    keyEvent(key);
	    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
	}

        usi_button_dev.key_status[key] = KEYSTATUS_UP;
	usi_button_dev.key_timer_count[key] = 0;
        //enable_irq(key_info_tab[key].irq_no);
        //printk("<0>" "libo ......BUTTON UP %s.\n", __func__);
	/* Open interrupt */
	//MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, (MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
    }
}

static int usi_button_ioctl(struct inode *inodep, struct file *filp, unsigned
						int cmd, unsigned long arg)
{
    unsigned long flags;
    
    switch (cmd)
    {
	case BUF_CLEAR:
	    local_irq_save(flags);
	    usi_button_dev.head = usi_button_dev.tail = 0;
	    local_irq_restore(flags);
	    //printk(KERN_INFO "key buffer is cleared\n");
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
    copy_to_user(buffer, &key_ret, sizeof(unsigned int));
    
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
    .ioctl = usi_button_ioctl,
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
	
    //printk("<0>""libo...... %s\n", __func__);
    /* close main interrupt */
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) & ~(BIT4 | BIT8)));
    result = request_irqs();
    if (result) {
        //unregister_chrdev_region(devno,1);
        //return result;
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

    init_waitqueue_head(&(usi_button_dev.wq));
    usi_button_dev.head = usi_button_dev.tail = 0;
	
    for(i = 0; i < KEY_NUM; i++)
    {
        usi_button_dev.key_status[i] = KEYSTATUS_UP;
    }
	
    for(i = 0; i < KEY_NUM; i++)
    {        
        key_timer[i].function = usi_button_timer_callback;
        key_timer[i].data = i;
        init_timer(&key_timer[i]);
    }
    /* Open main interrupt */
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
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
    /* Open main interrupt */
    //MV_REG_READ(CPU_MAIN_INT_CAUSE_HIGH_REG);
    MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG,(MV_REG_READ(CPU_MAIN_IRQ_MASK_HIGH_REG) | BIT4 | BIT8));
    return result;
}

static void __exit usi_button_exit(void)
{
    int i;
    device_destroy(usi_button_class, MKDEV(usi_button_major, 0));
    cdev_del(&usi_button_dev.cdev);
    kfree(usi_button_devp);          
    unregister_chrdev_region(MKDEV(usi_button_major, 0), 1);
    free_irqs();
    for(i = 0; i < KEY_NUM; i++)
    {
        del_timer(&key_timer[i]);
    }
}

MODULE_AUTHOR("usi nas team");
MODULE_LICENSE("GPL");

module_init(usi_button_init);
module_exit(usi_button_exit); 
