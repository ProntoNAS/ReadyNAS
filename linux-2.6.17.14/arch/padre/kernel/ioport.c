/* $Id: ioport.c,v 1.1.1.1 2006-08-28 17:52:12 bzhang Exp $
 * ioport.c:  Simple io mapping allocator.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1995 Miguel de Icaza (miguel@nuclecu.unam.mx)
 *
 * 1996: sparc_free_io, 1999: ioremap()/iounmap() by Pete Zaitcev.
 *
 * 2000/01/29
 * <rth> zait: as long as pci_alloc_consistent produces something addressable, 
 *	things are ok.
 * <zaitcev> rth: no, it is relevant, because get_free_pages returns you a
 *	pointer into the big page mapping
 * <rth> zait: so what?
 * <rth> zait: remap_it_my_way(virt_to_phys(get_free_page()))
 * <zaitcev> Hmm
 * <zaitcev> Suppose I did this remap_it_my_way(virt_to_phys(get_free_page())).
 *	So far so good.
 * <zaitcev> Now, driver calls pci_free_consistent(with result of
 *	remap_it_my_way()).
 * <zaitcev> How do you find the address to pass to free_pages()?
 * <rth> zait: walk the page tables?  It's only two or three level after all.
 * <rth> zait: you have to walk them anyway to remove the mapping.
 * <zaitcev> Hmm
 * <zaitcev> Sounds reasonable
 */

#include <linux/config.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/pci.h>		/* struct pci_dev */
#include <linux/proc_fs.h>

#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
//#include <asm/pgtable.h>
#include <asm/cacheflush.h>

#ifdef CONFIG_PCI

/* Allocate and map kernel buffer using consistent mode DMA for a device.
 * hwdev should be valid struct pci_dev pointer for PCI devices.
 */
void *pci_alloc_consistent(struct pci_dev *pdev, size_t len, dma_addr_t *pba)
{
	unsigned long len_total = (len + PAGE_SIZE-1) & PAGE_MASK;
	unsigned long va;
	int order;

	if (len == 0) {
		return NULL;
	}
	if (len > 256*1024) {			/* __get_free_pages() limit */
		return NULL;
	}

	order = get_order(len_total);
	va = __get_free_pages(GFP_KERNEL | GFP_DMA, order);
	if (va == 0) {
		printk(KERN_ERR "pci_alloc_consistent: no %ld pages\n", len_total>>PAGE_SHIFT);
		return NULL;
	}

	//*pba = virt_to_phys(va); /* equals virt_to_bus (R.I.P.) for us. */
	*pba = virt_to_bus(va); /* equals virt_to_bus (R.I.P.) for us. */
	return (void *) va;
}

/* Free and unmap a consistent DMA buffer.
 * cpu_addr is what was returned from pci_alloc_consistent,
 * size must be the same as what as passed into pci_alloc_consistent,
 * and likewise dma_addr must be the same as what *dma_addrp was set to.
 *
 * References to the memory and mappings assosciated with cpu_addr/dma_addr
 * past this call are illegal.
 */
void pci_free_consistent(struct pci_dev *pdev, size_t n, void *p, dma_addr_t ba)
{
	if (((unsigned long)p & (PAGE_SIZE-1)) != 0) {
		printk("pci_free_consistent: unaligned va %p\n", p);
		return;
	}

	free_pages((unsigned long)p, get_order(n));
}

/* Map a single buffer of the indicated size for DMA in streaming mode.
 * The 32-bit bus address to use is returned.
 *
 * Once the device is given the dma address, the device owns this memory
 * until either pci_unmap_single or pci_dma_sync_single_* is performed.
 */
dma_addr_t pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size,
    int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	/* IIep is write-through, not flushing. */
	flush_cache_dma(direction);
	//return virt_to_phys(ptr);
	return virt_to_bus(ptr);
}

/* Unmap a single streaming mode DMA translation.  The dma_addr and size
 * must match what was provided for in a previous pci_map_single call.  All
 * other usages are undefined.
 *
 * After this call, reads by the cpu to the buffer are guaranteed to see
 * whatever the device wrote there.
 */
void pci_unmap_single(struct pci_dev *hwdev, dma_addr_t ba, size_t size,
    int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
}

/*
 * Same as pci_map_single, but with pages.
 */
dma_addr_t pci_map_page(struct pci_dev *hwdev, struct page *page,
			unsigned long offset, size_t size, int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	/* IIep is write-through, not flushing. */
//	return page_to_phys(page) + offset;
	return virt_to_bus(page_address(page)) + offset;
}

void pci_unmap_page(struct pci_dev *hwdev,
			dma_addr_t dma_address, size_t size, int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	/* mmu_inval_dma_area XXX */
}

/* Map a set of buffers described by scatterlist in streaming
 * mode for DMA.  This is the scather-gather version of the
 * above pci_map_single interface.  Here the scatter gather list
 * elements are each tagged with the appropriate dma address
 * and length.  They are obtained via sg_dma_{address,length}(SG).
 *
 * NOTE: An implementation may be able to use a smaller number of
 *       DMA address/length pairs than there are SG table elements.
 *       (for example via virtual mapping capabilities)
 *       The routine returns the number of addr/length pairs actually
 *       used, at most nents.
 *
 * Device ownership issues as mentioned above for pci_map_single are
 * the same here.
 */
int pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
    int direction)
{
	int n;

	if (direction == PCI_DMA_NONE)
		BUG();
	/* IIep is write-through, not flushing. */
	for (n = 0; n < nents; n++) {
		if (page_address(sg->page) == NULL) BUG();
		//sg->dvma_address = virt_to_phys(page_address(sg->page));
		sg->dvma_address = virt_to_bus(page_address(sg->page)) + sg->offset;
		sg->dvma_length = sg->length;
		sg++;
	}
	return nents;
}

/* Unmap a set of streaming mode DMA translations.
 * Again, cpu read rules concerning calls here are the same as for
 * pci_unmap_single() above.
 */
void pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents,
    int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
}

/* Make physical memory consistent for a single
 * streaming mode DMA translation before or after a transfer.
 *
 * If you perform a pci_map_single() but wish to interrogate the
 * buffer using the cpu, yet do not wish to teardown the PCI dma
 * mapping, you must call this function before doing so.  At the
 * next point you give the PCI dma address back to the card, you
 * must first perform a pci_dma_sync_for_device, and then the
 * device again owns the buffer.
 */
void pci_dma_sync_single_for_cpu(struct pci_dev *hwdev, dma_addr_t ba, size_t size, int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	//if (direction != PCI_DMA_TODEVICE) 
		flush_cache_dma(direction);
}

void pci_dma_sync_single_for_device(struct pci_dev *hwdev, dma_addr_t ba, size_t size, int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	//if (direction != PCI_DMA_TODEVICE) 
		flush_cache_dma(direction);
}

/* Make physical memory consistent for a set of streaming
 * mode DMA translations after a transfer.
 *
 * The same as pci_dma_sync_single_* but for a scatter-gather list,
 * same rules and usage.
 */
void pci_dma_sync_sg_for_cpu(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	if (direction == PCI_DMA_NONE)
		BUG();
	//if (direction != PCI_DMA_TODEVICE) 
		flush_cache_dma(direction);
}

void pci_dma_sync_sg_for_device(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{

	if (direction == PCI_DMA_NONE)
		BUG();
	//if (direction != PCI_DMA_TODEVICE) 
		flush_cache_dma(direction);
}
#endif /* CONFIG_PCI */

