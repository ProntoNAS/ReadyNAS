/*
 * Kernel Debugger Architecture Independent Support Functions
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2006 Silicon Graphics, Inc.  All Rights Reserved.
 * Copyright (C) David Mosberger-Tang <davidm@hpl.hp.com>
 */

#include <linux/string.h>
#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/module.h>
#ifdef CONFIG_KDB_KDUMP
#include <linux/kexec.h>
#endif

#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/rse.h>
#include <asm/delay.h>
#ifdef CONFIG_SMP
#include <asm/hw_irq.h>
#endif
#include <asm/mca.h>

struct lkdb_running_process *lkdb_running_process_save; /* [NR_CPUS] */
static int kdba_show_handlers = 0;

static int
kdba_itm (int argc, const char **argv)
{
	int diag;
	unsigned long val;

	diag = lkdbgetularg(argv[1], &val);
	if (diag)
		return diag;
	lkdb_printf("new itm=" kdb_machreg_fmt "\n", val);

	ia64_set_itm(val);
	return 0;
}

static void
kdba_show_intregs(void)
{
	u64 lid, tpr, lrr0, lrr1, itv, pmv, cmcv;

	asm ("mov %0=cr.lid" : "=r"(lid));
	asm ("mov %0=cr.tpr" : "=r"(tpr));
	asm ("mov %0=cr.lrr0" : "=r"(lrr0));
	asm ("mov %0=cr.lrr1" : "=r"(lrr1));
	lkdb_printf("lid=" kdb_machreg_fmt64 ", tpr=" kdb_machreg_fmt64 ", lrr0=" kdb_machreg_fmt64 ", llr1=" kdb_machreg_fmt64 "\n", lid, tpr, lrr0, lrr1);

	asm ("mov %0=cr.itv" : "=r"(itv));
	asm ("mov %0=cr.pmv" : "=r"(pmv));
	asm ("mov %0=cr.cmcv" : "=r"(cmcv));
	lkdb_printf("itv=" kdb_machreg_fmt64 ", pmv=" kdb_machreg_fmt64 ", cmcv=" kdb_machreg_fmt64 "\n", itv, pmv, cmcv);

	lkdb_printf("irr=0x%016llx,0x%016llx,0x%016llx,0x%016llx\n",
		ia64_getreg(_IA64_REG_CR_IRR0), ia64_getreg(_IA64_REG_CR_IRR1), ia64_getreg(_IA64_REG_CR_IRR2), ia64_getreg(_IA64_REG_CR_IRR3));

	lkdb_printf("itc=0x%016lx, itm=0x%016lx\n", ia64_get_itc(), ia64_get_itm());
}

static int
kdba_sir (int argc, const char **argv)
{
	kdba_show_intregs();

	return 0;
}

/*
 * kdba_pt_regs
 *
 *	Format a struct pt_regs
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
 *	If no address is supplied, it uses the current irq pt_regs.
 */

static int
kdba_pt_regs(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset = 0;
	int nextarg;
	struct pt_regs *p;

	if (argc == 0) {
		addr = (kdb_machreg_t) get_irq_regs();
	} else if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;
	} else {
		return LKDB_ARGCOUNT;
	}

	p = (struct pt_regs *) addr;
	lkdb_printf("struct pt_regs %p-%p\n", p, (unsigned char *)p + sizeof(*p) - 1);
	lkdb_print_nameval("b6", p->b6);
	lkdb_print_nameval("b7", p->b7);
	lkdb_printf("  ar_csd      0x%lx\n", p->ar_csd);
	lkdb_printf("  ar_ssd      0x%lx\n", p->ar_ssd);
	lkdb_print_nameval("r8", p->r8);
	lkdb_print_nameval("r9", p->r9);
	lkdb_print_nameval("r10", p->r10);
	lkdb_print_nameval("r11", p->r11);
	lkdb_printf("  cr_ipsr     0x%lx\n", p->cr_ipsr);
	lkdb_print_nameval("cr_iip", p->cr_iip);
	lkdb_printf("  cr_ifs      0x%lx\n", p->cr_ifs);
	lkdb_printf("  ar_unat     0x%lx\n", p->ar_unat);
	lkdb_printf("  ar_pfs      0x%lx\n", p->ar_pfs);
	lkdb_printf("  ar_rsc      0x%lx\n", p->ar_rsc);
	lkdb_printf("  ar_rnat     0x%lx\n", p->ar_rnat);
	lkdb_printf("  ar_bspstore 0x%lx\n", p->ar_bspstore);
	lkdb_printf("  pr          0x%lx\n", p->pr);
	lkdb_print_nameval("b0", p->b0);
	lkdb_printf("  loadrs      0x%lx\n", p->loadrs);
	lkdb_print_nameval("r1", p->r1);
	lkdb_print_nameval("r12", p->r12);
	lkdb_print_nameval("r13", p->r13);
	lkdb_printf("  ar_fpsr     0x%lx\n", p->ar_fpsr);
	lkdb_print_nameval("r15", p->r15);
	lkdb_print_nameval("r14", p->r14);
	lkdb_print_nameval("r2", p->r2);
	lkdb_print_nameval("r3", p->r3);
	lkdb_print_nameval("r16", p->r16);
	lkdb_print_nameval("r17", p->r17);
	lkdb_print_nameval("r18", p->r18);
	lkdb_print_nameval("r19", p->r19);
	lkdb_print_nameval("r20", p->r20);
	lkdb_print_nameval("r21", p->r21);
	lkdb_print_nameval("r22", p->r22);
	lkdb_print_nameval("r23", p->r23);
	lkdb_print_nameval("r24", p->r24);
	lkdb_print_nameval("r25", p->r25);
	lkdb_print_nameval("r26", p->r26);
	lkdb_print_nameval("r27", p->r27);
	lkdb_print_nameval("r28", p->r28);
	lkdb_print_nameval("r29", p->r29);
	lkdb_print_nameval("r30", p->r30);
	lkdb_print_nameval("r31", p->r31);
	lkdb_printf("  ar_ccv      0x%lx\n", p->ar_ccv);
	lkdb_printf("  f6          0x%lx 0x%lx\n", p->f6.u.bits[0], p->f6.u.bits[1]);
	lkdb_printf("  f7          0x%lx 0x%lx\n", p->f7.u.bits[0], p->f7.u.bits[1]);
	lkdb_printf("  f8          0x%lx 0x%lx\n", p->f8.u.bits[0], p->f8.u.bits[1]);
	lkdb_printf("  f9          0x%lx 0x%lx\n", p->f9.u.bits[0], p->f9.u.bits[1]);
	lkdb_printf("  f10         0x%lx 0x%lx\n", p->f10.u.bits[0], p->f10.u.bits[1]);
	lkdb_printf("  f11         0x%lx 0x%lx\n", p->f11.u.bits[0], p->f11.u.bits[1]);

	return 0;
}

/*
 * kdba_stackdepth
 *
 *	Print processes that are using more than a specific percentage of their
 *	stack.
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
 *	If no percentage is supplied, it uses 60.
 */

static int
kdba_stackdepth(int argc, const char **argv)
{
	int diag, threshold, used;
	unsigned long percentage;
	unsigned long esp;
	long offset = 0;
	int nextarg;
	struct task_struct *p, *g;
	struct switch_stack *sw;

	if (argc == 0) {
		percentage = 60;
	} else if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &percentage, &offset, NULL);
		if (diag)
			return diag;
	} else {
		return LKDB_ARGCOUNT;
	}
	percentage = max_t(int, percentage, 1);
	percentage = min_t(int, percentage, 100);
	threshold = ((2 * THREAD_SIZE * percentage) / 100 + 1) >> 1;
	lkdb_printf("stackdepth: processes using more than %ld%% (%d bytes) of stack\n",
		percentage, threshold);
	lkdb_do_each_thread(g, p) {
		if (lkdb_task_has_cpu(p)) {
			struct lkdb_running_process *krp = lkdb_running_process + lkdb_process_cpu(p);
			if (krp->seqno)
				sw = krp->arch.sw;
			else
				sw = NULL;
		} else
			sw = (struct switch_stack *) (p->thread.ksp + 16);
		if (!sw)
			continue;
		esp = (unsigned long) sw;
		used = THREAD_SIZE - (esp - sw->ar_bspstore);
		if (used >= threshold) {
			lkdb_ps1(p);
			lkdb_printf("  esp %lx bsp %lx used %d\n", esp, sw->ar_bspstore, used);
		}
	} lkdb_while_each_thread(g, p);

	return 0;
}

/*
 * kdb_switch_stack
 *
 *	Format a struct switch_stack
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
 *	If no address is supplied, it uses lkdb_running_process[smp_processor_id()].arch.sw.
 */

static int
kdba_switch_stack(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset = 0;
	int nextarg;
	struct switch_stack *p;

	if (argc == 0) {
		addr = (kdb_machreg_t) lkdb_running_process[smp_processor_id()].arch.sw;
	} else if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;
	} else {
		return LKDB_ARGCOUNT;
	}

	p = (struct switch_stack *) addr;
	lkdb_printf("struct switch_stack %p-%p\n", p, (unsigned char *)p + sizeof(*p) - 1);
	lkdb_printf("  caller_unat 0x%lx\n", p->caller_unat);
	lkdb_printf("  ar_fpsr     0x%lx\n", p->ar_fpsr);
	lkdb_printf("  f2          0x%lx 0x%lx\n", p->f2.u.bits[0], p->f2.u.bits[1]);
	lkdb_printf("  f3          0x%lx 0x%lx\n", p->f3.u.bits[0], p->f3.u.bits[1]);
	lkdb_printf("  f4          0x%lx 0x%lx\n", p->f4.u.bits[0], p->f4.u.bits[1]);
	lkdb_printf("  f5          0x%lx 0x%lx\n", p->f5.u.bits[0], p->f5.u.bits[1]);
	lkdb_printf("  f12         0x%lx 0x%lx\n", p->f12.u.bits[0], p->f12.u.bits[1]);
	lkdb_printf("  f13         0x%lx 0x%lx\n", p->f13.u.bits[0], p->f13.u.bits[1]);
	lkdb_printf("  f14         0x%lx 0x%lx\n", p->f14.u.bits[0], p->f14.u.bits[1]);
	lkdb_printf("  f15         0x%lx 0x%lx\n", p->f15.u.bits[0], p->f15.u.bits[1]);
	lkdb_printf("  f16         0x%lx 0x%lx\n", p->f16.u.bits[0], p->f16.u.bits[1]);
	lkdb_printf("  f17         0x%lx 0x%lx\n", p->f17.u.bits[0], p->f17.u.bits[1]);
	lkdb_printf("  f18         0x%lx 0x%lx\n", p->f18.u.bits[0], p->f18.u.bits[1]);
	lkdb_printf("  f19         0x%lx 0x%lx\n", p->f19.u.bits[0], p->f19.u.bits[1]);
	lkdb_printf("  f20         0x%lx 0x%lx\n", p->f20.u.bits[0], p->f20.u.bits[1]);
	lkdb_printf("  f21         0x%lx 0x%lx\n", p->f21.u.bits[0], p->f21.u.bits[1]);
	lkdb_printf("  f22         0x%lx 0x%lx\n", p->f22.u.bits[0], p->f22.u.bits[1]);
	lkdb_printf("  f23         0x%lx 0x%lx\n", p->f23.u.bits[0], p->f23.u.bits[1]);
	lkdb_printf("  f24         0x%lx 0x%lx\n", p->f24.u.bits[0], p->f24.u.bits[1]);
	lkdb_printf("  f25         0x%lx 0x%lx\n", p->f25.u.bits[0], p->f25.u.bits[1]);
	lkdb_printf("  f26         0x%lx 0x%lx\n", p->f26.u.bits[0], p->f26.u.bits[1]);
	lkdb_printf("  f27         0x%lx 0x%lx\n", p->f27.u.bits[0], p->f27.u.bits[1]);
	lkdb_printf("  f28         0x%lx 0x%lx\n", p->f28.u.bits[0], p->f28.u.bits[1]);
	lkdb_printf("  f29         0x%lx 0x%lx\n", p->f29.u.bits[0], p->f29.u.bits[1]);
	lkdb_printf("  f30         0x%lx 0x%lx\n", p->f30.u.bits[0], p->f30.u.bits[1]);
	lkdb_printf("  f31         0x%lx 0x%lx\n", p->f31.u.bits[0], p->f31.u.bits[1]);
	lkdb_print_nameval("r4", p->r4);
	lkdb_print_nameval("r5", p->r5);
	lkdb_print_nameval("r6", p->r6);
	lkdb_print_nameval("r7", p->r7);
	lkdb_print_nameval("b0", p->b0);
	lkdb_print_nameval("b1", p->b1);
	lkdb_print_nameval("b2", p->b2);
	lkdb_print_nameval("b3", p->b3);
	lkdb_print_nameval("b4", p->b4);
	lkdb_print_nameval("b5", p->b5);
	lkdb_printf("  ar_pfs      0x%lx\n", p->ar_pfs);
	lkdb_printf("  ar_lc       0x%lx\n", p->ar_lc);
	lkdb_printf("  ar_unat     0x%lx\n", p->ar_unat);
	lkdb_printf("  ar_rnat     0x%lx\n", p->ar_rnat);
	lkdb_printf("  ar_bspstore 0x%lx\n", p->ar_bspstore);
	lkdb_printf("  pr          0x%lx\n", p->pr);

	return 0;
}

/*
 * kdb_minstate
 *
 *	Format the PAL minstate area.
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
 *	None.
 */

static int
kdba_minstate(int argc, const char **argv)
{
	int diag;
	kdb_machreg_t addr;
	long offset = 0;
	int nextarg;
	pal_min_state_area_t *p;

	if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;
	} else {
		return LKDB_ARGCOUNT;
	}

	p = (pal_min_state_area_t *) addr;
	lkdb_printf("PAL minstate %p-%p\n", p, (unsigned char *)p + sizeof(*p) - 1);
	lkdb_printf("  pmsa_nat_bits 0x%llx\n", p->pmsa_nat_bits);
	lkdb_print_nameval("r1", p->pmsa_gr[1-1]);
	lkdb_print_nameval("r2", p->pmsa_gr[2-1]);
	lkdb_print_nameval("r3", p->pmsa_gr[3-1]);
	lkdb_print_nameval("r4", p->pmsa_gr[4-1]);
	lkdb_print_nameval("r5", p->pmsa_gr[5-1]);
	lkdb_print_nameval("r6", p->pmsa_gr[6-1]);
	lkdb_print_nameval("r7", p->pmsa_gr[7-1]);
	lkdb_print_nameval("r8", p->pmsa_gr[8-1]);
	lkdb_print_nameval("r9", p->pmsa_gr[9-1]);
	lkdb_print_nameval("r10", p->pmsa_gr[10-1]);
	lkdb_print_nameval("r11", p->pmsa_gr[11-1]);
	lkdb_print_nameval("r12", p->pmsa_gr[12-1]);
	lkdb_print_nameval("r13", p->pmsa_gr[13-1]);
	lkdb_print_nameval("r14", p->pmsa_gr[14-1]);
	lkdb_print_nameval("r15", p->pmsa_gr[15-1]);
	lkdb_printf("  Bank 0\n");
	lkdb_print_nameval("r16", p->pmsa_bank0_gr[16-16]);
	lkdb_print_nameval("r17", p->pmsa_bank0_gr[17-16]);
	lkdb_print_nameval("r18", p->pmsa_bank0_gr[18-16]);
	lkdb_print_nameval("r19", p->pmsa_bank0_gr[19-16]);
	lkdb_print_nameval("r20", p->pmsa_bank0_gr[20-16]);
	lkdb_print_nameval("r21", p->pmsa_bank0_gr[21-16]);
	lkdb_print_nameval("r22", p->pmsa_bank0_gr[22-16]);
	lkdb_print_nameval("r23", p->pmsa_bank0_gr[23-16]);
	lkdb_print_nameval("r24", p->pmsa_bank0_gr[24-16]);
	lkdb_print_nameval("r25", p->pmsa_bank0_gr[25-16]);
	lkdb_print_nameval("r26", p->pmsa_bank0_gr[26-16]);
	lkdb_print_nameval("r27", p->pmsa_bank0_gr[27-16]);
	lkdb_print_nameval("r28", p->pmsa_bank0_gr[28-16]);
	lkdb_print_nameval("r29", p->pmsa_bank0_gr[29-16]);
	lkdb_print_nameval("r30", p->pmsa_bank0_gr[30-16]);
	lkdb_print_nameval("r31", p->pmsa_bank0_gr[31-16]);
	lkdb_printf("  Bank 1\n");
	lkdb_print_nameval("r16", p->pmsa_bank1_gr[16-16]);
	lkdb_print_nameval("r17", p->pmsa_bank1_gr[17-16]);
	lkdb_print_nameval("r18", p->pmsa_bank1_gr[18-16]);
	lkdb_print_nameval("r19", p->pmsa_bank1_gr[19-16]);
	lkdb_print_nameval("r20", p->pmsa_bank1_gr[20-16]);
	lkdb_print_nameval("r21", p->pmsa_bank1_gr[21-16]);
	lkdb_print_nameval("r22", p->pmsa_bank1_gr[22-16]);
	lkdb_print_nameval("r23", p->pmsa_bank1_gr[23-16]);
	lkdb_print_nameval("r24", p->pmsa_bank1_gr[24-16]);
	lkdb_print_nameval("r25", p->pmsa_bank1_gr[25-16]);
	lkdb_print_nameval("r26", p->pmsa_bank1_gr[26-16]);
	lkdb_print_nameval("r27", p->pmsa_bank1_gr[27-16]);
	lkdb_print_nameval("r28", p->pmsa_bank1_gr[28-16]);
	lkdb_print_nameval("r29", p->pmsa_bank1_gr[29-16]);
	lkdb_print_nameval("r30", p->pmsa_bank1_gr[30-16]);
	lkdb_print_nameval("r31", p->pmsa_bank1_gr[31-16]);
	lkdb_printf("  pr          0x%llx\n", p->pmsa_pr);
	lkdb_print_nameval("b0", p->pmsa_br0);
	lkdb_printf("  ar.rsc      0x%llx\n", p->pmsa_rsc);
	lkdb_print_nameval("cr.iip", p->pmsa_iip);
	lkdb_printf("  cr.ipsr     0x%llx\n", p->pmsa_ipsr);
	lkdb_printf("  cr.ifs      0x%llx\n", p->pmsa_ifs);
	lkdb_print_nameval("cr.xip", p->pmsa_xip);
	lkdb_printf("  cr.xpsr     0x%llx\n", p->pmsa_xpsr);
	lkdb_printf("  cr.xfs      0x%llx\n", p->pmsa_xfs);
	lkdb_print_nameval("b1", p->pmsa_br1);

	return 0;
}

/*
 * kdba_cpuinfo
 *
 *	Format struct cpuinfo_ia64.
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
 *	If no cpu is supplied, it prints cpuinfo for all online cpus.
 */

static int
kdba_cpuinfo(int argc, const char **argv)
{
	int diag;
	unsigned long cpunum = -1;
	long offset = 0;
	int nextarg, c, i;
	struct cpuinfo_ia64 *cpuinfo;

	if (argc == 1) {
		nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &cpunum, &offset, NULL);
		if (diag)
			return diag;
		if (cpunum >= NR_CPUS || !cpu_online(cpunum))
			return LKDB_BADCPUNUM;
	} else if (argc > 1) {
		return LKDB_ARGCOUNT;
	}

	for (c = (cpunum == -1 ? 0 : cpunum);
	     c < (cpunum == -1 ? NR_CPUS : cpunum+1);
	     ++c) {
		if (!cpu_online(c))
			continue;
		cpuinfo = cpu_data(c);
		lkdb_printf("struct cpuinfo_ia64 for cpu %d is at 0x%p\n", c, cpuinfo);
		lkdb_printf("  softirq_pending  0x%x\n",   cpuinfo->softirq_pending);
		lkdb_printf("  itm_delta        %ld\n",    cpuinfo->itm_delta);
		lkdb_printf("  itm_next         %ld\n",    cpuinfo->itm_next);
		lkdb_printf("  nsec_per_cyc     %ld\n",    cpuinfo->nsec_per_cyc);
		lkdb_printf("  unimpl_va_mask   0x%lx\n",  cpuinfo->unimpl_va_mask);
		lkdb_printf("  unimpl_pa_mask   0x%lx\n",  cpuinfo->unimpl_pa_mask);
		lkdb_printf("  itc_freq         %ld\n",    cpuinfo->itc_freq);
		lkdb_printf("  proc_freq        %ld\n",    cpuinfo->proc_freq);
		lkdb_printf("  cyc_per_usec     %ld\n",    cpuinfo->cyc_per_usec);
		lkdb_printf("  cyc_per_usec     %ld\n",    cpuinfo->cyc_per_usec);
#if 0	/* RJA per-cpu MCA */
		lkdb_printf("  percpu_paddr     0x%lx\n",  cpuinfo->percpu_paddr);
#endif
		lkdb_printf("  ptce_base        0x%lx\n",  cpuinfo->ptce_base);
		lkdb_printf("  ptce_count       %d %d\n",  cpuinfo->ptce_count[0], cpuinfo->ptce_count[1]);
		lkdb_printf("  ptce_stride      %d %d\n",  cpuinfo->ptce_stride[0], cpuinfo->ptce_stride[1]);
#if 0	/* RJA per-cpu MCA */
		lkdb_printf("  pal_paddr        0x%lx\n",  cpuinfo->pal_paddr);
		lkdb_printf("  pal_base         0x%lx\n",  cpuinfo->pal_base);
#endif
		lkdb_printf("  ksoftirqd        0x%p\n",   cpuinfo->ksoftirqd);
#ifdef CONFIG_SMP
		lkdb_printf("  loops_per_jiffy  %ld\n",    cpuinfo->loops_per_jiffy);
		lkdb_printf("  cpu              %d\n",     cpuinfo->cpu);
		lkdb_printf("  socket_id        %d\n",     cpuinfo->socket_id);
		lkdb_printf("  core_id          %d\n",     cpuinfo->core_id);
		lkdb_printf("  thread_id        %d\n",     cpuinfo->thread_id);
		lkdb_printf("  num_log          %d\n",     cpuinfo->num_log);
		lkdb_printf("  cores_per_socket %d\n",     cpuinfo->cores_per_socket);
		lkdb_printf("  threads_per_core %d\n",     cpuinfo->threads_per_core);
#endif
		lkdb_printf("  ppn              0x%lx\n",  cpuinfo->ppn);
		lkdb_printf("  features         0x%lx\n",  cpuinfo->features);
		lkdb_printf("  number           %d\n",     cpuinfo->number);
		lkdb_printf("  revision         %d\n",     cpuinfo->revision);
		lkdb_printf("  model            %d\n",     cpuinfo->model);
		lkdb_printf("  family           %d\n",     cpuinfo->family);
		lkdb_printf("  archrev          %d\n",     cpuinfo->archrev);
		lkdb_printf("  vendor          ");
		for (i = 0; i < sizeof(cpuinfo->vendor); ++i)
			lkdb_printf(" 0x%02x", cpuinfo->vendor[i]);
		lkdb_printf("\n");
#ifdef CONFIG_NUMA
		lkdb_printf("  node_data        0x%p\n",   cpuinfo->node_data);
#endif
#if 0	/* RJA per-cpu MCA */
		lkdb_printf("  ia64_pa_mca_data 0x%p\n",   cpuinfo->ia64_pa_mca_data);
#endif
	}
	return 0;
}

#ifdef CONFIG_KDB_HARDWARE_BREAKPOINTS
void
kdba_installdbreg(lkdb_bp_t *bp)
{
	unsigned long mask;
	unsigned int regbase;
	static unsigned long masks[] = {
	    0x00FFFFFFFFFFFFFFUL,   // 1 byte long
	    0x00FFFFFFFFFFFFFEUL,   // 2 bytes long
	    0x0000000000000000UL,   // invalid
	    0x00FFFFFFFFFFFFFCUL    // 4 bytes long
	};
	static unsigned char modes[] = {
	    0x81,		    // instruction => x, plm=priv level 0 only
	    0x41,		    // write => w, plm=priv level 0 only
	    0x00,		    // io
	    0x81		    // read => r, plm=priv level 0 only
	};

	/* Note that bp->bp_hard[NR_CPU] is for x86.
	 * The ia64 uses bp->bp_hard[0] only.
	 */
	if (KDB_DEBUG(BP))
		lkdb_printf("kdba_installdbreg:\n");
	mask = masks[bp->bp_hard[0]->bph_length] |
	       (((unsigned long)(modes[bp->bp_hard[0]->bph_mode])) << 56);
	regbase = 2*bp->bp_hard[0]->bph_reg;

	switch (bp->bp_hard[0]->bph_mode)
	{
	case 1:
	case 3:
		if (KDB_DEBUG(BP)) {
			lkdb_printf("kdba_installdbreg: dbr[%u]=%016lx\n",
					regbase, bp->bp_addr);
			lkdb_printf("kdba_installdbreg: dbr[%u]=%016lx\n",
					regbase+1, mask);
		}

		ia64_set_dbr(regbase, bp->bp_addr);
		ia64_set_dbr(regbase+1, mask);
		ia64_srlz_d();
		break;

	case 0:	/* instruction */
#if 0
		ia64_set_ibr(regbase, bp->bp_addr);
		ia64_set_ibr(regbase+1, mask);
		ia64_srlz_d();
#else
		lkdb_printf("\"instr\" mode not implemented\n");
#endif
		break;

	case 2:	/* io */
		lkdb_printf("\"io\" mode not implemented\n");
		break;
	}
}

void
kdba_removedbreg(lkdb_bp_t *bp)
{
	unsigned int regbase = 2*bp->bp_hard[0]->bph_reg;

	/* Note that bp->bp_hard[NR_CPU] is for x86.
	 * The ia64 uses bp->bp_hard[0] only.
	 */
	switch (bp->bp_hard[0]->bph_mode)
	{
	case 1:
	case 3:
		ia64_set_dbr(regbase, 0);
		ia64_set_dbr(regbase+1, 0);
		ia64_srlz_d();
		break;

	case 0:	/* instruction */
#if 0
		ia64_set_ibr(regbase, 0);
		ia64_set_ibr(regbase+1, 0);
		ia64_srlz_d();
#else
		lkdb_printf("\"instr\" mode not implemented\n");
#endif
		break;

	case 2:	/* io */
		lkdb_printf("\"io\" mode not implemented\n");
		break;
	}
}
#endif /* CONFIG_KDB_HARDWARE_BREAKPOINTS */


static kdb_machreg_t
kdba_getdr(int regnum)
{
	kdb_machreg_t contents = 0;
	unsigned long reg = (unsigned long)regnum;

	__asm__ ("mov %0=ibr[%1]"::"r"(contents),"r"(reg));
//        __asm__ ("mov ibr[%0]=%1"::"r"(dbreg_cond),"r"(value));

	return contents;
}


static void
get_fault_regs(fault_regs_t *fr)
{
	fr->ifa = 0 ;
	fr->isr = 0 ;

	__asm__ ("rsm psr.ic;;") ;
	ia64_srlz_d();
	__asm__ ("mov %0=cr.ifa" : "=r"(fr->ifa));
	__asm__ ("mov %0=cr.isr" : "=r"(fr->isr));
	__asm__ ("ssm psr.ic;;") ;
	ia64_srlz_d();
}

static void
show_kernel_regs (void)
{
	unsigned long kr[8];
	int i;

	asm ("mov %0=ar.k0" : "=r"(kr[0])); asm ("mov %0=ar.k1" : "=r"(kr[1]));
	asm ("mov %0=ar.k2" : "=r"(kr[2])); asm ("mov %0=ar.k3" : "=r"(kr[3]));
	asm ("mov %0=ar.k4" : "=r"(kr[4])); asm ("mov %0=ar.k5" : "=r"(kr[5]));
	asm ("mov %0=ar.k6" : "=r"(kr[6])); asm ("mov %0=ar.k7" : "=r"(kr[7]));

	for (i = 0; i < 4; ++i)
		lkdb_printf(" kr%d: %016lx  kr%d: %016lx\n", 2*i, kr[2*i], 2*i+1, kr[2*i+1]);
	lkdb_printf("\n");
}

static int
change_cur_stack_frame(int regno, unsigned long *contents)
{
	unsigned long sof, i, cfm, sp, *bsp, __user *ubsp;
	struct unw_frame_info info;
	mm_segment_t old_fs;
	int cpu = lkdb_process_cpu(lkdb_current_task);
	struct lkdb_running_process *krp = lkdb_running_process + cpu;

	if (lkdb_current_task != krp->p) {
		lkdb_printf("Stacked registers are not available for tasks that are not running.\n");
		lkdb_printf("Use bt with a large BTARGS value instead\n");
		return 0;
	}
	unw_init_frame_info(&info, krp->p, krp->arch.sw);
	do {
		if (unw_unwind(&info) < 0) {
			lkdb_printf("Failed to unwind\n");
			return 0;
		}
		unw_get_sp(&info, &sp);
	} while (sp <= (unsigned long) lkdb_current_regs);
	unw_get_bsp(&info, (unsigned long *) &bsp);
	unw_get_cfm(&info, &cfm);

	if (!bsp) {
		lkdb_printf("Unable to get Current Stack Frame\n");
		return 0;
	}

	sof = (cfm & 0x7f);

	if(((unsigned long)regno - 32) >= (sof - 2)) return 1;

	old_fs = set_fs(KERNEL_DS);
	for (i = 0; i < (regno - 32); ++i)
		bsp = ia64_rse_skip_regs(bsp, 1);
	ubsp = (unsigned long __user *) bsp;
	put_user(*contents, ubsp);
	set_fs(old_fs);

	return 0 ;
}

static int
show_cur_stack_frame(int regno, unsigned long *contents)
{
	unsigned long sof, i, cfm, val, sp, *bsp, __user *ubsp;
	struct unw_frame_info info;
	mm_segment_t old_fs;
	int cpu = lkdb_process_cpu(lkdb_current_task);
	struct lkdb_running_process *krp = lkdb_running_process + cpu;

	if (lkdb_current_task != krp->p) {
		lkdb_printf("Stacked registers are not available for tasks that are not running.\n");
		lkdb_printf("Use bt with a large BTARGS value instead\n");
		return 0;
	}
	unw_init_frame_info(&info, krp->p, krp->arch.sw);
	do {
		if (unw_unwind(&info) < 0) {
			lkdb_printf("Failed to unwind\n");
			return 0;
		}
		unw_get_sp(&info, &sp);
	} while (sp <= (unsigned long) lkdb_current_regs);
	unw_get_bsp(&info, (unsigned long *) &bsp);
	unw_get_cfm(&info, &cfm);

	if (!bsp) {
		lkdb_printf("Unable to display Current Stack Frame\n");
		return 0;
	}

	sof = (cfm & 0x7f);

	if (regno) {
		if ((unsigned) regno - 32 >= sof)
			return 0;
		bsp = ia64_rse_skip_regs(bsp, regno - 32);
		old_fs = set_fs(KERNEL_DS);
		ubsp = (unsigned long __user *) bsp;
		get_user(val, ubsp);
		set_fs(old_fs);
		*contents = val;
		return 1;
	}

	old_fs = set_fs(KERNEL_DS);
	for (i = 0; i < sof; ++i) {
		ubsp = (unsigned long __user *) bsp;
		get_user(val, ubsp);
		lkdb_printf(" r%lu: %016lx ", 32 + i, val);
		if (!((i + 1) % 3))
			lkdb_printf("\n");
		bsp = ia64_rse_skip_regs(bsp, 1);
	}
	lkdb_printf("\n");
	set_fs(old_fs);

	return 0 ;
}

/*
 * kdba_getregcontents
 *
 *	Return the contents of the register specified by the
 *	input string argument.   Return an error if the string
 *	does not match a machine register.
 *
 *	The following pseudo register names are supported:
 *	   &regs	 - Prints address of exception frame
 *	   kesp		 - Prints kernel stack pointer at time of fault
 * 	   sstk		 - Prints switch stack for ia64
 *	   %<regname>	 - Uses the value of the registers at the
 *			   last time the user process entered kernel
 *			   mode, instead of the registers at the time
 *			   kdb was entered.
 *
 * Parameters:
 *	regname		Pointer to string naming register
 *	regs		Pointer to structure containing registers.
 * Outputs:
 *	*contents	Pointer to unsigned long to recieve register contents
 * Returns:
 *	0		Success
 *	LKDB_BADREG	Invalid register name
 * Locking:
 * 	None.
 * Remarks:
 *
 * 	Note that this function is really machine independent.   The kdb
 *	register list is not, however.
 */

static struct kdbregs {
	char   *reg_name;
	size_t	reg_offset;
} kdbreglist[] = {
	{ "psr",	offsetof(struct pt_regs, cr_ipsr) },
	{ "ifs",	offsetof(struct pt_regs, cr_ifs) },
	{ "ip",	offsetof(struct pt_regs, cr_iip) },

	{ "unat", 	offsetof(struct pt_regs, ar_unat) },
	{ "pfs",	offsetof(struct pt_regs, ar_pfs) },
	{ "rsc", 	offsetof(struct pt_regs, ar_rsc) },

	{ "rnat",	offsetof(struct pt_regs, ar_rnat) },
	{ "bsps",	offsetof(struct pt_regs, ar_bspstore) },
	{ "pr",	offsetof(struct pt_regs, pr) },

	{ "ldrs",	offsetof(struct pt_regs, loadrs) },
	{ "ccv",	offsetof(struct pt_regs, ar_ccv) },
	{ "fpsr",	offsetof(struct pt_regs, ar_fpsr) },

	{ "b0",	offsetof(struct pt_regs, b0) },
	{ "b6",	offsetof(struct pt_regs, b6) },
	{ "b7",	offsetof(struct pt_regs, b7) },

	{ "r1",offsetof(struct pt_regs, r1) },
	{ "r2",offsetof(struct pt_regs, r2) },
	{ "r3",offsetof(struct pt_regs, r3) },

	{ "r8",offsetof(struct pt_regs, r8) },
	{ "r9",offsetof(struct pt_regs, r9) },
	{ "r10",offsetof(struct pt_regs, r10) },

	{ "r11",offsetof(struct pt_regs, r11) },
	{ "r12",offsetof(struct pt_regs, r12) },
	{ "r13",offsetof(struct pt_regs, r13) },

	{ "r14",offsetof(struct pt_regs, r14) },
	{ "r15",offsetof(struct pt_regs, r15) },
	{ "r16",offsetof(struct pt_regs, r16) },

	{ "r17",offsetof(struct pt_regs, r17) },
	{ "r18",offsetof(struct pt_regs, r18) },
	{ "r19",offsetof(struct pt_regs, r19) },

	{ "r20",offsetof(struct pt_regs, r20) },
	{ "r21",offsetof(struct pt_regs, r21) },
	{ "r22",offsetof(struct pt_regs, r22) },

	{ "r23",offsetof(struct pt_regs, r23) },
	{ "r24",offsetof(struct pt_regs, r24) },
	{ "r25",offsetof(struct pt_regs, r25) },

	{ "r26",offsetof(struct pt_regs, r26) },
	{ "r27",offsetof(struct pt_regs, r27) },
	{ "r28",offsetof(struct pt_regs, r28) },

	{ "r29",offsetof(struct pt_regs, r29) },
	{ "r30",offsetof(struct pt_regs, r30) },
	{ "r31",offsetof(struct pt_regs, r31) },

};

static const int nkdbreglist = sizeof(kdbreglist) / sizeof(struct kdbregs);

int
kdba_getregcontents(const char *regname, struct pt_regs *regs, unsigned long *contents)
{
	int i;

	if (strcmp(regname, "isr") == 0) {
		fault_regs_t fr ;
		get_fault_regs(&fr) ;
		*contents = fr.isr ;
		return 0 ;
	}

	if (!regs) {
		lkdb_printf("%s: pt_regs not available, use bt* or pid to select a different task\n", __FUNCTION__);
		return LKDB_BADREG;
	}

	if (strcmp(regname, "&regs") == 0) {
		*contents = (unsigned long)regs;
		return 0;
	}

	if (strcmp(regname, "sstk") == 0) {
		*contents = (unsigned long)getprsregs(regs) ;
		return 0;
	}

	if (strcmp(regname, "ksp") == 0) {
		*contents = (unsigned long) (regs + 1);
		return 0;
	}

	for (i=0; i<nkdbreglist; i++) {
		if (strstr(kdbreglist[i].reg_name, regname))
			break;
	}

	if (i == nkdbreglist) {
		/* Lets check the rse maybe */
		if (regname[0] == 'r')
			if (show_cur_stack_frame(simple_strtoul(regname+1, NULL, 0), contents))
				return 0 ;
		return LKDB_BADREG;
	}

	*contents = *(unsigned long *)((unsigned long)regs +
			kdbreglist[i].reg_offset);

	return 0;
}

/*
 * kdba_setregcontents
 *
 *	Set the contents of the register specified by the
 *	input string argument.   Return an error if the string
 *	does not match a machine register.
 *
 *	Supports modification of user-mode registers via
 *	%<register-name>
 *
 * Parameters:
 *	regname		Pointer to string naming register
 *	regs		Pointer to structure containing registers.
 *	contents	Unsigned long containing new register contents
 * Outputs:
 * Returns:
 *	0		Success
 *	LKDB_BADREG	Invalid register name
 * Locking:
 * 	None.
 * Remarks:
 */

int
kdba_setregcontents(const char *regname,
		  struct pt_regs *regs,
		  unsigned long contents)
{
	int i, ret = 0, fixed = 0;
	char *endp;
	unsigned long regno;

	if (regname[0] == '%') {
		regname++;
		regs = (struct pt_regs *)
			(lkdb_current_task->thread.ksp - sizeof(struct pt_regs));
	}

	if (!regs) {
		lkdb_printf("%s: pt_regs not available, use bt* or pid to select a different task\n", __FUNCTION__);
		return LKDB_BADREG;
	}

	/* fixed registers */
	for (i=0; i<nkdbreglist; i++) {
		if (strnicmp(kdbreglist[i].reg_name,
			     regname,
			     strlen(regname)) == 0) {
			fixed = 1;
			break;
		}
	}

	/* stacked registers */
	if (!fixed) {
		regno = (simple_strtoul(&regname[1], &endp, 10));
		if ((regname[0] == 'r') && regno > (unsigned long)31) {
			ret = change_cur_stack_frame(regno, &contents);
			if(!ret) return 0;
		}
	}

	if ((i == nkdbreglist)
	    || (strlen(kdbreglist[i].reg_name) != strlen(regname))
	    || ret) {
		return LKDB_BADREG;
	}

	/* just in case of "standard" register */
	*(unsigned long *)((unsigned long)regs + kdbreglist[i].reg_offset) =
		contents;

	return 0;
}

/*
 * kdba_dumpregs
 *
 *	Dump the specified register set to the display.
 *
 * Parameters:
 *	regs		Pointer to structure containing registers.
 *	type		Character string identifying register set to dump
 *	extra		string further identifying register (optional)
 * Outputs:
 * Returns:
 *	0		Success
 * Locking:
 * 	None.
 * Remarks:
 *	This function will dump the general register set if the type
 *	argument is NULL (struct pt_regs).   The alternate register
 *	set types supported by this function:
 *
 *	d 		Debug registers
 *	c		Control registers
 *	u		User registers at most recent entry to kernel
 *	i		Interrupt registers -- same as "irr" command
 * Following not yet implemented:
 *	m		Model Specific Registers (extra defines register #)
 *	r		Memory Type Range Registers (extra defines register)
 *
 *	For now, all registers are covered as follows:
 *
 * 	rd 		- dumps all regs
 *	rd	%isr	- current interrupt status reg, read freshly
 *	rd	s	- valid stacked regs
 * 	rd 	%sstk	- gets switch stack addr. dump memory and search
 *	rd	d	- debug regs, may not be too useful
 *	rd	k	- dump kernel regs
 *
 *	ARs		TB Done
 *	OTHERS		TB Decided ??
 *
 *	Intel wish list
 *	These will be implemented later - Srinivasa
 *
 *      type        action
 *      ----        ------
 *      g           dump all General static registers
 *      s           dump all general Stacked registers
 *      f           dump all Floating Point registers
 *      p           dump all Predicate registers
 *      b           dump all Branch registers
 *      a           dump all Application registers
 *      c           dump all Control registers
 *
 */

int
kdba_dumpregs(struct pt_regs *regs,
	    const char *type,
	    const char *extra)

{
	int i;
	int count = 0;

	if (type
	 && (type[0] == 'u')) {
		type = NULL;
		regs = (struct pt_regs *)
			(lkdb_current_task->thread.ksp - sizeof(struct pt_regs));
	}

	if (type == NULL) {
		if (!regs) {
			lkdb_printf("%s: pt_regs not available, use bt* or pid to select a different task\n", __FUNCTION__);
			return LKDB_BADREG;
		}
		for (i=0; i<nkdbreglist; i++) {
			lkdb_printf("%4s: 0x%16.16lx  ",
				   kdbreglist[i].reg_name,
				   *(unsigned long *)((unsigned long)regs +
						  kdbreglist[i].reg_offset));

			if ((++count % 3) == 0)
				lkdb_printf("\n");
		}

		lkdb_printf("&regs = %p\n", (void *)regs);

		return 0;
	}

	switch (type[0]) {
	case 'd':
	{
		for(i=0; i<8; i+=2) {
			lkdb_printf("idr%d: 0x%16.16lx  idr%d: 0x%16.16lx\n", i,
					kdba_getdr(i), i+1, kdba_getdr(i+1));

		}
		return 0;
	}
	case 'i':
		kdba_show_intregs();
		break;
	case 'k':
		show_kernel_regs();
		break;
	case 'm':
		break;
	case 'r':
		break;

	case 's':
	{
		if (!regs) {
			lkdb_printf("%s: pt_regs not available, use bt* or pid to select a different task\n", __FUNCTION__);
			return LKDB_BADREG;
		}
		show_cur_stack_frame(0, NULL) ;

		return 0 ;
	}

	case '%':
	{
		unsigned long contents ;

		if (!kdba_getregcontents(type+1, regs, &contents))
			lkdb_printf("%s = 0x%16.16lx\n", type+1, contents) ;
		else
			lkdb_printf("diag: Invalid register %s\n", type+1) ;

		return 0 ;
	}

	default:
		return LKDB_BADREG;
	}

	/* NOTREACHED */
	return 0;
}
EXPORT_SYMBOL(kdba_dumpregs);

kdb_machreg_t
kdba_getpc(struct pt_regs *regs)
{
	return regs ? regs->cr_iip + ia64_psr(regs)->ri : 0;
}

int
kdba_setpc(struct pt_regs *regs, kdb_machreg_t newpc)
{
	if (KDB_NULL_REGS(regs))
		return LKDB_BADREG;
	regs->cr_iip = newpc & ~0xf;
	ia64_psr(regs)->ri = newpc & 0x3;
	KDB_STATE_SET(IP_ADJUSTED);
	return 0;
}

struct kdba_main_loop_data {
	lkdb_reason_t reason;
	lkdb_reason_t reason2;
	int error;
	kdb_dbtrap_t db_result;
	struct pt_regs *regs;
	int ret;
};

/*
 * do_kdba_main_loop
 *
 *	Invoked from kdba_main_loop via unw_init_running() after that routine
 *	has pushed a struct switch_stack.
 *
 * Inputs:
 *	info	Unwind information.
 *	data	kdb data passed as void * to unw_init_running.
 * Returns:
 *	none (unw_init_running requires void).  vdata->ret is set to
 *	0	KDB was invoked for an event which it wasn't responsible
 *	1	KDB handled the event for which it was invoked.
 * Outputs:
 *	none
 * Locking:
 *	None.
 * Remarks:
 *	unw_init_running() creates struct switch_stack then struct
 *	unw_frame_info.  We get the address of the info so step over
 *	that to get switch_stack.  Just hope that unw_init_running
 *	does not change its stack usage.  unw_init_running adds padding
 *	to put switch_stack on a 16 byte boundary.
 */

static void
do_kdba_main_loop(struct unw_frame_info *info, void *vdata)
{
	struct kdba_main_loop_data *data = vdata;
	struct switch_stack *sw, *prev_sw;
	struct pt_regs *prev_regs;
	struct lkdb_running_process *krp =
		lkdb_running_process + smp_processor_id();
	KDB_DEBUG_STATE(__FUNCTION__, data->reason);
	prev_sw = krp->arch.sw;
	sw = (struct switch_stack *)(info+1);
	/* padding from unw_init_running */
	sw = (struct switch_stack *)(((unsigned long)sw + 15) & ~15);
	krp->arch.sw = sw;
	prev_regs = krp->regs;
	data->ret = kdb_save_running(data->regs, data->reason, data->reason2,
			data->error, data->db_result);
	kdb_unsave_running(data->regs);
	krp->regs = prev_regs;
	krp->arch.sw = prev_sw;
}

/*
 * kdba_main_loop
 *
 *	Do any architecture specific set up before entering the main kdb loop.
 *	The primary function of this routine is to make all processes look the
 *	same to kdb, kdb must be able to list a process without worrying if the
 *	process is running or blocked, so make all processes look as though they
 *	are blocked.
 *
 * Inputs:
 *	reason		The reason KDB was invoked
 *	error		The hardware-defined error code
 *	error2		kdb's current reason code.  Initially error but can change
 *			acording to kdb state.
 *	db_result	Result from break or debug point.
 *	regs		The exception frame at time of fault/breakpoint.  If reason
 *			is SILENT or CPU_UP then regs is NULL, otherwise it should
 *			always be valid.
 * Returns:
 *	0	KDB was invoked for an event which it wasn't responsible
 *	1	KDB handled the event for which it was invoked.
 * Outputs:
 *	Builds a switch_stack structure before calling the main loop.
 * Locking:
 *	None.
 * Remarks:
 *	none.
 */

int
kdba_main_loop(lkdb_reason_t reason, lkdb_reason_t reason2, int error,
	       kdb_dbtrap_t db_result, struct pt_regs *regs)
{
	struct kdba_main_loop_data data;
	KDB_DEBUG_STATE("kdba_main_loop", reason);
	data.reason = reason;
	data.reason2 = reason2;
	data.error = error;
	data.db_result = db_result;
	data.regs = regs;
	unw_init_running(do_kdba_main_loop, &data);
	return(data.ret);
}

void
kdba_disableint(lkdb_intstate_t *state)
{
	unsigned long *fp = (unsigned long *)state;
	unsigned long flags;

	local_irq_save(flags);
	*fp = flags;
}

void
kdba_restoreint(lkdb_intstate_t *state)
{
	unsigned long flags = *(unsigned long *)state;
	local_irq_restore(flags);
}

void
kdba_setsinglestep(struct pt_regs *regs)
{
	if (KDB_NULL_REGS(regs))
		return;
	ia64_psr(regs)->ss = 1;
}

void
kdba_clearsinglestep(struct pt_regs *regs)
{
	if (KDB_NULL_REGS(regs))
		return;
	ia64_psr(regs)->ss = 0;
}

/*
 * kdb_tpa
 *
 * 	Virtual to Physical address translation command.
 *
 *	tpa  <addr>
 *
 * Parameters:
 *	argc	Count of arguments in argv
 *	argv	Space delimited command line arguments
 * Outputs:
 *	None.
 * Returns:
 *	Zero for success, a kdb diagnostic if failure.
 * Locking:
 *	None.
 * Remarks:
 */
#define __xtpa(x)		({ia64_va _v; asm("tpa %0=%1" : "=r"(_v.l) : "r"(x)); _v.l;})
static int
kdba_tpa(int argc, const char **argv)
{
	kdb_machreg_t addr;
	int diag;
	long offset = 0;
	int nextarg;
	char c;

	nextarg = 1;
	if (argc != 1)
		return LKDB_ARGCOUNT;
	diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
	if (diag)
		return diag;
	if (lkdb_getarea(c, addr))
		return(0);
	lkdb_printf("vaddr: 0x%lx , paddr: 0x%lx\n", addr, __xtpa(addr));
	return(0);
}
#if defined(CONFIG_NUMA)
static int
kdba_tpav(int argc, const char **argv)
{
	kdb_machreg_t addr, end, paddr;
	int diag;
	long offset = 0;
	int nextarg, nid, nid_old;
	char c;

	nextarg = 1;
	if (argc != 2)
		return LKDB_ARGCOUNT;
	diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
	if (diag)
		return diag;
	diag = lkdbgetaddrarg(argc, argv, &nextarg, &end, &offset, NULL);
	if (diag)
		return diag;
	if (lkdb_getarea(c, addr))
		return(0);
	if (lkdb_getarea(c, end))
		return(0);
	paddr=__xtpa(addr);
	nid = paddr_to_nid(paddr);
	lkdb_printf("begin: 0x%lx , paddr: 0x%lx , nid: %d\n", addr, __xtpa(addr), nid);
	for(;addr<end; addr += PAGE_SIZE) {
		nid_old=nid;
		paddr =__xtpa(addr);
		nid = paddr_to_nid(paddr);
		if (nid != nid_old)
			lkdb_printf("NOT on same NODE: addr: 0x%lx , paddr: 0x%lx , nid: %d\n", addr, paddr, nid);
	}
	paddr=__xtpa(end);
	nid=paddr_to_nid(end);
	lkdb_printf("end: 0x%lx , paddr: 0x%lx , nid: %d\n", end, paddr, nid);
	return(0);
}
#endif

#if defined(CONFIG_SMP)
/*
 * kdba_sendinit
 *
 *      This function implements the 'init' command.
 *
 *      init    [<cpunum>]
 *
 * Inputs:
 *      argc    argument count
 *      argv    argument vector
 * Outputs:
 *      None.
 * Returns:
 *      zero for success, a kdb diagnostic if error
 * Locking:
 *      none.
 * Remarks:
 */

static int
kdba_sendinit(int argc, const char **argv)
{
	unsigned long cpunum;
	int diag;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	diag = lkdbgetularg(argv[1], &cpunum);
	if (diag)
		return diag;

	if (cpunum >= NR_CPUS || !cpu_online(cpunum))
		return LKDB_BADCPUNUM;

	platform_send_ipi(cpunum, 0, IA64_IPI_DM_INIT, 0);
	return 0;
}

/* Invoked once from kdb_wait_for_cpus when waiting for cpus.  For those cpus
 * that have not responded to the normal KDB interrupt yet, hit them with an
 * INIT event.
 */
void
kdba_wait_for_cpus(void)
{
	int c;
	if (LKDB_FLAG(CATASTROPHIC))
		return;
	lkdb_printf("  Sending INIT to cpus that have not responded yet\n");
	for_each_online_cpu(c)
		if (lkdb_running_process[c].seqno < lkdb_seqno - 1)
			platform_send_ipi(c, 0, IA64_IPI_DM_INIT, 0);
}

#endif /* CONFIG_SMP */

/* This code is sensitive to the layout of the MCA/INIT stack (see mca_asm.h)
 * and to the stack layout that ia64_mca_modify_original_stack() creates when
 * it makes the original task look blocked.
 */
static void
kdba_handlers_modify(struct task_struct *task, int cpu)
{
	struct lkdb_running_process *work, *save;
	work = lkdb_running_process + cpu;
	save = lkdb_running_process_save + cpu;
	*work = *save;
	if (!kdba_show_handlers && REGION_NUMBER(task) >= RGN_GATE &&
	    (task_thread_info(task)->flags & _TIF_MCA_INIT)) {
		struct ia64_sal_os_state *sos = (struct ia64_sal_os_state *)
			((unsigned long)save->p + MCA_SOS_OFFSET);
		char *p;
		if (!sos->prev_task)
			return;
		work->p = sos->prev_task;
		p = (char *)sos->prev_task->thread.ksp;
		p += 16;
		work->arch.sw = (struct switch_stack *)p;
		p += sizeof(struct switch_stack);
		work->regs = (struct pt_regs *)p;
		work->irq_depth = 2;	/* any value >1 will do */
	}
}

/* Turn the display of the MCA/INIT handlers on or off, or display the status
 * of the MCA/INIT handlers.
 */
static int
kdba_handlers(int argc, const char **argv)
{
	int cpu;
	struct lkdb_running_process *krp;
	if (argc != 1)
		return LKDB_ARGCOUNT;
	if (strcmp(argv[1], "show") == 0)
		kdba_show_handlers = 1;
	else if (strcmp(argv[1], "hide") == 0)
		kdba_show_handlers = 0;
	else if (strcmp(argv[1], "status") != 0) {
		lkdb_printf("handlers <show|hide|status>\n");
		return 0;
	}
	for (cpu = 0, krp = lkdb_running_process_save; cpu < NR_CPUS; ++cpu, ++krp) {
		if (krp->p)
			kdba_handlers_modify(krp->p, cpu);
	}
	if (strcmp(argv[1], "status") != 0)
		return 0;
	lkdb_printf("handlers status is %s\n", kdba_show_handlers ? "'show'" : "'hide'");
	lkdb_printf(" cpu handler task       command            original task      command\n");
	for (cpu = 0, krp = lkdb_running_process_save; cpu < NR_CPUS; ++cpu, ++krp) {
		struct task_struct *p = krp->p;
		if (!p)
			continue;
		lkdb_printf("%4d", cpu);
		if (task_thread_info(p)->flags & _TIF_MCA_INIT) {
			struct ia64_sal_os_state *sos;
			lkdb_printf(" " kdb_machreg_fmt0 " %-*s  ",
				   (unsigned long)p, (int)sizeof(p->comm), p->comm);
			sos = (struct ia64_sal_os_state *)((unsigned long)p + MCA_SOS_OFFSET);
			p = sos->prev_task;
		} else
			lkdb_printf("%*s", (int)(1+2+16+1+sizeof(p->comm)+2), " ");
		if (p)
			lkdb_printf(" " kdb_machreg_fmt0 " %-*s",
				   (unsigned long)p, (int)sizeof(p->comm), p->comm);
		lkdb_printf("\n");
	}
	return 0;
}

/* Executed once on each cpu at startup. */
void
kdba_cpu_up(void)
{
}

/*
 * kdba_init
 *
 * 	Architecture specific initialization.
 *
 * Parameters:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *	None.
 */

void
kdba_init(void)
{
	lkdb_running_process_save = kzalloc(
		sizeof(*lkdb_running_process_save) * NR_CPUS, GFP_KERNEL);
	BUG_ON(!lkdb_running_process_save);
	lkdb_register("irr", kdba_sir, "", "Show interrupt registers", 0);
	lkdb_register("itm", kdba_itm, "", "Set new ITM value", 0);
#if defined(CONFIG_SMP)
	lkdb_register("init", kdba_sendinit, "", "Send INIT to cpu", 0);
#endif
	lkdb_register("pt_regs", kdba_pt_regs, "address", "Format struct pt_regs", 0);
	lkdb_register("switch_stack", kdba_switch_stack, "address", "Format struct switch_stack", 0);
	lkdb_register("minstate", kdba_minstate, "address", "Format PAL minstate", 0);
	lkdb_register("tpa", kdba_tpa, "<vaddr>", "Translate virtual to physical address", 0);
#if defined(CONFIG_NUMA)
	lkdb_register("tpav", kdba_tpav, "<begin addr> <end addr>", "Verify that physical addresses corresponding to virtual addresses from <begin addr> to <end addr> are in same node", 0);
#endif
	lkdb_register("stackdepth", kdba_stackdepth, "[percentage]", "Print processes using >= stack percentage", 0);
	lkdb_register("cpuinfo", kdba_cpuinfo, "[cpu]", "Print struct cpuinfo_ia64", 0);
	lkdb_register("handlers", kdba_handlers, "<show|hide|status>", "Control the display of MCA/INIT handlers", 0);

#ifdef CONFIG_SERIAL_8250_CONSOLE
	kdba_serial_console = KDBA_SC_STANDARD;
#endif
#ifdef CONFIG_SERIAL_SGI_L1_CONSOLE
	if (ia64_platform_is("sn2"))
		kdba_serial_console = KDBA_SC_SGI_L1;
#endif
	return;
}

/*
 * kdba_adjust_ip
 *
 * 	Architecture specific adjustment of instruction pointer before leaving
 *	kdb.
 *
 * Parameters:
 *	reason		The reason KDB was invoked
 *	error		The hardware-defined error code
 *	regs		The exception frame at time of fault/breakpoint.  If reason
 *			is SILENT or CPU_UP then regs is NULL, otherwise it should
 *			always be valid.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *	On IA64, KDB_ENTER() and KDB_ENTER_SLAVE() use break which is a fault,
 *	not a trap.  The instruction pointer must be stepped before leaving
 *	kdb, otherwise we get a loop.
 */

void
kdba_adjust_ip(lkdb_reason_t reason, int error, struct pt_regs *regs)
{
	if ((reason == LKDB_REASON_ENTER || reason == LKDB_REASON_ENTER_SLAVE) &&
	    !KDB_STATE(IP_ADJUSTED)) {
		if (KDB_NULL_REGS(regs))
			return;
		if (ia64_psr(regs)->ri < 2)
			kdba_setpc(regs, regs->cr_iip + ia64_psr(regs)->ri + 1);
		else
			kdba_setpc(regs, regs->cr_iip + 16);
	}
}

void
kdba_save_running(struct kdba_running_process *k, struct pt_regs *regs)
{
	struct lkdb_running_process *work, *save;
	int cpu = smp_processor_id();
	work = lkdb_running_process + cpu;
	save = lkdb_running_process_save + cpu;
	*save = *work;
	if (!regs)
		return;
	kdba_handlers_modify((struct task_struct *)regs->r13, cpu);
}

void
kdba_unsave_running(struct kdba_running_process *k, struct pt_regs *regs)
{
	memset(lkdb_running_process_save + smp_processor_id(), 0,
			sizeof(*lkdb_running_process_save));
}

void
kdba_set_current_task(struct task_struct *p)
{
	int cpu = lkdb_process_cpu(p);
	struct lkdb_running_process *work, *save;
	work = lkdb_running_process + cpu;
	save = lkdb_running_process_save + cpu;
	lkdb_current_task = p;
	if (lkdb_task_has_cpu(p)) {
		lkdb_current_regs = work->regs;
		return;
	}
	lkdb_current_regs = NULL;
	/* For most blocked tasks we cannot get the pt_regs without doing an
	 * unwind, which is not worth doing.  For tasks interrupted by
	 * MCA/INIT, when the user is not working on the handlers, we must use
	 * the registers at the time of interrupt.
	 */
	if (work->p == save->p || work->p != p)
		return;
	lkdb_current_regs = (struct pt_regs *)(work->p->thread.ksp + 16 +
		sizeof(struct switch_stack));
}

/*
 * asm-ia64 uaccess.h supplies __copy_to_user which relies on MMU to
 * trap invalid addresses in the _xxx fields.  Verify the other address
 * of the pair is valid by accessing the first and last byte ourselves,
 * then any access violations should only be caused by the _xxx
 * addresses,
 */

int
kdba_putarea_size(unsigned long to_xxx, void *from, size_t size)
{
	mm_segment_t oldfs = get_fs();
	int r;
	char c;
	c = *((volatile char *)from);
	c = *((volatile char *)from + size - 1);

	if (to_xxx >> 61 <= 4) {
		return kdb_putuserarea_size(to_xxx, from, size);
	}

	set_fs(KERNEL_DS);
	r = __copy_to_user_inatomic((void __user *)to_xxx, from, size);
	set_fs(oldfs);
	return r;
}

int
kdba_getarea_size(void *to, unsigned long from_xxx, size_t size)
{
	mm_segment_t oldfs = get_fs();
	int r;
	*((volatile char *)to) = '\0';
	*((volatile char *)to + size - 1) = '\0';

	if (from_xxx >> 61 <= 4)
		return kdb_getuserarea_size(to, from_xxx, size);

	set_fs(KERNEL_DS);
	switch (size) {
	case 1:
		r = __copy_to_user_inatomic((void __user *)to, (void *)from_xxx, 1);
		break;
	case 2:
		r = __copy_to_user_inatomic((void __user *)to, (void *)from_xxx, 2);
		break;
	case 4:
		r = __copy_to_user_inatomic((void __user *)to, (void *)from_xxx, 4);
		break;
	case 8:
		r = __copy_to_user_inatomic((void __user *)to, (void *)from_xxx, 8);
		break;
	default:
		r = __copy_to_user_inatomic((void __user *)to, (void *)from_xxx, size);
		break;
	}
	set_fs(oldfs);
	return r;
}

int
kdba_verify_rw(unsigned long addr, size_t size)
{
	unsigned char data[(__force size_t) size];
	return(kdba_getarea_size(data, addr, size) || kdba_putarea_size(addr, data, size));
}

#ifdef CONFIG_KDB_KDUMP
void
kdba_kdump_prepare(struct pt_regs *fixed_regs)
{
	int i;

	/* Set on KEXEC bit on all onlinr cpus */
	for (i = 1; i < NR_CPUS; ++i) {
		if (!cpu_online(i))
			continue;

		KDB_STATE_SET_CPU(KEXEC, i);
	}

	machine_crash_shutdown(fixed_regs);
}

void kdba_kdump_shutdown_slave(struct pt_regs *regs)
{
	if (lkdb_kdump_state != LKDB_KDUMP_RESET) {
		unw_init_running(kdump_cpu_freeze, NULL);
	}
}
#endif
