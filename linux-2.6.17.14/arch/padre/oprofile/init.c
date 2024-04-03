/**
 * @file init.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author John Levon <levon@movementarian.org>
 */

#include <linux/oprofile.h>
#include <linux/init.h>
#include <linux/errno.h>
 
extern int op_timer_init(struct oprofile_operations * ops);
extern void padre_backtrace(struct pt_regs * const regs, unsigned int depth);

int __init oprofile_arch_init(struct oprofile_operations * ops)
{
	int ret;

	ret = -ENODEV;
	ret = op_timer_init(ops);
	ops->backtrace = padre_backtrace;

	return ret;
}


void oprofile_arch_exit(void)
{
}
