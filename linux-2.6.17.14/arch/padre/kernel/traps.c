/* $Id: traps.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 * arch/sparc/kernel/traps.c
 *
 * Copyright 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright 2000 Jakub Jelinek (jakub@redhat.com)
 */

/*
 * I hate traps on the sparc, grrr...
 */

#include <linux/config.h>
#include <linux/sched.h>  /* for jiffies */
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/signal.h>

#include <asm/delay.h>
#include <asm/system.h>
#include <asm/ptrace.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/kdebug.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <asm/cacheflush.h>

/* #define TRAP_DEBUG */

struct trap_trace_entry {
	unsigned long pc;
	unsigned long type;
};

int trap_curbuf = 0;
struct trap_trace_entry trapbuf[1024];

unsigned long SAVE_GLOB_TMP  __dc3data;

void syscall_trace_entry(struct pt_regs *regs)
{
	printk("%s[%d]: ", current->comm, current->pid);
	printk("scall<%d> (could be %d)\n", (int) regs->u_regs[UREG_G1],
	       (int) regs->u_regs[UREG_I0]);
}

void syscall_trace_exit(struct pt_regs *regs)
{
}

void instruction_dump (unsigned long *pc)
{
	int i;
	
	if((((unsigned long) pc) & 3))
                return;

	for(i = -3; i < 6; i++)
		printk("%c%08lx%c",i?' ':'<',pc[i],i?' ':'>');
	printk("\n");
}

#define __SAVE __asm__ __volatile__("save %sp, -0x40, %sp\n\t")
#define __RESTORE __asm__ __volatile__("restore %g0, %g0, %g0\n\t")

void die_if_kernel(char *str, struct pt_regs *regs)
{
	static int die_counter;
	int count = 0;

	/* Amuse the user. */
	printk(
"              \\|/ ____ \\|/\n"
"              \"@'/ ,. \\`@\"\n"
"              /_| \\__/ |_\\\n"
"                 \\__U_/\n");

	printk("%s(%d): %s [#%d]\n", current->comm, current->pid, str, ++die_counter);
	show_regs(regs);

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
		      count++ < 30				&&
                      (((unsigned long) rw) >= PAGE_OFFSET)	&&
		      !(((unsigned long) rw) & 0x7)) {
			printk("Caller[%08lx]", rw->ins[7]);
			print_symbol(": %s\n", rw->ins[7]);
			rw = (struct reg_window *)rw->ins[6];
		}
	}
	printk("Instruction DUMP:");
	instruction_dump ((unsigned long *) regs->pc);
	if(regs->psr & PSR_PS)
		do_exit(SIGKILL);
	do_exit(SIGSEGV);
}

void do_hw_interrupt(struct pt_regs *regs, unsigned long type)
{
	siginfo_t info;

	if(type < 0x80) {
		/* Sun OS's puke from bad traps, Linux survives! */
		printk("Unimplemented Sparc TRAP, type = %02lx\n", type);
		die_if_kernel("Whee... Hello Mr. Penguin", regs);
	}	

	if(regs->psr & PSR_PS)
		die_if_kernel("Kernel bad trap", regs);

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_ILLTRP;
	info.si_addr = (void __user *)regs->pc;
	info.si_trapno = type - 0x80;
	force_sig_info(SIGILL, &info, current);
}

static unsigned long illegal_instruction_count = 0;

void do_illegal_instruction(struct pt_regs *regs, unsigned long pc, unsigned long npc,
			    unsigned long psr)
{
	extern int do_user_muldiv (struct pt_regs *, unsigned long);
	siginfo_t info;

	if(psr & PSR_PS)
		die_if_kernel("Kernel illegal instruction", regs);
#ifdef TRAP_DEBUG
	printk("Ill instr. at pc=%08lx instruction is %08lx\n",
	       regs->pc, *(unsigned long *)regs->pc);
#endif
	if (!do_user_muldiv (regs, pc))
		return;
#if 1
    if(illegal_instruction_count < 100) {
        illegal_instruction_count++;
        flush_sig_insns(NULL,pc);
        return ;
    }
    illegal_instruction_count = 0;
#endif
			
	
	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_ILLOPC;
	info.si_addr = (void __user *)pc;
	info.si_trapno = 0;
	send_sig_info(SIGILL, &info, current);
}

void do_priv_instruction(struct pt_regs *regs, unsigned long pc, unsigned long npc,
			 unsigned long psr)
{
	siginfo_t info;

	if(psr & PSR_PS)
		die_if_kernel("Penguin instruction from Penguin mode??!?!", regs);
	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_PRVOPC;
	info.si_addr = (void __user *)pc;
	info.si_trapno = 0;
	send_sig_info(SIGILL, &info, current);
}

/* XXX User may want to be allowed to do this. XXX */

void do_memaccess_unaligned(struct pt_regs *regs, unsigned long pc, unsigned long npc,
			    unsigned long psr)
{
	siginfo_t info;

	if(regs->psr & PSR_PS) {
		printk("KERNEL MNA at pc %08lx npc %08lx called by %08lx\n", pc, npc,
		       regs->u_regs[UREG_RETPC]);
		die_if_kernel("BOGUS", regs);
		/* die_if_kernel("Kernel MNA access", regs); */
	}
#if 0
	show_regs (regs);
	instruction_dump ((unsigned long *) regs->pc);
	printk ("do_MNA!\n");
#endif
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRALN;
	info.si_addr = /* FIXME: Should dig out mna address */ (void *)0;
	info.si_trapno = 0;
	send_sig_info(SIGBUS, &info, current);
}

extern int do_mathemu(struct pt_regs *, struct task_struct *);

void do_fpd_trap(struct pt_regs *regs, unsigned long pc, unsigned long npc,
		 unsigned long psr)
{
	/* Sanity check... */
	if(psr & PSR_PS)
		die_if_kernel("Kernel gets FloatingPenguinUnit disabled trap", regs);

    last_task_used_math = current;
	set_used_math();

   	if(!do_mathemu(regs,last_task_used_math)) {
		struct task_struct *fpt = current;
	    unsigned long fsr;
		siginfo_t info;
			
    	printk("WARNING: FPU cannot be emulated\n");
		fsr = fpt->thread.fsr;
		info.si_signo = SIGFPE;
		info.si_errno = 0;
		info.si_addr = (void __user *)pc;
		info.si_trapno = 0;
		info.si_code = __SI_FAULT;
		if ((fsr & 0x1c000) == (1 << 14)) {
			if (fsr & 0x10)
				info.si_code = FPE_FLTINV;
			else if (fsr & 0x08)
				info.si_code = FPE_FLTOVF;
			else if (fsr & 0x04)
				info.si_code = FPE_FLTUND;
			else if (fsr & 0x02)
				info.si_code = FPE_FLTDIV;
			else if (fsr & 0x01)
				info.si_code = FPE_FLTRES;
		}
		send_sig_info(SIGFPE, &info, fpt);
	}
}

void handle_tag_overflow(struct pt_regs *regs, unsigned long pc, unsigned long npc,
			 unsigned long psr)
{
	siginfo_t info;

	if(psr & PSR_PS)
		die_if_kernel("Penguin overflow trap from kernel mode", regs);
	info.si_signo = SIGEMT;
	info.si_errno = 0;
	info.si_code = EMT_TAGOVF;
	info.si_addr = (void __user *)pc;
	info.si_trapno = 0;
	send_sig_info(SIGEMT, &info, current);
}

void handle_watchpoint(struct pt_regs *regs, unsigned long pc, unsigned long npc,
		       unsigned long psr)
{
#ifdef TRAP_DEBUG
	printk("Watchpoint detected at PC %08lx NPC %08lx PSR %08lx\n",
	       pc, npc, psr);
#endif
	if(psr & PSR_PS)
		panic("Tell me what a watchpoint trap is, and I'll then deal "
		      "with such a beast...");
}

void handle_cp_disabled(struct pt_regs *regs, unsigned long pc, unsigned long npc,
			unsigned long psr)
{
	siginfo_t info;

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_COPROC;
	info.si_addr = (void __user *)pc;
	info.si_trapno = 0;
	send_sig_info(SIGILL, &info, current);
}

void handle_hw_divzero(struct pt_regs *regs, unsigned long pc, unsigned long npc,
		       unsigned long psr)
{
	siginfo_t info;

	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_code = FPE_INTDIV;
	info.si_addr = (void __user *)pc;
	info.si_trapno = 0;
	send_sig_info(SIGFPE, &info, current);
}

#ifdef CONFIG_DEBUG_BUGVERBOSE
void do_BUG(const char *file, int line)
{
        // bust_spinlocks(1);   XXX Not in our original BUG()
        printk("kernel BUG at %s:%d!\n", file, line);
}
#endif

/* Since we have our mappings set up, on multiprocessors we can spin them
 * up here so that timer interrupts work during initialization.
 */


void trap_init(void)
{
	extern void thread_info_offsets_are_bolixed_pete(void);

	/* Force linker to barf if mismatched */
	if (TI_UWINMASK    != offsetof(struct thread_info, uwinmask) ||
	    TI_TASK        != offsetof(struct thread_info, task) ||
	    TI_EXECDOMAIN  != offsetof(struct thread_info, exec_domain) ||
	    TI_FLAGS       != offsetof(struct thread_info, flags) ||
	    TI_CPU         != offsetof(struct thread_info, cpu) ||
	    TI_PREEMPT     != offsetof(struct thread_info, preempt_count) ||
	    TI_SOFTIRQ     != offsetof(struct thread_info, softirq_count) ||
	    TI_HARDIRQ     != offsetof(struct thread_info, hardirq_count) ||
	    TI_KSP         != offsetof(struct thread_info, ksp) ||
	    TI_KPC         != offsetof(struct thread_info, kpc) ||
	    TI_KPSR        != offsetof(struct thread_info, kpsr) ||
	    TI_KWIM        != offsetof(struct thread_info, kwim) ||
	    TI_REG_WINDOW  != offsetof(struct thread_info, reg_window) ||
	    TI_RWIN_SPTRS  != offsetof(struct thread_info, rwbuf_stkptrs) ||
	    TI_W_SAVED     != offsetof(struct thread_info, w_saved))
		thread_info_offsets_are_bolixed_pete();

	/* Attach to the address space of init_task. */
	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
}
