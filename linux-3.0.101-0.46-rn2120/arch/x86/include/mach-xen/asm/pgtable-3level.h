#ifndef _ASM_X86_PGTABLE_3LEVEL_H
#define _ASM_X86_PGTABLE_3LEVEL_H

/*
 * Intel Physical Address Extension (PAE) Mode - three-level page
 * tables on PPro+ CPUs.
 *
 * Copyright (C) 1999 Ingo Molnar <mingo@redhat.com>
 */

#define pte_ERROR(e)							\
	printk("%s:%d: bad pte %p(%016Lx pfn %08lx).\n",		\
	        __FILE__, __LINE__, &(e), __pte_val(e), pte_pfn(e))
#define pmd_ERROR(e)							\
	printk("%s:%d: bad pmd %p(%016Lx pfn %08Lx).\n",		\
	       __FILE__, __LINE__, &(e), __pmd_val(e),			\
	       (pmd_val(e) & PTE_PFN_MASK) >> PAGE_SHIFT)
#define pgd_ERROR(e)							\
	printk("%s:%d: bad pgd %p(%016Lx pfn %08Lx).\n",		\
	       __FILE__, __LINE__, &(e), __pgd_val(e),			\
	       (pgd_val(e) & PTE_PFN_MASK) >> PAGE_SHIFT)

/* Rules for using set_pte: the pte being assigned *must* be
 * either not present or in a state where the hardware will
 * not attempt to update the pte.  In places where this is
 * not possible, use pte_get_and_clear to obtain the old pte
 * value and then use set_pte to update it.  -ben
 */

static inline void xen_set_pte(pte_t *ptep, pte_t pte)
{
	ptep->pte_high = pte.pte_high;
	smp_wmb();
	ptep->pte_low = pte.pte_low;
}

#define pmd_read_atomic pmd_read_atomic
/*
 * pte_offset_map_lock on 32bit PAE kernels was reading the pmd_t with
 * a "*pmdp" dereference done by gcc. Problem is, in certain places
 * where pte_offset_map_lock is called, concurrent page faults are
 * allowed, if the mmap_sem is hold for reading. An example is mincore
 * vs page faults vs MADV_DONTNEED. On the page fault side
 * pmd_populate rightfully does a set_64bit, but if we're reading the
 * pmd_t with a "*pmdp" on the mincore side, a SMP race can happen
 * because gcc will not read the 64bit of the pmd atomically. To fix
 * this all places running pmd_offset_map_lock() while holding the
 * mmap_sem in read mode, shall read the pmdp pointer using this
 * function to know if the pmd is null nor not, and in turn to know if
 * they can run pmd_offset_map_lock or pmd_trans_huge or other pmd
 * operations.
 *
 * Without THP if the mmap_sem is hold for reading, the pmd can only
 * transition from null to not null while pmd_read_atomic runs. So
 * we can always return atomic pmd values with this function.
 *
 * With THP if the mmap_sem is hold for reading, the pmd can become
 * trans_huge or none or point to a pte (and in turn become "stable")
 * at any time under pmd_read_atomic. We could read it really
 * atomically here with a atomic64_read for the THP enabled case (and
 * it would be a whole lot simpler), but to avoid using cmpxchg8b we
 * only return an atomic pmdval if the low part of the pmdval is later
 * found stable (i.e. pointing to a pte). And we're returning a none
 * pmdval if the low part of the pmd is none. In some cases the high
 * and low part of the pmdval returned may not be consistent if THP is
 * enabled (the low part may point to previously mapped hugepage,
 * while the high part may point to a more recently mapped hugepage),
 * but pmd_none_or_trans_huge_or_clear_bad() only needs the low part
 * of the pmd to be read atomically to decide if the pmd is unstable
 * or not, with the only exception of when the low part of the pmd is
 * zero in which case we return a none pmd.
 */
static inline pmd_t pmd_read_atomic(pmd_t *pmdp)
{
	pmdval_t ret;
	u32 *tmp = (u32 *)pmdp;

	ret = (pmdval_t) (*tmp);
	if (ret) {
		/*
		 * If the low part is null, we must not read the high part
		 * or we can end up with a partial pmd.
		 */
		smp_rmb();
		ret |= ((pmdval_t)*(tmp + 1)) << 32;
	}

	return (pmd_t) { ret };
}

static inline void xen_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	xen_l2_entry_update(pmdp, pmd);
}

static inline void xen_set_pud(pud_t *pudp, pud_t pud)
{
	xen_l3_entry_update(pudp, pud);
}

/*
 * For PTEs and PDEs, we must clear the P-bit first when clearing a page table
 * entry, so clear the bottom half first and enforce ordering with a compiler
 * barrier.
 */
static inline void __xen_pte_clear(pte_t *ptep)
{
	ptep->pte_low = 0;
	smp_wmb();
	ptep->pte_high = 0;
}

#define xen_pmd_clear(pmd)			\
({						\
	pmd_t *__pmdp = (pmd);			\
	PagePinned(virt_to_page(__pmdp))	\
	? set_pmd(__pmdp, __pmd(0))		\
	: (void)(*__pmdp = __pmd(0));		\
})

static inline void __xen_pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));

	/*
	 * According to Intel App note "TLBs, Paging-Structure Caches,
	 * and Their Invalidation", April 2007, document 317080-001,
	 * section 8.1: in PAE mode we explicitly have to flush the
	 * TLB via cr3 if the top-level pgd is changed...
	 *
	 * Currently all places where pud_clear() is called either have
	 * flush_tlb_mm() followed or don't need TLB flush (x86_64 code or
	 * pud_clear_bad()), so we don't need TLB flush here.
	 */
}

#define xen_pud_clear(pudp)			\
({						\
	pud_t *__pudp = (pudp);			\
	PagePinned(virt_to_page(__pudp))	\
	? __xen_pud_clear(__pudp)		\
	: (void)(*__pudp = __pud(0));		\
})

#ifdef CONFIG_SMP
static inline pte_t xen_ptep_get_and_clear(pte_t *ptep, pte_t res)
{
	uint64_t val = __pte_val(res);
	if (__cmpxchg64(&ptep->pte, val, 0) != val) {
		/* xchg acts as a barrier before the setting of the high bits */
		res.pte_low = xchg(&ptep->pte_low, 0);
		res.pte_high = ptep->pte_high;
		ptep->pte_high = 0;
	}
	return res;
}
#else
#define xen_ptep_get_and_clear(xp, pte) xen_local_ptep_get_and_clear(xp, pte)
#endif

#define __pte_mfn(_pte) (((_pte).pte_low >> PAGE_SHIFT) | \
			 ((_pte).pte_high << (32-PAGE_SHIFT)))

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#ifdef CONFIG_SMP
union split_pmd {
	struct {
		u32 pmd_low;
		u32 pmd_high;
	};
	pmd_t pmd;
};
static inline pmd_t xen_pmdp_get_and_clear(pmd_t *pmdp)
{
	union split_pmd res, *orig = (union split_pmd *)pmdp;

	/* xchg acts as a barrier before setting of the high bits */
	res.pmd_low = xchg(&orig->pmd_low, 0);
	res.pmd_high = orig->pmd_high;
	orig->pmd_high = 0;

	return res.pmd;
}
#else
#define xen_pmdp_get_and_clear(xp) xen_local_pmdp_get_and_clear(xp)
#endif
#endif

/*
 * Bits 0, 6 and 7 are taken in the low part of the pte,
 * put the 32 bits of offset into the high part.
 */
#define pte_to_pgoff(pte) ((pte).pte_high)
#define pgoff_to_pte(off)						\
	((pte_t) { { .pte_low = _PAGE_FILE, .pte_high = (off) } })
#define PTE_FILE_MAX_BITS       32

/* Encode and de-code a swap entry */
#define MAX_SWAPFILES_CHECK() BUILD_BUG_ON(MAX_SWAPFILES_SHIFT > 5)
#define __swp_type(x)			(((x).val) & 0x1f)
#define __swp_offset(x)			((x).val >> 5)
#define __swp_entry(type, offset)	((swp_entry_t){(type) | (offset) << 5})
#define __pte_to_swp_entry(pte)		((swp_entry_t){ (pte).pte_high })
#define __swp_entry_to_pte(x)		((pte_t){ { .pte_high = (x).val } })

#endif /* _ASM_X86_PGTABLE_3LEVEL_H */
