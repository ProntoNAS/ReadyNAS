/*
 * readynas-lcd.c
 *
 * NETGEAR ReadyNAS LCD alpha numeric display driver using Hitach HD44780
 *
 * Copyright (c)2015 NETGEAR, INC.
 * Author: Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "hd44780-lcd.h"
#ifdef CONFIG_READYNAS_LCD_PROC_INTERNAL
#include <../fs/proc/internal.h>
#endif
#include <linux/ctype.h>
#include <linux/kdebug.h>
#include <linux/kallsyms.h>
#include <asm/stacktrace.h>

/*#define DEBUG_LCD_PANIC_OOPS*/

static struct hd44780_lcd *phys_lcd;

static ssize_t LCD_proc_write(struct file *file, const char __user *buffer,
				size_t count, loff_t *data)
{
	char buf[40], c;
	const char *p, *pm;

	if (!phys_lcd)
		return -ENODEV;
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

	mutex_lock(&phys_lcd->lcm_lock);
	switch (c) {
	case '0':
		/* Clear LCD */
		phys_lcd->clear_lcd(phys_lcd);
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
		/* Set line; */
		phys_lcd->set_cursor_pos(phys_lcd, c - '1', 0);

		/* Put chars */
		while (pm < p)
			phys_lcd->data_write(phys_lcd, *pm++);
		break;
	case '3':
		/* Turn on BL */
		phys_lcd->entry_display_on(phys_lcd, 1);
		phys_lcd->backlight_on(phys_lcd, 1);
		break;
	case '4':
		/* Turn off BL */
		phys_lcd->entry_display_on(phys_lcd, 0);
		phys_lcd->backlight_on(phys_lcd, 0);
		break;
	case '7':
		/* Set command. */
		if (!p || !*p)
			break;
		while (*p && isspace(*p))
			p++;
		if (isxdigit(*p))
			phys_lcd->command_write(phys_lcd,
						simple_strtoul(p, NULL, 16));
		break;
	default:
		count = -EINVAL;
		break;
	}
	mutex_unlock(&phys_lcd->lcm_lock);
	
	return count;
}

static int LCDstatus_show(struct seq_file *s, void *p)
{
	if (!phys_lcd)
		return -ENODEV;

	seq_printf(s, "%d\n", phys_lcd->backlight_state(phys_lcd));
	return 0;
}

static int LCDstatus_open(struct inode *inode, struct file *file)
{
	return single_open(file, LCDstatus_show, NULL);
}

static int LCDinfo_show(struct seq_file *s, void *p)
{
	if (!phys_lcd)
		return -ENODEV;

	seq_printf(s, "%dx%d\n",
			phys_lcd->lcm_num_cols, phys_lcd->lcm_num_rows);
	return 0;
}

static int LCDinfo_open(struct inode *inode, struct file *file)
{
	return single_open(file, LCDinfo_show, NULL);
}

#ifdef DEBUG_LCD_PANIC_OOPS
static ssize_t LCD_proc_panic(struct file *file, const char __user *buffer,
				size_t count, loff_t *data)
{
	panic("%s:%d",__FILE__,__LINE__);
	return 0;
}

static ssize_t LCD_proc_oops(struct file *file, const char __user *buffer,
				size_t count, loff_t *data)
{
	*(int *)0 = 0;
	return 0;
}
#endif

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
	{
		.name		= "LCDinfo",
		.fops.open	= LCDinfo_open,
		.fops.read	= seq_read,
		.fops.llseek	= seq_lseek,
		.fops.release	= single_release,			
	},
#ifdef DEBUG_LCD_PANIC_OOPS
	{
		.name		= "LCDpanic",
		.fops.write	= LCD_proc_panic,
	},
	{
		.name		= "LCDoops",
		.fops.write	= LCD_proc_oops,
	},
#endif
	{}
};
		
static void __lcd_symbol_dump(unsigned long raddr)
{
	static char namebuf[KSYM_NAME_LEN];

	unsigned long addr = (unsigned long)raddr;
	int bufsiz = phys_lcd->lcm_num_rows * phys_lcd->lcm_num_cols;
	char buf[bufsiz + 1];
	const char *p = buf;
	const char *pm = p + phys_lcd->lcm_num_cols;
	int row;
	unsigned long symbolsize, offset;
	const char *symbol =
		    kallsyms_lookup(addr, &symbolsize, &offset, NULL, namebuf);

	snprintf(buf, bufsiz, "%s+%lx", symbol, offset);

	phys_lcd->clear_lcd(phys_lcd);
	phys_lcd->entry_display_on(phys_lcd, 1);
	phys_lcd->backlight_on(phys_lcd, 1);

	for (row = 0; row < phys_lcd->lcm_num_rows;
				row++, pm += phys_lcd->lcm_num_cols) {
		phys_lcd->set_cursor_pos(phys_lcd, row, 0);
		while (p < pm && *p)
			phys_lcd->data_write(phys_lcd, *p++);
		if (!*p)
			break;
	}
}

static inline void lcd_symbol_dump(unsigned long addr)
{
	if (phys_lcd)
		__lcd_symbol_dump(addr);
}

static int lcd_panic_handler(struct notifier_block *this,
			unsigned long event, void *data)
{
#if defined(CONFIG_ARM)
	struct stackframe frame;
	int urc;
#elif defined(CONFIG_X86) || defined(CONFIG_X86_64)
	unsigned long raddrs[] = {
		(unsigned long)__builtin_return_address(2),
		(unsigned long)__builtin_return_address(3),
		(unsigned long)__builtin_return_address(4),
		(unsigned long)__builtin_return_address(5),
		(unsigned long)__builtin_return_address(6),
		(unsigned long)__builtin_return_address(7),
		0UL
	};
	int i;
#define ret_addr(x)	raddrs[(x)]
#else
#error "Unsupported architecture."
#endif
	unsigned long symbolsize, offset;
	unsigned long p, p1 = (unsigned long)panic, p2 = 0;

	if (test_taint(TAINT_DIE) || oops_in_progress > 1)
		return NOTIFY_OK;

	/*
	 * Heuristically determine nesting level, i.e. depth from panic()
	 * caller, to get the panic() caller's stack frame.
	 */
	/* First, obtain panic()'s text range. */
	if (!kallsyms_lookup_size_offset(p1, &symbolsize, &offset) || offset)
		return NOTIFY_OK;
	for (p = p1 + 1; p < p1 + 10000; p++) {
		if (!kallsyms_lookup_size_offset(p, &symbolsize, &offset))
			return NOTIFY_OK;
		/* Out of panic()'s text range (new symbol with zero offset)? */
		if (!offset) {
			p2 = p;
			break;
		}
	}

	if (!p2)
		return NOTIFY_OK;
	/* Next, look for level to panic(). */
#if defined(CONFIG_ARM)
	frame.fp = (unsigned long)__builtin_frame_address(0);
	frame.sp = current_stack_pointer;
	frame.lr = (unsigned long)__builtin_return_address(0);
	frame.pc = (unsigned long)lcd_panic_handler;

	while ((urc = unwind_frame(&frame)) >= 0 &&
		(frame.pc < p1 || p2 <= frame.pc))
			;

	/* panic() caller is one level above. */
	/* Adjust one insn size (4 bytes). */
	if (urc >= 0 && unwind_frame(&frame) >= 0)
		lcd_symbol_dump(frame.pc - 4);
#elif defined(CONFIG_X86) || defined(CONFIG_X86_64)
	for (i = 0; ret_addr(i + 1); i++)
		if (p1 <= ret_addr(i) && ret_addr(i) < p2)
			break;

	/* panic() caller is one level above. */
	/* Adjust call/callq insn size (5 bytes). */
	if (ret_addr(++i))
		lcd_symbol_dump(ret_addr(i) - 5);
#else
#error "Unsupported architecture."
#endif
	return NOTIFY_OK;
}

static struct notifier_block lcd_panic_notifier = {
	.notifier_call	= lcd_panic_handler,
	.next		= NULL,
	.priority	= 100,
};

#ifndef GET_IP
#ifdef ARM_pc
#define GET_IP(regs)	(regs)->ARM_pc
#endif
#endif

static int lcd_die_handler(struct notifier_block *this,
			unsigned long event, void *data)
{
	struct die_args *da	= data;

	lcd_symbol_dump(GET_IP(da->regs));
	return NOTIFY_OK;
}

static struct notifier_block lcd_die_notifier = {
	.notifier_call	= lcd_die_handler,
	.next		= NULL,
	.priority	= 100,
};

#ifndef CONFIG_OF
static int lcd_dev_match(struct device *dev, void *data)
{
	struct hd44780_lcd *gl = dev_get_drvdata(dev);

	return gl && !strncmp(gl->magic, (const char *)data, sizeof gl->magic);
}
#endif

static int readynas_lcd_probe(struct platform_device *pdev)
{
	struct proc_dir_entry *parent = NULL;
	int i;
#ifdef CONFIG_OF
	struct platform_device *lcd_pdev;
	phandle phandle = 0;
	struct device_node *np = pdev->dev.of_node, *np2;

	if (!np || of_property_read_u32(np, "lcd-device", &phandle) ||
		!(np2 = of_find_node_by_phandle(phandle)))
		return -ENODEV;

	lcd_pdev = of_find_device_by_node(np2);
	if (IS_ERR(lcd_pdev))
		return PTR_ERR(lcd_pdev);
	else if (!lcd_pdev)
		return -ENODEV;
	phys_lcd = platform_get_drvdata(lcd_pdev);
#else
	struct device *phys_dev = bus_find_device(&platform_bus_type, NULL,
					LCD_PLATFORM_MAGIC, lcd_dev_match);
	if (!phys_dev) {
		pr_debug("%s: platform device with \"%s\" magic not found.\n",
			__func__, LCD_PLATFORM_MAGIC);
		return -ENODEV;
	}
	phys_lcd = dev_get_drvdata(phys_dev);
#endif
	if (!phys_lcd)
		return -ENODEV;

	mutex_init(&phys_lcd->lcm_lock);

#ifdef CONFG_READYNAS_LCD_PROC_INTERNAL
	/* If procs internal is available, set LCD and LCDstatus under
	 * /proc/readynas.
	 */
	{
		mm_segment_t fs = get(fs);
		struct file *f;

		set_fs(get_ds());
		if ((f = filp_open((char __user *)"/proc/readynas",
					O_RDONLY, 0))) {
			parent = PDE(file_inode(f));
			filp_close(f, NULL);
		}
		set_fs(fs);
	}
#endif

	for (i = 0; lcd_procs[i].name; i++) {
		umode_t mode = 0;

		if (lcd_procs[i].fops.read)
			mode |= 0444;
		if (lcd_procs[i].fops.write)
			mode |= 0200;
		lcd_procs[i].proc =
			proc_create(lcd_procs[i].name, mode, parent,
					&lcd_procs[i].fops);
		if (!lcd_procs[i].proc)
			break;
	}

	if (lcd_procs[i].name) {
		while (--i >= 0)
			proc_remove(lcd_procs[i].proc);
		return -EIO;
	}

	atomic_notifier_chain_register(&panic_notifier_list,
					&lcd_panic_notifier);
	register_die_notifier(&lcd_die_notifier);
	return 0;
}

static int readynas_lcd_remove(struct platform_device *pdev)
{
	int i;

	unregister_die_notifier(&lcd_die_notifier);
	atomic_notifier_chain_unregister(&panic_notifier_list,
					&lcd_panic_notifier);
	for (i = 0; lcd_procs[i].name; i++)
		proc_remove(lcd_procs[i].proc);
	return 0;
}

static struct of_device_id readynas_lcd_dt_ids[] = {
	{
		.compatible = "readynas-lcd",
	}, {},
};

static struct platform_driver readynas_lcd_driver = {
	.probe	= readynas_lcd_probe,
	.remove	= readynas_lcd_remove,
	.driver = {
		.name = "readynas-lcd",
		.of_match_table = readynas_lcd_dt_ids,
	},
};

#ifdef CONFIG_OF
module_platform_driver(readynas_lcd_driver);
#else
static struct platform_device *readynas_lcd_pdev;

static int __init readynas_lcd_init(void)
{
	int err;

	pr_info("%s: installing ReadyNAS LCD driver.\n", __func__);
	if ((err  = platform_driver_register(&readynas_lcd_driver)))
		return err;

	readynas_lcd_pdev = 
		platform_device_register_simple(readynas_lcd_driver.driver.name,
						-1, NULL, 0);
	if (IS_ERR(readynas_lcd_pdev))
		err = PTR_ERR(readynas_lcd_pdev);
	else if (!readynas_lcd_pdev)
		err = -ENODEV;
	if (err) {
		platform_driver_unregister(&readynas_lcd_driver);
		return err;
	} else if (!phys_lcd) {
		platform_device_unregister(readynas_lcd_pdev);
		platform_driver_unregister(&readynas_lcd_driver);
		return -ENODEV;
	} else {
		const char str[] = "Booting..";
		phys_lcd->set_cursor_pos(phys_lcd, 0, 0);
		for (err = 0; err < sizeof(str)-1; err++)
			phys_lcd->data_write(phys_lcd, str[err]);
	}
	return 0;
}

static void __exit readynas_lcd_exit(void)
{
	platform_device_unregister(readynas_lcd_pdev);
	platform_driver_unregister(&readynas_lcd_driver);
}

/* This module must be loaded after readynas-io. */
device_initcall_sync(readynas_lcd_init);
module_exit(readynas_lcd_exit);
#endif

MODULE_AUTHOR("hiro.sugawara@netgear.com");
MODULE_DESCRIPTION("ReadyNAS LCD driver");
MODULE_LICENSE("GPL");
