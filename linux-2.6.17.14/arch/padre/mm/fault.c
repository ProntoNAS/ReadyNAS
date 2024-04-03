/* $Id: fault.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 * fault.c:  Page fault handlers for the Sparc.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1996 Eddie C. Dost (ecd@skynet.be)
 * Copyright (C) 1997 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 */

#include <asm/head.h>

#include <linux/string.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/threads.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#include <asm/system.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/smp.h>
#include <asm/traps.h>
#include <asm/kdebug.h>
#include <asm/uaccess.h>

unsigned long tmp_register[32] __dc3data ____cacheline_aligned;
unsigned long icmu_ctrl=0,dcmu_ctrl=0;

static void unhandled_fault(unsigned long, struct task_struct *,
		struct pt_regs *) __attribute__ ((noreturn));

static void unhandled_fault(unsigned long address, struct task_struct *tsk,
                     struct pt_regs *regs)
{
	if((unsigned long) address < PAGE_SIZE) {
		printk(KERN_ALERT
		    "Unable to handle kernel NULL pointer dereference\n");
	} else {
		printk(KERN_ALERT "Unable to handle kernel paging request "
		       "at virtual address %08lx\n", address);
	}
	printk(KERN_ALERT "tsk->{mm,active_mm}->context = %08lx\n",
		(tsk->mm ? tsk->mm->context : tsk->active_mm->context));
	printk(KERN_ALERT "tsk->{mm,active_mm}->pgd = %08lx\n",
		(tsk->mm ? (unsigned long) tsk->mm->pgd :
		 	(unsigned long) tsk->active_mm->pgd));
	die_if_kernel("Oops", regs);
}

asmlinkage int lookup_fault(unsigned long pc, unsigned long ret_pc, 
			    unsigned long address)
{
	struct pt_regs regs;
	unsigned long g2;
	unsigned int insn;
	int i;
	
	i = search_extables_range(ret_pc, &g2);
	switch (i) {
	case 3:
		/* load & store will be handled by fixup */
		return 3;

	case 1:
		/* store will be handled by fixup, load will bump out */
		/* for _to_ macros */
		insn = *((unsigned int *) pc);
		if ((insn >> 21) & 1)
			return 1;
		break;

	case 2:
		/* load will be handled by fixup, store will bump out */
		/* for _from_ macros */
		insn = *((unsigned int *) pc);
		if (!((insn >> 21) & 1) || ((insn>>19)&0x3f) == 15)
			return 2; 
		break; 

	default:
		break;
	};

	memset(&regs, 0, sizeof (regs));
	regs.pc = pc;
	regs.npc = pc + 4;
	__asm__ __volatile__(
		"rd %%psr, %0\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n" : "=r" (regs.psr));
	unhandled_fault(address, current, &regs);

	/* Not reached */
	return 0;
}

extern unsigned long safe_compute_effective_address(struct pt_regs *,
						    unsigned int);

static unsigned long compute_si_addr(struct pt_regs *regs, int text_fault)
{
	unsigned int insn;

	if (text_fault)
		return regs->pc;

	if (regs->psr & PSR_PS) {
		insn = *(unsigned int *) regs->pc;
	} else {
		__get_user(insn, (unsigned int *) regs->pc);
	}

	return safe_compute_effective_address(regs, insn);
}

u32 current_pgd;
asmlinkage void __ic3call do_sparc_fault(struct pt_regs *regs, int text_fault, 
		int write, unsigned long address)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	unsigned int fixup;
	unsigned long g2;
	siginfo_t info;
	int from_user = !(regs->psr & PSR_PS);

	if(text_fault)
		address = regs->pc;

	/*
	 * We fault-in kernel-space virtual memory on-demand. The
	 * 'reference' page table is init_mm.pgd.
	 *
	 * NOTE! We MUST NOT take any locks for this case. We may
	 * be in an interrupt or a critical region, and should
	 * only copy the information from the master page table,
	 * nothing more.
	 */
	if (!from_user && address >= TASK_SIZE)
		goto vmalloc_fault;

	info.si_code = SEGV_MAPERR;

	/*
	 * If we're in an interrupt or have no user
	 * context, we must not take the fault..
	 */
        if (in_atomic() || !mm)
                goto no_context;

	down_read(&mm->mmap_sem);

	vma = find_vma(mm, address);
	if(!vma)
		goto bad_area;
	if(vma->vm_start <= address)
		goto good_area;
	if(!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;
	if(expand_stack(vma, address))
		goto bad_area;
	/*
	 * Ok, we have a good vm_area for this memory access, so
	 * we can handle it..
	 */
good_area:
	info.si_code = SEGV_ACCERR;
	if(write) {
		if(!(vma->vm_flags & VM_WRITE))
			goto bad_area;
	} else {
		/* Allow reads even for write-only mappings */
		if(!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto bad_area;
	}

	/*
	 * If for any reason at all we couldn't handle the fault,
	 * make sure we exit gracefully rather than endlessly redo
	 * the fault.
	 */

        switch (handle_mm_fault(mm, vma, address, write)) {
        case VM_FAULT_SIGBUS:
                goto do_sigbus;
        case VM_FAULT_OOM:
                goto out_of_memory;
        case VM_FAULT_MAJOR:
                current->maj_flt++;
                break;
        case VM_FAULT_MINOR:
        default:
                current->min_flt++;
                break;
        }

	up_read(&mm->mmap_sem);
	return;

	/*
	 * Something tried to access memory that isn't in our memory map..
	 * Fix it, but check if it's kernel or user first..
	 */
bad_area:
	up_read(&mm->mmap_sem);

bad_area_nosemaphore:
	/* User mode accesses just cause a SIGSEGV */
	if(from_user) {
#if 0
		printk("Fault whee %s [%d]: segfaults at %08lx pc=%08lx\n",
		       tsk->comm, tsk->pid, address, regs->pc);
#endif
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		/* info.si_code set above to make clear whether
		   this was a SEGV_MAPERR or SEGV_ACCERR fault.  */
		info.si_addr = (void __user *) compute_si_addr(regs, text_fault);
		info.si_trapno = 0;
		force_sig_info (SIGSEGV, &info, tsk);
		return;
	}

	/* Is this in ex_table? */
no_context:
	g2 = regs->u_regs[UREG_G2];
	if (!from_user && (fixup = search_extables_range(regs->pc, &g2))) {
		if (fixup > 10) { /* Values below are reserved for other things */
			extern const unsigned __memset_start[];
			extern const unsigned __memset_end[];
			extern const unsigned __csum_partial_copy_start[];
			extern const unsigned __csum_partial_copy_end[];

#ifdef DEBUG_EXCEPTIONS
			printk("Exception: PC<%08lx> faddr<%08lx>\n", regs->pc, address);
			printk("EX_TABLE: insn<%08lx> fixup<%08x> g2<%08lx>\n",
				regs->pc, fixup, g2);
#endif
			if ((regs->pc >= (unsigned long)__memset_start &&
			     regs->pc < (unsigned long)__memset_end) ||
			    (regs->pc >= (unsigned long)__csum_partial_copy_start &&
			     regs->pc < (unsigned long)__csum_partial_copy_end)) {
			        regs->u_regs[UREG_I4] = address;
				regs->u_regs[UREG_I5] = regs->pc;
			}
			regs->u_regs[UREG_G2] = g2;
			regs->pc = fixup;
			regs->npc = regs->pc + 4;
			return;
		}
	}
	
	unhandled_fault (address, tsk, regs);
	do_exit(SIGKILL);

/*
 * We ran out of memory, or some other thing happened to us that made
 * us unable to handle the page fault gracefully.
 */
out_of_memory:
	up_read(&mm->mmap_sem);
	printk("VM: killing process %s\n", tsk->comm);
	if (from_user)
		do_exit(SIGKILL);
	goto no_context;

do_sigbus:
	up_read(&mm->mmap_sem);
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void __user *) compute_si_addr(regs, text_fault);
	info.si_trapno = 0;
	force_sig_info (SIGBUS, &info, tsk);
	if (!from_user)
		goto no_context;

vmalloc_fault:
	{
		/*
		 * Synchronize this task's top level page-table
		 * with the 'reference' page table.
		 */
		int offset = pgd_index(address);
		pgd_t *pgd, *pgd_k;
		pmd_t *pmd, *pmd_k;
     		pte_t *pte_k;
	 
		
		pgd = tsk->active_mm->pgd + offset;
		pgd_k = init_mm.pgd + offset;

		if (!pgd_present(*pgd)) {
			if (!pgd_present(*pgd_k))
				goto bad_area_nosemaphore;
			pgd_val(*pgd) = pgd_val(*pgd_k);
			return;
		}

		pmd = pmd_offset(pgd, address);
		pmd_k = pmd_offset(pgd_k, address);

		if (pmd_present(*pmd) || !pmd_present(*pmd_k))
			goto bad_area_nosemaphore;
		pmd_val(*pmd) = pmd_val(*pmd_k);

        	pte_k = pte_offset(pmd_k, address);
#ifdef CONFIG_PADRE_8K_PAGE
		address = (address >> PAGE_SHIFT) << PAGE_SHIFT;
#endif
#ifdef CONFIG_PADRE_16K_PAGE
		address = (address >> PAGE_SHIFT) << PAGE_SHIFT;
#endif

        	/* update ptek entry, our hardware is very special */
        	__asm__ __volatile__(
            		"sta %0, [%1]%2\t\n"
#ifdef CONFIG_PADRE_8K_PAGE
			"sta %3, [%4]%2\t\n"
#endif
#ifdef CONFIG_PADRE_16K_PAGE
			"sta %3, [%4]%2\t\n"
			"sta %5, [%6]%2\t\n"
			"sta %7, [%8]%2\t\n"
#endif
            		:
#ifdef CONFIG_PADRE_16K_PAGE
            		: "r"(pte_k->pte1),"r"(address),"i"(ASI_TLBPROBE),
			  "r"(pte_k->pte2),"r"(address+0x1000),
			  "r"(pte_k->pte3),"r"(address+0x2000),
			  "r"(pte_k->pte4),"r"(address+0x3000)
#else
#ifdef CONFIG_PADRE_8K_PAGE
            		: "r"(pte_k->pte_low),"r"(address),"i"(ASI_TLBPROBE),
			  "r"(pte_k->pte_high),"r"(address+0x1000)
#else
            		: "r"(*pte_k),"r"(address),"i"(ASI_TLBPROBE)
#endif
#endif
        	);
		
		return;
	}
}

/* This always deals with user addresses. */
inline void force_user_fault(unsigned long address, int write)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	siginfo_t info;

	info.si_code = SEGV_MAPERR;

#if 0
	printk("wf<pid=%d,wr=%d,addr=%08lx>\n",
	       tsk->pid, write, address);
#endif
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, address);
	if(!vma)
		goto bad_area;
	if(vma->vm_start <= address)
		goto good_area;
	if(!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;
	if(expand_stack(vma, address))
		goto bad_area;
good_area:
	info.si_code = SEGV_ACCERR;
	if(write) {
		if(!(vma->vm_flags & VM_WRITE))
			goto bad_area;
	} else {
		if(!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto bad_area;
	}
#ifdef CONFIG_PADRE
        switch (handle_mm_fault(mm, vma, address, write)) {
        case VM_FAULT_SIGBUS:
        case VM_FAULT_OOM:
                goto do_sigbus;
        }
#else
	if (!handle_mm_fault(mm, vma, address, write))
		goto do_sigbus;
#endif
	up_read(&mm->mmap_sem);
	return;
bad_area:
	up_read(&mm->mmap_sem);
#if 0
	printk("Window whee %s [%d]: segfaults at %08lx\n",
	       tsk->comm, tsk->pid, address);
#endif
	info.si_signo = SIGSEGV;
	info.si_errno = 0;
	/* info.si_code set above to make clear whether
	   this was a SEGV_MAPERR or SEGV_ACCERR fault.  */
	info.si_addr = (void __user *) address;
	info.si_trapno = 0;
	force_sig_info (SIGSEGV, &info, tsk);
	return;

do_sigbus:
	up_read(&mm->mmap_sem);
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void __user *) address;
	info.si_trapno = 0;
	force_sig_info (SIGBUS, &info, tsk);
}

void window_overflow_fault(void)
{
	unsigned long sp;

	sp = current_thread_info()->rwbuf_stkptrs[0];
	if(((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 1);
	force_user_fault(sp, 1);
}

void window_underflow_fault(unsigned long sp)
{
	if(((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 0);
	force_user_fault(sp, 0);
}

void window_ret_fault(struct pt_regs *regs)
{
	unsigned long sp;

	sp = regs->u_regs[UREG_FP];
	if(((sp + 0x38) & PAGE_MASK) != (sp & PAGE_MASK))
		force_user_fault(sp + 0x38, 0);
	force_user_fault(sp, 0);
}
