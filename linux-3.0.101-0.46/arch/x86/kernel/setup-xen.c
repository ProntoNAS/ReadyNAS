/*
 *  Copyright (C) 1995  Linus Torvalds
 *
 *  Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999
 *
 *  Memory region support
 *	David Parsons <orc@pell.chi.il.us>, July-August 1999
 *
 *  Added E820 sanitization routine (removes overlapping memory regions);
 *  Brian Moyle <bmoyle@mvista.com>, February 2001
 *
 * Moved CPU detection code to cpu/${cpu}.c
 *    Patrick Mochel <mochel@osdl.org>, March 2002
 *
 *  Provisions for empty E820 memory regions (reported by certain BIOSes).
 *  Alex Achenbach <xela@slit.de>, December 2002.
 *
 */

/*
 * This file handles the architecture-dependent parts of initialization
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/screen_info.h>
#include <linux/ioport.h>
#include <linux/acpi.h>
#include <linux/sfi.h>
#include <linux/apm_bios.h>
#include <linux/initrd.h>
#include <linux/bootmem.h>
#include <linux/memblock.h>
#include <linux/seq_file.h>
#include <linux/console.h>
#include <linux/mca.h>
#include <linux/root_dev.h>
#include <linux/highmem.h>
#include <linux/module.h>
#include <linux/efi.h>
#include <linux/init.h>
#include <linux/edd.h>
#include <linux/iscsi_ibft.h>
#include <linux/nodemask.h>
#include <linux/kexec.h>
#include <linux/dmi.h>
#include <linux/pfn.h>
#include <linux/pci.h>
#include <asm/pci-direct.h>
#include <linux/init_ohci1394_dma.h>
#include <linux/kvm_para.h>

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/delay.h>

#include <linux/kallsyms.h>
#include <linux/cpufreq.h>
#include <linux/dma-mapping.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>

#include <linux/percpu.h>
#include <linux/crash_dump.h>
#include <linux/tboot.h>

#include <video/edid.h>

#include <asm/mtrr.h>
#include <asm/apic.h>
#include <asm/trampoline.h>
#include <asm/e820.h>
#include <asm/mpspec.h>
#include <asm/setup.h>
#include <asm/efi.h>
#include <asm/timer.h>
#include <asm/i8259.h>
#include <asm/sections.h>
#include <asm/dmi.h>
#include <asm/io_apic.h>
#include <asm/ist.h>
#include <asm/setup_arch.h>
#include <asm/bios_ebda.h>
#include <asm/cacheflush.h>
#include <asm/processor.h>
#include <asm/bugs.h>

#include <asm/system.h>
#include <asm/vsyscall.h>
#include <asm/cpu.h>
#include <asm/desc.h>
#include <asm/dma.h>
#include <asm/iommu.h>
#include <asm/gart.h>
#include <asm/mmu_context.h>
#include <asm/proto.h>

#include <asm/paravirt.h>
#include <asm/hypervisor.h>
#include <asm/olpc_ofw.h>

#include <asm/percpu.h>
#include <asm/topology.h>
#include <asm/apicdef.h>
#include <asm/amd_nb.h>
#ifdef CONFIG_X86_64
#include <asm/numa_64.h>
#endif
#include <asm/mce.h>
#include <asm/alternative.h>
#include <asm/prom.h>

#ifdef CONFIG_XEN
#include <asm/hypervisor.h>
#include <xen/interface/kexec.h>
#include <xen/interface/memory.h>
#include <xen/interface/nmi.h>
#include <xen/interface/physdev.h>
#include <xen/features.h>
#include <xen/firmware.h>
#include <xen/xencons.h>

static int xen_panic_event(struct notifier_block *, unsigned long, void *);
static struct notifier_block xen_panic_block = {
	xen_panic_event, NULL, 0 /* try to go last */
};

unsigned long *phys_to_machine_mapping;
EXPORT_SYMBOL(phys_to_machine_mapping);

static unsigned long *pfn_to_mfn_frame_list_list, **pfn_to_mfn_frame_list;

/* Raw start-of-day parameters from the hypervisor. */
start_info_t *xen_start_info;
EXPORT_SYMBOL(xen_start_info);
#endif

/*
 * end_pfn only includes RAM, while max_pfn_mapped includes all e820 entries.
 * The direct mapping extends to max_pfn_mapped, so that we can directly access
 * apertures, ACPI and other tables without having to play with fixmaps.
 */
unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

#ifdef CONFIG_DMI
RESERVE_BRK(dmi_alloc, 65536);
#endif


static __initdata unsigned long _brk_start = (unsigned long)__brk_base;
unsigned long _brk_end = (unsigned long)__brk_base;

#ifndef CONFIG_XEN
#ifdef CONFIG_X86_64
int default_cpu_present_to_apicid(int mps_cpu)
{
	return __default_cpu_present_to_apicid(mps_cpu);
}

int default_check_phys_apicid_present(int phys_apicid)
{
	return __default_check_phys_apicid_present(phys_apicid);
}
#endif

#ifndef CONFIG_DEBUG_BOOT_PARAMS
struct boot_params __initdata boot_params;
#else
struct boot_params boot_params;
#endif
#else /* CONFIG_XEN */
/*
 * Initialise the list of the frames that specify the list of
 * frames that make up the p2m table. Used by save/restore and
 * kexec/crash.
 */
#ifdef CONFIG_PM_SLEEP
void
#else
static void __init
#endif
setup_pfn_to_mfn_frame_list(typeof(__alloc_bootmem) *__alloc_bootmem)
{
	unsigned long i, j, size;
	unsigned int k, order, fpp = PAGE_SIZE / sizeof(unsigned long);

	size = (max_pfn + fpp - 1) / fpp;
	size = (size + fpp - 1) / fpp;
	++size; /* include a zero terminator for crash tools */
	size *= sizeof(unsigned long);
	order = get_order(size);
	if (__alloc_bootmem)
		pfn_to_mfn_frame_list_list =
			alloc_bootmem_pages(PAGE_SIZE << order);
	if (order) {
		if (xen_create_contiguous_region((unsigned long)
						 pfn_to_mfn_frame_list_list,
						 order, 0))
			pr_err("List of P2M frame lists is not contiguous, %s will not work",
			       is_initial_xendomain()
			       ? "kdump" : "save/restore");
		memset(pfn_to_mfn_frame_list_list, 0, size);
	}
	size -= sizeof(unsigned long);
	if (__alloc_bootmem)
		pfn_to_mfn_frame_list = alloc_bootmem(size);

	for (i = j = 0, k = -1; i < max_pfn; i += fpp, j++) {
		if (j == fpp)
			j = 0;
		if (j == 0) {
			k++;
			BUG_ON(k * sizeof(unsigned long) >= size);
			if (__alloc_bootmem)
				pfn_to_mfn_frame_list[k] =
					alloc_bootmem_pages(PAGE_SIZE);
			pfn_to_mfn_frame_list_list[k] =
				virt_to_mfn(pfn_to_mfn_frame_list[k]);
		}
		pfn_to_mfn_frame_list[k][j] =
			virt_to_mfn(&phys_to_machine_mapping[i]);
	}
	HYPERVISOR_shared_info->arch.max_pfn = max_pfn;
	HYPERVISOR_shared_info->arch.pfn_to_mfn_frame_list_list =
		virt_to_mfn(pfn_to_mfn_frame_list_list);
}
#endif

/*
 * Machine setup..
 */
static struct resource data_resource = {
	.name	= "Kernel data",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};

static struct resource code_resource = {
	.name	= "Kernel code",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};

static struct resource bss_resource = {
	.name	= "Kernel bss",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};


#ifdef CONFIG_X86_32
/* cpu data as detected by the assembly code in head.S */
struct cpuinfo_x86 new_cpu_data __cpuinitdata = { .wp_works_ok = 1, .hard_math = 1 };
/* common cpu data for all cpus */
struct cpuinfo_x86 boot_cpu_data __read_mostly = { .wp_works_ok = 1, .hard_math = 1 };
EXPORT_SYMBOL(boot_cpu_data);
#ifndef CONFIG_XEN
static void set_mca_bus(int x)
{
#ifdef CONFIG_MCA
	MCA_bus = x;
#endif
}

unsigned int def_to_bigsmp;

/* for MCA, but anyone else can use it if they want */
unsigned int machine_id;
unsigned int machine_submodel_id;
unsigned int BIOS_revision;

struct apm_info apm_info;
EXPORT_SYMBOL(apm_info);
#endif

#if defined(CONFIG_X86_SPEEDSTEP_SMI_MODULE)
struct ist_info ist_info;
EXPORT_SYMBOL(ist_info);
#elif defined(CONFIG_X86_SPEEDSTEP_SMI)
struct ist_info ist_info;
#endif

#else
struct cpuinfo_x86 boot_cpu_data __read_mostly = {
	.x86_phys_bits = MAX_PHYSMEM_BITS,
};
EXPORT_SYMBOL(boot_cpu_data);
#endif


#if !defined(CONFIG_X86_PAE) || defined(CONFIG_X86_64)
unsigned long mmu_cr4_features;
#else
unsigned long mmu_cr4_features = X86_CR4_PAE;
#endif

/* Boot loader ID and version as integers, for the benefit of proc_dointvec */
int bootloader_type, bootloader_version;

/*
 * Setup options
 */
struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);
struct edid_info edid_info;
EXPORT_SYMBOL_GPL(edid_info);

extern int root_mountflags;

unsigned long saved_video_mode;

#define RAMDISK_IMAGE_START_MASK	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000

static char __initdata command_line[COMMAND_LINE_SIZE];
#ifdef CONFIG_CMDLINE_BOOL
static char __initdata builtin_cmdline[COMMAND_LINE_SIZE] = CONFIG_CMDLINE;
#endif

#if defined(CONFIG_EDD) || defined(CONFIG_EDD_MODULE)
struct edd edd;
#ifdef CONFIG_EDD_MODULE
EXPORT_SYMBOL(edd);
#endif
#ifndef CONFIG_XEN
/**
 * copy_edd() - Copy the BIOS EDD information
 *              from boot_params into a safe place.
 *
 */
static inline void __init copy_edd(void)
{
     memcpy(edd.mbr_signature, boot_params.edd_mbr_sig_buffer,
	    sizeof(edd.mbr_signature));
     memcpy(edd.edd_info, boot_params.eddbuf, sizeof(edd.edd_info));
     edd.mbr_signature_nr = boot_params.edd_mbr_sig_buf_entries;
     edd.edd_info_nr = boot_params.eddbuf_entries;
}
#endif
#else
static inline void __init copy_edd(void)
{
}
#endif

void * __init extend_brk(size_t size, size_t align)
{
	size_t mask = align - 1;
	void *ret;

	BUG_ON(_brk_start == 0);
	BUG_ON(align & mask);

	_brk_end = (_brk_end + mask) & ~mask;
	BUG_ON((char *)(_brk_end + size) > __brk_limit);

	ret = (void *)_brk_end;
	_brk_end += size;

	memset(ret, 0, size);

	return ret;
}

#if defined(CONFIG_X86_64) && !defined(CONFIG_XEN)
static void __init init_gbpages(void)
{
	if (direct_gbpages && cpu_has_gbpages)
		printk(KERN_INFO "Using GB pages for direct mapping\n");
	else
		direct_gbpages = 0;
}
#else
static inline void init_gbpages(void)
{
}
static void __init cleanup_highmap(void)
{
}
#endif

static void __init reserve_brk(void)
{
	if (_brk_end > _brk_start)
		memblock_x86_reserve_range(__pa(_brk_start), __pa(_brk_end), "BRK");

	/* Mark brk area as locked down and no longer taking any
	   new allocations */
	_brk_start = 0;
}

#ifdef CONFIG_BLK_DEV_INITRD

#define MAX_MAP_CHUNK	(NR_FIX_BTMAPS << PAGE_SHIFT)
static void __init relocate_initrd(void)
{
#ifndef CONFIG_XEN
	/* Assume only end is not page aligned */
	u64 ramdisk_image = boot_params.hdr.ramdisk_image;
	u64 ramdisk_size  = boot_params.hdr.ramdisk_size;
	u64 area_size     = PAGE_ALIGN(ramdisk_size);
	u64 end_of_lowmem = max_low_pfn_mapped << PAGE_SHIFT;
	u64 ramdisk_here;
	unsigned long slop, clen, mapaddr;
	char *p, *q;

	/* We need to move the initrd down into lowmem */
	ramdisk_here = memblock_find_in_range(0, end_of_lowmem, area_size,
					 PAGE_SIZE);

	if (ramdisk_here == MEMBLOCK_ERROR)
		panic("Cannot find place for new RAMDISK of size %lld\n",
			 ramdisk_size);

	/* Note: this includes all the lowmem currently occupied by
	   the initrd, we rely on that fact to keep the data intact. */
	memblock_x86_reserve_range(ramdisk_here, ramdisk_here + area_size, "NEW RAMDISK");
	initrd_start = ramdisk_here + PAGE_OFFSET;
	initrd_end   = initrd_start + ramdisk_size;
	printk(KERN_INFO "Allocated new RAMDISK: %08llx - %08llx\n",
			 ramdisk_here, ramdisk_here + ramdisk_size);

	q = (char *)initrd_start;

	/* Copy any lowmem portion of the initrd */
	if (ramdisk_image < end_of_lowmem) {
		clen = end_of_lowmem - ramdisk_image;
		p = (char *)__va(ramdisk_image);
		memcpy(q, p, clen);
		q += clen;
		ramdisk_image += clen;
		ramdisk_size  -= clen;
	}

	/* Copy the highmem portion of the initrd */
	while (ramdisk_size) {
		slop = ramdisk_image & ~PAGE_MASK;
		clen = ramdisk_size;
		if (clen > MAX_MAP_CHUNK-slop)
			clen = MAX_MAP_CHUNK-slop;
		mapaddr = ramdisk_image & PAGE_MASK;
		p = early_memremap(mapaddr, clen+slop);
		memcpy(q, p+slop, clen);
		early_iounmap(p, clen+slop);
		q += clen;
		ramdisk_image += clen;
		ramdisk_size  -= clen;
	}
	/* high pages is not converted by early_res_to_bootmem */
	ramdisk_image = boot_params.hdr.ramdisk_image;
	ramdisk_size  = boot_params.hdr.ramdisk_size;
	printk(KERN_INFO "Move RAMDISK from %016llx - %016llx to"
		" %08llx - %08llx\n",
		ramdisk_image, ramdisk_image + ramdisk_size - 1,
		ramdisk_here, ramdisk_here + ramdisk_size - 1);
#else
	printk(KERN_ERR "initrd extends beyond end of memory "
	       "(0x%08lx > 0x%08lx)\ndisabling initrd\n",
	       xen_initrd_start + xen_start_info->mod_len,
	       max_low_pfn_mapped << PAGE_SHIFT);
	initrd_start = 0;
#endif
}

static void __init reserve_initrd(void)
{
	/* Assume only end is not page aligned */
#ifndef CONFIG_XEN
	u64 ramdisk_image = boot_params.hdr.ramdisk_image;
	u64 ramdisk_size  = boot_params.hdr.ramdisk_size;
	u64 ramdisk_end   = PAGE_ALIGN(ramdisk_image + ramdisk_size);
	u64 end_of_lowmem = max_low_pfn_mapped << PAGE_SHIFT;

	if (!boot_params.hdr.type_of_loader ||
	    !ramdisk_image || !ramdisk_size)
		return;		/* No initrd provided by bootloader */
#else
	unsigned long ramdisk_image = xen_initrd_start;
	unsigned long ramdisk_size  = xen_start_info->mod_len;
	unsigned long ramdisk_end   = PAGE_ALIGN(ramdisk_image + ramdisk_size);
	unsigned long end_of_lowmem = max_low_pfn_mapped << PAGE_SHIFT;

	if (!xen_start_info->mod_start || !ramdisk_size)
		return;		/* No initrd provided by bootloader */
#endif

	initrd_start = 0;

	if (ramdisk_size >= (end_of_lowmem>>1)) {
		memblock_x86_free_range(ramdisk_image, ramdisk_end);
		printk(KERN_ERR "initrd too large to handle, "
		       "disabling initrd\n");
		return;
	}

	printk(KERN_INFO "RAMDISK: %08lx - %08lx\n", ramdisk_image,
			ramdisk_end);


	if (ramdisk_end <= end_of_lowmem) {
		/* All in lowmem, easy case */
		/*
		 * don't need to reserve again, already reserved early
		 * in i386_start_kernel
		 */
		initrd_start = ramdisk_image + PAGE_OFFSET;
		initrd_end = initrd_start + ramdisk_size;
#ifdef CONFIG_X86_64_XEN
		initrd_below_start_ok = 1;
#endif
		return;
	}

	relocate_initrd();

	memblock_x86_free_range(ramdisk_image, ramdisk_end);
}
#else
static void __init reserve_initrd(void)
{
}
#endif /* CONFIG_BLK_DEV_INITRD */

static void __init parse_setup_data(void)
{
#ifndef CONFIG_XEN
	struct setup_data *data;
	u64 pa_data;

	if (boot_params.hdr.version < 0x0209)
		return;
	pa_data = boot_params.hdr.setup_data;
	while (pa_data) {
		u32 data_len, map_len;

		map_len = max(PAGE_SIZE - (pa_data & ~PAGE_MASK),
			      (u64)sizeof(struct setup_data));
		data = early_memremap(pa_data, map_len);
		data_len = data->len + sizeof(struct setup_data);
		if (data_len > map_len) {
			early_iounmap(data, map_len);
			data = early_memremap(pa_data, data_len);
			map_len = data_len;
		}

		switch (data->type) {
		case SETUP_E820_EXT:
			parse_e820_ext(data);
			break;
		case SETUP_DTB:
			add_dtb(pa_data);
			break;
		default:
			break;
		}
		pa_data = data->next;
		early_iounmap(data, map_len);
	}
#endif
}

static void __init e820_reserve_setup_data(void)
{
#ifndef CONFIG_XEN
	struct setup_data *data;
	u64 pa_data;
	int found = 0;

	if (boot_params.hdr.version < 0x0209)
		return;
	pa_data = boot_params.hdr.setup_data;
	while (pa_data) {
		data = early_memremap(pa_data, sizeof(*data));
		e820_update_range(pa_data, sizeof(*data)+data->len,
			 E820_RAM, E820_RESERVED_KERN);
		found = 1;
		pa_data = data->next;
		early_iounmap(data, sizeof(*data));
	}
	if (!found)
		return;

	sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &e820.nr_map);
	memcpy(&e820_saved, &e820, sizeof(struct e820map));
	printk(KERN_INFO "extended physical RAM map:\n");
	e820_print_map("reserve setup_data");
#endif
}

static void __init memblock_x86_reserve_range_setup_data(void)
{
#ifndef CONFIG_XEN
	struct setup_data *data;
	u64 pa_data;
	char buf[32];

	if (boot_params.hdr.version < 0x0209)
		return;
	pa_data = boot_params.hdr.setup_data;
	while (pa_data) {
		data = early_memremap(pa_data, sizeof(*data));
		sprintf(buf, "setup data %x", data->type);
		memblock_x86_reserve_range(pa_data, pa_data+sizeof(*data)+data->len, buf);
		pa_data = data->next;
		early_iounmap(data, sizeof(*data));
	}
#endif
}

#ifndef CONFIG_XEN
/*
 * --------- Crashkernel reservation ------------------------------
 */

#ifdef CONFIG_KEXEC

static inline unsigned long long get_total_mem(void)
{
	unsigned long long total;

	total = max_pfn - min_low_pfn;

	return total << PAGE_SHIFT;
}

/*
 * Keep the crash kernel below this limit.  On 32 bits earlier kernels
 * would limit the kernel to the low 512 MiB due to mapping restrictions.
 * On 64 bits, kexec-tools currently limits us to 896 MiB; increase this
 * limit once kexec-tools are fixed.
 */
#ifdef CONFIG_X86_32
# define CRASH_KERNEL_ADDR_MAX	(512 << 20)
#else
# define CRASH_KERNEL_ADDR_MAX	(896 << 20)
#endif

static void __init reserve_crashkernel(void)
{
	unsigned long long total_mem;
	unsigned long long crash_size, crash_base;
	int ret;

	total_mem = get_total_mem();

	ret = parse_crashkernel(boot_command_line, total_mem,
			&crash_size, &crash_base);
	if (ret != 0 || crash_size <= 0)
		return;

	/* 0 means: find the address automatically */
	if (crash_base <= 0) {
		const unsigned long long alignment = 16<<20;	/* 16M */

		/*
		 *  kexec want bzImage is below CRASH_KERNEL_ADDR_MAX
		 */
		crash_base = memblock_find_in_range(alignment,
			       CRASH_KERNEL_ADDR_MAX, crash_size, alignment);

		if (crash_base == MEMBLOCK_ERROR) {
			pr_info("crashkernel reservation failed - No suitable area found.\n");
			return;
		}
	} else {
		unsigned long long start;

		start = memblock_find_in_range(crash_base,
				 crash_base + crash_size, crash_size, 1<<20);
		if (start != crash_base) {
			pr_info("crashkernel reservation failed - memory is in use.\n");
			return;
		}
	}
	memblock_x86_reserve_range(crash_base, crash_base + crash_size, "CRASH KERNEL");

	printk(KERN_INFO "Reserving %ldMB of memory at %ldMB "
			"for crashkernel (System RAM: %ldMB)\n",
			(unsigned long)(crash_size >> 20),
			(unsigned long)(crash_base >> 20),
			(unsigned long)(total_mem >> 20));

	crashk_res.start = crash_base;
	crashk_res.end   = crash_base + crash_size - 1;
	insert_resource(&iomem_resource, &crashk_res);
}
#else
static void __init reserve_crashkernel(void)
{
}
#endif
#endif /* CONFIG_XEN */

static struct resource standard_io_resources[] = {
	{ .name = "dma1", .start = 0x00, .end = 0x1f,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "pic1", .start = 0x20, .end = 0x21,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "timer0", .start = 0x40, .end = 0x43,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "timer1", .start = 0x50, .end = 0x53,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "keyboard", .start = 0x60, .end = 0x60,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "keyboard", .start = 0x64, .end = 0x64,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "dma page reg", .start = 0x80, .end = 0x8f,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "pic2", .start = 0xa0, .end = 0xa1,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "dma2", .start = 0xc0, .end = 0xdf,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO },
	{ .name = "fpu", .start = 0xf0, .end = 0xff,
		.flags = IORESOURCE_BUSY | IORESOURCE_IO }
};

void __init reserve_standard_io_resources(void)
{
	int i;

	/* Nothing to do if not running in dom0. */
	if (!is_initial_xendomain())
		return;

	/* request I/O space for devices used on all i[345]86 PCs */
	for (i = 0; i < ARRAY_SIZE(standard_io_resources); i++)
		request_resource(&ioport_resource, &standard_io_resources[i]);

}

static __init void reserve_ibft_region(void)
{
	unsigned long addr, size = 0;

	addr = find_ibft_region(&size);

#ifndef CONFIG_XEN
	if (size)
		memblock_x86_reserve_range(addr, addr + size, "* ibft");
#endif
}

#ifndef CONFIG_XEN
static unsigned reserve_low = CONFIG_X86_RESERVE_LOW << 10;

static bool __init snb_gfx_workaround_needed(void)
{
#ifdef CONFIG_PCI
	int i;
	u16 vendor, devid;
	static const u16 snb_ids[] = {
		0x0102,
		0x0112,
		0x0122,
		0x0106,
		0x0116,
		0x0126,
		0x010a,
	};

	/* Assume no if something weird is going on with PCI */
	if (!early_pci_allowed())
		return false;

	vendor = read_pci_config_16(0, 2, 0, PCI_VENDOR_ID);
	if (vendor != 0x8086)
		return false;

	devid = read_pci_config_16(0, 2, 0, PCI_DEVICE_ID);
	for (i = 0; i < ARRAY_SIZE(snb_ids); i++)
		if (devid == snb_ids[i])
			return true;
#endif

	return false;
}

/*
 * Sandy Bridge graphics has trouble with certain ranges, exclude
 * them from allocation.
 */
static void __init trim_snb_memory(void)
{
	static const unsigned long bad_pages[] = {
		0x20050000,
		0x20110000,
		0x20130000,
		0x20138000,
		0x40004000,
	};
	int i;

	if (!snb_gfx_workaround_needed())
		return;

	printk(KERN_DEBUG "reserving inaccessible SNB gfx pages\n");

	/*
	 * Reserve all memory below the 1 MB mark that has not
	 * already been reserved.
	 */
	memblock_reserve(0, 1<<20);

	for (i = 0; i < ARRAY_SIZE(bad_pages); i++) {
		if (memblock_reserve(bad_pages[i], PAGE_SIZE))
			printk(KERN_WARNING "failed to reserve 0x%08lx\n",
			       bad_pages[i]);
	}
}

/*
 * Here we put platform-specific memory range workarounds, i.e.
 * memory known to be corrupt or otherwise in need to be reserved on
 * specific platforms.
 *
 * If this gets used more widely it could use a real dispatch mechanism.
 */
static void __init trim_platform_memory_ranges(void)
{
	trim_snb_memory();
}

static void __init trim_bios_range(void)
{
	/*
	 * A special case is the first 4Kb of memory;
	 * This is a BIOS owned area, not kernel ram, but generally
	 * not listed as such in the E820 table.
	 *
	 * This typically reserves additional memory (64KiB by default)
	 * since some BIOSes are known to corrupt low memory.  See the
	 * Kconfig help text for X86_RESERVE_LOW.
	 */
	e820_update_range(0, ALIGN(reserve_low, PAGE_SIZE),
			  E820_RAM, E820_RESERVED);

	/*
	 * special case: Some BIOSen report the PC BIOS
	 * area (640->1Mb) as ram even though it is not.
	 * take them out.
	 */
	e820_remove_range(BIOS_BEGIN, BIOS_END - BIOS_BEGIN, E820_RAM, 1);

	sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &e820.nr_map);
}

static int __init parse_reservelow(char *p)
{
	unsigned long long size;

	if (!p)
		return -EINVAL;

	size = memparse(p, &p);

	if (size < 4096)
		size = 4096;

	if (size > 640*1024)
		size = 640*1024;

	reserve_low = size;

	return 0;
}

early_param("reservelow", parse_reservelow);
#endif

/*
 * Determine if we were loaded by an EFI loader.  If so, then we have also been
 * passed the efi memmap, systab, etc., so we should use these data structures
 * for initialization.  Note, the efi init code path is determined by the
 * global efi_enabled. This allows the same kernel image to be used on existing
 * systems (with a traditional BIOS) as well as on EFI systems.
 */
/*
 * setup_arch - architecture-specific boot-time initializations
 *
 * Note: On x86_64, fixmaps are ready for use even before this is called.
 */

void __init setup_arch(char **cmdline_p)
{
#ifdef CONFIG_XEN
	unsigned long p2m_pages;
	struct physdev_set_iopl set_iopl;

	if (!is_initial_xendomain()) {
#ifdef CONFIG_X86_32
		/* Force a quick death if the kernel panics (not domain 0). */
		extern int panic_timeout;
		if (!panic_timeout)
			panic_timeout = 1;
#endif

		/* Register a call for panic conditions. */
		atomic_notifier_chain_register(&panic_notifier_list, &xen_panic_block);
	}

	set_iopl.iopl = 1;
	WARN_ON(HYPERVISOR_physdev_op(PHYSDEVOP_set_iopl, &set_iopl));
#endif /* CONFIG_XEN */

#ifdef CONFIG_X86_32
	memcpy(&boot_cpu_data, &new_cpu_data, sizeof(new_cpu_data));
	visws_early_detect();

#ifndef CONFIG_XEN
	/*
	 * copy kernel address range established so far and switch
	 * to the proper swapper page table
	 */
	clone_pgd_range(swapper_pg_dir     + KERNEL_PGD_BOUNDARY,
			initial_page_table + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);

	load_cr3(swapper_pg_dir);
	__flush_tlb_all();
#endif
#else
	printk(KERN_INFO "Command line: %s\n", boot_command_line);
#endif

	/*
	 * If we have OLPC OFW, we might end up relocating the fixmap due to
	 * reserve_top(), so do this before touching the ioremap area.
	 */
	olpc_ofw_detect();

	early_trap_init();
	early_cpu_init();
	early_ioremap_init();

	setup_olpc_ofw_pgd();

#ifndef CONFIG_XEN
	ROOT_DEV = old_decode_dev(boot_params.hdr.root_dev);
	screen_info = boot_params.screen_info;
	edid_info = boot_params.edid_info;
#ifdef CONFIG_X86_32
	apm_info.bios = boot_params.apm_bios_info;
	ist_info = boot_params.ist_info;
	if (boot_params.sys_desc_table.length != 0) {
		set_mca_bus(boot_params.sys_desc_table.table[3] & 0x2);
		machine_id = boot_params.sys_desc_table.table[0];
		machine_submodel_id = boot_params.sys_desc_table.table[1];
		BIOS_revision = boot_params.sys_desc_table.table[2];
	}
#endif
	saved_video_mode = boot_params.hdr.vid_mode;
	bootloader_type = boot_params.hdr.type_of_loader;
	if ((bootloader_type >> 4) == 0xe) {
		bootloader_type &= 0xf;
		bootloader_type |= (boot_params.hdr.ext_loader_type+0x10) << 4;
	}
	bootloader_version  = bootloader_type & 0xf;
	bootloader_version |= boot_params.hdr.ext_loader_ver << 4;

#ifdef CONFIG_BLK_DEV_RAM
	rd_image_start = boot_params.hdr.ram_size & RAMDISK_IMAGE_START_MASK;
	rd_prompt = ((boot_params.hdr.ram_size & RAMDISK_PROMPT_FLAG) != 0);
	rd_doload = ((boot_params.hdr.ram_size & RAMDISK_LOAD_FLAG) != 0);
#endif
#ifdef CONFIG_EFI
	if (!strncmp((char *)&boot_params.efi_info.efi_loader_signature,
#ifdef CONFIG_X86_32
		     "EL32",
#else
		     "EL64",
#endif
	 4)) {
		efi_enabled = 1;
		efi_memblock_x86_reserve_range();
	}
#endif
#else /* CONFIG_XEN */
#ifdef CONFIG_X86_32
	/* This must be initialized to UNNAMED_MAJOR for ipconfig to work
	   properly.  Setting ROOT_DEV to default to /dev/ram0 breaks initrd.
	*/
	ROOT_DEV = MKDEV(UNNAMED_MAJOR,0);
#else
 	ROOT_DEV = MKDEV(RAMDISK_MAJOR,0);
#endif
	if (is_initial_xendomain()) {
		const struct dom0_vga_console_info *info =
			(void *)((char *)xen_start_info +
			         xen_start_info->console.dom0.info_off);

		dom0_init_screen_info(info,
		                      xen_start_info->console.dom0.info_size);
		xen_start_info->console.domU.mfn = 0;
		xen_start_info->console.domU.evtchn = 0;

		efi_probe();
	} else
		screen_info.orig_video_isVGA = 0;
	copy_edid();
#endif /* CONFIG_XEN */

	x86_init.oem.arch_setup();

	iomem_resource.end = (1ULL << boot_cpu_data.x86_phys_bits) - 1;
	setup_memory_map();
	parse_setup_data();
	/* update the e820_saved too */
	e820_reserve_setup_data();

	copy_edd();

#ifndef CONFIG_XEN
	if (!boot_params.hdr.root_flags)
		root_mountflags &= ~MS_RDONLY;
#endif
	init_mm.start_code = (unsigned long) _text;
	init_mm.end_code = (unsigned long) _etext;
	init_mm.end_data = (unsigned long) _edata;
	init_mm.brk = _brk_end;

	code_resource.start = virt_to_phys(_text);
	code_resource.end = virt_to_phys(_etext)-1;
	data_resource.start = virt_to_phys(_etext);
	data_resource.end = virt_to_phys(_edata)-1;
	bss_resource.start = virt_to_phys(&__bss_start);
	bss_resource.end = virt_to_phys(&__bss_stop)-1;

#ifdef CONFIG_CMDLINE_BOOL
#ifdef CONFIG_CMDLINE_OVERRIDE
	strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
#else
	if (builtin_cmdline[0]) {
		/* append boot loader cmdline to builtin */
		strlcat(builtin_cmdline, " ", COMMAND_LINE_SIZE);
		strlcat(builtin_cmdline, boot_command_line, COMMAND_LINE_SIZE);
		strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
	}
#endif
#endif

	strlcpy(command_line, boot_command_line, COMMAND_LINE_SIZE);
	*cmdline_p = command_line;

	/*
	 * x86_configure_nx() is called before parse_early_param() to detect
	 * whether hardware doesn't support NX (so that the early EHCI debug
	 * console setup can safely call set_fixmap()). It may then be called
	 * again from within noexec_setup() during parsing early parameters
	 * to honor the respective command line option.
	 */
	x86_configure_nx();

	parse_early_param();

	x86_report_nx();

	/* after early param, so could get panic from serial */
	memblock_x86_reserve_range_setup_data();

	if (acpi_mps_check()) {
#if defined(CONFIG_X86_LOCAL_APIC) && !defined(CONFIG_XEN)
		disable_apic = 1;
#endif
		setup_clear_cpu_cap(X86_FEATURE_APIC);
	}

#ifdef CONFIG_PCI
	if (pci_early_dump_regs)
		early_dump_pci_devices();
#endif

	finish_e820_parsing();

	if (efi_enabled)
		efi_init();

	if (is_initial_xendomain())
		dmi_scan_machine();

	/*
	 * VMware detection requires dmi to be available, so this
	 * needs to be done after dmi_scan_machine, for the BP.
	 */
	init_hypervisor_platform();

	x86_init.resources.probe_roms();

#ifndef CONFIG_XEN
	/* after parse_early_param, so could debug it */
	insert_resource(&iomem_resource, &code_resource);
	insert_resource(&iomem_resource, &data_resource);
	insert_resource(&iomem_resource, &bss_resource);

	trim_bios_range();
#ifdef CONFIG_X86_32
	if (ppro_with_ram_bug()) {
		e820_update_range(0x70000000ULL, 0x40000ULL, E820_RAM,
				  E820_RESERVED);
		sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &e820.nr_map);
		printk(KERN_INFO "fixed physical RAM map:\n");
		e820_print_map("bad_ppro");
	}
#else
	early_gart_iommu_check();
#endif
#endif /* CONFIG_XEN */

	/*
	 * partially used pages are not usable - thus
	 * we are rounding upwards:
	 */
	max_pfn = e820_end_of_ram_pfn();

	/* update e820 for memory not covered by WB MTRRs */
	mtrr_bp_init();
#ifndef CONFIG_XEN
	if (mtrr_trim_uncached_memory(max_pfn))
		max_pfn = e820_end_of_ram_pfn();
#else
	if (max_pfn > xen_start_info->nr_pages)
		memblock_x86_reserve_range(PFN_PHYS(xen_start_info->nr_pages),
					   PFN_PHYS(max_pfn), "BALLOON");
#endif

#ifdef CONFIG_X86_32
	/* max_low_pfn get updated here */
	find_low_pfn_range();
#else
	num_physpages = max_pfn;
	max_mapnr = max_pfn;

#ifdef CONFIG_X86_LOCAL_APIC
	check_x2apic();
#endif

	/* How many end-of-memory variables you have, grandma! */
	/* need this before calling reserve_initrd */
	if (max_pfn > (1UL<<(32 - PAGE_SHIFT)))
		max_low_pfn = e820_end_of_low_ram_pfn();
	else
		max_low_pfn = max_pfn;

	high_memory = (void *)__va(max_pfn * PAGE_SIZE - 1) + 1;
#endif

	/*
	 * Find and reserve possible boot-time SMP configuration:
	 */
	find_smp_config();

	reserve_ibft_region();

	/*
	 * Need to conclude brk, before memblock_x86_fill()
	 *  it could use memblock_find_in_range, could overlap with
	 *  brk area.
	 */
	reserve_brk();

	cleanup_highmap();

	memblock.current_limit = get_max_mapped();
	memblock_x86_fill();

	/*
	 * The EFI specification says that boot service code won't be called
	 * after ExitBootServices(). This is, in fact, a lie.
	 */
	if (efi_enabled)
		efi_reserve_boot_services();

	/* preallocate 4k for mptable mpc */
	early_reserve_e820_mpc_new();

#ifdef CONFIG_X86_CHECK_BIOS_CORRUPTION
	setup_bios_corruption_check();
#endif

	printk(KERN_DEBUG "initial memory mapped : 0 - %08lx\n",
			max_pfn_mapped<<PAGE_SHIFT);

#ifndef CONFIG_XEN
	setup_trampolines();

	trim_platform_memory_ranges();
#endif

	init_gbpages();

	/* max_pfn_mapped is updated here */
#ifdef CONFIG_X86_64_XEN
	if (xen_start_info->mfn_list < __START_KERNEL_map) {
		/* Map P2M space only after all usable memory. */
		unsigned long p2m_start = xen_start_info->first_p2m_pfn;
		unsigned long p2m_end = p2m_start
					+ xen_start_info->nr_p2m_frames;
		unsigned long temp;

		max_low_pfn_mapped = init_memory_mapping(
			0, min(max_low_pfn, p2m_start) << PAGE_SHIFT);
		max_pfn_mapped = max_low_pfn_mapped;

		if (p2m_end < max_low_pfn)
			max_low_pfn_mapped = init_memory_mapping(
				p2m_end << PAGE_SHIFT,
				max_low_pfn << PAGE_SHIFT);
		max_pfn_mapped = max_low_pfn_mapped;

		if (max_low_pfn < p2m_start)
			max_pfn_mapped = init_memory_mapping(
				max_low_pfn << PAGE_SHIFT,
				p2m_start << PAGE_SHIFT);

		if (max(max_low_pfn, p2m_end) < max_pfn)
			max_pfn_mapped = init_memory_mapping(
				max(max_low_pfn, p2m_end) << PAGE_SHIFT,
				max_pfn << PAGE_SHIFT);

		temp = max_pfn_mapped;
		if (p2m_start < max_low_pfn) {
			temp = init_memory_mapping(
				p2m_start << PAGE_SHIFT,
				min(max_low_pfn, p2m_end) << PAGE_SHIFT);
			if (temp > max_low_pfn_mapped)
				max_low_pfn_mapped = temp;
		}

		if (max_low_pfn < p2m_end)
			temp = init_memory_mapping(
				max(max_low_pfn, p2m_start) << PAGE_SHIFT,
				p2m_end << PAGE_SHIFT);
		if (temp > max_pfn_mapped)
			max_pfn_mapped = temp;

		goto init_memory_mapping_done;
	}
#endif

	max_low_pfn_mapped = init_memory_mapping(0, max_low_pfn<<PAGE_SHIFT);
	max_pfn_mapped = max_low_pfn_mapped;

#ifdef CONFIG_X86_64
	if (max_pfn > max_low_pfn) {
		int i;
		for (i = 0; i < e820.nr_map; i++) {
			struct e820entry *ei = &e820.map[i];

			if (ei->addr + ei->size <= 1UL << 32)
				continue;

			if (ei->type == E820_RESERVED)
				continue;

			max_pfn_mapped = init_memory_mapping(
				ei->addr < 1UL << 32 ? 1UL << 32 : ei->addr,
				ei->addr + ei->size);
		}

 init_memory_mapping_done:
		/* can we preseve max_low_pfn ?*/
		max_low_pfn = max_pfn;
	}
#endif
	memblock.current_limit = get_max_mapped();

	/*
	 * NOTE: On x86-32, only from this point on, fixmaps are ready for use.
	 */

#ifdef CONFIG_PROVIDE_OHCI1394_DMA_INIT
	if (init_ohci1394_dma_early)
		init_ohci1394_dma_on_all_controllers();
#endif
	/* Allocate bigger log buffer */
	setup_log_buf(1);

	reserve_initrd();

#ifndef CONFIG_XEN
	reserve_crashkernel();

	vsmp_init();
#endif

	io_delay_init();

#ifndef CONFIG_XEN
	if (boot_params.secure_boot) {
#else
	if (efi_enabled && efi_get_secure_boot()) {
#endif
		enforce_signed_modules();
#ifdef CONFIG_EFI
		secure_boot_enabled = 1;
#endif
	}

#ifdef CONFIG_ACPI
	if (!is_initial_xendomain()) {
		printk(KERN_INFO "ACPI in unprivileged domain disabled\n");
		disable_acpi();
	}
#endif

	/*
	 * Parse the ACPI tables for possible boot-time SMP configuration.
	 */
	acpi_boot_table_init();

	early_acpi_boot_init();

	initmem_init();
	memblock_find_dma_reserve();

#ifdef CONFIG_KVM_CLOCK
	kvmclock_init();
#endif

	x86_init.paging.pagetable_setup_start(swapper_pg_dir);
	paging_init();
	x86_init.paging.pagetable_setup_done(swapper_pg_dir);

	if (boot_cpu_data.cpuid_level >= 0) {
		/* A CPU has %cr4 if and only if it has CPUID */
		mmu_cr4_features = read_cr4();
	}

#if defined(CONFIG_X86_32) && !defined(CONFIG_XEN)
	/* sync back kernel address range */
	clone_pgd_range(initial_page_table + KERNEL_PGD_BOUNDARY,
			swapper_pg_dir     + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);
#endif

	tboot_probe();

#ifdef CONFIG_X86_64
	map_vsyscall();
#endif

#ifdef CONFIG_XEN
#ifdef CONFIG_KEXEC
	xen_machine_kexec_setup_resources();
# define kexec_enabled() (crashk_res.start < crashk_res.end)
#else
# define kexec_enabled() 0
#endif
	p2m_pages = max_pfn;
	if (xen_start_info->nr_pages > max_pfn) {
		/*
		 * the max_pfn was shrunk (probably by mem= or highmem=
		 * kernel parameter); shrink reservation with the HV
		 */
		struct xen_memory_reservation reservation = {
			.domid = DOMID_SELF
		};
		unsigned int difference;
		int ret;

		difference = xen_start_info->nr_pages - max_pfn;

		set_xen_guest_handle(reservation.extent_start,
				     phys_to_machine_mapping + max_pfn);
		reservation.nr_extents = difference;
		ret = HYPERVISOR_memory_op(XENMEM_decrease_reservation,
					   &reservation);
		BUG_ON(ret != difference);
	} else if (max_pfn > xen_start_info->nr_pages)
		p2m_pages = xen_start_info->nr_pages;

	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		/* Make sure we have a large enough P->M table. */
		phys_to_machine_mapping = alloc_bootmem_pages(
			max_pfn * sizeof(unsigned long));
#ifdef CONFIG_X86_32
		memcpy(phys_to_machine_mapping,
		       (unsigned long *)xen_start_info->mfn_list,
		       p2m_pages * sizeof(unsigned long));
		memset(phys_to_machine_mapping + p2m_pages, ~0,
		       (max_pfn - p2m_pages) * sizeof(unsigned long));
#else /* We must not use memcpy() here, as it's not capable
         of dealing with 4Gb or more at a time. */
		{
			void *src = __va(__pa(xen_start_info->mfn_list));
			unsigned long size, *dst = phys_to_machine_mapping;
			unsigned int fpp = PAGE_SIZE / sizeof(*dst);

			for (size = p2m_pages; size >= fpp; size -= fpp) {
				copy_page(dst, src);
				src += PAGE_SIZE;
				dst += fpp;
			}
			memcpy(dst, src, size * sizeof(*dst));
			dst += size;
			for (size = max_pfn - p2m_pages; size; --size)
				*dst++ = INVALID_P2M_ENTRY;
		}

		if (xen_start_info->mfn_list == VMEMMAP_START) {
			/*
			 * Since it is well isolated we can (and since it is
			 * perhaps large we should) also free the page tables
			 * mapping the initial P->M table.
			 */
			unsigned long va = VMEMMAP_START, pa;
			pgd_t *pgd = pgd_offset_k(va);
			pud_t *pud_page = pud_offset(pgd, 0);

			BUILD_BUG_ON(VMEMMAP_START & ~PGDIR_MASK);
			xen_l4_entry_update(pgd, __pgd(0));
			do {
				pud_t *pud = pud_page + pud_index(va);

				if (pud_none(*pud))
					va += PUD_SIZE;
				else if (pud_large(*pud)) {
					pa = pud_val(*pud) & PHYSICAL_PAGE_MASK;
					make_pages_writable(__va(pa),
						PUD_SIZE >> PAGE_SHIFT,
						XENFEAT_writable_page_tables);
					free_bootmem(pa, PUD_SIZE);
					va += PUD_SIZE;
				} else {
					pmd_t *pmd = pmd_offset(pud, va);

					if (pmd_large(*pmd)) {
						pa = pmd_val(*pmd) & PHYSICAL_PAGE_MASK;
						make_pages_writable(__va(pa),
							PMD_SIZE >> PAGE_SHIFT,
							XENFEAT_writable_page_tables);
						free_bootmem(pa, PMD_SIZE);
					} else if (!pmd_none(*pmd)) {
						unsigned int i;
						pte_t *pte = pte_offset_kernel(pmd, va);

						for (i = 0; i < PTRS_PER_PTE; ++i) {
							if (pte_none(pte[i]))
								break;
							pa = pte_pfn(pte[i]) << PAGE_SHIFT;
							make_page_writable(__va(pa),
								XENFEAT_writable_page_tables);
							free_bootmem(pa, PAGE_SIZE);
						}
						ClearPagePinned(virt_to_page(pte));
						make_page_writable(pte,
							XENFEAT_writable_page_tables);
						free_bootmem(__pa(pte), PAGE_SIZE);
					}
					va += PMD_SIZE;
					if (pmd_index(va))
						continue;
					ClearPagePinned(virt_to_page(pmd));
					make_page_writable(pmd,
						XENFEAT_writable_page_tables);
					free_bootmem(__pa((unsigned long)pmd
							  & PAGE_MASK),
						     PAGE_SIZE);
				}
			} while (pud_index(va) | pmd_index(va));
			ClearPagePinned(virt_to_page(pud_page));
			make_page_writable(pud_page,
					   XENFEAT_writable_page_tables);
			free_bootmem(__pa((unsigned long)pud_page & PAGE_MASK),
				     PAGE_SIZE);
		} else if (!WARN_ON(xen_start_info->mfn_list
				    < __START_KERNEL_map))
#endif
			free_bootmem(__pa(xen_start_info->mfn_list),
				     PFN_PHYS(PFN_UP(xen_start_info->nr_pages *
						     sizeof(unsigned long))));

		if (!is_initial_xendomain() || kexec_enabled())
			setup_pfn_to_mfn_frame_list(__alloc_bootmem);
	}

#ifdef CONFIG_ISA_DMA_API
# define ch p2m_pages
	/* Mark all ISA DMA channels in-use - using them wouldn't work. */
	for (ch = 0; ch < MAX_DMA_CHANNELS; ++ch)
		if (ch != 4 && request_dma(ch, "xen") != 0)
			BUG();
# undef ch
#endif
#else /* CONFIG_XEN */
	generic_apic_probe();

	early_quirks();
#endif

	/*
	 * Read APIC and some other early information from ACPI tables.
	 */
	acpi_boot_init();
	sfi_init();
	x86_dtb_init();

	/*
	 * get boot-time SMP configuration:
	 */
	if (smp_found_config)
		get_smp_config();

	prefill_possible_map();

	init_cpu_to_node();

#ifndef CONFIG_XEN
	init_apic_mappings();
	ioapic_and_gsi_init();

	kvm_guest_init();

	e820_reserve_resources();
	e820_mark_nosave_regions(max_low_pfn);
#else
	if (is_initial_xendomain())
		e820_reserve_resources();
#endif

	x86_init.resources.reserve_resources();

#ifdef CONFIG_XEN
	if (is_initial_xendomain())
#endif
		e820_setup_gap();

#ifdef CONFIG_VT
#ifdef CONFIG_DUMMY_CONSOLE
	conswitchp = &dummy_con;
#endif
#ifdef CONFIG_VGA_CONSOLE
#ifdef CONFIG_XEN
	if (!is_initial_xendomain())
		;
	else
#endif
	if (!efi_enabled || efi_mem_type(0xa0000) != EFI_CONVENTIONAL_MEMORY)
		conswitchp = &vga_con;
#endif
#endif
	x86_init.oem.banner();

	x86_init.timers.wallclock_init();

	mcheck_init();

	arch_init_ideal_nops();
}

#ifdef CONFIG_X86_32

static struct resource video_ram_resource = {
	.name	= "Video RAM area",
	.start	= 0xa0000,
	.end	= 0xbffff,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};

void __init i386_reserve_resources(void)
{
	if (is_initial_xendomain())
		request_resource(&iomem_resource, &video_ram_resource);
	reserve_standard_io_resources();
}

#endif /* CONFIG_X86_32 */

#ifdef CONFIG_XEN
static int
xen_panic_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	HYPERVISOR_shutdown(SHUTDOWN_crash);
	/* we're never actually going to get here... */
	return NOTIFY_DONE;
}
#endif /* !CONFIG_XEN */
