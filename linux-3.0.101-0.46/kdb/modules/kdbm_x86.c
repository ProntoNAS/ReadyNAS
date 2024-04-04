/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Author: Vamsi Krishna S. <vamsi_krishna@in.ibm.com>
 * (C) 2003 IBM Corporation.
 * 2006-10-10 Keith Owens
 *   Reworked to include x86_64 support
 * Copyright (c) 2006 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>

#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/desc.h>
#include <asm/debugreg.h>
#if 0
#include <asm/pgtable.h>
#endif

MODULE_AUTHOR("Vamsi Krishna S./IBM");
MODULE_DESCRIPTION("x86 specific information (gdt/idt/ldt/page tables)");
MODULE_LICENSE("GPL");

/* Isolate as many of the i386/x86_64 differences as possible in one spot */

#ifdef	CONFIG_X86_64

#define LKDB_X86_64 1
#define	MOVLQ "movq"

typedef struct desc_struct kdb_desc_t;
typedef struct gate_struct64 kdb_gate_desc_t;

#define LKDB_SYS_DESC_OFFSET(d) ((unsigned long)d->offset_high << 32 | d->offset_middle << 16 | d->offset_low)
#define LKDB_SYS_DESC_CALLG_COUNT(d) 0

#else	/* !CONFIG_X86_64 */

#define LKDB_X86_64 0
#define	MOVLQ "movl"

/* i386 has no detailed mapping for the 8 byte segment descriptor, copy the
 * x86_64 one and merge the l and avl bits.
 */
struct kdb_desc {
	u16 limit0;
	u16 base0;
	unsigned base1 : 8, type : 4, s : 1, dpl : 2, p : 1;
	unsigned limit : 4, avl : 2, d : 1, g : 1, base2 : 8;
} __attribute__((packed));
typedef struct kdb_desc kdb_desc_t;

/* i386 has no detailed mapping for the 8 byte gate descriptor, base it on the
 * x86_64 one.
 */
struct kdb_gate_desc {
	u16 offset_low;
	u16 segment;
	unsigned res : 8, type : 4, s : 1, dpl : 2, p : 1;
	u16 offset_middle;
} __attribute__((packed));
typedef struct kdb_gate_desc kdb_gate_desc_t;

#define LKDB_SYS_DESC_OFFSET(d) ((unsigned long)(d->offset_middle << 16 | d->offset_low))
#define LKDB_SYS_DESC_CALLG_COUNT(d) ((unsigned int)(d->res & 0x0F))

#endif	/* CONFIG_X86_64 */

#define KDB_SEL_MAX 			0x2000
#define KDB_IDT_MAX 			0x100
#define LKDB_SYS_DESC_TYPE_TSS16		0x01
#define LKDB_SYS_DESC_TYPE_LDT		0x02
#define LKDB_SYS_DESC_TYPE_TSSB16	0x03
#define LKDB_SYS_DESC_TYPE_CALLG16	0x04
#define LKDB_SYS_DESC_TYPE_TASKG		0x05
#define LKDB_SYS_DESC_TYPE_INTG16	0x06
#define LKDB_SYS_DESC_TYPE_TRAP16	0x07

#define LKDB_SYS_DESC_TYPE_TSS 		0x09
#define LKDB_SYS_DESC_TYPE_TSSB		0x0b
#define LKDB_SYS_DESC_TYPE_CALLG		0x0c
#define LKDB_SYS_DESC_TYPE_INTG		0x0e
#define LKDB_SYS_DESC_TYPE_TRAPG		0x0f

#define LKDB_SEG_DESC_TYPE_CODE		0x08
#define LKDB_SEG_DESC_TYPE_CODE_R	0x02
#define LKDB_SEG_DESC_TYPE_DATA_W	0x02
#define LKDB_SEG_DESC_TYPE_CODE_C	0x02    /* conforming */
#define LKDB_SEG_DESC_TYPE_DATA_D	0x02    /* expand-down */
#define LKDB_SEG_DESC_TYPE_A		0x01	/* accessed */

#define _LIMIT(d) ((unsigned long)((d)->limit << 16 | (d)->limit0))
#define LKDB_SEG_DESC_LIMIT(d) ((d)->g ? ((_LIMIT(d)+1) << 12) -1 : _LIMIT(d))

static unsigned long kdb_seg_desc_base(kdb_desc_t *d)
{
	unsigned long base = d->base2 << 24 | d->base1 << 16 | d->base0;
#ifdef	CONFIG_X86_64
	switch (d->type) {
	case LKDB_SYS_DESC_TYPE_TSS:
	case LKDB_SYS_DESC_TYPE_TSSB:
	case LKDB_SYS_DESC_TYPE_LDT:
		base += (unsigned long)(((struct ldttss_desc64 *)d)->base3) << 32;
		break;
	}
#endif
	return base;
}

/* helper functions to display system registers in verbose mode */
static void display_gdtr(void)
{
	struct desc_ptr gdtr;

	__asm__ __volatile__ ("sgdt %0\n\t" : "=m"(gdtr));
	lkdb_printf("gdtr.address = " kdb_machreg_fmt0 ", gdtr.size = 0x%x\n",
		   gdtr.address, gdtr.size);

	return;
}

static void display_ldtr(void)
{
	struct desc_ptr gdtr;
	unsigned long ldtr;

	__asm__ __volatile__ ("sgdt %0\n\t" : "=m"(gdtr));
	__asm__ __volatile__ ("sldt %0\n\t" : "=m"(ldtr));
	ldtr &= 0xfff8;		/* extract the index */

	lkdb_printf("ldtr = " kdb_machreg_fmt0 " ", ldtr);

	if (ldtr < gdtr.size) {
		kdb_desc_t *ldt_desc =
			(kdb_desc_t *)(gdtr.address + ldtr);
		lkdb_printf("base=" kdb_machreg_fmt0
			   ", limit=" kdb_machreg_fmt "\n",
				kdb_seg_desc_base(ldt_desc),
				LKDB_SEG_DESC_LIMIT(ldt_desc));
	} else {
		lkdb_printf("invalid\n");
	}

	return;
}

static void display_idtr(void)
{
	struct desc_ptr idtr;
	__asm__ __volatile__ ("sidt %0\n\t" : "=m"(idtr));
	lkdb_printf("idtr.address = " kdb_machreg_fmt0 ", idtr.size = 0x%x\n",
		   idtr.address, idtr.size);
	return;
}

static const char *cr0_flags[] = {
	"pe", "mp", "em", "ts", "et", "ne", NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"wp", NULL, "am", NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, "nw", "cd", "pg"};

static void display_cr0(void)
{
	kdb_machreg_t cr0;
	int i;
	__asm__ (MOVLQ " %%cr0,%0\n\t":"=r"(cr0));
	lkdb_printf("cr0 = " kdb_machreg_fmt0, cr0);
	for (i = 0; i < ARRAY_SIZE(cr0_flags); i++) {
		if (test_bit(i, &cr0) && cr0_flags[i])
			lkdb_printf(" %s", cr0_flags[i]);
	}
	lkdb_printf("\n");
	return;
}

static void display_cr3(void)
{
	kdb_machreg_t cr3;
	__asm__ (MOVLQ " %%cr3,%0\n\t":"=r"(cr3));
	lkdb_printf("cr3 = " kdb_machreg_fmt0 " ", cr3);
	if (cr3 & 0x08)
		lkdb_printf("pwt ");
	if (cr3 & 0x10)
		lkdb_printf("pcd ");
	lkdb_printf("%s=" kdb_machreg_fmt0 "\n",
		   LKDB_X86_64 ? "pml4" : "pgdir", cr3 & PAGE_MASK);
	return;
}

static const char *cr4_flags[] = {
	"vme", "pvi", "tsd", "de",
	"pse", "pae", "mce", "pge",
	"pce", "osfxsr" "osxmmexcpt"};

static void display_cr4(void)
{
	kdb_machreg_t cr4;
	int i;
	__asm__ (MOVLQ " %%cr4,%0\n\t":"=r"(cr4));
	lkdb_printf("cr4 = " kdb_machreg_fmt0, cr4);
	for (i = 0; i < ARRAY_SIZE(cr4_flags); i++) {
		if (test_bit(i, &cr4))
			lkdb_printf(" %s", cr4_flags[i]);
	}
	lkdb_printf("\n");
	return;
}

static void display_cr8(void)
{
#ifdef	CONFIG_X86_64
	kdb_machreg_t cr8;
	__asm__ (MOVLQ " %%cr8,%0\n\t":"=r"(cr8));
	lkdb_printf("cr8 = " kdb_machreg_fmt0 "\n", cr8);
	return;
#endif	/* CONFIG_X86_64 */
}

static char *dr_type_name[] = { "exec", "write", "io", "rw" };

static void display_dr_status(int nr, int enabled, int local, int len, int type)
{
	if (!enabled) {
		lkdb_printf("\tdebug register %d: not enabled\n", nr);
		return;
	}

	lkdb_printf("    debug register %d: %s, len = %d, type = %s\n",
			nr,
			local? " local":"global",
			len,
			dr_type_name[type]);
}

static void display_dr(void)
{
	kdb_machreg_t dr0, dr1, dr2, dr3, dr6, dr7;
	int dbnr, set;

	__asm__ (MOVLQ " %%db0,%0\n\t":"=r"(dr0));
	__asm__ (MOVLQ " %%db1,%0\n\t":"=r"(dr1));
	__asm__ (MOVLQ " %%db2,%0\n\t":"=r"(dr2));
	__asm__ (MOVLQ " %%db3,%0\n\t":"=r"(dr3));
	__asm__ (MOVLQ " %%db6,%0\n\t":"=r"(dr6));
	__asm__ (MOVLQ " %%db7,%0\n\t":"=r"(dr7));

	lkdb_printf("dr0 = " kdb_machreg_fmt0 " dr1 = " kdb_machreg_fmt0
		   " dr2 = " kdb_machreg_fmt0 " dr3 = " kdb_machreg_fmt0 "\n",
		   dr0, dr1, dr2, dr3);
	lkdb_printf("dr6 = " kdb_machreg_fmt0 " ", dr6);
	dbnr = dr6 & DR6_DR_MASK;
	if (dbnr) {
		int nr;
		switch(dbnr) {
		case 1:
			nr = 0; break;
		case 2:
			nr = 1; break;
		case 4:
			nr = 2; break;
		default:
			nr = 3; break;
		}
		lkdb_printf("debug register hit = %d", nr);
	} else if (dr6 & DR_STEP) {
		lkdb_printf("single step");
	} else if (dr6 & DR_SWITCH) {
		lkdb_printf("task switch");
	}
	lkdb_printf("\n");

	lkdb_printf("dr7 = " kdb_machreg_fmt0 "\n", dr7);
	set = DR7_L0(dr7) || DR7_G0(dr7);
	display_dr_status(0, set, DR7_L0(dr7), DR7_LEN0(dr7), DR7_RW0(dr7));
	set = DR7_L1(dr7) || DR7_G1(dr7);
	display_dr_status(1, set, DR7_L1(dr7), DR7_LEN1(dr7), DR7_RW1(dr7));
	set = DR7_L2(dr7) || DR7_G2(dr7);
	display_dr_status(2, set, DR7_L2(dr7), DR7_LEN2(dr7), DR7_RW2(dr7));
	set = DR7_L3(dr7) || DR7_G3(dr7);
	display_dr_status(3, set, DR7_L3(dr7), DR7_LEN3(dr7), DR7_RW3(dr7));
}

static char *set_eflags[] = {
	"carry", NULL, "parity",  NULL, "adjust",  NULL, "zero", "sign",
	"trace", "intr-on", "dir", "overflow",  NULL, NULL, "nestedtask", NULL,
	"resume", "vm", "align", "vif", "vip", "id"};

static void display_eflags(unsigned long ef)
{
	int i, iopl;
	lkdb_printf("eflags = " kdb_machreg_fmt0 " ", ef);
	for (i = 0; i < ARRAY_SIZE(set_eflags); i++) {
		if (test_bit(i, &ef) && set_eflags[i])
			lkdb_printf("%s ", set_eflags[i]);
	}

	iopl = (ef & 0x00003000) >> 12;
	lkdb_printf("iopl=%d\n", iopl);
	return;
}

static void display_tss(struct tss_struct *t)
{
#ifdef	CONFIG_X86_64
	int i;
	lkdb_printf("    sp0 = 0x%016Lx,  sp1 = 0x%016Lx\n",
		   t->x86_tss.sp0, t->x86_tss.sp1);
	lkdb_printf("    sp2 = 0x%016Lx\n", t->x86_tss.sp2);
	for (i = 0; i < ARRAY_SIZE(t->x86_tss.ist); ++i)
		lkdb_printf("    ist[%d] = 0x%016Lx\n",
			  i, t->x86_tss.ist[i]);
	lkdb_printf("   iomap = 0x%04x\n", t->x86_tss.io_bitmap_base);
#else	/* !CONFIG_X86_64 */
	lkdb_printf("    cs = %04x,  ip = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.es, t->x86_tss.ip);
	lkdb_printf("    ss = %04x,  sp = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ss, t->x86_tss.sp);
	lkdb_printf("   ss0 = %04x, sp0 = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ss0, t->x86_tss.sp0);
	lkdb_printf("   ss1 = %04x, sp1 = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ss1, t->x86_tss.sp1);
	lkdb_printf("   ss2 = %04x, sp2 = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ss2, t->x86_tss.sp2);
	lkdb_printf("   ldt = %04x, cr3 = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ldt, t->x86_tss.__cr3);
	lkdb_printf("    ds = %04x, es = %04x fs = %04x gs = %04x\n",
			t->x86_tss.ds, t->x86_tss.es, t->x86_tss.fs, t->x86_tss.gs);
	lkdb_printf("   ax = " kdb_machreg_fmt0 ", bx = " kdb_machreg_fmt0
		   " cx = " kdb_machreg_fmt0 " dx = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.ax, t->x86_tss.bx, t->x86_tss.cx, t->x86_tss.dx);
	lkdb_printf("   si = " kdb_machreg_fmt0 ", di = " kdb_machreg_fmt0
		   " bp = " kdb_machreg_fmt0 "\n",
		   t->x86_tss.si, t->x86_tss.di, t->x86_tss.bp);
	lkdb_printf("   trace = %d, iomap = 0x%04x\n", t->x86_tss.trace, t->x86_tss.io_bitmap_base);
#endif	/* CONFIG_X86_64 */
}

static char *gate_desc_types[] = {
#ifdef	CONFIG_X86_64
	"reserved-0", "reserved-1", "ldt", "reserved-3",
	"reserved-4", "reserved-5", "reserved-6", "reserved-7",
	"reserved-8", "tss-avlb", "reserved-10", "tss-busy",
	"callgate", "reserved-13", "intgate", "trapgate",
#else	/* CONFIG_X86_64 */
	"reserved-0", "tss16-avlb", "ldt", "tss16-busy",
	"callgate16", "taskgate", "intgate16", "trapgate16",
	"reserved-8", "tss-avlb", "reserved-10", "tss-busy",
	"callgate", "reserved-13", "intgate", "trapgate",
#endif	/* CONFIG_X86_64 */
};

static void
display_gate_desc(kdb_gate_desc_t *d)
{
	lkdb_printf("%-11s ", gate_desc_types[d->type]);

	switch(d->type) {
	case LKDB_SYS_DESC_TYPE_LDT:
		lkdb_printf("base=");
	       	lkdb_symbol_print(kdb_seg_desc_base((kdb_desc_t *)d), NULL,
				 KDB_SP_DEFAULT);
		lkdb_printf(" limit=" kdb_machreg_fmt " dpl=%d\n",
			   LKDB_SEG_DESC_LIMIT((kdb_desc_t *)d), d->dpl);
		break;
	case LKDB_SYS_DESC_TYPE_TSS:
	case LKDB_SYS_DESC_TYPE_TSS16:
	case LKDB_SYS_DESC_TYPE_TSSB:
	case LKDB_SYS_DESC_TYPE_TSSB16:
	{
		struct tss_struct *tss =
			(struct tss_struct *)
			kdb_seg_desc_base((kdb_desc_t *)d);
		lkdb_printf("base=");
	       	lkdb_symbol_print((unsigned long)tss, NULL, KDB_SP_DEFAULT);
		lkdb_printf(" limit=" kdb_machreg_fmt " dpl=%d\n",
			   LKDB_SEG_DESC_LIMIT((kdb_desc_t *)d), d->dpl);
		display_tss(tss);
		break;
	}
	case LKDB_SYS_DESC_TYPE_CALLG16:
		lkdb_printf("segment=0x%4.4x off=", d->segment);
	       	lkdb_symbol_print(LKDB_SYS_DESC_OFFSET(d), NULL, KDB_SP_DEFAULT);
		lkdb_printf(" dpl=%d wc=%d\n",
			   d->dpl, LKDB_SYS_DESC_CALLG_COUNT(d));
		break;
	case LKDB_SYS_DESC_TYPE_CALLG:
		lkdb_printf("segment=0x%4.4x off=", d->segment);
	       	lkdb_symbol_print(LKDB_SYS_DESC_OFFSET(d), NULL, KDB_SP_DEFAULT);
		lkdb_printf(" dpl=%d\n", d->dpl);
		break;
	default:
		lkdb_printf("segment=0x%4.4x off=", d->segment);
		if (LKDB_SYS_DESC_OFFSET(d))
			lkdb_symbol_print(LKDB_SYS_DESC_OFFSET(d), NULL,
					 KDB_SP_DEFAULT);
		else
			lkdb_printf(kdb_machreg_fmt0, LKDB_SYS_DESC_OFFSET(d));
		lkdb_printf(" dpl=%d", d->dpl);
#ifdef	CONFIG_X86_64
		if (d->ist)
			lkdb_printf(" ist=%d", d->ist);
#endif	/* CONFIG_X86_64 */
		lkdb_printf("\n");
		break;
	}
}

static void
display_seg_desc(kdb_desc_t *d)
{
	unsigned char type = d->type;

	if (type & LKDB_SEG_DESC_TYPE_CODE) {
		lkdb_printf("%-11s base=" kdb_machreg_fmt0 " limit="
			   kdb_machreg_fmt " dpl=%d %c%c%c %s %s %s \n",
			   "code",
			   kdb_seg_desc_base(d), LKDB_SEG_DESC_LIMIT(d),
			   d->dpl,
			   (type & LKDB_SEG_DESC_TYPE_CODE_R)?'r':'-',
			   '-', 'x',
#ifdef	CONFIG_X86_64
			   d->l ? "64b" : d->d ? "32b" : "16b",
#else	/* !CONFIG_X86_64 */
			   d->d ? "32b" : "16b",
#endif	/* CONFIG_X86_64 */
			   (type & LKDB_SEG_DESC_TYPE_A)?"ac":"",
			   (type & LKDB_SEG_DESC_TYPE_CODE_C)?"conf":"");
	} else {
		lkdb_printf("%-11s base=" kdb_machreg_fmt0 " limit="
			   kdb_machreg_fmt " dpl=%d %c%c%c %s %s %s \n",
			   "data",
			   kdb_seg_desc_base(d), LKDB_SEG_DESC_LIMIT(d),
			   d->dpl,
			   'r',
			   (type & LKDB_SEG_DESC_TYPE_DATA_W)?'w':'-',
			   '-',
			   d->d ? "32b" : "16b",
			   (type & LKDB_SEG_DESC_TYPE_A)?"ac":"",
			   (type & LKDB_SEG_DESC_TYPE_DATA_D)?"down":"");
	}
}

static int
lkdb_parse_two_numbers(int argc, const char **argv, int *sel, int *count,
		      int *last_sel, int *last_count)
{
	int diag;

	if (argc > 2)
		return LKDB_ARGCOUNT;

	lkdbgetintenv("MDCOUNT", count);

	if (argc == 0) {
		*sel = *last_sel;
		if (*last_count)
			*count = *last_count;
	} else {
		unsigned long val;

		if (argc >= 1) {
			diag = lkdbgetularg(argv[1], &val);
			if (diag)
				return diag;
			*sel = val;
		}
		if (argc >= 2) {
			diag = lkdbgetularg(argv[2], &val);
			if (diag)
				return diag;
			*count = (int) val;
			*last_count = (int) val;
		} else if (*last_count) {
			*count = *last_count;
		}
	}
	return 0;
}

/*
 * kdb_gdt
 *
 *	This function implements the 'gdt' command.
 *
 *	gdt [<selector> [<line count>]]
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
 */
static int
kdb_gdt(int argc, const char **argv)
{
	int sel = 0;
	struct desc_ptr gdtr;
	int diag, count = 8;
	kdb_desc_t *gdt;
	unsigned int max_sel;
	static int last_sel = 0, last_count = 0;

	diag = lkdb_parse_two_numbers(argc, argv, &sel, &count,
				     &last_sel, &last_count);
	if (diag)
		return diag;

	__asm__ __volatile__ ("sgdt %0\n\t" : "=m"(gdtr));
	gdt = (kdb_desc_t *) gdtr.address;

	max_sel = (gdtr.size + 1) / sizeof(kdb_desc_t);
	if (sel >= max_sel) {
		lkdb_printf("Maximum selector (%d) reached\n", max_sel);
		return 0;
	}

	if (sel + count > max_sel)
		count = max_sel - sel;

	while (count--) {
		kdb_desc_t *d = &gdt[sel];
		lkdb_printf("0x%4.4x ", sel++);

		if (!d->p) {
			lkdb_printf("not present\n");
			continue;
		}
		if (d->s) {
			display_seg_desc(d);
		} else {
			display_gate_desc((kdb_gate_desc_t *)d);
			if (LKDB_X86_64 && count) {
				++sel;	/* this descriptor occupies two slots */
				--count;
			}
		}
	}

	last_sel = sel;
	return 0;
}

/*
 * kdb_ldt
 *
 *	This function implements the 'ldt' command.
 *
 *	ldt [<selector> [<line count>]]
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
 */
static int
kdb_ldt(int argc, const char **argv)
{
	int sel = 0;
	struct desc_ptr gdtr;
	unsigned long ldtr = 0;
	int diag, count = 8;
	kdb_desc_t *ldt, *ldt_desc;
	unsigned int max_sel;
	static int last_sel = 0, last_count = 0;

	diag = lkdb_parse_two_numbers(argc, argv, &sel, &count,
				     &last_sel, &last_count);
	if (diag)
		return diag;

	if (strcmp(argv[0], "ldtp") == 0) {
		lkdb_printf("pid=%d, process=%s\n",
			   lkdb_current_task->pid, lkdb_current_task->comm);
		if (!lkdb_current_task->mm ||
		    !lkdb_current_task->mm->context.ldt) {
			lkdb_printf("no special LDT for this process\n");
			return 0;
		}
		ldt = lkdb_current_task->mm->context.ldt;
		max_sel = lkdb_current_task->mm->context.size;
	} else {

		/* sldt gives the GDT selector for the segment containing LDT */
		__asm__ __volatile__ ("sgdt %0\n\t" : "=m"(gdtr));
		__asm__ __volatile__ ("sldt %0\n\t" : "=m"(ldtr));
		ldtr &= 0xfff8;		/* extract the index */

		if (ldtr > gdtr.size+1) {
			lkdb_printf("invalid ldtr\n");
			return 0;
		}

		ldt_desc = (kdb_desc_t *)(gdtr.address + ldtr);
		ldt = (kdb_desc_t *)kdb_seg_desc_base(ldt_desc);
		max_sel = (LKDB_SEG_DESC_LIMIT(ldt_desc)+1) / sizeof(kdb_desc_t);
	}

	if (sel >= max_sel) {
		lkdb_printf("Maximum selector (%d) reached\n", max_sel);
		return 0;
	}

	if (sel + count > max_sel)
		count = max_sel - sel;

	while (count--) {
		kdb_desc_t *d = &ldt[sel];
		lkdb_printf("0x%4.4x ", sel++);

		if (!d->p) {
			lkdb_printf("not present\n");
			continue;
		}
		if (d->s) {
			display_seg_desc(d);
		} else {
			display_gate_desc((kdb_gate_desc_t *)d);
			if (LKDB_X86_64 && count) {
				++sel;	/* this descriptor occupies two slots */
				--count;
			}
		}
	}

	last_sel = sel;
	return 0;
}

/*
 * kdb_idt
 *
 *	This function implements the 'idt' command.
 *
 *	idt [<vector> [<line count>]]
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
 */
static int
kdb_idt(int argc, const char **argv)
{
	int vec = 0;
	struct desc_ptr idtr;
	int diag, count = 8;
	kdb_gate_desc_t *idt;
	unsigned int max_entries;
	static int last_vec = 0, last_count = 0;

	diag = lkdb_parse_two_numbers(argc, argv, &vec, &count,
				     &last_vec, &last_count);
	if (diag)
		return diag;

	__asm__ __volatile__ ("sidt %0\n\t" : "=m"(idtr));
	idt = (kdb_gate_desc_t *)idtr.address;

	max_entries = (idtr.size+1) / sizeof(kdb_gate_desc_t);
	if (vec >= max_entries) {
		lkdb_printf("Maximum vector (%d) reached\n", max_entries);
		return 0;
	}

	if (vec + count > max_entries)
		count = max_entries - vec;

	while (count--) {
		kdb_gate_desc_t *d = &idt[vec];
		lkdb_printf("0x%4.4x ", vec++);
		if (!d->p) {
			lkdb_printf("not present\n");
			continue;
		}
#ifndef	CONFIG_X86_64
		if (d->s) {
			lkdb_printf("invalid\n");
			continue;
		}
#endif	/* CONFIG_X86_64 */
		display_gate_desc(d);
	}

	last_vec = vec;

	return 0;
}

#if 0
static int
get_pagetables(unsigned long addr, pgd_t **pgdir, pmd_t **pgmiddle, pte_t **pte)
{
	pgd_t *d;
	pmd_t *m;
	pte_t *t;

	if (addr > PAGE_OFFSET) {
		d = pgd_offset_k(addr);
	} else {
		lkdb_printf("pid=%d, process=%s\n", lkdb_current_task->pid, lkdb_current_task->comm);
		d = pgd_offset(lkdb_current_task->mm, addr);
	}

	if (pgd_none(*d) || pgd_bad(*d)) {
		*pgdir = NULL;
		*pgmiddle = NULL;
		*pte = NULL;
		return 0;
	} else {
		*pgdir = d;
	}

	/* if _PAGE_PSE is set, pgdir points directly to the page. */
	if (pgd_val(*d) & _PAGE_PSE) {
		*pgmiddle = NULL;
		*pte = NULL;
		return 0;
	}

	m = pmd_offset(d, addr);
	if (pmd_none(*m) || pmd_bad(*m)) {
		*pgmiddle = NULL;
		*pte = NULL;
		return 0;
	} else {
		*pgmiddle = m;
	}

	t = pte_offset(m, addr);
	if (pte_none(*t)) {
		*pte = NULL;
		return 0;
	} else {
		*pte = t;
	}
	lkdb_printf("\naddr=%08lx, pgd=%08lx, pmd=%08lx, pte=%08lx\n",
			addr,
			(unsigned long) pgd_val(*d),
			(unsigned long) pmd_val(*m),
			(unsigned long) pte_val(*t));
	return 0;
}
#endif

#define FORMAT_PGDIR(entry) \
	lkdb_printf("frame=%05lx %c %s %c %c %c %s %c %s %s \n",\
			(entry >> PAGE_SHIFT), 				\
			(entry & _PAGE_PRESENT)?'p':'n', 		\
			(entry & _PAGE_RW)?"rw":"ro", 			\
			(entry & _PAGE_USER)?'u':'s', 			\
			(entry & _PAGE_ACCESSED)?'a':' ', 		\
			' ', 						\
			(entry & _PAGE_PSE)?"4M":"4K", 			\
			(entry & _PAGE_GLOBAL)?'g':' ', 		\
			(entry & _PAGE_PWT)?"wt":"wb", 			\
			(entry & _PAGE_PCD)?"cd":"  ");

#define FORMAT_PTE(p, entry) \
	lkdb_printf("frame=%05lx %c%c%c %c %c %c %s %c %s %s\n",	\
			(entry >> PAGE_SHIFT), 			\
			(pte_read(p))? 'r':'-', 		\
			(pte_write(p))? 'w':'-', 		\
			(pte_exec(p))? 'x':'-', 		\
			(pte_dirty(p))? 'd':' ', 		\
			(pte_young(p))? 'a':' ', 		\
			(entry & _PAGE_USER)? 'u':'s', 		\
			"  ", 					\
			(entry & _PAGE_GLOBAL)? 'g':' ',	\
			(entry & _PAGE_PWT)? "wt":"wb", 	\
			(entry & _PAGE_PCD)? "cd":"  ");
#if 0
static int
display_pgdir(unsigned long addr, pgd_t *pgdir, int count)
{
	unsigned long entry;
	int i;
	int index = pgdir - ((pgd_t *)(((unsigned long)pgdir) & PAGE_MASK));

	count = min(count, PTRS_PER_PGD - index);
	addr &= ~(PGDIR_SIZE-1);

	for (i = 0; i < count; i++, pgdir++) {
		entry = pgd_val(*pgdir);
		lkdb_printf("pgd: addr=%08lx ", addr);
		if (pgd_none(*pgdir)) {
			lkdb_printf("pgdir not present\n");
		} else {
			FORMAT_PGDIR(entry);
		}
		addr += PGDIR_SIZE;
	}
	return i;
}
#endif

#if 0	/* for now, let's not print pgmiddle. */
static int
display_pgmiddle(unsigned long addr, pmd_t *pgmiddle, int count)
{
	unsigned long entry;
	int i;
	int index = pgmiddle - ((pmd_t *)(((unsigned long)pgmiddle) & PAGE_MASK));

	count = min(count, PTRS_PER_PMD - index);
	addr &= ~(PMD_SIZE-1);

	for (i = 0; i < count; i++, pgmiddle++) {
		entry = pmd_val(*pgmiddle);
		lkdb_printf("pmd: addr=%08lx ", addr);
		if (pmd_none(*pgmiddle)) {
			lkdb_printf("pgmiddle not present\n");
		} else {
			FORMAT_PGDIR(entry);
		}
		addr += PMD_SIZE;
	}
	return i;
}
#endif

#if 0
static int
display_pte(unsigned long addr, pte_t *pte, int count)
{
	unsigned long entry;
	int i;
	int index = pte - ((pte_t *)(((unsigned long)pte) & PAGE_MASK));

	count = min(count, PTRS_PER_PTE - index);
	addr &= PAGE_MASK;

	for (i = 0; i < count; i++, pte++) {
		entry = pte_val(*pte);
		lkdb_printf("pte: addr=%08lx ", addr);
		if (pte_none(*pte)) {
			lkdb_printf("pte not present\n");
		} else if (!pte_present(*pte)) {
			lkdb_printf("page swapped out. swp_offset=%08lx ", SWP_OFFSET(pte_to_swp_entry(*pte)));
			lkdb_printf("swp_type=%8lx", SWP_TYPE(pte_to_swp_entry(*pte)));
		} else {
			FORMAT_PTE(*pte, entry);
		}
		addr += PAGE_SIZE;
	}
	return i;
}


/*
 * kdb_pte
 *
 *	This function implements the 'pte' command.
 *
 *	pte  <addr arg> [<line count>]
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
 */
static int
kdb_pte(int argc, const char **argv)
{
	static unsigned long last_addr = 0, last_count = 0;
	int count = 8;
	unsigned long addr;
	long offset = 0;
	pgd_t *pgdir;
	pmd_t *pgmiddle;
	pte_t *pte;

#ifdef CONFIG_X86_PAE
	lkdb_printf("This kernel is compiled with PAE support.");
	return LKDB_NOTIMP;
#endif
	lkdbgetintenv("MDCOUNT", &count);

	if (argc == 0) {
		if (last_addr == 0)
			return LKDB_ARGCOUNT;
		addr = last_addr;
		if (last_count)
			count = last_count;
	} else {
		kdb_machreg_t val;
		int diag, nextarg = 1;
		diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL);
		if (diag)
			return diag;
		if (argc > nextarg+1)
			return LKDB_ARGCOUNT;

		if (argc >= nextarg) {
			diag = lkdbgetularg(argv[nextarg], &val);
			if (!diag) {
				count = (int) val;
				last_count = count;
			} else if (last_count) {
				count = last_count;
			}
		}
	}

	/*
	 * round off the addr to a page boundary.
	 */
	addr &= PAGE_MASK;

	get_pagetables(addr, &pgdir, &pgmiddle, &pte);

	if (pgdir)
		display_pgdir(addr, pgdir, 1);
#if 0	/* for now, let's not print pgmiddle. */
	   if (pgmiddle)
		display_pgmiddle(addr, pgmiddle, 1);
#endif
	if (pte) {
		int displayed;
		displayed = display_pte(addr, pte, count);
		addr += (displayed << PAGE_SHIFT);
	}
	last_addr = addr;
	return 0;
}
#else
/*
 * Todo - In 2.5 the pte_offset macro in asm/pgtable.h seems to be
 * renamed to pte_offset_kernel.
 */
static int
kdb_pte(int argc, const char **argv)
{
	lkdb_printf("not supported.");
	return LKDB_NOTIMP;
}
#endif

/*
 * kdb_rdv
 *
 *	This function implements the 'rdv' command.
 *	It displays all registers of the current processor
 *	included control registers in verbose mode.
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
 * 	This should have been an option to rd command say "rd v",
 * 	but it is here as it is a non-essential x86-only command,
 * 	that need not clutter arch/i386/kdb/kdbasupport.c.
 */
static int
kdb_rdv(int argc, const char **argv)
{
	struct pt_regs *regs = get_irq_regs();
	kdba_dumpregs(regs, NULL, NULL);
	lkdb_printf("\n");
	display_eflags(regs->flags);
	lkdb_printf("\n");
	display_gdtr();
	display_idtr();
	display_ldtr();
	lkdb_printf("\n");
	display_cr0();
	display_cr3();
	display_cr4();
	display_cr8();
	lkdb_printf("\n");
	display_dr();
	return 0;
}

static int
kdb_rdmsr(int argc, const char **argv)
{
	unsigned long addr;
	uint32_t l, h;
	int diag;
	struct cpuinfo_x86 *c = &cpu_data(smp_processor_id());

	if (argc != 1)
		return LKDB_ARGCOUNT;

	if ((diag = lkdbgetularg(argv[1], &addr)))
		return diag;

	if (!cpu_has(c, X86_FEATURE_MSR))
		return LKDB_NOTIMP;

	lkdb_printf("msr(0x%lx) = ", addr);
	if ((diag = rdmsr_safe(addr, &l, &h))) {
		lkdb_printf("error %d\n", diag);
		return LKDB_BADINT;
	} else {
		lkdb_printf("0x%08x_%08x\n", h, l);
	}

	return 0;
}

static int
kdb_wrmsr(int argc, const char **argv)
{
	unsigned long addr;
	unsigned long l, h;
	int diag;
	struct cpuinfo_x86 *c = &cpu_data(smp_processor_id());

	if (argc != 3)
		return LKDB_ARGCOUNT;

	if ((diag = lkdbgetularg(argv[1], &addr))
			|| (diag = lkdbgetularg(argv[2], &h))
			|| (diag = lkdbgetularg(argv[3], &l)))
		return diag;

	if (!cpu_has(c, X86_FEATURE_MSR))
		return LKDB_NOTIMP;

	if ((diag = wrmsr_safe(addr, l, h))) {
		lkdb_printf("error %d\n", diag);
		return LKDB_BADINT;
	}

	return 0;
}

static int __init kdbm_x86_init(void)
{
	lkdb_register("rdv", kdb_rdv, NULL, "Display registers in verbose mode", 0);
	lkdb_register_repeat("gdt", kdb_gdt, "<sel> [<count>]", "Display GDT", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("idt", kdb_idt, "<int> [<count>]", "Display IDT", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("ldt", kdb_ldt, "<sel> [<count>]", "Display LDT", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("ptex", kdb_pte, "<addr> [<count>]", "Display pagetables", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register_repeat("ldtp", kdb_ldt, "<sel> [<count>]", "Display Process LDT", 0, LKDB_REPEAT_NO_ARGS);
	lkdb_register("rdmsr", kdb_rdmsr, "<maddr>", "Display Model Specific Register", 0);
	lkdb_register("wrmsr", kdb_wrmsr, "<maddr> <h> <l>", "Modify Model Specific Register", 0);
	return 0;
}

static void __exit kdbm_x86_exit(void)
{
	lkdb_unregister("rdv");
	lkdb_unregister("gdt");
	lkdb_unregister("ldt");
	lkdb_unregister("idt");
	lkdb_unregister("ptex");
	lkdb_unregister("ldtp");
	lkdb_unregister("rdmsr");
	lkdb_unregister("wrmsr");
}

module_init(kdbm_x86_init)
module_exit(kdbm_x86_exit)
