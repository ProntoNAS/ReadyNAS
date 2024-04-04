#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/tty.h>
#include <linux/proc_fs.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/dmi.h>

#define BUFFER_SIZE     128
#define LCD_PORT	"/dev/ttyS1"

/* Global definitions */

static struct platform_device *pdev;
int backlight_state;
struct semaphore door;

struct disk_led_info {
	const char *name;
	int channel;
	struct led_classdev cdev;
};

struct disk_led_info oled_leds[] = {
	{ "readynas::sata1", 1 },
	{ "readynas::sata2", 2 },
	{ "readynas::sata3", 3 },
	{ "readynas::sata4", 4 },
	{ "readynas::sata5", 5 },
	{ "readynas::sata6", 6 },
};

/* Local functions */

static void
lcd_delay(int ms)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(ms * HZ / 1000);
}

static void nv6_lcd_output(const char *buf, size_t count)
{
	mm_segment_t fs;
	struct file *filp;
	
	lcd_delay(150);

	fs = get_fs();
	set_fs(get_ds());
	filp = filp_open(LCD_PORT, O_RDWR|O_NOCTTY|O_NDELAY, 0);
	if (IS_ERR(filp)) {
		printk(KERN_ERR "lcd driver cannot open %s!\n", LCD_PORT);
		goto exit_output;
	}
	if (filp->f_op && filp->f_op->write) {
		filp->f_pos = 0;
		filp->f_op->write(filp, buf, count, &filp->f_pos);
		filp->f_op->unlocked_ioctl(filp, TCSBRK, 1);
	}
	filp_close(filp, NULL);
exit_output:
	set_fs(fs);
}

static void print_line(const char *buf, size_t count)
{
	char copy[32];
	char buffer[32];
	int i;

	for (i = 0; i < count && buf[i] && i < sizeof(copy)-1; i++) {
		copy[i] = buf[i];
		if (buf[i] == '\r' || buf[i] == '\n')
			break;
		if ((uint8_t)buf[i] == (uint8_t)0xFF)
			copy[i] = '#';
		else if ((uint8_t)buf[i] == (uint8_t)0x01)
			copy[i] = '>';
	}
	copy[i] = '\0';
	i = snprintf(buffer, sizeof(buffer), "L %-25.25s\r", copy);
	nv6_lcd_output(buffer, i);
}

static ssize_t nv6_lcd_line1(const char *buf, size_t count)
{
	down(&door);
	/* Line 1 starts from (0x18, 0) */
	nv6_lcd_output("C 18 0\r", 7);
	print_line(buf, count);
	up(&door);

	return count;
}

static ssize_t nv6_lcd_line2(const char *buf, size_t count)
{
	down(&door);
	/* Line 2 starts from (0x18, 0xD) */
	nv6_lcd_output("C 18 D\r", 7);
	print_line(buf, count);
	up(&door);

	return count;
}

static inline void clear_lcd(void)
{
	nv6_lcd_output("E\r", 2);
}

static inline void backlight_on(void)
{
	nv6_lcd_output("D 1\r", 4);
	backlight_state = 1;
}

static inline void backlight_off(void)
{
	nv6_lcd_output("D 0\r", 4);
	backlight_state = 0;
}

static void nv6_lcd_position_disk(int dnum)
{
	lcd_delay(50);
	switch (dnum) {
		case 1:
			nv6_lcd_output("C 0 0\r", 6);
			break;
		case 2:
			nv6_lcd_output("C 8 0\r", 6);
			break;
		case 3:
			nv6_lcd_output("C 10 0\r", 7);
			break;
		case 4:
			nv6_lcd_output("C 0 D\r", 6);
			break;
		case 5:
			nv6_lcd_output("C 8 D\r", 6);
			break;
		case 6:
			nv6_lcd_output("C 10 D\r", 7);
			break;
		default:
			break;
	}
	lcd_delay(50);
}

static void nv6_lcd_disp_std_disk(int dnum)
{
	nv6_lcd_position_disk(dnum);

	switch(dnum) {
		case 1:
			nv6_lcd_output("B 8 A 0 7F 41 49 59 49 49 5D 41 7F 0\r", 37);
			break;
		case 2:
			nv6_lcd_output("B 8 A 0 7F 41 5D 45 5D 51 5D 41 7F 0\r", 37);
			break;
		case 3:
			nv6_lcd_output("B 8 A 0 7F 41 5D 45 5D 45 5D 41 7F 0\r", 37);
			break;
		case 4:
			nv6_lcd_output("B 8 A 0 7F 41 55 55 5D 45 45 41 7F 0\r", 37);
			break;
		case 5:
			nv6_lcd_output("B 8 A 0 7F 41 5D 51 5D 45 5D 41 7F 0\r", 37);
			break;
		case 6:
			nv6_lcd_output("B 8 A 0 7F 41 5D 51 5D 55 5D 41 7F 0\r", 37);
			break;
		default:
			break;
	}
}

static void nv6_lcd_disp_blank_disk(int dnum)
{
	nv6_lcd_position_disk(dnum);

	if (dnum > 0 && dnum < 7)
		nv6_lcd_output("B 8 A 0 7F 41 41 41 41 41 41 41 7F 0\r", 37);
}

static void nv6_lcd_disp_fail_disk(int dnum)
{
	nv6_lcd_position_disk(dnum);

	if (dnum > 0 && dnum < 7)
		nv6_lcd_output("B 8 A 0 7F 41 63 55 49 55 63 41 7F 0\r", 37);
}

static void init_lcd_screen(void)
{
	//1. stop progress bar
	nv6_lcd_output("P 0\r", 4);
	nv6_lcd_output("P 0\r", 4);
	//2. clear screen
	clear_lcd();
	//3. highlight screen
	backlight_on();
	//4. display 6 disk icons
	nv6_lcd_disp_blank_disk(1);
	nv6_lcd_disp_blank_disk(2);
	nv6_lcd_disp_blank_disk(3);
	nv6_lcd_disp_blank_disk(4);
	nv6_lcd_disp_blank_disk(5);
	nv6_lcd_disp_blank_disk(6);
	nv6_lcd_output("F 0\r", 4);
	//5. display line 1
	nv6_lcd_line1("  ReadyNAS", 10);
	//6. display line 2
	nv6_lcd_line2("", 0);
}

/* Proc interface */

static ssize_t LCD_proc_write(struct file *file, const char __user *buffer,
				size_t count, loff_t *data)
{
	char buf[40], c;
	const char *p, *pm;

	if (!buffer || !count)
		return 0;

	memset(buf, 0, sizeof(buf));

	if (copy_from_user(buf, buffer, min_t(size_t, count, sizeof buf) - 1))
		return -EFAULT;

	for (p = buf; *p && isspace(*p); p++)
		;
	if (!*p)
		return count;
	c = *p++;
	if (*p && !isspace(*p))
		return count;

	switch (c) {
	case '0':
		/* Clear LCD */
		clear_lcd();
		break;
	case '1':
	case '2': /* echo 1 \"test LCD\" */
		/* collect strings bewteen 2 of '\"'  */
		while (*p && *p != '"')
			p++;
		if (!*p)
			break;
		pm = ++p;
		if (!*p)
			break;

		while (*p && *p != '"')
			p++;

		if (!*p)
			break;
		/* Set line */
		if (c == '1')
			nv6_lcd_line1(pm, (p - pm));
		else
			nv6_lcd_line2(pm, (p - pm));
		break;
	case '3':
		/* Turn on BL */
		backlight_on();
		break;
	case '4':
		/* Turn off BL */
		backlight_off();
		break;
	default:
		count = -EINVAL;
		break;
	}

	return count;
}

static int LCDstatus_show(struct seq_file *s, void *p)
{
	seq_printf(s, "%d\n", backlight_state);
	return 0;
}

static int LCDstatus_open(struct inode *inode, struct file *file)
{
	return single_open(file, LCDstatus_show, NULL);
}

static struct lcd_proc {
	const char *name;
	struct proc_dir_entry *proc;
	const struct file_operations fops;
} lcd_procs[] = {
	{
		.name		= "LCD",
		.fops.write	= LCD_proc_write,
	},
	{
		.name		= "LCDstatus",
		.fops.open	= LCDstatus_open,
		.fops.read	= seq_read,
		.fops.llseek	= seq_lseek,
		.fops.release	= single_release,
	},
	{}
};

//Here is for /sys/devices/system/...

static int oled_blink_set(struct led_classdev *led_cdev, unsigned long *delay_on,
				unsigned long *delay_off)
{
	struct disk_led_info *led =
		container_of(led_cdev, struct disk_led_info, cdev);

	if (!*delay_on || !*delay_off)
		*delay_on = *delay_off = 500;

	if (*delay_on < 500 || *delay_off < 500)
		nv6_lcd_disp_std_disk(led->channel);
	else
		nv6_lcd_disp_fail_disk(led->channel);

	return 0;
}

static void oled_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct disk_led_info *led =
		container_of(led_cdev, struct disk_led_info, cdev);

	if (value)
		nv6_lcd_disp_std_disk(led->channel);
	else
		nv6_lcd_disp_blank_disk(led->channel);
}

static int register_led(struct device *parent, struct disk_led_info *led)
{
	led->cdev.name = led->name;
	led->cdev.brightness_set = oled_set;
	led->cdev.brightness = 0;
	led->cdev.blink_set = oled_blink_set;
	led->cdev.flags |= LED_CORE_SUSPENDRESUME;

	return led_classdev_register(parent, &led->cdev);
}

static int nv6lcd_probe(struct platform_device *pdev)
{
	int ret;
	int i;

	for (i = 0; i < sizeof(oled_leds) / sizeof(oled_leds[0]); i++)
	{
		ret = register_led(&pdev->dev, &oled_leds[i]);
		if (ret)
			return ret;
	}

	return 0;
}

static int nv6lcd_remove(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < sizeof(oled_leds) / sizeof(oled_leds[0]); i++)
		led_classdev_unregister(&oled_leds[i].cdev);

	return 0;
}

static const struct dmi_system_id nv6lcd_dmi_table[] = {
	{
		.ident = "ReadyNAS Pro 6",
		.matches = {
			DMI_MATCH(DMI_PRODUCT_VERSION, "FLAME6-MB"),
		},
	},
	{
		.ident = "ReadyNAS Ultra 6",
		.matches = {
			DMI_MATCH(DMI_PRODUCT_VERSION, "FLAME6-2"),
		},
	},
	{ }
};

static struct platform_driver nv6lcd_driver = {
	.probe          = nv6lcd_probe,
	.remove         = nv6lcd_remove,
	.driver         = {
		.name           = "nv6lcd",
	},
};

static int nv6lcd_init(void) 
{
	int ret, i;

	if (!dmi_check_system(nv6lcd_dmi_table))
		return -ENODEV;

	sema_init(&door, 1);

	ret = platform_driver_register(&nv6lcd_driver);
	if (ret) {
		printk(KERN_ERR "Error initialzing LCD sysfs interface\n");
		return -EIO;
	}
	pdev = platform_device_register_simple("nv6lcd", -1, NULL, 0);
	if (IS_ERR(pdev)) {
		ret = PTR_ERR(pdev);
		platform_driver_unregister(&nv6lcd_driver);
		return ret;
	}

	init_lcd_screen();

	for (i = 0; lcd_procs[i].name; i++) {
		umode_t mode = 0;

		if (lcd_procs[i].fops.read)
			mode |= 0444;
		if (lcd_procs[i].fops.write)
			mode |= 0200;
		lcd_procs[i].proc =
			proc_create(lcd_procs[i].name, mode, NULL,
					&lcd_procs[i].fops);
		if (!lcd_procs[i].proc)
			return -EIO;
	}

	if (lcd_procs[i].name) {
		platform_device_unregister(pdev);
		platform_driver_unregister(&nv6lcd_driver);
		while (--i >= 0)
			proc_remove(lcd_procs[i].proc);
		return -EIO;
	}

	printk(KERN_INFO "nv6lcd v3.1 loaded.\n");
	return 0;
}

static void nv6lcd_exit(void)
{
	int i;

	platform_device_unregister(pdev);
	platform_driver_unregister(&nv6lcd_driver);

	for (i = 0; lcd_procs[i].name; i++)
		proc_remove(lcd_procs[i].proc);

	printk(KERN_INFO "nv6lcd unloaded.\n");
}

module_init(nv6lcd_init);
module_exit(nv6lcd_exit);

MODULE_DESCRIPTION("ReadyNAS Pro LCD Driver");
MODULE_AUTHOR("Jason Qian");
MODULE_LICENSE("GPL");

