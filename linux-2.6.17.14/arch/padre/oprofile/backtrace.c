/**
 * @file backtrace.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author John Levon
 * @author David Smith
 */

#include <linux/oprofile.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>

static struct reg_window *
dump_backtrace(struct reg_window * rw)
{
	return NULL;
}


#define __SAVE __asm__ __volatile__("save %sp, -0x40, %sp\n\t")
#define __RESTORE __asm__ __volatile__("restore %g0, %g0, %g0\n\t")

void
padre_backtrace(struct pt_regs * const regs, unsigned int depth)
{
        __SAVE; __SAVE; __SAVE; __SAVE;
        __SAVE; __SAVE; __SAVE; __SAVE;
        __RESTORE; __RESTORE; __RESTORE; __RESTORE;
        __RESTORE; __RESTORE; __RESTORE; __RESTORE;


	{
		struct reg_window *rw = (struct reg_window *)regs->u_regs[UREG_FP];

		/* Stop the back trace when we hit userland or we
		 * find some badly aligned kernel stack. Set an upper
		 * bound in case our stack is trashed and we loop.
		 */
		while(rw					&&
		      (depth--)				        &&
                      (((unsigned long) rw) >= PAGE_OFFSET)	&&
		      !(((unsigned long) rw) & 0x7)) {
			//printk("Caller[%08lx]", rw->ins[7]);
			//print_symbol(": %s\n", rw->ins[7]);
			oprofile_add_trace(rw->ins[7]);
			rw = (struct reg_window *)rw->ins[6];
		}
	}

}
