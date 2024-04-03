#include <asm/cacheflush.h>

/*
 * Low level routines
 */

static inline void write_dcctrl(unsigned long val)
{
    __asm__ __volatile__(
       "sta %0, [%1]%2\n"
       :
       : "r"(val),"r"(PADRE_DCMU_CTRL),"i"(ASI_DCMU));
}

static inline unsigned long read_dcctrl(void)
{
   	unsigned long val;
   	__asm__ __volatile__(
      "lda [%1]%2, %0\n"
      :"=r"(val)
      :"r"(PADRE_DCMU_CTRL),"i"(ASI_DCMU));
   	return val;
}

//**********************************************************************************
//
static void __clean_icache_range_workaround(unsigned long start, unsigned long end)
{
    __asm__ __volatile__(
        "sta %%g0, [%7]%0\n"
        "nop;nop;nop\n"
        "sta %8, [%7]%0\n"
        "nop;nop;nop\n"
        "1:\n"
	    "sta %%g0, [%3+%1]%0\n"
	    "sta %%g0, [%4+%1]%0\n"
	    "sta %%g0, [%5+%1]%0\n"
	    "sta %%g0, [%6+%1]%0\n"
	    "cmp %1, %2\n"
	    "blu 1b\n"
	    " add %1, 16, %1\n"
	    "sta %%g0, [%7]%0\n"
	    "nop;nop;nop\n"
	    "sta %9, [%7]%0\n"
	    "nop;nop;nop\n"
	    :
	    :"i"(ASI_ICMU),"r"(start),"r"(end), 
		 "r"(PADRE_ICMU_ITAG),"r"(PADRE_ICMU_ITAG+0x1000),
		 "r"(PADRE_ICMU_ITAG+0x2000),"r"(PADRE_ICMU_ITAG+0x3000),
		 "r"(PADRE_ICMU_CTRL),"r"(PADRE_ICMU_CTRL_TM),"r"(icmu_ctrl)
		:"cc");
}

static void __clean_dcache_range_workaround(unsigned long start,unsigned long end)
{
	unsigned long val;
	unsigned long flags;

	local_irq_save(flags);
	val = read_dcctrl();
	write_dcctrl(val | (PADRE_DCMU_CTRL_PROBE__CLEAN<<4));
	__asm__ __volatile__(
		"1:\n"
		"ld [%0], %%g0\n"
		"cmp %0, %1\n"
		"bleu 1b\n"
		" add %0, 16, %0"
		:
		: "r"(start),"r"(end));
	write_dcctrl(val);
	local_irq_restore(flags);
}

static void __flush_dcache_workaround(void)
{
	unsigned long val = read_dcctrl();
	unsigned long start = PADRE_S3_RAMTOP_CACHEABLE;
    unsigned long end = PADRE_S3_RAMTOP_CACHEABLE + 0x1000-16;
    write_dcctrl(val | (PADRE_DCMU_CTRL_PROBE__INVCLN<<4) |
            PADRE_DCMU_CTRL_FORCE  | 0x80 );
    __asm__ __volatile__(
        "1:\n"
	    "ld [%0], %%g0\n"
	    "cmp %0, %1\n"
	    "bleu 1b\n"
	    " add %0, 16, %0"
	    :
	    : "r"(start), "r"(end));
	write_dcctrl(val);
}

static void __flush_icache_workaround(void)
{
	register unsigned long icmu_reg = PADRE_ICMU_CTRL;
    register unsigned long icmu_reg_value = icmu_ctrl;

    __asm__ __volatile__(
		"sta %%g0, [%0]%1\t\n"
        "nop;nop;nop\t\n"
        :
        :"r"(icmu_reg),"i"(ASI_ICMU));
    writel(0x3ff,PADRE_MBIST_ADDR);
    writel(0x0,PADRE_MBIST_DATA);
    writel(PADRE_MBIST_MODE_EN | (PADRE_MBIST_MODE_ALGORITHM__W << 2) |
		           (1 << 20), PADRE_MBIST_MODE);

    while( ! (readl(PADRE_MBIST_MODE) & PADRE_MBIST_MODE_DONE));
    writel(0x0,PADRE_MBIST_MODE);

    __asm__ __volatile__(
   		"sta %0, [%1]%2\t\n"
		"nop;nop;nop\t\n"
        :
        : "r"(icmu_reg_value),"r"(icmu_reg),"i"(ASI_ICMU));
}


static void flush_icache_range_workaround(unsigned long start, unsigned long end)
{
	unsigned long flags;

	local_irq_save(flags);
	__flush_dcache_workaround();
 	__flush_icache_workaround();
	local_irq_restore(flags);
}

static void flush_icache_page_workaround(struct vm_area_struct *vma, struct page *page)
{
	unsigned long flags;

	if (!(vma->vm_flags & VM_EXEC))
		return;

    	local_irq_save(flags);
    	__clean_dcache_range_workaround((unsigned long)(page_address(page)),
		 (unsigned long)(page_address(page))+PAGE_SIZE-16);
	__flush_icache_workaround();
	local_irq_restore(flags);
}


static void flush_sig_insns_workaround(struct mm_struct *mm, unsigned long insn_addr)
{
    unsigned long flags;

    local_irq_save(flags);
    __clean_dcache_range_workaround(insn_addr & 0xfffffff0UL,(insn_addr+4) & 0xfffffff0UL);
    if( (insn_addr & 0xffc) == 0xffc) {
	    __clean_icache_range_workaround(0,0);
	    __clean_icache_range_workaround(0xff0,0xff0);
    } else
	    __clean_icache_range_workaround(insn_addr & 0xff0 , (insn_addr+4) & 0xff0);
    local_irq_restore(flags);
}

static void __ic3call flush_cache_dma_workaround(int direction)
{
	unsigned long flags;
	local_irq_save(flags);
	__flush_dcache_workaround();
	local_irq_restore(flags);
}

static void __ic3call flush_dcache_range_workaround(unsigned long start,unsigned long end)
{
	unsigned long val;
	unsigned long flags;

	local_irq_save(flags);
	val = read_dcctrl();
	write_dcctrl(val | (PADRE_DCMU_CTRL_PROBE__INVCLN<<4));
	__asm__ __volatile__(
		"1:\n"
		"ld [%0], %%g0\n"
		"cmp %0, %1\n"
		"bleu 1b\n"
		" add %0, 16, %0"
		: 
		: "r"(start), "r"(end));
	write_dcctrl(val);
	local_irq_restore(flags);
}

//**********************************************************************************
//
static void __ic3call __clean_dcache_range_cachefix(unsigned long start,unsigned long end)
{
	unsigned long val;
	unsigned long flags;

	if ((start - 0x40000000ul) >= PAGE_NONCACHE_MARK) {
		// in non-cacheable, simply return.
		return;
	}

	local_irq_save(flags);
	val = read_dcctrl();
	write_dcctrl(val | (PADRE_DCMU_CTRL_PROBE__CLEAN<<4));
	__asm__ __volatile__(
		"1:\n"
		"ld [%0+0x00], %%g0\n"
		"ld [%0+0x10], %%g0\n"
		"ld [%0+0x20], %%g0\n"
		"add %0, 0x40, %0\n"
		"cmp %0, %1\n"
		"bleu 1b\n"
		" ld [%0-0x10], %%g0\n"
		"ld [%0], %%g0\n"
		: 
		: "r"(start&~3),"r"(end)			// start/end is inclusive
		: "cc");
	write_dcctrl(val);
	local_irq_restore(flags);
}

static void __flush_dcache_cachefix(int clean_only)
{
	unsigned long val = read_dcctrl();
	unsigned long start = PADRE_S3_RAMTOP_CACHEABLE;
	unsigned long end = PADRE_S3_RAMTOP_CACHEABLE + 0x1000-16;

	if (clean_only) {
		// flush using PROBE_CLEAN, but not bit7 (work around for bug#2122)
		write_dcctrl(val |
			     (PADRE_DCMU_CTRL_PROBE__CLEAN<<4) |
			     PADRE_DCMU_CTRL_FORCE );
		__asm__ __volatile__(
			"1:\n"
			"subcc %4, 16, %4\n"
			"ld [%0+%4], %%g0\n"
			"ld [%1+%4], %%g0\n"
			"ld [%2+%4], %%g0\n"
			"bg 1b\n"
			" ld [%3+%4], %%g0\n"
			: 
			: "r"(start), "r"(start+0x1000), "r"(start+0x2000), "r"(start+0x3000), "r"(0x1000)
			: "cc");
		write_dcctrl(val);
	}
	else {
		write_dcctrl(val |
			     (PADRE_DCMU_CTRL_PROBE__INVCLN<<4) |
			     PADRE_DCMU_CTRL_FORCE |
			     0x80);
		__asm__ __volatile__(
			"1:\n"
			"ld [%0], %%g0\n"
			"ld [%0+16], %%g0\n"
			"ld [%0+32], %%g0\n"
			"ld [%0+48], %%g0\n"
			"cmp %0, %1\n"
			"bleu 1b\n"
			" add %0, 64, %0"
			: 
			: "r"(start), "r"(end)
			: "cc");
		write_dcctrl(val);
	}
}

static void __flush_icache_cachefix(void)
{
	u32 reg = PADRE_ICMU_CTRL;
	u32 value = icmu_ctrl;

	__asm__ __volatile__(
		"sta %%g0, [%0]%1\n"
		"nop;nop;nop\n"
		:
		:"r"(reg),"i"(ASI_ICMU));
	writel(0x0ff,PADRE_MBIST_ADDR);
	writel(0x0,PADRE_MBIST_DATA);
	writel(PADRE_MBIST_MODE_EN | (PADRE_MBIST_MODE_ALGORITHM__W << 2) |
		   (1<<20) | (1<<10), PADRE_MBIST_MODE);
	
	while( ! (readl(PADRE_MBIST_MODE) & PADRE_MBIST_MODE_DONE));
	writel(0x0,PADRE_MBIST_MODE);
	
	__asm__ __volatile__(
		"sta %0, [%1]%2\n"
		"nop;nop;nop\n"
		:
		: "r"(value),"r"(reg),"i"(ASI_ICMU));	
}


static void flush_icache_range_cachefix(unsigned long start, unsigned long end)
{
	unsigned long flags;

	local_irq_save(flags);
	__flush_dcache_cachefix(0);
 	__flush_icache_cachefix();
	local_irq_restore(flags);
}

static void __ic3call __clean_dcache_page_cachefix(unsigned long page)
{
	unsigned long val = read_dcctrl();

	write_dcctrl(val | (PADRE_DCMU_CTRL_PROBE__CLEAN<<4));
	__asm__ __volatile__(
		"1:\n"
		"ld [%0+0x00], %%g0\n"
		"ld [%0+0x10], %%g0\n"
		"ld [%0+0x20], %%g0\n"
		"add %0, 0x40, %0\n"
		"cmp %0, %1\n"
		"blu 1b\n"
		" ld [%0-0x10], %%g0\n"
		: 
#ifdef CONFIG_PADRE_16K_PAGE
		: "r"(page),"r"(page+0x3ff0)
#else
#ifdef CONFIG_PADRE_8K_PAGE
		: "r"(page),"r"(page+0x1ff0)
#else
		: "r"(page),"r"(page+0xff0)
#endif
#endif
		: "cc");
	write_dcctrl(val);
}

static void flush_icache_page_cachefix(struct vm_area_struct *vma, struct page *page)
{
	unsigned long flags;

	if (!(vma->vm_flags & VM_EXEC))
		return;

    	local_irq_save(flags);
    	__clean_dcache_page_cachefix((unsigned long)(page_address(page)));
	__flush_icache_cachefix();
	local_irq_restore(flags);
}


static void flush_sig_insns_cachefix(struct mm_struct *mm, unsigned long insn_addr)
{
    unsigned long flags;

    local_irq_save(flags);
    __clean_dcache_range_cachefix(insn_addr & 0xfffffff0UL,(insn_addr+4) & 0xfffffff0UL);

#if 0
	
	__asm__ __volatile__(
		"sta %%g0, [%7]%0\n"
		"nop;nop;nop\n"
		"sta %8, [%7]%0\n"
		"nop;nop;nop\n"
		"1:\n"
		"sta %%g0, [%3+%1]%0\n"
		"sta %%g0, [%4+%1]%0\n"
		"sta %%g0, [%5+%1]%0\n"
		"sta %%g0, [%6+%1]%0\n"
		"sta %%g0, [%3+%2]%0\n"
		"sta %%g0, [%4+%2]%0\n"
		"sta %%g0, [%5+%2]%0\n"
		"sta %%g0, [%6+%2]%0\n"
		"sta %%g0, [%7]%0\n"
		"nop;nop;nop\n"
		"sta %9, [%7]%0\n"
		"nop;nop;nop\n"
		:
		:"i"(ASI_ICMU),"r"(insn_addr & 0xff0),"r"((insn_addr+4) & 0xff0),
		 "r"(PADRE_ICMU_ITAG),"r"(PADRE_ICMU_ITAG+0x1000),
		 "r"(PADRE_ICMU_ITAG+0x2000),"r"(PADRE_ICMU_ITAG+0x3000),
		 "r"(PADRE_ICMU_CTRL),"r"(PADRE_ICMU_CTRL_TM),"r"(icmu_ctrl)
		:"cc");
#endif
    local_irq_restore(flags);
}

static void __ic3call flush_cache_dma_cachefix(int direction)
{
	unsigned long flags;
#ifdef CONFIG_PADRE_DCACHE_WT
	if( direction == PADRE_DMA_TODEVICE)
		return;
	else {
    	local_irq_save(flags);
		__flush_dcache_cachefix(0);
		local_irq_restore(flags);
	}

#else
	local_irq_save(flags);
	if (direction==PADRE_DMA_TODEVICE) {
		// when direction is TODEVICE,
		// need write-back only and no need to invalidate.
		__flush_dcache_cachefix(1);
	}
	else {
		// when direction is FROMDEVICE or BIDIRECTIONAL,
		// need write-back and invalidate.
		__flush_dcache_cachefix(0);
	}
	local_irq_restore(flags);
#endif
}

static void __ic3call flush_dcache_range_cachefix(unsigned long start, unsigned long end)
{
	unsigned long val;
	unsigned long flags;

	local_irq_save(flags);
	val = read_dcctrl();
	write_dcctrl(val |
		     (PADRE_DCMU_CTRL_PROBE__INVCLN<<4) );
	__asm__ __volatile__(
		"ld [%0+%1], %%g0\n"
		"1:\n"
		"subcc %1, 0x10, %1\n"
		"bg 1b\n"
		" ld [%0+%1], %%g0\n"
		: 
		: "r"(start&~15), "r"((end&~15)-(start&~15))
		: "cc");
	write_dcctrl(val);
	local_irq_restore(flags);
}

//**********************************************************************************
//
extern void __ic3call clean_dcache_range(unsigned long start, unsigned long end)
{
  if (padre_cachefix) __clean_dcache_range_cachefix(start, end);
  else                __clean_dcache_range_workaround(start, end);
}

extern void __ic3call flush_icache_range(unsigned long start, unsigned long end)
{
  if (padre_cachefix) flush_icache_range_cachefix(start, end);
  else                flush_icache_range_workaround(start, end);
}

extern void __ic3call flush_icache_page(struct vm_area_struct *vma, struct page *page)
{
  if (padre_cachefix) flush_icache_page_cachefix(vma, page);
  else                flush_icache_page_workaround(vma, page);
}

extern void __ic3call flush_dcache_range(unsigned long start, unsigned long end)
{
  if (padre_cachefix) flush_dcache_range_cachefix(start, end);
  else                flush_dcache_range_workaround(start, end);
}

extern void __ic3call flush_sig_insns(struct mm_struct *mm, unsigned long insn_addr)
{
  if (padre_cachefix) flush_sig_insns_cachefix(mm, insn_addr);
  else                flush_sig_insns_workaround(mm, insn_addr);
}

extern void __ic3call flush_cache_dma(int direction)
{
  if (padre_cachefix) flush_cache_dma_cachefix(direction);
  else                flush_cache_dma_workaround(direction);
}

