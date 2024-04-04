#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/memblock.h>
#include <linux/pci.h>

#include <asm/setup.h>
#ifndef CONFIG_XEN
#include <asm/bios_ebda.h>

/*
 * The BIOS places the EBDA/XBDA at the top of conventional
 * memory, and usually decreases the reported amount of
 * conventional memory (int 0x12) too. This also contains a
 * workaround for Dell systems that neglect to reserve EBDA.
 * The same workaround also avoids a problem with the AMD768MPX
 * chipset: reserve a page before VGA to prevent PCI prefetch
 * into it (errata #56). Usually the page is reserved anyways,
 * unless you have no PS/2 mouse plugged in.
 *
 * This functions is deliberately very conservative.  Losing
 * memory in the bottom megabyte is rarely a problem, as long
 * as we have enough memory to install the trampoline.  Using
 * memory that is in use by the BIOS or by some DMA device
 * the BIOS didn't shut down *is* a big problem.
 */

#define BIOS_LOWMEM_KILOBYTES	0x413
#define LOWMEM_CAP		0x9f000U	/* Absolute maximum */
#define INSANE_CUTOFF		0x20000U	/* Less than this = insane */

void __init reserve_ebda_region(void)
{
	unsigned int lowmem, ebda_addr;

	/*
	 * To determine the position of the EBDA and the
	 * end of conventional memory, we need to look at
	 * the BIOS data area. In a paravirtual environment
	 * that area is absent. We'll just have to assume
	 * that the paravirt case can handle memory setup
	 * correctly, without our help.
	 */
	if (paravirt_enabled())
		return;

	/* end of low (conventional) memory */
	lowmem = *(unsigned short *)__va(BIOS_LOWMEM_KILOBYTES);
	lowmem <<= 10;

	/* start of EBDA area */
	ebda_addr = get_bios_ebda();

	/*
	 * Note: some old Dells seem to need 4k EBDA without
	 * reporting so, so just consider the memory above 0x9f000
	 * to be off limits (bugzilla 2990).
	 */

	/* If the EBDA address is below 128K, assume it is bogus */
	if (ebda_addr < INSANE_CUTOFF)
		ebda_addr = LOWMEM_CAP;

	/* If lowmem is less than 128K, assume it is bogus */
	if (lowmem < INSANE_CUTOFF)
		lowmem = LOWMEM_CAP;

	/* Use the lower of the lowmem and EBDA markers as the cutoff */
	lowmem = min(lowmem, ebda_addr);
	lowmem = min(lowmem, LOWMEM_CAP); /* Absolute cap */

	/* reserve all memory between lowmem and the 1MB mark */
	memblock_x86_reserve_range(lowmem, 0x100000, "* BIOS reserved");
}
#else /* CONFIG_XEN */
#include <linux/module.h>
#include <asm/fixmap.h>
#include <asm/mc146818rtc.h>
#include <asm/pgtable.h>
#include <asm/sections.h>
#include <xen/interface/callback.h>
#include <xen/interface/memory.h>

extern void hypervisor_callback(void);
extern void failsafe_callback(void);
extern void nmi(void);

#ifdef CONFIG_X86_64
#include <asm/proto.h>
#define CALLBACK_ADDR(fn) ((unsigned long)(fn))
#else
#define CALLBACK_ADDR(fn) { __KERNEL_CS, (unsigned long)(fn) }
#endif

unsigned long __initdata xen_initrd_start;

const unsigned long *__read_mostly machine_to_phys_mapping =
	(void *)MACH2PHYS_VIRT_START;
EXPORT_SYMBOL(machine_to_phys_mapping);
unsigned long __read_mostly machine_to_phys_nr;
EXPORT_SYMBOL(machine_to_phys_nr);

void __init xen_start_kernel(void)
{
	unsigned int i;
	struct xen_machphys_mapping mapping;

	xen_setup_features();

	if (HYPERVISOR_memory_op(XENMEM_machphys_mapping, &mapping) == 0) {
		machine_to_phys_mapping = (void *)mapping.v_start;
		machine_to_phys_nr = mapping.max_mfn + 1;
	} else
		machine_to_phys_nr = MACH2PHYS_NR_ENTRIES;
#ifdef CONFIG_X86_32
	WARN_ON(machine_to_phys_mapping + (machine_to_phys_nr - 1)
		< machine_to_phys_mapping);
#endif

	if (!xen_feature(XENFEAT_auto_translated_physmap))
		phys_to_machine_mapping =
			(unsigned long *)xen_start_info->mfn_list;

	WARN_ON(HYPERVISOR_vm_assist(VMASST_CMD_enable,
				     VMASST_TYPE_writable_pagetables));

	memblock_init();
	memblock_x86_reserve_range(ALIGN(__pa_symbol(&_end), PAGE_SIZE),
				   __pa(xen_start_info->pt_base)
				   + (xen_start_info->nr_pt_frames
				      << PAGE_SHIFT),
				   "Xen provided");

#ifdef CONFIG_X86_32
{
	extern pte_t swapper_pg_fixmap[PTRS_PER_PTE];
	unsigned long addr;

	/* Do an early initialization of the fixmap area */
	make_lowmem_page_readonly(swapper_pg_fixmap, XENFEAT_writable_page_tables);
	addr = __fix_to_virt(FIX_EARLYCON_MEM_BASE);
	set_pmd(pmd_offset(pud_offset(swapper_pg_dir + pgd_index(addr),
				      addr),
			   addr),
		__pmd(__pa_symbol(swapper_pg_fixmap) | _PAGE_TABLE));
}
#else
	x86_configure_nx();
	xen_init_pt();
#endif

#define __FIXADDR_TOP (-PAGE_SIZE)
#define pmd_index(addr) (((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define FIX_BUG_ON(fix) BUILD_BUG_ON(pmd_index(__fix_to_virt(FIX_##fix)) \
			!= pmd_index(__fix_to_virt(FIX_EARLYCON_MEM_BASE)))
	FIX_BUG_ON(SHARED_INFO);
	FIX_BUG_ON(ISAMAP_BEGIN);
	FIX_BUG_ON(ISAMAP_END);
#undef pmd_index
#undef __FIXADDR_TOP

	/* Switch to the real shared_info page, and clear the dummy page. */
	set_fixmap(FIX_SHARED_INFO, xen_start_info->shared_info);
	HYPERVISOR_shared_info = (shared_info_t *)fix_to_virt(FIX_SHARED_INFO);
	clear_page(empty_zero_page);

	setup_vcpu_info(0);

	/* Set up mapping of lowest 1MB of physical memory. */
	for (i = 0; i < NR_FIX_ISAMAPS; i++)
		if (is_initial_xendomain())
			set_fixmap(FIX_ISAMAP_BEGIN - i, i * PAGE_SIZE);
		else
			__set_fixmap(FIX_ISAMAP_BEGIN - i,
				     virt_to_machine(empty_zero_page),
				     PAGE_KERNEL_RO);

	if (is_initial_xendomain()) {
		x86_platform.get_wallclock = mach_get_cmos_time;

		pci_request_acs();
	} else
		x86_init.resources.probe_roms = x86_init_noop;
}

void __init xen_arch_setup(void)
{
	int ret;
	static const struct callback_register __initconst event = {
		.type = CALLBACKTYPE_event,
		.address = CALLBACK_ADDR(hypervisor_callback)
	};
	static const struct callback_register __initconst failsafe = {
		.type = CALLBACKTYPE_failsafe,
		.address = CALLBACK_ADDR(failsafe_callback)
	};
#ifdef CONFIG_X86_64
	static const struct callback_register __initconst syscall = {
		.type = CALLBACKTYPE_syscall,
		.address = CALLBACK_ADDR(system_call)
	};
#endif
	static const struct callback_register __initconst nmi_cb = {
		.type = CALLBACKTYPE_nmi,
		.address = CALLBACK_ADDR(nmi)
	};

	ret = HYPERVISOR_callback_op(CALLBACKOP_register, &event);
	if (ret == 0)
		ret = HYPERVISOR_callback_op(CALLBACKOP_register, &failsafe);
#ifdef CONFIG_X86_64
	if (ret == 0)
		ret = HYPERVISOR_callback_op(CALLBACKOP_register, &syscall);
#endif
#if CONFIG_XEN_COMPAT <= 0x030002
#ifdef CONFIG_X86_32
	if (ret == -ENOSYS)
		ret = HYPERVISOR_set_callbacks(
			event.address.cs, event.address.eip,
			failsafe.address.cs, failsafe.address.eip);
#else
		ret = HYPERVISOR_set_callbacks(
			event.address,
			failsafe.address,
			syscall.address);
#endif
#endif
	BUG_ON(ret);

	ret = HYPERVISOR_callback_op(CALLBACKOP_register, &nmi_cb);
#if CONFIG_XEN_COMPAT <= 0x030002
	if (ret == -ENOSYS) {
		static struct xennmi_callback __initdata cb = {
			.handler_address = (unsigned long)nmi
		};

		HYPERVISOR_nmi_op(XENNMI_register_callback, &cb);
	}
#endif
}
#endif /* CONFIG_XEN */
