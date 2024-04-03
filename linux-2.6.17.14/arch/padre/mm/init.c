/*  $Id: init.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 *  linux/arch/sparc/mm/init.c
 *
 *  Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *  Copyright (C) 1995 Eddie C. Dost (ecd@skynet.be)
 *  Copyright (C) 1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 *  Copyright (C) 2000 Anton Blanchard (anton@samba.org)
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/initrd.h>
#include <linux/init.h>
#include <linux/bootmem.h>

#include <asm/system.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>	/* bug in asm-generic/tlb.h: check_pgt_cache */
#include <asm/tlb.h>
#include <asm/mmu_context.h>


unsigned long PAGE_NONCACHE_MARK __dc3data ____cacheline_aligned;

struct eth_desc_list
{
        struct eth_desc_list *next;
        int data;
};


#define S3_RAMTOP       0x70001000
#define SLOW_LIST_SIZE  (1024*128)
#define FAST_LIST_SIZE  (1024*4*2)
#define DESC_SIZE       (16)

static struct eth_desc_list *fast_eth_head, *slow_eth_head;
static unsigned long slow_eth_data[SLOW_LIST_SIZE];
static struct eth_desc_list fast_eth_l[FAST_LIST_SIZE/DESC_SIZE];
static struct eth_desc_list slow_eth_l[SLOW_LIST_SIZE/DESC_SIZE];
static int fast_eth_count,slow_eth_count;

void init_eth_desc ( void )
{
        int i;
        struct eth_desc_list *head;
        head = NULL;
        slow_eth_count = SLOW_LIST_SIZE/DESC_SIZE;
        for( i = 0 ; i < slow_eth_count; i++ ) {
                slow_eth_l[i].data = (unsigned long)&slow_eth_data[i*DESC_SIZE];
                slow_eth_l[i].next = head;
                head = &slow_eth_l[i];
        }
        slow_eth_head = head;

        head = NULL;
        fast_eth_count = FAST_LIST_SIZE/DESC_SIZE;
        for( i = 0 ; i < fast_eth_count; i++) {
                fast_eth_l[i].data = i *DESC_SIZE+S3_RAMTOP;
                fast_eth_l[i].next = head;
                head = &fast_eth_l[i];
        }
        fast_eth_head = head;
}

unsigned long alloc_eth_desc ( void )
{
        unsigned long addr;
        if( fast_eth_count > 0 ) {
                addr = fast_eth_head->data;
                fast_eth_head = fast_eth_head->next;
                fast_eth_count--;
        }
        else if( slow_eth_count > 0 ) {
                addr = slow_eth_head->data;
                slow_eth_head = slow_eth_head->next;
                slow_eth_count--;
        }
        else
                panic("no enough descriptor memory\n");
        return addr;
}

void free_eth_desc(unsigned long addr)
{
        if( addr < 0x80000000 ) {
                addr = (addr - S3_RAMTOP)>>4;
                fast_eth_l[addr].next = fast_eth_head;
                fast_eth_head = &fast_eth_l[addr];
                fast_eth_count++;
        }
        else {
                addr = (addr - (unsigned long)(&slow_eth_data[0])) >>4;
                slow_eth_l[addr].next = slow_eth_head;
                slow_eth_head =  &slow_eth_l[addr];
                slow_eth_count++;
        }
}


DEFINE_PER_CPU(struct mmu_gather, mmu_gathers);


/* References to section boundaries */
extern char __init_begin, __init_end, _start, _end, etext , edata;

void show_mem(void)
{
	printk("Mem-info:\n");
	show_free_areas();
	printk("Free swap:       %6ldkB\n",
	       nr_swap_pages << (PAGE_SHIFT-10));
	printk("%ld pages of RAM\n", totalram_pages);
	printk("%d free pages\n", nr_free_pages());
}

unsigned long __init bootmem_init(void)
{
	unsigned long bootmap_size, start_pfn;
	unsigned long bootmap_pfn;

	/* Start with page aligned address of last symbol in kernel
	 * image.  
	 */
	start_pfn  = (unsigned long)__pa(PAGE_ALIGN((unsigned long) &_end));
	max_pfn = padre_boot_param.mem_size >> PAGE_SHIFT;
	
    PAGE_NONCACHE_MARK = (((padre_boot_param.mem_size>>24)-CONFIG_PADRE_NONCACHESIZE)<<24) + 0x40000000UL;
	
	
	/* Now shift down to get the real physical page frame number. */
	start_pfn >>= PAGE_SHIFT;

	bootmap_pfn = start_pfn;

	/* Initialize the boot-time allocator. */
    	bootmap_size = init_bootmem(start_pfn,max_pfn);
    	free_bootmem(start_pfn<<PAGE_SHIFT,(max_pfn-start_pfn)<<PAGE_SHIFT);
	   

#ifdef CONFIG_BLK_DEV_INITRD
	initrd_start = 0x1000000;
	initrd_end =   0x2000000;
	/* Reserve the initrd image area. */
	reserve_bootmem(initrd_start, initrd_end - initrd_start);

	initrd_start = initrd_start  + PAGE_OFFSET;
	initrd_end = initrd_end  + PAGE_OFFSET;		
#endif
	/* Reserve the bootmem map.   We do not account for it
	 * in pages_avail because we will release that memory
	 * in free_all_bootmem.
	 */
	reserve_bootmem((bootmap_pfn << PAGE_SHIFT), bootmap_size);

#ifdef CONFIG_PADRE_RESERVE_DMA
	reserve_bootmem(PAGE_NONCACHE_MARK-0x40000000UL,(2720+256)*0x1000);
#endif

	return max_pfn;
}

#ifdef CONFIG_PADRE_RESERVE_DMA
unsigned long padre_pgd_cache[256];
unsigned long padre_pgd_cache_count;
unsigned long padre_pte_cache[2720];
unsigned long padre_pte_cache_count;

static void padre_pgtable_cache_init(void)
{
	int i;

	padre_pgd_cache_count=256;
	padre_pte_cache_count = 2720;
	padre_pgd_cache[0] = PAGE_NONCACHE_MARK+0x40000000UL;
	for( i = 1 ; i < padre_pgd_cache_count; i++) {
		padre_pgd_cache[i] = padre_pgd_cache[i-1]+0x1000;
	}

	padre_pte_cache[0] = padre_pgd_cache[255]+0x1000;
	for( i = 1 ; i < padre_pte_cache_count; i++) {
		padre_pte_cache[i] = padre_pte_cache[i-1]+0x1000;
	}
}

void check_pgt_cache(void)
{
}

#else

struct pgtable_cache_struct pgt_quicklists;

int do_check_pgt_cache(int low, int high)
{
	int freed = 0;
	if(pgtable_cache_size > high) {
		do {
	    		if (pgd_quicklist) {
				free_pgd_slow(get_pgd_fast());
				freed++;
			}
#if 0
			if (pte_quicklist) {
				pte_free_slow(pte_alloc_one_fast(NULL, 0));
				freed++;
			}
#endif
		} while(pgtable_cache_size > low);
	}
						
	return freed;
}

int pgt_cache_water[2] = { 25, 50 };
 
void check_pgt_cache(void)
{
	do_check_pgt_cache(pgt_cache_water[0], pgt_cache_water[1]);
}
#endif

/*
 * paging_init() sets up the page tables: We call the MMU specific
 * init routine based upon the Sun model type on the Sparc.
 *
 */
void __init paging_init(void)
{
    	unsigned long zones_size[MAX_NR_ZONES] = {0, 0, 0,0};

	bootmem_init();
#if 0	
	memset(swapper_pg_dir, 0, PAGE_SIZE);
	memset(pg0, 0, PAGE_SIZE);
	memset(pg1, 0, PAGE_SIZE);
	memset(pg2, 0, PAGE_SIZE);
	memset(pg3, 0, PAGE_SIZE);
#endif

	zones_size[ZONE_DMA] = CONFIG_PADRE_NONCACHESIZE*4096/
		(PAGE_SIZE/4096);
    	zones_size[ZONE_NORMAL] = max_low_pfn-zones_size[ZONE_DMA];
    	free_area_init(zones_size);
}

void __init mem_init(void)
{
	int codepages = 0;
	int datapages = 0;
	int initpages = 0; 
	int reservedpages = 0;
	int i;

    	max_mapnr = num_physpages = max_low_pfn;

	/* Saves us work later. */
	memset((void *)&empty_zero_page, 0, PAGE_SIZE);

	totalram_pages = free_all_bootmem();

	codepages = (((unsigned long) &etext) - ((unsigned long)&_start));
	codepages = PAGE_ALIGN(codepages) >> PAGE_SHIFT;
	datapages = (((unsigned long) &edata) - ((unsigned long)&etext));
	datapages = PAGE_ALIGN(datapages) >> PAGE_SHIFT;
	initpages = (((unsigned long) &__init_end) - ((unsigned long) &__init_begin));
	initpages = PAGE_ALIGN(initpages) >> PAGE_SHIFT;

	/* Ignore memory holes for the purpose of counting reserved pages */
	for (i=0; i < max_low_pfn; i++)
		if (PageReserved(pfn_to_page(i)))
			reservedpages++;

	printk(KERN_INFO "Memory: %luk/%luk available (%dk kernel code, %dk reserved, %dk data, %dk init, %ldk highmem)\n",
	       (unsigned long) nr_free_pages() << (PAGE_SHIFT-10),
	       num_physpages << (PAGE_SHIFT - 10),
	       codepages << (PAGE_SHIFT-10),
	       reservedpages << (PAGE_SHIFT - 10),
	       datapages << (PAGE_SHIFT-10), 
	       initpages << (PAGE_SHIFT-10),
	       0L);
	// pray to succeed
#ifdef CONFIG_PADRE_RESERVE_DMA
	padre_pgtable_cache_init();
	padre_pgd_cache_count--;
	init_mm.pgd = (pgd_t*)padre_pgd_cache[padre_pgd_cache_count];
	memset(init_mm.pgd,0,4096);
#else
	init_mm.pgd = (pgd_t*)get_zeroed_page((GFP_KERNEL|GFP_DMA));
#endif
	printk("init_mm.pgd %p\n", init_mm.pgd);
	switch_mm(NULL, &init_mm, &init_task);
}

void free_initmem (void)
{
	unsigned long addr;

	addr = (unsigned long)(&__init_begin);
	for (; addr < (unsigned long)(&__init_end); addr += PAGE_SIZE) {
		struct page *p;

		p = virt_to_page(addr);

		ClearPageReserved(p);
		init_page_count(p);
		__free_page(p);
		totalram_pages++;
		num_physpages++;
	}
	printk (KERN_INFO "Freeing unused kernel memory: %dk freed\n", (&__init_end - &__init_begin) >> 10);
}

#ifdef CONFIG_BLK_DEV_INITRD
void free_initrd_mem(unsigned long start, unsigned long end)
{
	if (start < end)
		printk (KERN_INFO "Freeing initrd memory: %ldk freed\n", (end - start) >> 10);
	for (; start < end; start += PAGE_SIZE) {
		struct page *p = virt_to_page(start);

		ClearPageReserved(p);
		init_page_count(p);
		__free_page(p);
		num_physpages++;
	}
}
#endif

#ifdef CONFIG_DEBUG_PAGEALLOC

static DEFINE_SPINLOCK(cpa_lock);

pte_t *lookup_address(unsigned long address)
{
        pgd_t *pgd = pgd_offset_k(address);
        pud_t *pud;
        pmd_t *pmd;
        if (pgd_none(*pgd))
                return NULL;
        pud = pud_offset(pgd, address);
        if (pud_none(*pud))
                return NULL;
        pmd = pmd_offset(pud, address);
        if (pmd_none(*pmd))
                return NULL;
        return pte_offset_kernel(pmd, address);
}

static int
__change_page_attr(struct page *page, pgprot_t prot)
{
        pte_t *kpte;
        unsigned long address;
        struct page *kpte_page;

        BUG_ON(PageHighMem(page));
        address = (unsigned long)page_address(page);

        kpte = lookup_address(address);
        if (!kpte)
                return -EINVAL;
        kpte_page = virt_to_page(kpte);
        if (pgprot_val(prot) != pgprot_val(PAGE_KERNEL)) {
                set_pte(kpte, mk_pte(page, prot));
                get_page(kpte_page);
        } else {
                set_pte(kpte, mk_pte(page, PAGE_KERNEL));
                __put_page(kpte_page);
        }

        return 0;
}

/*
 * Change the page attributes of an page in the linear mapping.
 *
 * This should be used when a page is mapped with a different caching policy
 * than write-back somewhere - some CPUs do not like it when mappings with
 * different caching policies exist. This changes the page attributes of the
 * in kernel linear mapping too.
 *
 * The caller needs to ensure that there are no conflicting mappings elsewhere.
 * This function only deals with the kernel linear map.
 *
 * Caller must call global_flush_tlb() after this.
 */
int change_page_attr(struct page *page, int numpages, pgprot_t prot)
{
        int err = 0;
        int i;
        unsigned long flags;

        spin_lock_irqsave(&cpa_lock, flags);
        for (i = 0; i < numpages; i++, page++) {
                err = __change_page_attr(page, prot);
                if (err)
                        break;
        }
        spin_unlock_irqrestore(&cpa_lock, flags);
        return err;
}

void kernel_map_pages(struct page *page, int numpages, int enable)
{
        if (PageHighMem(page))
                return;
        /* the return value is ignored - the calls cannot fail,
         * large pages are disabled at boot time.
         */
        //change_page_attr(page, numpages, enable ? PAGE_KERNEL : __pgprot(0));
        /* we should perform an IPI and flush all tlbs,
         * but that can deadlock->flush only current cpu.
         */
        flush_tlb_all();
}
#endif

