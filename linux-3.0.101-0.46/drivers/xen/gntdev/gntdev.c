/******************************************************************************
 * gntdev.c
 * 
 * Device for accessing (in user-space) pages that have been granted by other
 * domains.
 *
 * Copyright (c) 2006-2007, D G Murray.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <asm/atomic.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <xen/gnttab.h>
#include <asm/hypervisor.h>
#include <xen/balloon.h>
#include <xen/evtchn.h>
#include <xen/public/gntdev.h>


#define DRIVER_AUTHOR "Derek G. Murray <Derek.Murray@cl.cam.ac.uk>"
#define DRIVER_DESC   "User-space granted page access driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

#define GNTDEV_NAME "gntdev"
MODULE_ALIAS("devname:xen/" GNTDEV_NAME);

#define MAX_GRANTS_LIMIT   1024
#define DEFAULT_MAX_GRANTS 128

/* A slot can be in one of three states:
 *
 * 0. GNTDEV_SLOT_INVALID:
 *    This slot is not associated with a grant reference, and is therefore free
 *    to be overwritten by a new grant reference.
 *
 * 1. GNTDEV_SLOT_NOT_YET_MAPPED:
 *    This slot is associated with a grant reference (via the 
 *    IOCTL_GNTDEV_MAP_GRANT_REF ioctl), but it has not yet been mmap()-ed.
 *
 * 2. GNTDEV_SLOT_MAPPED:
 *    This slot is associated with a grant reference, and has been mmap()-ed.
 */
typedef enum gntdev_slot_state {
	GNTDEV_SLOT_INVALID = 0,
	GNTDEV_SLOT_NOT_YET_MAPPED,
	GNTDEV_SLOT_MAPPED
} gntdev_slot_state_t;

#define GNTDEV_INVALID_HANDLE    -1
#define GNTDEV_FREE_LIST_INVALID -1
/* Each opened instance of gntdev is associated with a list of grants,
 * represented by an array of elements of the following type,
 * gntdev_grant_info_t.
 */
typedef struct gntdev_grant_info {
	gntdev_slot_state_t state;
	union {
		uint32_t free_list_index;
		struct {
			domid_t domid;
			grant_ref_t ref;
			grant_handle_t kernel_handle;
			grant_handle_t user_handle;
		} valid;
	} u;
} gntdev_grant_info_t;

/* Private data structure, which is stored in the file pointer for files
 * associated with this device.
 */
typedef struct gntdev_file_private_data {
  
	/* Array of grant information. */
	gntdev_grant_info_t *grants;
	uint32_t grants_size;

	/* Read/write semaphore used to protect the grants array. */
	struct rw_semaphore grants_sem;

	/* An array of indices of free slots in the grants array.
	 * N.B. An entry in this list may temporarily have the value
	 * GNTDEV_FREE_LIST_INVALID if the corresponding slot has been removed
	 * from the list by the contiguous allocator, but the list has not yet
	 * been compressed. However, this is not visible across invocations of
	 * the device.
	 */
	int32_t *free_list;
	
	/* The number of free slots in the grants array. */
	uint32_t free_list_size;

	/* Read/write semaphore used to protect the free list. */
	struct rw_semaphore free_list_sem;
	
	/* Index of the next slot after the most recent contiguous allocation, 
	 * for use in a next-fit allocator.
	 */
	uint32_t next_fit_index;

	/* Used to map grants into the kernel, before mapping them into user
	 * space.
	 */
	struct page **foreign_pages;

} gntdev_file_private_data_t;

/* Module lifecycle operations. */
static int __init gntdev_init(void);
static void __exit gntdev_exit(void);

module_init(gntdev_init);
module_exit(gntdev_exit);

/* File operations. */
static int gntdev_open(struct inode *inode, struct file *flip);
static int gntdev_release(struct inode *inode, struct file *flip);
static int gntdev_mmap(struct file *flip, struct vm_area_struct *vma);
static long gntdev_ioctl(struct file *flip,
			 unsigned int cmd, unsigned long arg);

static const struct file_operations gntdev_fops = {
	.owner = THIS_MODULE,
	.open = gntdev_open,
	.llseek = no_llseek,
	.release = gntdev_release,
	.mmap = gntdev_mmap,
	.unlocked_ioctl = gntdev_ioctl
};

/* VM operations. */
static void gntdev_vma_close(struct vm_area_struct *vma);
static pte_t gntdev_clear_pte(struct vm_area_struct *vma, unsigned long addr,
			      pte_t *ptep, int is_fullmm);

static struct vm_operations_struct gntdev_vmops = {
	.close = gntdev_vma_close,
	.zap_pte = gntdev_clear_pte
};

/* Memory mapping functions
 * ------------------------
 *
 * Every granted page is mapped into both kernel and user space, and the two
 * following functions return the respective virtual addresses of these pages.
 *
 * When shadow paging is disabled, the granted page is mapped directly into
 * user space; when it is enabled, it is mapped into the kernel and remapped
 * into user space using vm_insert_page() (see gntdev_mmap(), below).
 */

/* Returns the virtual address (in user space) of the @page_index'th page
 * in the given VM area.
 */
static inline unsigned long get_user_vaddr (struct vm_area_struct *vma,
					    int page_index)
{
	return (unsigned long) vma->vm_start + (page_index << PAGE_SHIFT);
}

/* Returns the virtual address (in kernel space) of the @slot_index'th page
 * mapped by the gntdev instance that owns the given private data struct.
 */
static inline unsigned long get_kernel_vaddr (gntdev_file_private_data_t *priv,
					      int slot_index)
{
	unsigned long pfn;
	void *kaddr;
	pfn = page_to_pfn(priv->foreign_pages[slot_index]);
	kaddr = pfn_to_kaddr(pfn);
	return (unsigned long) kaddr;
}

/* Helper functions. */

/* Adds information about a grant reference to the list of grants in the file's
 * private data structure. Returns non-zero on failure. On success, sets the
 * value of *offset to the offset that should be mmap()-ed in order to map the
 * grant reference.
 */
static int add_grant_reference(gntdev_file_private_data_t *private_data,
			       struct ioctl_gntdev_grant_ref *op,
			       uint64_t *offset)
{
	uint32_t slot_index;

	slot_index = private_data->free_list[--private_data->free_list_size];
	private_data->free_list[private_data->free_list_size]
		= GNTDEV_FREE_LIST_INVALID;

	/* Copy the grant information into file's private data. */
	private_data->grants[slot_index].state = GNTDEV_SLOT_NOT_YET_MAPPED;
	private_data->grants[slot_index].u.valid.domid = op->domid;
	private_data->grants[slot_index].u.valid.ref = op->ref;

	/* The offset is calculated as the index of the chosen entry in the
	 * file's private data's array of grant information. This is then
	 * shifted to give an offset into the virtual "file address space".
	 */
	*offset = slot_index << PAGE_SHIFT;

	return 0;
}

/* Adds the @count grant references to the contiguous range in the slot array
 * beginning at @first_slot. It is assumed that @first_slot was returned by a
 * previous invocation of find_contiguous_free_range(), during the same
 * invocation of the driver.
 */
static int add_grant_references(gntdev_file_private_data_t *private_data,
				uint32_t count,
				struct ioctl_gntdev_grant_ref *ops,
				uint32_t first_slot)
{
	uint32_t i;
	
	for (i = 0; i < count; ++i) {

		/* First, mark the slot's entry in the free list as invalid. */
		uint32_t free_list_index =
			private_data->grants[first_slot+i].u.free_list_index;
		private_data->free_list[free_list_index] = 
			GNTDEV_FREE_LIST_INVALID;

		/* Now, update the slot. */
		private_data->grants[first_slot+i].state = 
			GNTDEV_SLOT_NOT_YET_MAPPED;
		private_data->grants[first_slot+i].u.valid.domid =
			ops[i].domid;
		private_data->grants[first_slot+i].u.valid.ref = ops[i].ref;
	}

	return 0;	
}

/* Scans through the free list for @flip, removing entries that are marked as
 * GNTDEV_SLOT_INVALID. This will reduce the recorded size of the free list to
 * the number of valid entries.
 */
static void compress_free_list(gntdev_file_private_data_t *private_data)
{
	uint32_t i, j = 0, old_size;
	
	old_size = private_data->free_list_size;
	for (i = 0; i < old_size; ++i) {
		if (private_data->free_list[i] != GNTDEV_FREE_LIST_INVALID) {
			if (i > j) {
				int32_t slot_index;

				slot_index = private_data->free_list[i];
				private_data->free_list[j] = slot_index;
				private_data->grants[slot_index].u
					.free_list_index = j;
				private_data->free_list[i] 
					= GNTDEV_FREE_LIST_INVALID;
			}
			++j;
		} else {
			--private_data->free_list_size;
		}
	}
}

/* Searches the grant array in the private data of @flip for a range of
 * @num_slots contiguous slots in the GNTDEV_SLOT_INVALID state.
 *
 * Returns the index of the first slot if a range is found, otherwise -ENOMEM.
 */
static int find_contiguous_free_range(gntdev_file_private_data_t *private_data,
				      uint32_t num_slots) 
{
	/* First search from next_fit_index to the end of the array. */
	uint32_t start_index = private_data->next_fit_index;
	uint32_t end_index = private_data->grants_size;

	for (;;) {
		uint32_t i, range_start = 0, range_length = 0;

		for (i = start_index; i < end_index; ++i) {
			if (private_data->grants[i].state == GNTDEV_SLOT_INVALID) {
				if (range_length == 0)
					range_start = i;
				if (++range_length == num_slots)
					return range_start;
			} else
				range_length = 0;
		}
		/* Now search from the start of the array to next_fit_index. */
		if (!start_index)
			break;
		end_index = start_index;
		start_index = 0;
	}
	
	return -ENOMEM;
}

static int init_private_data(gntdev_file_private_data_t *priv,
			     uint32_t max_grants)
{
	int i;

	/* Allocate space for the kernel-mapping of granted pages. */
	priv->foreign_pages = 
		alloc_empty_pages_and_pagevec(max_grants);
	if (!priv->foreign_pages)
		goto nomem_out;

	/* Allocate the grant list and free-list. */
	priv->grants = kmalloc(max_grants * sizeof(gntdev_grant_info_t),
			       GFP_KERNEL);
	if (!priv->grants)
		goto nomem_out2;
	priv->free_list = kmalloc(max_grants * sizeof(int32_t), GFP_KERNEL);
	if (!priv->free_list)
		goto nomem_out3;

	/* Initialise the free-list, which contains all slots at first. */
	for (i = 0; i < max_grants; ++i) {
		priv->free_list[max_grants - i - 1] = i;
		priv->grants[i].state = GNTDEV_SLOT_INVALID;
		priv->grants[i].u.free_list_index = max_grants - i - 1;
	}
	priv->grants_size = max_grants;
	priv->free_list_size = max_grants;
	priv->next_fit_index = 0;

	return 0;

nomem_out3:
	kfree(priv->grants);
nomem_out2:
	free_empty_pages_and_pagevec(priv->foreign_pages, max_grants);
nomem_out:
	return -ENOMEM;

}

/* Interface functions. */

static struct miscdevice gntdev_miscdev = {
	.minor        = MISC_DYNAMIC_MINOR,
	.name         = GNTDEV_NAME,
	.nodename     = "xen/" GNTDEV_NAME,
	.fops         = &gntdev_fops,
};

/* Initialises the driver. Called when the module is loaded. */
static int __init gntdev_init(void)
{
	int err;

	if (!is_running_on_xen()) {
		pr_err("You must be running Xen to use gntdev\n");
		return -ENODEV;
	}

	err = misc_register(&gntdev_miscdev);
	if (err)
	{
		pr_err("Could not register gntdev device\n");
		return err;
	}

	return 0;
}

/* Cleans up and unregisters the driver. Called when the driver is unloaded.
 */
static void __exit gntdev_exit(void)
{
	misc_deregister(&gntdev_miscdev);
}

/* Called when the device is opened. */
static int gntdev_open(struct inode *inode, struct file *flip)
{
	gntdev_file_private_data_t *private_data;

	nonseekable_open(inode, flip);

	/* Allocate space for the per-instance private data. */
	private_data = kmalloc(sizeof(*private_data), GFP_KERNEL);
	if (!private_data)
		goto nomem_out;

	/* These will be lazily initialised by init_private_data. */
	private_data->grants = NULL;
	private_data->free_list = NULL;
	private_data->foreign_pages = NULL;

	init_rwsem(&private_data->grants_sem);
	init_rwsem(&private_data->free_list_sem);

	flip->private_data = private_data;

	return 0;

nomem_out:
	return -ENOMEM;
}

/* Called when the device is closed.
 */
static int gntdev_release(struct inode *inode, struct file *flip)
{
	if (flip->private_data) {
		gntdev_file_private_data_t *private_data = 
			(gntdev_file_private_data_t *) flip->private_data;
		if (private_data->foreign_pages)
			free_empty_pages_and_pagevec
				(private_data->foreign_pages,
				 private_data->grants_size);
		if (private_data->grants) 
			kfree(private_data->grants);
		if (private_data->free_list)
			kfree(private_data->free_list);
		kfree(private_data);
	}
	return 0;
}

/* Called when an attempt is made to mmap() the device. The private data from
 * @flip contains the list of grant references that can be mapped. The vm_pgoff
 * field of @vma contains the index into that list that refers to the grant
 * reference that will be mapped. Only mappings that are a multiple of
 * PAGE_SIZE are handled.
 */
static int gntdev_mmap (struct file *flip, struct vm_area_struct *vma) 
{
	struct gnttab_map_grant_ref op;
	unsigned long slot_index = vma->vm_pgoff;
	unsigned long kernel_vaddr, user_vaddr, mfn;
	unsigned long size = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
	uint64_t ptep;
	int ret, exit_ret;
	unsigned int i, flags;
	struct page *page;
	gntdev_file_private_data_t *private_data = flip->private_data;
	gntdev_grant_info_t *grants;
	struct vm_foreign_map *foreign_map;

	if (unlikely(!private_data)) {
		pr_err("file's private data is NULL\n");
		return -EINVAL;
	}

	/* Test to make sure that the grants array has been initialised. */
	down_read(&private_data->grants_sem);
	grants = private_data->grants;
	up_read(&private_data->grants_sem);

	if (unlikely(!grants)) {
		pr_err("attempted to mmap before ioctl\n");
		return -EINVAL;
	}
	grants += slot_index;

	if (unlikely(size + slot_index <= slot_index ||
		     size + slot_index > private_data->grants_size)) {
		pr_err("Invalid number of pages or offset"
		       "(num_pages = %lu, first_slot = %lu)\n",
		       size, slot_index);
		return -ENXIO;
	}

	if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) {
		pr_err("writable mappings must be shared\n");
		return -EINVAL;
	}

	foreign_map = kmalloc(sizeof(*foreign_map), GFP_KERNEL);
	if (!foreign_map) {
		pr_err("couldn't allocate mapping structure for VM area\n");
		return -ENOMEM;
	}
	foreign_map->map = &private_data->foreign_pages[slot_index];

	/* Slots must be in the NOT_YET_MAPPED state. */
	down_write(&private_data->grants_sem);
	for (i = 0; i < size; ++i) {
		if (grants[i].state != GNTDEV_SLOT_NOT_YET_MAPPED) {
			pr_err("Slot (index = %lu) is in the wrong state (%d)\n",
			       slot_index + i, grants[i].state);
			up_write(&private_data->grants_sem);
			kfree(foreign_map);
			return -EINVAL;
		}
	}

	/* Install the hook for unmapping. */
	vma->vm_ops = &gntdev_vmops;
    
	/* The VM area contains pages from another VM. */
	vma->vm_private_data = foreign_map;
	vma->vm_flags |= VM_FOREIGN;

	/* This flag prevents Bad PTE errors when the memory is unmapped. */
	vma->vm_flags |= VM_RESERVED;

	/* This flag prevents this VM area being copied on a fork(). A better
	 * behaviour might be to explicitly carry out the appropriate mappings
	 * on fork(), but I don't know if there's a hook for this.
	 */
	vma->vm_flags |= VM_DONTCOPY;

#ifdef CONFIG_X86
	/* This flag ensures that the page tables are not unpinned before the
	 * VM area is unmapped. Therefore Xen still recognises the PTE as
	 * belonging to an L1 pagetable, and the grant unmap operation will
	 * succeed, even if the process does not exit cleanly.
	 */
	vma->vm_mm->context.has_foreign_mappings = 1;
#endif

	exit_ret = -ENOMEM;
	for (i = 0; i < size; ++i) {

		flags = GNTMAP_host_map;
		if (!(vma->vm_flags & VM_WRITE))
			flags |= GNTMAP_readonly;

		kernel_vaddr = get_kernel_vaddr(private_data, slot_index + i);
		page = private_data->foreign_pages[slot_index + i];

		gnttab_set_map_op(&op, kernel_vaddr, flags,   
				  grants[i].u.valid.ref,
				  grants[i].u.valid.domid);

		/* Carry out the mapping of the grant reference. */
		ret = HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref, 
						&op, 1);
		BUG_ON(ret);
		if (op.status != GNTST_okay) {
			if (op.status != GNTST_eagain)
				pr_err("Error mapping the grant reference "
				       "into the kernel (%d). domid = %d; ref = %d\n",
				       op.status,
				       grants[i].u.valid.domid,
				       grants[i].u.valid.ref);
			else
				/* Propagate eagain instead of trying to fix it up */
				exit_ret = -EAGAIN;
			goto undo_map_out;
		}

		/* Mark mapped page as reserved. */
		SetPageReserved(page);

		/* Record the grant handle, for use in the unmap operation. */
		grants[i].u.valid.kernel_handle = op.handle;
		
		grants[i].state = GNTDEV_SLOT_MAPPED;
		grants[i].u.valid.user_handle = GNTDEV_INVALID_HANDLE;

		/* Now perform the mapping to user space. */
		user_vaddr = get_user_vaddr(vma, i);

		if (xen_feature(XENFEAT_auto_translated_physmap)) {
			/* USING SHADOW PAGE TABLES. */
			/* In this case, we simply insert the page into the VM
			 * area. */
			ret = vm_insert_page(vma, user_vaddr, page);
			if (!ret)
				continue;
			exit_ret = ret;
			goto undo_map_out;
		}

		/* NOT USING SHADOW PAGE TABLES. */
		/* In this case, we map the grant(s) straight into user
		 * space.
		 */
		mfn = op.dev_bus_addr >> PAGE_SHIFT;

		/* Get the machine address of the PTE for the user page. */
		if ((ret = create_lookup_pte_addr(vma->vm_mm,
						  user_vaddr,
						  &ptep)))
		{
			pr_err("Error obtaining PTE pointer (%d)\n", ret);
			goto undo_map_out;
		}

		/* Configure the map operation. */

		/* Specifies a user space mapping. */
		flags |= GNTMAP_application_map;

		/* The map request contains the machine address of the
		 * PTE to update.
		 */
		flags |= GNTMAP_contains_pte;

		gnttab_set_map_op(&op, ptep, flags,
				  grants[i].u.valid.ref,
				  grants[i].u.valid.domid);

		/* Carry out the mapping of the grant reference. */
		ret = HYPERVISOR_grant_table_op(GNTTABOP_map_grant_ref,
						&op, 1);
		BUG_ON(ret);
		if (op.status != GNTST_okay) {
			pr_err("Error mapping the grant reference "
			       "into user space (%d). domid = %d; ref = %d\n",
			       op.status,
			       grants[i].u.valid.domid,
			       grants[i].u.valid.ref);
			/* This should never happen after we've mapped into
			 * the kernel space. */
			BUG_ON(op.status == GNTST_eagain);
			goto undo_map_out;
		}

		/* Record the grant handle, for use in the unmap
		 * operation.
		 */
		grants[i].u.valid.user_handle = op.handle;

		/* Update p2m structure with the new mapping. */
		set_phys_to_machine(__pa(kernel_vaddr) >> PAGE_SHIFT,
				    FOREIGN_FRAME(mfn));
	}
	exit_ret = 0;

	up_write(&private_data->grants_sem);
	return exit_ret;

undo_map_out:
	/* If we have a mapping failure, the unmapping will be taken care of
	 * by do_mmap_pgoff(), which will eventually call gntdev_clear_pte().
	 * All we need to do here is free the vma_private_data.
	 */
	vma->vm_flags &= ~VM_FOREIGN;
	kfree(foreign_map);

	/* THIS IS VERY UNPLEASANT: do_mmap_pgoff() will set the vma->vm_file
	 * to NULL on failure. However, we need this in gntdev_clear_pte() to
	 * unmap the grants. Therefore, we smuggle a reference to the file's
	 * private data in the VM area's private data pointer.
	 */
	vma->vm_private_data = private_data;
	
	up_write(&private_data->grants_sem);

	return exit_ret;
}

static pte_t gntdev_clear_pte(struct vm_area_struct *vma, unsigned long addr,
			      pte_t *ptep, int is_fullmm)
{
	int ret;
	unsigned int nr;
	unsigned long slot_index;
	pte_t copy;
	struct gnttab_unmap_grant_ref op[2];
	gntdev_file_private_data_t *private_data;

	/* THIS IS VERY UNPLEASANT: do_mmap_pgoff() will set the vma->vm_file
	 * to NULL on failure. However, we need this in gntdev_clear_pte() to
	 * unmap the grants. Therefore, we smuggle a reference to the file's
	 * private data in the VM area's private data pointer.
	 */
	if (vma->vm_file) {
		private_data = (gntdev_file_private_data_t *)
			vma->vm_file->private_data;
	} else if (vma->vm_private_data) {
		private_data = (gntdev_file_private_data_t *)
			vma->vm_private_data;
	} else {
		private_data = NULL; /* gcc warning */
		BUG();
	}

	/* Calculate the grant relating to this PTE. */
	slot_index = vma->vm_pgoff + ((addr - vma->vm_start) >> PAGE_SHIFT);

	/* Only unmap grants if the slot has been mapped. This could be being
	 * called from a failing mmap().
	 */
	if (private_data->grants[slot_index].state != GNTDEV_SLOT_MAPPED)
		return xen_ptep_get_and_clear_full(vma, addr, ptep, is_fullmm);

	/* Clear the user space mapping, if it has been made. */
	if (private_data->grants[slot_index].u.valid.user_handle !=
	    GNTDEV_INVALID_HANDLE) {
		/* NOT USING SHADOW PAGE TABLES (and user handle valid). */

		/* Copy the existing value of the PTE for returning. */
		copy = *ptep;

		gnttab_set_unmap_op(&op[0], ptep_to_machine(ptep),
				    GNTMAP_contains_pte,
				    private_data->grants[slot_index].u.valid
				    .user_handle);
		nr = 1;
	} else {
		/* USING SHADOW PAGE TABLES (or user handle invalid). */
		copy = xen_ptep_get_and_clear_full(vma, addr, ptep, is_fullmm);
		nr = 0;
	}

	/* We always unmap the grant from kernel space. */
	gnttab_set_unmap_op(&op[nr],
			    get_kernel_vaddr(private_data, slot_index),
			    GNTMAP_host_map,
			    private_data->grants[slot_index].u.valid
			    .kernel_handle);

	ret = HYPERVISOR_grant_table_op(GNTTABOP_unmap_grant_ref, op, nr + 1);
	BUG_ON(ret);

	if (nr && op[0].status != GNTST_okay)
		pr_warning("User unmap grant status = %d\n", op[0].status);
	if (op[nr].status != GNTST_okay)
		pr_warning("Kernel unmap grant status = %d\n", op[nr].status);

	/* Return slot to the not-yet-mapped state, so that it may be
	 * mapped again, or removed by a subsequent ioctl.
	 */
	private_data->grants[slot_index].state = GNTDEV_SLOT_NOT_YET_MAPPED;

	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		/* Invalidate the physical to machine mapping for this page. */
		set_phys_to_machine(
			page_to_pfn(private_data->foreign_pages[slot_index]),
			INVALID_P2M_ENTRY);
	}

	return copy;
}

/* "Destructor" for a VM area.
 */
static void gntdev_vma_close(struct vm_area_struct *vma) {
	if (vma->vm_flags & VM_FOREIGN)
		kfree(vma->vm_private_data);
}

/* Called when an ioctl is made on the device.
 */
static long gntdev_ioctl(struct file *flip,
			 unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	gntdev_file_private_data_t *private_data = 
		(gntdev_file_private_data_t *) flip->private_data;

	/* On the first invocation, we will lazily initialise the grant array
	 * and free-list.
	 */
	if (unlikely(!private_data->grants) 
	    && likely(cmd != IOCTL_GNTDEV_SET_MAX_GRANTS)) {
		down_write(&private_data->grants_sem);
		
		if (unlikely(private_data->grants)) {
			up_write(&private_data->grants_sem);
			goto private_data_initialised;
		}
		
		/* Just use the default. Setting to a non-default is handled
		 * in the ioctl switch.
		 */
		rc = init_private_data(private_data, DEFAULT_MAX_GRANTS);
		
		up_write(&private_data->grants_sem);

		if (rc) {
			pr_err("Initialising gntdev private data failed\n");
			return rc;
		}
	}
	    
private_data_initialised:
	switch (cmd) {
	case IOCTL_GNTDEV_MAP_GRANT_REF:
	{
		struct ioctl_gntdev_map_grant_ref op;
		struct ioctl_gntdev_grant_ref *refs = NULL;

		if (copy_from_user(&op, (void __user *)arg, sizeof(op)))
			return -EFAULT;
		if (unlikely(op.count <= 0))
			return -EINVAL;

		if (op.count > 1 && op.count <= private_data->grants_size) {
			struct ioctl_gntdev_grant_ref *u;

			refs = kmalloc(op.count * sizeof(*refs), GFP_KERNEL);
			if (!refs)
				return -ENOMEM;
			u = ((struct ioctl_gntdev_map_grant_ref *)arg)->refs;
			if (copy_from_user(refs, (void __user *)u,
					   sizeof(*refs) * op.count)) {
				kfree(refs);
				return -EFAULT;
			}
		}

		down_write(&private_data->grants_sem);
		down_write(&private_data->free_list_sem);

		if (unlikely(op.count > private_data->free_list_size)) {
			rc = -ENOMEM;
			goto map_out;
		}

		if (op.count == 1) {
			if ((rc = add_grant_reference(private_data, op.refs,
						      &op.index)) < 0) {
				pr_err("Adding grant reference failed (%d)\n",
				       rc);
				goto map_out;
			}
		} else {
			if ((rc = find_contiguous_free_range(private_data,
							     op.count)) < 0) {
				pr_err("Finding contiguous range failed"
				       " (%d)\n", rc);
				goto map_out;
			}
			op.index = rc << PAGE_SHIFT;
			if ((rc = add_grant_references(private_data, op.count,
						       refs, rc))) {
				pr_err("Adding grant references failed (%d)\n",
				       rc);
				goto map_out;
			}
			compress_free_list(private_data);
		}

	map_out:
		up_write(&private_data->free_list_sem);
		up_write(&private_data->grants_sem);

		kfree(refs);

		if (!rc && copy_to_user((void __user *)arg, &op, sizeof(op)))
			rc = -EFAULT;
		return rc;
	}
	case IOCTL_GNTDEV_UNMAP_GRANT_REF:
	{
		struct ioctl_gntdev_unmap_grant_ref op;
		uint32_t i, start_index;

		if (copy_from_user(&op, (void __user *)arg, sizeof(op)))
			return -EFAULT;

		start_index = op.index >> PAGE_SHIFT;
		if (start_index + op.count < start_index ||
		    start_index + op.count > private_data->grants_size)
			return -EINVAL;

		down_write(&private_data->grants_sem);

		/* First, check that all pages are in the NOT_YET_MAPPED
		 * state.
		 */
		for (i = 0; i < op.count; ++i) {
			const char *what;

			switch (private_data->grants[start_index + i].state) {
			case GNTDEV_SLOT_NOT_YET_MAPPED:
				continue;
			case GNTDEV_SLOT_INVALID:
				what = "invalid";
				rc = -EINVAL;
				break;
			case GNTDEV_SLOT_MAPPED:
				what = "currently mmap()-ed";
				rc = -EBUSY;
				break;
			default:
				what = "in an invalid state";
				rc = -ENXIO;
				break;
			}
			pr_err("%s[%d] tried to remove a grant which is %s at %#x+%#x\n",
			       current->comm, current->pid,
			       what, start_index, i);
			goto unmap_out;
		}

		down_write(&private_data->free_list_sem);

		/* Unmap pages and add them to the free list.
		 */
		for (i = 0; i < op.count; ++i) {
			private_data->grants[start_index+i].state = 
				GNTDEV_SLOT_INVALID;
			private_data->grants[start_index+i].u.free_list_index =
				private_data->free_list_size;
			private_data->free_list[private_data->free_list_size] =
				start_index + i;
			++private_data->free_list_size;
		}

		up_write(&private_data->free_list_sem);
	unmap_out:
		up_write(&private_data->grants_sem);
		return rc;
	}
	case IOCTL_GNTDEV_GET_OFFSET_FOR_VADDR:
	{
		struct ioctl_gntdev_get_offset_for_vaddr op;
		struct vm_area_struct *vma;
		unsigned long vaddr;

		if (copy_from_user(&op, (void __user *)arg, sizeof(op)))
			return -EFAULT;

		vaddr = (unsigned long)op.vaddr;

		down_read(&current->mm->mmap_sem);		
		vma = find_vma(current->mm, vaddr);
		if (!vma || vma->vm_ops != &gntdev_vmops) {
			rc = -EFAULT;
			goto get_offset_out;
		}
		if (vma->vm_start != vaddr) {
			pr_err("The vaddr specified in an "
			       "IOCTL_GNTDEV_GET_OFFSET_FOR_VADDR must be at "
			       "the start of the VM area. vma->vm_start = "
			       "%#lx; vaddr = %#lx\n",
			       vma->vm_start, vaddr);
			rc = -EFAULT;
			goto get_offset_out;
		}
		op.offset = vma->vm_pgoff << PAGE_SHIFT;
		op.count = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
	get_offset_out:
		up_read(&current->mm->mmap_sem);
		if (!rc && copy_to_user((void __user *)arg, &op, sizeof(op)))
			rc = -EFAULT;
		return rc;
	}
	case IOCTL_GNTDEV_SET_MAX_GRANTS:
	{
		struct ioctl_gntdev_set_max_grants op;

		if (copy_from_user(&op, (void __user *)arg, sizeof(op)))
			return -EFAULT;
		if (op.count > MAX_GRANTS_LIMIT)
			return -EINVAL;

		down_write(&private_data->grants_sem);
		if (unlikely(private_data->grants))
			rc = -EBUSY;
		else
			rc = init_private_data(private_data, op.count);
		up_write(&private_data->grants_sem);
		return rc;
	}
	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}
