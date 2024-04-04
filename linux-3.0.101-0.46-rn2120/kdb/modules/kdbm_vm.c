/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2006 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/blkdev.h>
#include <linux/types.h>
#include <linux/lkdb.h>
#include <linux/kdbprivate.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/rmap.h>
#include <linux/swap.h>
#include <linux/swapops.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <asm/pgtable.h>

MODULE_AUTHOR("SGI");
MODULE_DESCRIPTION("Debug VM information");
MODULE_LICENSE("GPL");

struct __vmflags {
	unsigned long mask;
	char *name;
};

static struct __vmflags vmflags[] = {
	{ VM_READ, "VM_READ " },
	{ VM_WRITE, "VM_WRITE " },
	{ VM_EXEC, "VM_EXEC " },
	{ VM_SHARED, "VM_SHARED " },
	{ VM_MAYREAD, "VM_MAYREAD " },
	{ VM_MAYWRITE, "VM_MAYWRITE " },
	{ VM_MAYEXEC, "VM_MAYEXEC " },
	{ VM_MAYSHARE, "VM_MAYSHARE " },
	{ VM_GROWSDOWN, "VM_GROWSDOWN " },
	{ VM_GROWSUP, "VM_GROWSUP " },
	{ VM_PFNMAP, "VM_PFNMAP " },
	{ VM_DENYWRITE, "VM_DENYWRITE " },
	{ VM_EXECUTABLE, "VM_EXECUTABLE " },
	{ VM_LOCKED, "VM_LOCKED " },
	{ VM_IO, "VM_IO " },
	{ VM_SEQ_READ, "VM_SEQ_READ " },
	{ VM_RAND_READ, "VM_RAND_READ " },
	{ VM_DONTCOPY, "VM_DONTCOPY " },
	{ VM_DONTEXPAND, "VM_DONTEXPAND " },
	{ VM_RESERVED, "VM_RESERVED " },
	{ VM_ACCOUNT, "VM_ACCOUNT " },
	{ VM_NORESERVE, "VM_NORESERVE" },
	{ VM_HUGETLB, "VM_HUGETLB " },
	{ VM_NONLINEAR, "VM_NONLINEAR " },
#ifndef CONFIG_TRANSPARENT_HUGEPAGE
	{ VM_MAPPED_COPY, "VM_MAPPED_COPY " },
#else
	{ VM_HUGETLB, "VM_HUGEPAGE " },
#endif
	{ VM_INSERTPAGE, "VM_INSERTPAGE " },
	{ VM_ALWAYSDUMP, "VM_ALWAYSDUMP " },
	{ VM_CAN_NONLINEAR, "VM_CAN_NONLINEAR " },
	{ VM_MIXEDMAP, "VM_MIXEDMAP " },
	{ VM_SAO, "VM_SAO " },
	{ VM_MERGEABLE, "VM_MERGEABLE " },
	{ 0, "" }
};

static int
kdbm_print_vm(struct vm_area_struct *vp, unsigned long addr, int verbose_flg)
{
	struct __vmflags *tp;

	lkdb_printf("struct vm_area_struct at 0x%lx for %d bytes\n",
		   addr, (int) sizeof (struct vm_area_struct));

	lkdb_printf("vm_start = 0x%p   vm_end = 0x%p\n", (void *) vp->vm_start,
		   (void *) vp->vm_end);
	lkdb_printf("vm_page_prot = 0x%llx\n",
		(unsigned long long)pgprot_val(vp->vm_page_prot));

	lkdb_printf("vm_flags: ");
	for (tp = vmflags; tp->mask; tp++) {
		if (vp->vm_flags & tp->mask) {
			lkdb_printf(" %s", tp->name);
		}
	}
	lkdb_printf("\n");

	if (!verbose_flg)
		return 0;

	lkdb_printf("vm_mm = 0x%p\n", (void *) vp->vm_mm);
	lkdb_printf("vm_next = 0x%p\n", (void *) vp->vm_next);
	lkdb_printf("shared.vm_set.list.next = 0x%p\n", (void *) vp->shared.vm_set.list.next);
	lkdb_printf("shared.vm_set.list.prev = 0x%p\n", (void *) vp->shared.vm_set.list.prev);
	lkdb_printf("shared.vm_set.parent = 0x%p\n", (void *) vp->shared.vm_set.parent);
	lkdb_printf("shared.vm_set.head = 0x%p\n", (void *) vp->shared.vm_set.head);
	lkdb_printf("anon_vma.head.next = 0x%p\n", (void *) vp->anon_vma->head.next);
	lkdb_printf("anon_vma.head.prev = 0x%p\n", (void *) vp->anon_vma->head.prev);
	lkdb_printf("vm_ops = 0x%p\n", (void *) vp->vm_ops);
	if (vp->vm_ops != NULL) {
		lkdb_printf("vm_ops->open = 0x%p\n", vp->vm_ops->open);
		lkdb_printf("vm_ops->close = 0x%p\n", vp->vm_ops->close);
		lkdb_printf("vm_ops->fault = 0x%p\n", vp->vm_ops->fault);
#ifdef HAVE_VMOP_MPROTECT
		lkdb_printf("vm_ops->mprotect = 0x%p\n", vp->vm_ops->mprotect);
#endif
#ifdef CONFIG_NUMA
		lkdb_printf("vm_ops->set_policy = 0x%p\n", vp->vm_ops->set_policy);
		lkdb_printf("vm_ops->get_policy = 0x%p\n", vp->vm_ops->get_policy);
#endif
	}
	lkdb_printf("vm_pgoff = 0x%lx\n", vp->vm_pgoff);
	lkdb_printf("vm_file = 0x%p\n", (void *) vp->vm_file);
	lkdb_printf("vm_private_data = 0x%p\n", vp->vm_private_data);
#ifdef CONFIG_NUMA
	lkdb_printf("vm_policy = 0x%p\n", vp->vm_policy);
#endif

	return 0;
}

static int
kdbm_print_vmp(struct vm_area_struct *vp, int verbose_flg)
{
	struct __vmflags *tp;

	if (verbose_flg) {
		lkdb_printf("0x%lx:  ", (unsigned long) vp);
	}

	lkdb_printf("0x%p  0x%p ", (void *) vp->vm_start, (void *) vp->vm_end);

	for (tp = vmflags; tp->mask; tp++) {
		if (vp->vm_flags & tp->mask) {
			lkdb_printf(" %s", tp->name);
		}
	}
	lkdb_printf("\n");

	return 0;
}


#ifdef CONFIG_NUMA
#include <linux/mempolicy.h>

/*
 * kdbm_mpol
 *
 *	This function implements the 'mempolicy' command.
 *	Print a struct mempolicy.
 *
 *	mempolicy <address>	Print struct mempolicy at <address>
 */
static int
kdbm_mpol(int argc, const char **argv)
{
	unsigned long addr;
	long offset = 0;
	int nextarg;
	int err = 0;
	struct mempolicy *mp = NULL;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((err = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset,
				NULL)) != 0)
		return(err);

	if (!(mp = kmalloc(sizeof(*mp), GFP_ATOMIC))) {
		lkdb_printf("%s: cannot kmalloc mp\n", __FUNCTION__);
		goto out;
	}

	if ((err = lkdb_getarea(*mp, addr))) {
		lkdb_printf("%s: invalid mempolicy address\n", __FUNCTION__);
		goto out;
	}

	lkdb_printf("struct mempolicy at 0x%p\n", (struct mempolicy *)addr);
	lkdb_printf("  refcnt %d\n", atomic_read(&mp->refcnt));

	switch (mp->mode) {
	  case MPOL_DEFAULT:
		lkdb_printf("  mode %d (MPOL_DEFAULT)\n", mp->mode);
		break;

	  case MPOL_PREFERRED:
		lkdb_printf("  mode %d (MPOL_PREFERRED)\n", mp->mode);
		if (mp->flags & MPOL_F_LOCAL)
			lkdb_printf("  preferred_node local\n");
		else
			lkdb_printf("  preferred_node %d\n", mp->v.preferred_node);
		break;

	  case MPOL_BIND:
	  case MPOL_INTERLEAVE:
	  {
		int i, nlongs;
		unsigned long *longp;

		lkdb_printf("  mode %d (%s)\n", mp->mode,
			mp->mode == MPOL_INTERLEAVE
				? "MPOL_INTERLEAVE"
				: "MPOL_BIND");
		nlongs = (int)BITS_TO_LONGS(MAX_NUMNODES);
		lkdb_printf("  nodes:");
		longp = mp->v.nodes.bits;
		for (i = 0; i < nlongs; i++, longp++)
			lkdb_printf("  0x%lx ", *longp);
		lkdb_printf("\n");
		break;
	  }

	  default:
		lkdb_printf("  mode %d (unknown)\n", mp->mode);
		break;
	}
out:
	if (mp)
		kfree(mp);
	return err;
}

#endif /* CONFIG_NUMA */

/*
 * kdbm_pgdat
 *
 *	This function implements the 'pgdat' command.
 *	Print a struct pglist_data (pg_dat_t).
 *
 *	pgdat <node_id>		Print struct pglist_data for node <node_id>.
 *
 *	Print pglist_data for node 0 if node_id not specified,
 *	or print the one pglist_data structure if !CONFIG_NUMA.
 */
static int
kdbm_pgdat(int argc, const char **argv)
{
	int err = 0, node_id = 0, i;
	pg_data_t *pgdatp = NULL;

#ifdef CONFIG_NUMA
	if (argc > 1)
		return LKDB_ARGCOUNT;
	if (argc == 1) {
		int nextarg;
		long offset = 0;
		unsigned long node_id_ul;

		nextarg = 1;
		if ((err = lkdbgetaddrarg(argc, argv, &nextarg, &node_id_ul,
					 &offset, NULL)) != 0) {
			return(err);
		}
		node_id = (int)node_id_ul;
	}
#endif
	for_each_online_pgdat(pgdatp) {
		if (pgdatp->node_id == node_id)
			break;
	}
	if (!pgdatp) {
		lkdb_printf("%s: specified node not found\n", __FUNCTION__);
		return 0;
	}
	lkdb_printf("struct pglist_data at 0x%p  node_id = %d\n",
		   pgdatp, pgdatp->node_id);

	for (i = 0; i < MAX_ZONELISTS; i++) {
		int zr;
		struct zoneref *zonerefp;
		struct zone *zonep;

		zonerefp = pgdatp->node_zonelists[i]._zonerefs;
		lkdb_printf("  _zonerefs[%d] at 0x%p\n", i, zonerefp);

		for (zr = 0; zr <= MAX_ZONES_PER_ZONELIST; zr++, zonerefp++) {
			int z;
			pg_data_t *tmp_pgdatp;

			zonep = zonelist_zone(zonerefp);
			if (!zonep)
				break;

			lkdb_printf("    0x%p", zonep);

			for_each_online_pgdat(tmp_pgdatp) {
				for (z = 0; z < MAX_NR_ZONES; z++) {
					if (zonep == &tmp_pgdatp->node_zones[z]) {
						lkdb_printf ("  (node %d node_zones[%d])",
						     tmp_pgdatp->node_id, z);
						break;
					}
				}
				if (z != MAX_NR_ZONES)
					break;	/* found it */
			}
			lkdb_printf("\n");
		}
	}

	lkdb_printf("  nr_zones = %d", pgdatp->nr_zones);
#ifdef CONFIG_FLAT_NODE_MEM_MAP
	lkdb_printf("  node_mem_map = 0x%p\n", pgdatp->node_mem_map);
#endif
#ifndef CONFIG_NO_BOOTMEM
	lkdb_printf("  bdata = 0x%p", pgdatp->bdata);
#endif
	lkdb_printf("  node_start_pfn = 0x%lx\n", pgdatp->node_start_pfn);
	lkdb_printf("  node_present_pages = %ld (0x%lx)\n",
		   pgdatp->node_present_pages, pgdatp->node_present_pages);
	lkdb_printf("  node_spanned_pages = %ld (0x%lx)\n",
		   pgdatp->node_spanned_pages, pgdatp->node_spanned_pages);
	lkdb_printf("  kswapd = 0x%p\n", pgdatp->kswapd);

	return err;
}

/*
 * kdbm_vm
 *
 *     This function implements the 'vm' command.  Print a vm_area_struct.
 *
 *     vm [-v] <address>	Print vm_area_struct at <address>
 *     vmp [-v] <pid>		Print all vm_area_structs for <pid>
 */

static int
kdbm_vm(int argc, const char **argv)
{
	unsigned long addr;
	long offset = 0;
	int nextarg;
	int diag;
	int verbose_flg = 0;

	if (argc == 2) {
		if (strcmp(argv[1], "-v") != 0) {
			return LKDB_ARGCOUNT;
		}
		verbose_flg = 1;
	} else if (argc != 1) {
		return LKDB_ARGCOUNT;
	}

	if (strcmp(argv[0], "vmp") == 0) {
		struct task_struct *g, *tp;
		struct vm_area_struct *vp;
		pid_t pid;

		if ((diag = lkdbgetularg(argv[argc], (unsigned long *) &pid)))
			return diag;

		lkdb_do_each_thread(g, tp) {
			if (tp->pid == pid) {
				if (tp->mm != NULL) {
					if (verbose_flg)
						lkdb_printf
						    ("vm_area_struct       ");
					lkdb_printf
					    ("vm_start            vm_end              vm_flags\n");
					vp = tp->mm->mmap;
					while (vp != NULL) {
						kdbm_print_vmp(vp, verbose_flg);
						vp = vp->vm_next;
					}
				}
				return 0;
			}
		} lkdb_while_each_thread(g, tp);

		lkdb_printf("No process with pid == %d found\n", pid);

	} else {
		struct vm_area_struct v;

		nextarg = argc;
		if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset,
					  NULL))
		    || (diag = lkdb_getarea(v, addr)))
			return (diag);

		kdbm_print_vm(&v, addr, verbose_flg);
	}

	return 0;
}

static int
kdbm_print_pte(pte_t * pte)
{
	lkdb_printf("0x%lx (", (unsigned long) pte_val(*pte));

	if (pte_present(*pte)) {
#ifdef	pte_exec
		if (pte_exec(*pte))
			lkdb_printf("X");
#endif
		if (pte_write(*pte))
			lkdb_printf("W");
#ifdef	pte_read
		if (pte_read(*pte))
			lkdb_printf("R");
#endif
		if (pte_young(*pte))
			lkdb_printf("A");
		if (pte_dirty(*pte))
			lkdb_printf("D");

	} else {
		lkdb_printf("OFFSET=0x%lx ", swp_offset(pte_to_swp_entry(*pte)));
		lkdb_printf("TYPE=0x%ulx", swp_type(pte_to_swp_entry(*pte)));
	}

	lkdb_printf(")");

	/* final newline is output by caller of kdbm_print_pte() */

	return 0;
}

/*
 * kdbm_pte
 *
 *     This function implements the 'pte' command.  Print all pte_t structures
 *     that map to the given virtual address range (<address> through <address>
 *     plus <nbytes>) for the given process. The default value for nbytes is
 *     one.
 *
 *     pte -m <mm> <address> [<nbytes>]    Print all pte_t structures for
 *					   virtual <address> in address space
 *					   of <mm> which is a pointer to a
 *					   mm_struct
 *     pte -p <pid> <address> [<nbytes>]   Print all pte_t structures for
 *					   virtual <address> in address space
 *					   of <pid>
 */

static int
kdbm_pte(int argc, const char **argv)
{
	unsigned long addr;
	long offset = 0;
	int nextarg;
	unsigned long nbytes = 1;
	long npgs;
	int diag;
	int found;
	pid_t pid;
	struct task_struct *tp;
	struct mm_struct *mm, copy_of_mm;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	if (argc < 3 || argc > 4) {
		return LKDB_ARGCOUNT;
	}

	 if (strcmp(argv[1], "-p") == 0) {
		if ((diag = lkdbgetularg(argv[2], (unsigned long *) &pid))) {
			return diag;
		}

		found = 0;
		for_each_process(tp) {
			if (tp->pid == pid) {
				if (tp->mm != NULL) {
					found = 1;
					break;
				}
				lkdb_printf("task structure's mm field is NULL\n");
				return 0;
			}
		}

		if (!found) {
			lkdb_printf("No process with pid == %d found\n", pid);
			return 0;
		}
		mm = tp->mm;
	} else if (strcmp(argv[1], "-m") == 0) {


		nextarg = 2;
		if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset,
					  NULL))
		    || (diag = lkdb_getarea(copy_of_mm, addr)))
			return (diag);
		mm = &copy_of_mm;
	} else {
		return LKDB_ARGCOUNT;
	}

	if ((diag = lkdbgetularg(argv[3], &addr))) {
		return diag;
	}

	if (argc == 4) {
		if ((diag = lkdbgetularg(argv[4], &nbytes))) {
			return diag;
		}
	}

	lkdb_printf("vaddr              pte\n");

	npgs = ((((addr & ~PAGE_MASK) + nbytes) + ~PAGE_MASK) >> PAGE_SHIFT);
	while (npgs-- > 0) {

		lkdb_printf("0x%p ", (void *) (addr & PAGE_MASK));

		pgd = pgd_offset(mm, addr);
		if (pgd_present(*pgd)) {
			pud = pud_offset(pgd, addr);
			if (pud_present(*pud)) {
				pmd = pmd_offset(pud, addr);
				if (pmd_present(*pmd)) {
					pte = pte_offset_map(pmd, addr);
					if (pte_present(*pte)) {
						kdbm_print_pte(pte);
					}
				}
			}
		}

		lkdb_printf("\n");
		addr += PAGE_SIZE;
	}

	return 0;
}

/*
 * kdbm_rpte
 *
 *     This function implements the 'rpte' command.  Print all pte_t structures
 *     that contain the given physical page range (<pfn> through <pfn>
 *     plus <npages>) for the given process. The default value for npages is
 *     one.
 *
 *     rpte -m <mm> <pfn> [<npages>]	   Print all pte_t structures for
 *					   physical page <pfn> in address space
 *					   of <mm> which is a pointer to a
 *					   mm_struct
 *     rpte -p <pid> <pfn> [<npages>]	   Print all pte_t structures for
 *					   physical page <pfn> in address space
 *					   of <pid>
 */

static int
kdbm_rpte(int argc, const char **argv)
{
	unsigned long addr;
	unsigned long pfn;
	long offset = 0;
	int nextarg;
	unsigned long npages = 1;
	int diag;
	int found;
	pid_t pid;
	struct task_struct *tp;
	struct mm_struct *mm, copy_of_mm;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long g, u, m, t;

	if (argc < 3 || argc > 4) {
		return LKDB_ARGCOUNT;
	}

	 if (strcmp(argv[1], "-p") == 0) {
		if ((diag = lkdbgetularg(argv[2], (unsigned long *) &pid))) {
			return diag;
		}

		found = 0;
		for_each_process(tp) {
			if (tp->pid == pid) {
				if (tp->mm != NULL) {
					found = 1;
					break;
				}
				lkdb_printf("task structure's mm field is NULL\n");
				return 0;
			}
		}

		if (!found) {
			lkdb_printf("No process with pid == %d found\n", pid);
			return 0;
		}
		mm = tp->mm;
	} else if (strcmp(argv[1], "-m") == 0) {


		nextarg = 2;
		if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset,
					  NULL))
		    || (diag = lkdb_getarea(copy_of_mm, addr)))
			return (diag);
		mm = &copy_of_mm;
	} else {
		return LKDB_ARGCOUNT;
	}

	if ((diag = lkdbgetularg(argv[3], &pfn))) {
		return diag;
	}

	if (argc == 4) {
		if ((diag = lkdbgetularg(argv[4], &npages))) {
			return diag;
		}
	}

	/* spaces after vaddr depends on sizeof(unsigned long) */
	lkdb_printf("pfn              vaddr%*s pte\n",
		   (int)(2*sizeof(unsigned long) + 2 - 5), " ");

	for (g = 0, pgd = pgd_offset(mm, 0UL); g < PTRS_PER_PGD; ++g, ++pgd) {
		if (pgd_none(*pgd) || pgd_bad(*pgd))
			continue;
		for (u = 0, pud = pud_offset(pgd, 0UL); u < PTRS_PER_PUD; ++u, ++pud) {
			if (pud_none(*pud) || pud_bad(*pud))
				continue;
			for (m = 0, pmd = pmd_offset(pud, 0UL); m < PTRS_PER_PMD; ++m, ++pmd) {
				if (pmd_none(*pmd) || pmd_bad(*pmd))
					continue;
				for (t = 0, pte = pte_offset_map(pmd, 0UL); t < PTRS_PER_PTE; ++t, ++pte) {
					if (pte_none(*pte))
						continue;
					if (pte_pfn(*pte) < pfn || pte_pfn(*pte) >= (pfn + npages))
						continue;
					addr = g << PGDIR_SHIFT;
#ifdef __ia64__
					/* IA64 plays tricks with the pgd mapping to save space.
					 * This reverses pgd_index().
					 */
					{
						unsigned long region = g >> (PAGE_SHIFT - 6);
						unsigned long l1index = g - (region << (PAGE_SHIFT - 6));
						addr = (region << 61) + (l1index << PGDIR_SHIFT);
					}
#endif
					addr += (m << PMD_SHIFT) + (t << PAGE_SHIFT);
					lkdb_printf("0x%-14lx " kdb_bfd_vma_fmt0 " ",
						   pte_pfn(*pte), addr);
					kdbm_print_pte(pte);
					lkdb_printf("\n");
				}
			}
		}
	}

	return 0;
}

static int
kdbm_print_dentry(unsigned long daddr)
{
	struct dentry d;
	int diag;
	char buf[256];

	lkdb_printf("Dentry at 0x%lx\n", daddr);
	if ((diag = lkdb_getarea(d, (unsigned long)daddr)))
		return diag;

	if ((d.d_name.len > sizeof(buf)) || (diag = lkdb_getarea_size(buf, (unsigned long)(d.d_name.name), d.d_name.len)))
		lkdb_printf(" d_name.len = %d d_name.name = 0x%p\n",
					d.d_name.len, d.d_name.name);
	else
		lkdb_printf(" d_name.len = %d d_name.name = 0x%p <%.*s>\n",
					d.d_name.len, d.d_name.name,
					(int)(d.d_name.len), d.d_name.name);

	lkdb_printf(" d_count = %d d_flags = 0x%x d_inode = 0x%p\n",
					d.d_count, d.d_flags, d.d_inode);

	lkdb_printf(" d_parent = 0x%p\n", d.d_parent);

	lkdb_printf(" d_hash.nxt = 0x%p d_hash.prv = 0x%p\n",
					d.d_hash.next, d.d_hash.pprev);

	lkdb_printf(" d_lru.nxt = 0x%p d_lru.prv = 0x%p\n",
					d.d_lru.next, d.d_lru.prev);

	lkdb_printf(" d_child.nxt = 0x%p d_child.prv = 0x%p\n",
					d.d_u.d_child.next, d.d_u.d_child.prev);

	lkdb_printf(" d_subdirs.nxt = 0x%p d_subdirs.prv = 0x%p\n",
					d.d_subdirs.next, d.d_subdirs.prev);

	lkdb_printf(" d_alias.nxt = 0x%p d_alias.prv = 0x%p\n",
					d.d_alias.next, d.d_alias.prev);

	lkdb_printf(" d_op = 0x%p d_sb = 0x%p d_fsdata = 0x%p\n",
					d.d_op, d.d_sb, d.d_fsdata);

	lkdb_printf(" d_iname = %s\n",
					d.d_iname);

	if (d.d_inode) {
		struct inode i;
		lkdb_printf("\nInode Entry at 0x%p\n", d.d_inode);
		if ((diag = lkdb_getarea(i, (unsigned long)d.d_inode)))
			return diag;
		lkdb_printf(" i_mode = 0%o  i_nlink = %d  i_rdev = 0x%x\n",
						i.i_mode, i.i_nlink, i.i_rdev);

		lkdb_printf(" i_ino = %ld i_count = %d\n",
						i.i_ino, atomic_read(&i.i_count));

		lkdb_printf(" i_hash.nxt = 0x%p i_hash.prv = 0x%p\n",
						i.i_hash.next, i.i_hash.pprev);

		lkdb_printf(" i_wb_list.nxt = 0x%p i_wb_list.prv = 0x%p\n",
					i.i_wb_list.next, i.i_wb_list.prev);
		lkdb_printf(" i_lru.nxt = 0x%p i_lru.prv = 0x%p\n",
					i.i_lru.next, i.i_lru.prev);
		lkdb_printf(" i_sb_list.nxt = 0x%p i_sb_list.prv = 0x%p\n",
					i.i_sb_list.next, i.i_sb_list.prev);

		lkdb_printf(" i_dentry.nxt = 0x%p i_dentry.prv = 0x%p\n",
						i.i_dentry.next, i.i_dentry.prev);

	}
	lkdb_printf("\n");
	return 0;
}

static int
kdbm_filp(int argc, const char **argv)
{
	struct file   f;
	int nextarg;
	unsigned long addr;
	long offset;
	int diag;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)) ||
	    (diag = lkdb_getarea(f, addr)))
		return diag;

	lkdb_printf("File Pointer at 0x%lx\n", addr);

	lkdb_printf(" fu_list.nxt = 0x%p fu_list.prv = 0x%p\n",
			f.f_u.fu_list.next, f.f_u.fu_list.prev);

	lkdb_printf(" f_dentry = 0x%p f_vfsmnt = 0x%p f_op = 0x%p\n",
			f.f_dentry, f.f_vfsmnt, f.f_op);

	lkdb_printf(" f_count = " kdb_f_count_fmt
			" f_flags = 0x%x f_mode = 0x%x\n",
			atomic_long_read(&f.f_count), f.f_flags, f.f_mode);

	lkdb_printf(" f_pos = %Ld\n", f.f_pos);
#ifdef	CONFIG_SECURITY
	lkdb_printf(" security = 0x%p\n", f.f_security);
#endif

	lkdb_printf(" private_data = 0x%p f_mapping = 0x%p\n\n",
					f.private_data, f.f_mapping);

	return kdbm_print_dentry((unsigned long)f.f_dentry);
}

static int
kdbm_fl(int argc, const char **argv)
{
	struct file_lock fl;
	int nextarg;
	unsigned long addr;
	long offset;
	int diag;


	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)) ||
		(diag = lkdb_getarea(fl, addr)))
			return diag;

	lkdb_printf("File_lock at 0x%lx\n", addr);

	lkdb_printf(" fl_next = 0x%p fl_link.nxt = 0x%p fl_link.prv = 0x%p\n",
			fl.fl_next, fl.fl_link.next, fl.fl_link.prev);
	lkdb_printf(" fl_block.nxt = 0x%p fl_block.prv = 0x%p\n",
			fl.fl_block.next, fl.fl_block.prev);
	lkdb_printf(" fl_owner = 0x%p fl_pid = %d fl_wait = 0x%p\n",
			fl.fl_owner, fl.fl_pid, &fl.fl_wait);
	lkdb_printf(" fl_file = 0x%p fl_flags = 0x%x\n",
			fl.fl_file, fl.fl_flags);
	lkdb_printf(" fl_type = %d fl_start = 0x%llx fl_end = 0x%llx\n",
			fl.fl_type, fl.fl_start, fl.fl_end);

	lkdb_printf(" file_lock_operations");
	if (fl.fl_ops)
		lkdb_printf("\n   fl_copy_lock = 0x%p fl_release_private = 0x%p\n",
			fl.fl_ops->fl_copy_lock, fl.fl_ops->fl_release_private);
	else
		lkdb_printf("   empty\n");

	lkdb_printf(" lock_manager_operations");
	if (fl.fl_lmops)
		lkdb_printf("\n   lm_compare_owner = 0x%p lm_notify = 0x%p\n",
			fl.fl_lmops->fl_compare_owner, fl.fl_lmops->fl_notify);
	else
		lkdb_printf("   empty\n");

	lkdb_printf(" fl_fasync = 0x%p fl_break 0x%lx\n",
			fl.fl_fasync, fl.fl_break_time);

	return 0;
}


static int
kdbm_dentry(int argc, const char **argv)
{
	int nextarg;
	unsigned long addr;
	long offset;
	int diag;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)))
		return diag;

	return kdbm_print_dentry(addr);
}

static int
kdbm_kobject(int argc, const char **argv)
{
	struct kobject k;
	int nextarg;
	unsigned long addr;
	long offset;
	int diag;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)) ||
	    (diag = lkdb_getarea(k, addr)))
		return diag;


	lkdb_printf("kobject at 0x%lx\n", addr);

	if (k.name) {
		char c;
		lkdb_printf(" name 0x%p", k.name);
		if (lkdb_getarea(c, (unsigned long)k.name) == 0)
			lkdb_printf(" '%s'", k.name);
		lkdb_printf("\n");
	}

	if (k.name != kobject_name((struct kobject *)addr))
		lkdb_printf(" name '%.20s'\n", k.name);

	lkdb_printf(" kref.refcount %d'\n", atomic_read(&k.kref.refcount));

	lkdb_printf(" entry.next = 0x%p entry.prev = 0x%p\n",
					k.entry.next, k.entry.prev);

	lkdb_printf(" parent = 0x%p kset = 0x%p ktype = 0x%p sd = 0x%p\n",
					k.parent, k.kset, k.ktype, k.sd);

	return 0;
}

static int
kdbm_sh(int argc, const char **argv)
{
	int diag;
	int nextarg;
	unsigned long addr;
	long offset = 0L;
	struct Scsi_Host sh;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)) ||
	    (diag = lkdb_getarea(sh, addr)))
		return diag;

	lkdb_printf("Scsi_Host at 0x%lx\n", addr);
	lkdb_printf("host_queue = 0x%p\n", sh.__devices.next);
	lkdb_printf("ehandler = 0x%p eh_action = 0x%p\n",
		   sh.ehandler, sh.eh_action);
	lkdb_printf("host_wait = 0x%p hostt = 0x%p\n",
		   &sh.host_wait, sh.hostt);
	lkdb_printf("host_failed = %d  host_no = %d resetting = %d\n",
		   sh.host_failed, sh.host_no, sh.resetting);
	lkdb_printf("max id/lun/channel = [%d/%d/%d]  this_id = %d\n",
		   sh.max_id, sh.max_lun, sh.max_channel, sh.this_id);
	lkdb_printf("can_queue = %d cmd_per_lun = %d  sg_tablesize = %d u_isa_dma = %d\n",
		   sh.can_queue, sh.cmd_per_lun, sh.sg_tablesize, sh.unchecked_isa_dma);
	lkdb_printf("host_blocked = %d  reverse_ordering = %d \n",
		   sh.host_blocked, sh.reverse_ordering);

	return 0;
}

static int
kdbm_sd(int argc, const char **argv)
{
	int diag;
	int nextarg;
	unsigned long addr;
	long offset = 0L;
	struct scsi_device *sd = NULL;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)))
		goto out;
	if (!(sd = kmalloc(sizeof(*sd), GFP_ATOMIC))) {
		lkdb_printf("kdbm_sd: cannot kmalloc sd\n");
		goto out;
	}
	if ((diag = lkdb_getarea(*sd, addr)))
		goto out;

	lkdb_printf("scsi_device at 0x%lx\n", addr);
	lkdb_printf("next = 0x%p   prev = 0x%p  host = 0x%p\n",
		   sd->siblings.next, sd->siblings.prev, sd->host);
	lkdb_printf("device_busy = %d   current_cmnd 0x%p\n",
		   sd->device_busy, sd->current_cmnd);
	lkdb_printf("id/lun/chan = [%d/%d/%d]  single_lun = %d  device_blocked = %d\n",
		   sd->id, sd->lun, sd->channel, sd->sdev_target->single_lun, sd->device_blocked);
	lkdb_printf("queue_depth = %d current_tag = %d  scsi_level = %d\n",
		   sd->queue_depth, sd->current_tag, sd->scsi_level);
	lkdb_printf("%8.8s %16.16s %4.4s\n", sd->vendor, sd->model, sd->rev);
out:
	if (sd)
		kfree(sd);
	return diag;
}

static int
kdbm_sc(int argc, const char **argv)
{
	int diag;
	int nextarg;
	unsigned long addr;
	long offset = 0L;
	struct scsi_cmnd *sc = NULL;

	if (argc != 1)
		return LKDB_ARGCOUNT;

	nextarg = 1;
	if ((diag = lkdbgetaddrarg(argc, argv, &nextarg, &addr, &offset, NULL)))
		goto out;
	if (!(sc = kmalloc(sizeof(*sc), GFP_ATOMIC))) {
		lkdb_printf("kdbm_sc: cannot kmalloc sc\n");
		goto out;
	}
	if ((diag = lkdb_getarea(*sc, addr)))
		goto out;

	lkdb_printf("scsi_cmnd at 0x%lx\n", addr);
	lkdb_printf("device = 0x%p  next = 0x%p\n",
		   sc->device, sc->list.next);
	lkdb_printf("serial_number = %ld  retries = %d\n",
		   sc->serial_number, sc->retries);
	lkdb_printf("cmd_len = %d\n", sc->cmd_len);
	lkdb_printf("cmnd = [%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x/%2.2x]\n",
		   sc->cmnd[0], sc->cmnd[1], sc->cmnd[2], sc->cmnd[3], sc->cmnd[4],
		   sc->cmnd[5], sc->cmnd[6], sc->cmnd[7], sc->cmnd[8], sc->cmnd[9],
		   sc->cmnd[10], sc->cmnd[11]);
	lkdb_printf("request_buffer = 0x%p  request_bufflen = %d\n",
		   scsi_sglist(sc), scsi_bufflen(sc));
	lkdb_printf("use_sg = %d\n", scsi_sg_count(sc));
	lkdb_printf("underflow = %d transfersize = %d\n",
		   sc->underflow, sc->transfersize);
	lkdb_printf("tag = %d\n", sc->tag);

out:
	if (sc)
		kfree(sc);
	return diag;
}

static int __init kdbm_vm_init(void)
{
	lkdb_register("vm", kdbm_vm, "[-v] <vaddr>", "Display vm_area_struct", 0);
	lkdb_register("vmp", kdbm_vm, "[-v] <pid>", "Display all vm_area_struct for <pid>", 0);
#ifdef CONFIG_NUMA
	lkdb_register("mempolicy", kdbm_mpol, "<vaddr>", "Display mempolicy structure", 0);
	lkdb_register("pgdat", kdbm_pgdat, "<node_id>", "Display pglist_data node structure", 0);
#else
	lkdb_register("pgdat", kdbm_pgdat, "", "Display pglist_data node structure", 0);
#endif
	lkdb_register("pte", kdbm_pte, "( -m <mm> | -p <pid> ) <vaddr> [<nbytes>]", "Display pte_t for mm_struct or pid", 0);
	lkdb_register("rpte", kdbm_rpte, "( -m <mm> | -p <pid> ) <pfn> [<npages>]", "Find pte_t containing pfn for mm_struct or pid", 0);
	lkdb_register("dentry", kdbm_dentry, "<dentry>", "Display interesting dentry stuff", 0);
	lkdb_register("kobject", kdbm_kobject, "<kobject>", "Display interesting kobject stuff", 0);
	lkdb_register("filp", kdbm_filp, "<filp>", "Display interesting filp stuff", 0);
	lkdb_register("fl", kdbm_fl, "<fl>", "Display interesting file_lock stuff", 0);
	lkdb_register("sh", kdbm_sh, "<vaddr>", "Show scsi_host", 0);
	lkdb_register("sd", kdbm_sd, "<vaddr>", "Show scsi_device", 0);
	lkdb_register("sc", kdbm_sc, "<vaddr>", "Show scsi_cmnd", 0);

	return 0;
}

static void __exit kdbm_vm_exit(void)
{
	lkdb_unregister("vm");
	lkdb_unregister("vmp");
#ifdef CONFIG_NUMA
	lkdb_unregister("mempolicy");
#endif
	lkdb_unregister("pgdat");
	lkdb_unregister("pte");
	lkdb_unregister("rpte");
	lkdb_unregister("dentry");
	lkdb_unregister("kobject");
	lkdb_unregister("filp");
	lkdb_unregister("fl");
	lkdb_unregister("sh");
	lkdb_unregister("sd");
	lkdb_unregister("sc");
}

module_init(kdbm_vm_init)
module_exit(kdbm_vm_exit)
