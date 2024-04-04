/*
 * Kernel Debugger Architecture Independent Stack Traceback
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
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/nmi.h>
#include <asm/system.h>
#include <linux/unwind.h>

#ifdef CONFIG_X86_64
static void
kdb_dump_trace_unwind (struct unwind_frame_info *info)
{
	unsigned long sp = UNW_SP(info);

	if (info && info->regs.ip) {
		/* vsnprintf: %pS output the name of a symbol with offset */
		lkdb_printf(" [<%p>] %pS\n",
			(void *)info->regs.ip, (void *)info->regs.ip);
	}
	while (unwind(info) == 0 && UNW_PC(info)) {
		lkdb_printf(" [<%p>] %pS\n",
			(void *)UNW_PC(info), (void *)UNW_PC(info));
		if ((sp & ~(PAGE_SIZE - 1)) == (UNW_SP(info) & ~(PAGE_SIZE - 1))
		    && sp > UNW_SP(info))
			break;
		sp = UNW_SP(info);
	}
}

static void
kdb_stack_unwind(struct task_struct *task, struct pt_regs *regs)
{
	struct unwind_frame_info info;

	if (regs) {
		if (unwind_init_frame_info(&info, task, regs))
			return;
	} else {
		if (unwind_init_blocked(&info, task))
			return;
	}
	kdb_dump_trace_unwind(&info);
}
#endif

/*
 * lkdb_bt
 *
 *	This function implements the 'bt' command.  Print a stack
 *	traceback.
 *
 *	bt [<address-expression>]	(addr-exp is for alternate stacks)
 *	btp <pid>			Kernel stack for <pid>
 *	btt <address-expression>	Kernel stack for task structure at <address-expression>
 *	bta [DRSTCZEUIMA]		All useful processes, optionally filtered by state
 *	btc [<cpu>]			The current process on one cpu, default is all cpus
 *
 *	bt <address-expression> refers to a address on the stack, that location
 *	is assumed to contain a return address.
 *
 *	btt <address-expression> refers to the address of a struct task.
 *
 * Inputs:
 *	argc	argument count
 *	argv	argument vector
 * Outputs:
 *	None.
 * Returns:
 *	zero for success, a kdb diagnostic if error
 * Locking:
 *	none.
 * Remarks:
 *	Backtrack works best when the code uses frame pointers.  But even
 *	without frame pointers we should get a reasonable trace.
 *
 *	mds comes in handy when examining the stack to do a manual traceback or
 *	to get a starting point for bt <address-expression>.
 */

static int kdb_show_stack(struct task_struct *p, void *addr, int argcount)
{
	/* Use KDB arch-specific backtraces for ia64 */
#ifdef CONFIG_IA64
	return kdba_bt_process(p, argcount);
#elif defined(CONFIG_X86_64)
	/* Use the in-kernel backtraces */
	int old_lvl = console_loglevel;
	console_loglevel = 15;
	kdb_trap_printk++;
	kdba_set_current_task(p);
	if (addr)
		show_stack((struct task_struct *)p, addr);
	else
		kdb_stack_unwind(p, lkdb_current_regs);

	if (lkdb_current_regs && !KDB_STATE(BTNOREGS))
		kdba_dumpregs(lkdb_current_regs, NULL, NULL);

	console_loglevel = old_lvl;
	kdb_trap_printk--;
	return 0;
#else
	/* Use the in-kernel backtraces */
	int old_lvl = console_loglevel;
	console_loglevel = 15;
	kdba_set_current_task(p);
	if (addr) {
		show_stack((struct task_struct *)p, addr);
		if (lkdb_current_regs)
			kdba_dumpregs(lkdb_current_regs, NULL, NULL);
	} else if (lkdb_current_regs) {
#ifdef CONFIG_X86
		show_stack(p, &lkdb_current_regs->sp);
		if (lkdb_current_regs)
			kdba_dumpregs(lkdb_current_regs, NULL, NULL);
#else
		show_stack(p, NULL);
		if (lkdb_current_regs)
			kdba_dumpregs(lkdb_current_regs, NULL, NULL);
#endif
	} else {
		show_stack(p, NULL);
		if (lkdb_current_regs)
			kdba_dumpregs(lkdb_current_regs, NULL, NULL);
	}
	console_loglevel = old_lvl;
	return 0;
#endif /* CONFIG_IA64 */
}


static int
lkdb_bt1(struct task_struct *p, unsigned long mask, int argcount, int btaprompt)
{
	int diag;
	char buffer[2];
	if (lkdb_getarea(buffer[0], (unsigned long)p) ||
	    lkdb_getarea(buffer[0], (unsigned long)(p+1)-1))
		return LKDB_BADADDR;
	if (!lkdb_task_state(p, mask))
		return 0;
	lkdb_printf("Stack traceback for pid %d\n", p->pid);
	lkdb_ps1(p);
	diag = kdb_show_stack(p, NULL, argcount);
	if (btaprompt) {
		lkdb_getstr(buffer, sizeof(buffer),
			"Enter <q> to end, <cr> to continue:");
		if (buffer[0] == 'q') {
			lkdb_printf("\n");
			return 1;
		}
	}
	touch_nmi_watchdog();
	return 0;
}

int
lkdb_bt(int argc, const char **argv)
{
	int diag;
	int argcount = 5;
	int btaprompt = 1;
	int nextarg;
	unsigned long addr;
	long offset;
	int doregs = 1;

	lkdbgetintenv("BTREGS", &doregs);	/* Show reg info during multiple BT's */
	lkdbgetintenv("BTARGS", &argcount);	/* Arguments to print */
	lkdbgetintenv("BTAPROMPT", &btaprompt);	/* Prompt after each proc in bta */

	if (strcmp(argv[0], "bta") == 0) {
		struct task_struct *g, *p;
		unsigned long cpu;
		unsigned long mask = lkdb_task_state_string(argc ? argv[1] : NULL);
		if (!doregs)
			KDB_STATE_SET(BTNOREGS);
		if (argc == 0)
			lkdb_ps_suppressed();
		/* Run the active tasks first */
		for (cpu = 0; cpu < NR_CPUS; ++cpu) {
			if (!cpu_online(cpu))
				continue;
			p = lkdb_curr_task(cpu);
			if (lkdb_bt1(p, mask, argcount, btaprompt)) {
				KDB_STATE_CLEAR(BTNOREGS);
				return 0;
			}
		}
		/* Now the inactive tasks */
		lkdb_do_each_thread(g, p) {
			if (task_curr(p))
				continue;
			if (lkdb_bt1(p, mask, argcount, btaprompt)) {
				KDB_STATE_CLEAR(BTNOREGS);
				return 0;
			}
		} lkdb_while_each_thread(g, p);
	} else if (strcmp(argv[0], "btp") == 0) {
		struct task_struct *p;
		unsigned long pid;

		if (argc != 1)
			return LKDB_ARGCOUNT;
		if ((diag = lkdbgetularg((char *)argv[1], &pid)))
			return diag;
		if ((p = find_task_by_pid_ns(pid, &init_pid_ns))) {
			kdba_set_current_task(p);
			return lkdb_bt1(p, ~0UL, argcount, 0);
		}
		lkdb_printf("No process with pid == %ld found\n", pid);
		return 0;
	} else if (strcmp(argv[0], "btt") == 0) {
		if (argc != 1)
			return LKDB_ARGCOUNT;
		if ((diag = lkdbgetularg((char *)argv[1], &addr)))
			return diag;
		kdba_set_current_task((struct task_struct *)addr);
		return lkdb_bt1((struct task_struct *)addr, ~0UL, argcount, 0);
	} else if (strcmp(argv[0], "btc") == 0) {
		unsigned long cpu = ~0;
		struct lkdb_running_process *krp;
		struct task_struct *save_current_task = lkdb_current_task;
		char buf[80];
		if (argc > 1)
			return LKDB_ARGCOUNT;
		if (argc == 1 && (diag = lkdbgetularg((char *)argv[1], &cpu)))
			return diag;
		/* Recursive use of lkdb_parse, do not use argv after this point */
		argv = NULL;
		if (cpu != ~0) {
			krp = lkdb_running_process + cpu;
			if (cpu >= NR_CPUS || !krp->seqno || !cpu_online(cpu)) {
				lkdb_printf("no process for cpu %ld\n", cpu);
				return 0;
			}
			sprintf(buf, "btt 0x%p\n", krp->p);
			lkdb_parse(buf);
			return 0;
		}
		if (!doregs)
			KDB_STATE_SET(BTNOREGS);
		lkdb_printf("btc: cpu status: ");
		lkdb_parse("cpu\n");
		for (cpu = 0, krp = lkdb_running_process; cpu < NR_CPUS; ++cpu, ++krp) {
			if (!cpu_online(cpu) || !krp->seqno)
				continue;
			sprintf(buf, "btt 0x%p\n", krp->p);
			lkdb_parse(buf);
			touch_nmi_watchdog();
		}
		kdba_set_current_task(save_current_task);
		KDB_STATE_CLEAR(BTNOREGS);
		return 0;
	} else {
		if (argc) {
			nextarg = 1;
			diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr,
					     &offset, NULL);
			if (diag)
				return diag;
			return kdb_show_stack(lkdb_current_task, (void *)addr, argcount);
		} else {
			return lkdb_bt1(lkdb_current_task, ~0UL, argcount, 0);
		}
	}

	/* NOTREACHED */
	return 0;
}
