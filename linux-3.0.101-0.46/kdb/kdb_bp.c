/*
 * Kernel Debugger Architecture Independent Breakpoint Handler
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2004 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/system.h>

/*
 * Table of lkdb_breakpoints
 */
lkdb_bp_t lkdb_breakpoints[LKDB_MAXBPT];

/*
 *	Predicate to test whether a breakpoint should be installed
 *	on this CPU.
 *
 *	Note that for purposes of installation, hardware breakpoints
 *	are treated as local (even if the global flag is set), on
 *	the assumption that the require per-cpu registers to be set.
 */

static inline int kdb_is_installable_global_bp(const lkdb_bp_t *bp)
{
	return (bp->bp_enabled &&
		bp->bp_global &&
		!bp->bp_forcehw);
}

static int kdb_is_installable_local_bp(const lkdb_bp_t *bp)
{
	if (!bp->bp_enabled)
		return 0;

	if (bp->bp_forcehw) {
		if (bp->bp_cpu == smp_processor_id() || bp->bp_global)
			return 1;
	} else {
		if (bp->bp_cpu == smp_processor_id() && !bp->bp_global)
			return 1;
	}
	return 0;
}

/*
 * lkdb_bp_install_global
 *
 *	Install global lkdb_breakpoints prior to returning from the
 *	kernel debugger.  This allows the lkdb_breakpoints to be set
 *	upon functions that are used internally by kdb, such as
 *	printk().
 *
 * Parameters:
 *	regs	Execution frame.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *
 *	This function is only called once per kdb session.
 */

void
lkdb_bp_install_global(struct pt_regs *regs)
{
	int i;

	for(i=0; i<LKDB_MAXBPT; i++) {
		lkdb_bp_t *bp = &lkdb_breakpoints[i];

		if (KDB_DEBUG(BP)) {
			lkdb_printf("lkdb_bp_install_global bp %d bp_enabled %d bp_global %d\n",
				i, bp->bp_enabled, bp->bp_global);
		}
		/* HW BP local or global are installed in lkdb_bp_install_local*/
		if (kdb_is_installable_global_bp(bp))
			kdba_installbp(regs, bp);
	}
}

/*
 * lkdb_bp_install_local
 *
 *	Install local lkdb_breakpoints prior to returning from the
 *	kernel debugger.  This allows the lkdb_breakpoints to be set
 *	upon functions that are used internally by kdb, such as
 *	printk().
 *
 * Parameters:
 *	regs	Execution frame.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 *
 *	This function is called once per processor.
 */

void
lkdb_bp_install_local(struct pt_regs *regs)
{
	int i;

	for(i=0; i<LKDB_MAXBPT; i++) {
		lkdb_bp_t *bp = &lkdb_breakpoints[i];

		if (KDB_DEBUG(BP)) {
			lkdb_printf("lkdb_bp_install_local bp %d bp_enabled %d bp_global %d cpu %d bp_cpu %d\n",
				i, bp->bp_enabled, bp->bp_global,
				smp_processor_id(), bp->bp_cpu);
		}
		if (kdb_is_installable_local_bp(bp))
			kdba_installbp(regs, bp);
	}
}

/*
 * lkdb_bp_remove_global
 *
 * 	Remove global lkdb_breakpoints upon entry to the kernel debugger.
 *
 * Parameters:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 */

void
lkdb_bp_remove_global(void)
{
	int i;

	for(i=LKDB_MAXBPT-1; i>=0; i--) {
		lkdb_bp_t *bp = &lkdb_breakpoints[i];

		if (KDB_DEBUG(BP)) {
			lkdb_printf("lkdb_bp_remove_global bp %d bp_enabled %d bp_global %d\n",
				i, bp->bp_enabled, bp->bp_global);
		}
		if (kdb_is_installable_global_bp(bp))
			kdba_removebp(bp);
	}
}


/*
 * lkdb_bp_remove_local
 *
 * 	Remove local lkdb_breakpoints upon entry to the kernel debugger.
 *
 * Parameters:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 */

void
lkdb_bp_remove_local(void)
{
	int i;

	for(i=LKDB_MAXBPT-1; i>=0; i--) {
		lkdb_bp_t *bp = &lkdb_breakpoints[i];

		if (KDB_DEBUG(BP)) {
			lkdb_printf("lkdb_bp_remove_local bp %d bp_enabled %d bp_global %d cpu %d bp_cpu %d\n",
				i, bp->bp_enabled, bp->bp_global,
				smp_processor_id(), bp->bp_cpu);
		}
		if (kdb_is_installable_local_bp(bp))
			kdba_removebp(bp);
	}
}

/*
 * kdb_printbp
 *
 * 	Internal function to format and print a breakpoint entry.
 *
 * Parameters:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 */

static void
kdb_printbp(lkdb_bp_t *bp, int i)
{
	if (bp->bp_forcehw) {
		lkdb_printf("Forced ");
	}

	if (!bp->bp_template.bph_free) {
		lkdb_printf("%s ", kdba_bptype(&bp->bp_template));
	} else {
		lkdb_printf("Instruction(i) ");
	}

	lkdb_printf("BP #%d at ", i);
	lkdb_symbol_print(bp->bp_addr, NULL, KDB_SP_DEFAULT);

	if (bp->bp_enabled) {
		kdba_printbp(bp);
		if (bp->bp_global)
			lkdb_printf(" globally");
		else
			lkdb_printf(" on cpu %d", bp->bp_cpu);
		if (bp->bp_adjust)
			lkdb_printf(" adjust %d", bp->bp_adjust);
	} else {
		lkdb_printf("\n    is disabled");
	}

	lkdb_printf("\taddr at %016lx, hardtype=%d, forcehw=%d, installed=%d, hard=%p\n",
		bp->bp_addr, bp->bp_hardtype, bp->bp_forcehw,
		bp->bp_installed, bp->bp_hard);

	lkdb_printf("\n");
}

/*
 * kdb_bp
 *
 * 	Handle the bp, and bpa commands.
 *
 *	[bp|bpa|bph] <addr-expression> [DATAR|DATAW|IO [length]]
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
 *
 * 	bp	Set breakpoint.  Only use hardware assist if necessary.
 *	bpa	Set breakpoint on all cpus, only use hardware regs if necessary
 *	bph	Set breakpoint - force hardware register
 *	bpha	Set breakpoint on all cpus, force hardware register
 */

static int
kdb_bp(int argc, const char **argv)
{
	int i, bpno;
	lkdb_bp_t *bp, *bp_check;
	int diag;
	int free;
	char *symname = NULL;
	long offset = 0ul;
	int nextarg;
	static lkdb_bp_t kdb_bp_template;

	if (argc == 0) {
		/*
		 * Display breakpoint table
		 */
		for(bpno=0,bp=lkdb_breakpoints; bpno<LKDB_MAXBPT; bpno++, bp++) {
			if (bp->bp_free) continue;

			kdb_printbp(bp, bpno);
		}

		return 0;
	}

	memset(&kdb_bp_template, 0, sizeof(kdb_bp_template));

	kdb_bp_template.bp_global = ((strcmp(argv[0], "bpa") == 0)
			   || (strcmp(argv[0], "bpha") == 0));
	kdb_bp_template.bp_forcehw = ((strcmp(argv[0], "bph") == 0)
			   || (strcmp(argv[0], "bpha") == 0));

	/* Fix me: "bp" is treated as "bpa" to avoid system freeze. -jlan */
	if (strcmp(argv[0], "bp") == 0)
		kdb_bp_template.bp_global = 1;

	nextarg = 1;
	diag = lkdbgetaddrarg(argc, argv, &nextarg, &kdb_bp_template.bp_addr,
			     &offset, &symname);
	if (diag)
		return diag;
	if (!kdb_bp_template.bp_addr)
		return LKDB_BADINT;

	/*
	 * Find an empty bp structure, to allocate
	 */
	free = LKDB_MAXBPT;
	for(bpno=0,bp=lkdb_breakpoints; bpno<LKDB_MAXBPT; bpno++,bp++) {
		if (bp->bp_free) {
			break;
		}
	}

	if (bpno == LKDB_MAXBPT)
		return LKDB_TOOMANYBPT;

	/*
	 * Handle architecture dependent parsing
	 */
	diag = kdba_parsebp(argc, argv, &nextarg, &kdb_bp_template);
	if (diag) {
		return diag;
	}


	/*
	 * Check for clashing breakpoints.
	 *
	 * Note, in this design we can't have hardware breakpoints
	 * enabled for both read and write on the same address, even
	 * though ia64 allows this.
	 */
	for(i=0,bp_check=lkdb_breakpoints; i<LKDB_MAXBPT; i++,bp_check++) {
		if (!bp_check->bp_free &&
		    bp_check->bp_addr == kdb_bp_template.bp_addr &&
		    (bp_check->bp_global ||
		     bp_check->bp_cpu == kdb_bp_template.bp_cpu)) {
			lkdb_printf("You already have a breakpoint at "
				kdb_bfd_vma_fmt0 "\n", kdb_bp_template.bp_addr);
			return LKDB_DUPBPT;
		}
	}

	kdb_bp_template.bp_enabled = 1;

	/*
	 * Actually allocate the breakpoint found earlier
	 */
	*bp = kdb_bp_template;
	bp->bp_free = 0;

	if (!bp->bp_global) {
		bp->bp_cpu = smp_processor_id();
	}

	/*
	 * Allocate a hardware breakpoint.  If one is not available,
	 * disable the breakpoint, but leave it in the breakpoint
	 * table.  When the breakpoint is re-enabled (via 'be'), we'll
	 * attempt to allocate a hardware register for it.
	 */
	if (!bp->bp_template.bph_free) {
		kdba_alloc_hwbp(bp, &diag);
		if (diag) {
			bp->bp_enabled = 0;
			bp->bp_hardtype = 0;
			kdba_free_hwbp(bp);
			return diag;
		}
	}

	kdb_printbp(bp, bpno);

	return 0;
}

/*
 * kdb_bc
 *
 * 	Handles the 'bc', 'be', and 'bd' commands
 *
 *	[bd|bc|be] <breakpoint-number>
 *	[bd|bc|be] *
 *
 * Parameters:
 *	argc	Count of arguments in argv
 *	argv	Space delimited command line arguments
 * Outputs:
 *	None.
 * Returns:
 *	Zero for success, a kdb diagnostic for failure
 * Locking:
 *	None.
 * Remarks:
 */

#define KDBCMD_BC	0
#define KDBCMD_BE	1
#define KDBCMD_BD	2

static int
kdb_bc(int argc, const char **argv)
{
	kdb_machreg_t addr;
	lkdb_bp_t *bp = NULL;
	int lowbp = LKDB_MAXBPT;
	int highbp = 0;
	int done = 0;
	int i;
	int diag;
	int cmd;			/* KDBCMD_B? */

	if (strcmp(argv[0], "be") == 0) {
		cmd = KDBCMD_BE;
	} else if (strcmp(argv[0], "bd") == 0) {
		cmd = KDBCMD_BD;
	} else
		cmd = KDBCMD_BC;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	if (strcmp(argv[1], "*") == 0) {
		lowbp = 0;
		highbp = LKDB_MAXBPT;
	} else {
		diag = lkdbgetularg(argv[1], &addr);
		if (diag)
			return diag;

		/*
		 * For addresses less than the maximum breakpoint number,
		 * assume that the breakpoint number is desired.
		 */
		if (addr < LKDB_MAXBPT) {
			bp = &lkdb_breakpoints[addr];
			lowbp = highbp = addr;
			highbp++;
		} else {
			for(i=0, bp=lkdb_breakpoints; i<LKDB_MAXBPT; i++, bp++) {
				if (bp->bp_addr == addr) {
					lowbp = highbp = i;
					highbp++;
					break;
				}
			}
		}
	}

	/*
	 * Now operate on the set of breakpoints matching the input
	 * criteria (either '*' for all, or an individual breakpoint).
	 */
	for(bp=&lkdb_breakpoints[lowbp], i=lowbp;
	    i < highbp;
	    i++, bp++) {
		if (bp->bp_free)
			continue;

		done++;

		switch (cmd) {
		case KDBCMD_BC:
			if (bp->bp_hardtype)
				kdba_free_hwbp(bp);

			bp->bp_enabled = 0;
			bp->bp_global = 0;

			lkdb_printf("Breakpoint %d at " kdb_bfd_vma_fmt " cleared\n",
				i, bp->bp_addr);

			bp->bp_addr = 0;
			bp->bp_free = 1;

			break;
		case KDBCMD_BE:
			/*
			 * Allocate a hardware breakpoint.  If one is not
			 * available, don't enable the breakpoint.
			 */
			if (!bp->bp_template.bph_free
			 && !bp->bp_hardtype) {
				kdba_alloc_hwbp(bp, &diag);
				if (diag) {
					bp->bp_enabled = 0;
					bp->bp_hardtype = 0;
					kdba_free_hwbp(bp);
					return diag;
				}
			}

			bp->bp_enabled = 1;

			lkdb_printf("Breakpoint %d at " kdb_bfd_vma_fmt " enabled",
				i, bp->bp_addr);

			lkdb_printf("\n");
			break;
		case KDBCMD_BD:
			if (!bp->bp_enabled)
				break;

			/*
			 * Since this breakpoint is now disabled, we can
			 * give up the hardware register which is allocated
			 * to it.
			 */
			if (bp->bp_hardtype)
				kdba_free_hwbp(bp);

			bp->bp_enabled = 0;

			lkdb_printf("Breakpoint %d at " kdb_bfd_vma_fmt " disabled\n",
				i, bp->bp_addr);

			break;
		}
		if (bp->bp_delay && (cmd == KDBCMD_BC || cmd == KDBCMD_BD)) {
			bp->bp_delay = 0;
			KDB_STATE_CLEAR(SSBPT);
		}
	}

	return (!done)?LKDB_BPTNOTFOUND:0;
}

/*
 * kdb_ss
 *
 *	Process the 'ss' (Single Step) and 'ssb' (Single Step to Branch)
 *	commands.
 *
 *	ss
 *	ssb
 *
 * Parameters:
 *	argc	Argument count
 *	argv	Argument vector
 * Outputs:
 *	None.
 * Returns:
 *	KDB_CMD_SS[B] for success, a kdb error if failure.
 * Locking:
 *	None.
 * Remarks:
 *
 *	Set the arch specific option to trigger a debug trap after the next
 *	instruction.
 *
 *	For 'ssb', set the trace flag in the debug trap handler
 *	after printing the current insn and return directly without
 *	invoking the kdb command processor, until a branch instruction
 *	is encountered.
 */

static int
kdb_ss(int argc, const char **argv)
{
	int ssb = 0;
	struct pt_regs *regs = get_irq_regs();

	ssb = (strcmp(argv[0], "ssb") == 0);
	if (argc != 0)
		return LKDB_ARGCOUNT;

	if (!regs) {
		lkdb_printf("%s: pt_regs not available\n", __FUNCTION__);
		return LKDB_BADREG;
	}

	/*
	 * Set trace flag and go.
	 */
	KDB_STATE_SET(DOING_SS);
	if (ssb)
		KDB_STATE_SET(DOING_SSB);

	kdba_setsinglestep(regs);		/* Enable single step */

	if (ssb)
		return KDB_CMD_SSB;
	return KDB_CMD_SS;
}

/*
 * lkdb_initbptab
 *
 *	Initialize the breakpoint table.  Register breakpoint commands.
 *
 * Parameters:
 *	None.
 * Outputs:
 *	None.
 * Returns:
 *	None.
 * Locking:
 *	None.
 * Remarks:
 */

void __init
lkdb_initbptab(void)
{
	int i;
	lkdb_bp_t *bp;

	/*
	 * First time initialization.
	 */
	memset(&lkdb_breakpoints, '\0', sizeof(lkdb_breakpoints));

	for (i=0, bp=lkdb_breakpoints; i<LKDB_MAXBPT; i++, bp++) {
		bp->bp_free = 1;
		/*
		 * The bph_free flag is architecturally required.  It
		 * is set by architecture-dependent code to false (zero)
		 * in the event a hardware breakpoint register is required
		 * for this breakpoint.
		 *
		 * The rest of the template is reserved to the architecture
		 * dependent code and _must_ not be touched by the architecture
		 * independent code.
		 */
		bp->bp_template.bph_free = 1;
	}

	lkdb_register_repeat("bp", kdb_bp, "[<vaddr>]", "Set/Display breakpoints", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("bl", kdb_bp, "[<vaddr>]", "Display breakpoints", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("bpa", kdb_bp, "[<vaddr>]", "Set/Display global breakpoints", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("bph", kdb_bp, "[<vaddr>]", "Set hardware breakpoint", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("bpha", kdb_bp, "[<vaddr>]", "Set global hardware breakpoint", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("bc", kdb_bc, "<bpnum>",   "Clear Breakpoint", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("be", kdb_bc, "<bpnum>",   "Enable Breakpoint", 0, LKDB_REPEAT_NONE);
	lkdb_register_repeat("bd", kdb_bc, "<bpnum>",   "Disable Breakpoint", 0, LKDB_REPEAT_NONE);

	lkdb_register_repeat("ss", kdb_ss, "", "Single Step", 1, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("ssb", kdb_ss, "", "Single step to branch/call", 0, LKDB_REPEAT_NO_ARGS);
	/*
	 * Architecture dependent initialization.
	 */
	kdba_initbp();
}
