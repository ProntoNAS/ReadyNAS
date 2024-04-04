/*
 * Kernel Debugger Architecture Dependent Stack Traceback
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2004 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <asm/system.h>
#include <asm/sections.h>

/*
 * bt_print_one
 *
 *	Print one back trace entry.
 *
 * Inputs:
 *	ip	Current program counter.
 *	symtab	Information about symbol that ip falls within.
 *	argcount Maximum number of arguments to print.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *	None.
 */

static void
bt_print_one(kdb_machreg_t ip,
	     const lkdb_symtab_t *symtab, int argcount,
	     struct unw_frame_info *info)
{
	int btsymarg = 0;		/* Convert arguments to symbols */
	int btsp = 0;			/* Print stack and backing store pointers */
	int nosect = 0;			/* Suppress section data */
	int args;
	kdb_machreg_t sp, bsp, cfm;

	lkdbgetintenv("BTSYMARG", &btsymarg);
	lkdbgetintenv("BTSP", &btsp);
	lkdbgetintenv("NOSECT", &nosect);

	unw_get_sp(info, &sp);
	unw_get_bsp(info, &bsp);
	unw_get_cfm(info, &cfm);
	lkdb_symbol_print(ip, symtab, KDB_SP_VALUE|KDB_SP_NEWLINE);
	args = (cfm >> 7) & 0x7f;	/* sol */
	if (!args)
		args = cfm & 0x7f;	/* no in/local, use sof instead */
	if (argcount && args) {
		int i, argc = args;

		lkdb_printf("        args (");
		if (argc > argcount)
			argc = argcount;

		for(i = 0; i < argc; i++){
			kdb_machreg_t arg;
			char nat;
			if (unw_access_gr(info, i+32, &arg, &nat, 0))
				arg = 0;

			if (i)
				lkdb_printf(", ");
			lkdb_printf("0x%lx", arg);
		}
		lkdb_printf(")\n");
		if (btsymarg) {
			lkdb_symtab_t	arg_symtab;
			kdb_machreg_t	arg;
			for(i = 0; i < argc; i++){
				char nat;
				if (unw_access_gr(info, i+32, &arg, &nat, 0))
					arg = 0;
				if (lkdbnearsym(arg, &arg_symtab)) {
					lkdb_printf("        arg %d ", i);
					lkdb_symbol_print(arg, &arg_symtab, KDB_SP_DEFAULT|KDB_SP_NEWLINE);
				}
			}
		}
	}
	if (symtab->sym_name) {
		if (!nosect) {
			lkdb_printf("        %s", symtab->mod_name);
			if (symtab->sec_name)
				lkdb_printf(" %s 0x%lx", symtab->sec_name, symtab->sec_start);
			lkdb_printf(" 0x%lx", symtab->sym_start);
			if (symtab->sym_end)
				lkdb_printf(" 0x%lx", symtab->sym_end);
			lkdb_printf("\n");
		}
		if (strncmp(symtab->sym_name, "ia64_spinlock_contention", 24) == 0) {
			kdb_machreg_t r31;
			char nat;
			lkdb_printf("        r31 (spinlock address) ");
			if (unw_access_gr(info, 31, &r31, &nat, 0))
				r31 = 0;
			lkdb_symbol_print(r31, NULL, KDB_SP_VALUE|KDB_SP_NEWLINE);
		}
	}
	if (btsp)
		lkdb_printf("        sp 0x%016lx bsp 0x%016lx cfm 0x%016lx info->pfs_loc 0x%016lx 0x%016lx\n",
				sp, bsp, cfm, (u64) info->pfs_loc, info->pfs_loc ? *(info->pfs_loc) : 0);
}

/*
 * kdba_bt_stack
 *
 *	Unwind the ia64 backtrace for a specified process.
 *
 * Inputs:
 *	argcount
 *	p	Pointer to task structure to unwind.
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	none.
 */

static int
kdba_bt_stack(int argcount, const struct task_struct *p)
{
	lkdb_symtab_t symtab;
	struct unw_frame_info info;
	struct switch_stack *sw;
	struct pt_regs *regs = NULL;
	int count = 0;
	int btsp = 0;			/* Backtrace the kdb code as well */
	u64 *prev_pfs_loc = NULL;
	extern char __attribute__ ((weak)) ia64_spinlock_contention_pre3_4[];
	extern char __attribute__ ((weak)) ia64_spinlock_contention_pre3_4_end[];

	/*
	 * Upon entering kdb_main_loop, the stack frame looks like this:
	 *
	 *	+---------------------+
	 *	|   struct pt_regs    |
	 *	+---------------------+
	 *	|		      |
	 *	|   kernel stack      |
	 *	|		      |
	 *	+=====================+ <--- top of stack upon entering kdb
	 *	|   struct pt_regs    |
	 *	+---------------------+
	 *	|		      |
	 *	|   kdb stack         |
	 *	|		      |
	 *	+---------------------+
	 *	| struct switch_stack |
	 *	+=====================+ <--- kdb_running_process[cpu].arch.sw from do_kdba_main_loop
	 *
	 * When looking at another process, we do not have the address of the
	 * current pt_regs, it is NULL.  If the process has saved its state, use
	 * that pt_regs instead.
	 */

	lkdbgetintenv("BTSP", &btsp);

	if (lkdb_task_has_cpu(p)) {
		struct lkdb_running_process *krp = lkdb_running_process + lkdb_process_cpu(p);
		if (krp->seqno) {
			sw = krp->arch.sw;
			regs = krp->regs;
		}
		else
			sw = NULL;
	}
	else {
		/* Not running, assume blocked */
		sw = (struct switch_stack *) (p->thread.ksp + 16);
	}
	if (!sw) {
		lkdb_printf("Process does not have a switch_stack, cannot backtrace\n");
		lkdb_ps1(p);
		return 0;
	}

	unw_init_frame_info(&info, (struct task_struct *)p, sw);

	/* If we have the address of pt_regs, suppress backtrace on the frames below
	 * pt_regs.  No point in displaying kdb itself, unless the user is debugging
	 * the unwinder using set BTSP=1.
	 */
	if (regs && !btsp) {
		kdb_machreg_t sp;
		if (user_mode(regs)) {
			lkdb_printf("Process was interrupted in user mode, no backtrace available\n");
			return 0;
		}
		do {
			unw_get_sp(&info, &sp);
			if (sp >= (kdb_machreg_t)regs)
				break;
		} while (unw_unwind(&info) >= 0 && count++ < 200);
	}

	do {
		kdb_machreg_t ip;

		/* Avoid unsightly console message from unw_unwind() when attempting
		 * to unwind through the Interrupt Vector Table which has no unwind
		 * information.  dispatch_illegal_op_fault() is an exception, it sits
		 * in the 0x3c00 slot.
		 */
		if (info.ip >= (u64)__start_ivt_text && info.ip < (u64)__end_ivt_text) {
			if (info.ip < (u64)__start_ivt_text + 0x3c00 ||
			    info.ip >= (u64)__start_ivt_text + 0x4000)
				return 0;
		}

		/* WAR for spinlock contention from leaf functions.  ia64_spinlock_contention_pre3_4
		 * has ar.pfs == r0.  Leaf functions do not modify ar.pfs so ar.pfs remains
		 * as 0, stopping the backtrace.  Record the previous ar.pfs when the current
		 * IP is in ia64_spinlock_contention_pre3_4 then unwind, if pfs_loc has not changed
		 * after unwind then use pt_regs.ar_pfs which is where the real ar.pfs is for
		 * leaf functions.
		 */
		if (prev_pfs_loc && regs && info.pfs_loc == prev_pfs_loc)
			info.pfs_loc = &regs->ar_pfs;
		prev_pfs_loc = (info.ip >= (u64)ia64_spinlock_contention_pre3_4 &&
				info.ip < (u64)ia64_spinlock_contention_pre3_4_end) ?
			       info.pfs_loc : NULL;

		unw_get_ip(&info, &ip);
		if (ip == 0)
			break;

		lkdbnearsym(ip, &symtab);
		if (!symtab.sym_name) {
			lkdb_printf("0x%0*lx - No name.  May be an area that has no unwind data\n",
				(int)(2*sizeof(ip)), ip);
			return 0;
		}
		bt_print_one(ip, &symtab, argcount, &info);
	} while (unw_unwind(&info) >= 0 && count++ < 200);
	if (count >= 200)
		lkdb_printf("bt truncated, count limit reached\n");

	return 0;
}

int
kdba_bt_address(kdb_machreg_t addr, int argcount)
{
	lkdb_printf("Backtrace from a stack address is not supported on ia64\n");
	return LKDB_NOTIMP;
}

/*
 * kdba_bt_process
 *
 *	Do a backtrace for a specified process.
 *
 * Inputs:
 *	p	Struct task pointer extracted by 'bt' command.
 *	argcount
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 */

int
kdba_bt_process(const struct task_struct *p, int argcount)
{
	return kdba_bt_stack(argcount, p);
}
