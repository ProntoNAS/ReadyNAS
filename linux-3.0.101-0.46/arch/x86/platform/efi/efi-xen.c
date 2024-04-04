/*
 * Common EFI (Extensible Firmware Interface) support functions
 * Based on Extensible Firmware Interface Specification version 1.0
 *
 * Copyright (C) 1999 VA Linux Systems
 * Copyright (C) 1999 Walt Drummond <drummond@valinux.com>
 * Copyright (C) 1999-2002 Hewlett-Packard Co.
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 *	Stephane Eranian <eranian@hpl.hp.com>
 * Copyright (C) 2005-2008 Intel Co.
 *	Fenghua Yu <fenghua.yu@intel.com>
 *	Bibo Mao <bibo.mao@intel.com>
 *	Chandramouli Narayanan <mouli@linux.intel.com>
 *	Huang Ying <ying.huang@intel.com>
 *
 * Copied from efi_32.c to eliminate the duplicated code between EFI
 * 32/64 support code. --ying 2007-10-26
 *
 * All EFI Runtime Services are not implemented yet as EFI only
 * supports physical mode addressing on SoftSDV. This is to be fixed
 * in a future version.  --drummond 1999-07-20
 *
 * Implemented EFI runtime services and virtual mode calls.  --davidm
 *
 * Goutham Rao: <goutham.rao@intel.com>
 *	Skip non-WB memory and ignore empty memory ranges.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/efi.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/time.h>

#include <asm/setup.h>
#include <asm/efi.h>
#include <asm/time.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/x86_init.h>

#include <xen/interface/platform.h>

#define EFI_DEBUG	1
#define PFX 		"EFI: "

#define EFI_MIN_RESERVE 5120

#define EFI_DUMMY_GUID \
	EFI_GUID(0x4424ac57, 0xbe4b, 0x47dd, 0x9e, 0x97, 0xed, 0x50, 0xf0, 0x9f, 0x92, 0xa9)

static efi_char16_t efi_dummy_name[6] = { 'D', 'U', 'M', 'M', 'Y', 0 };

int __read_mostly efi_enabled;
EXPORT_SYMBOL(efi_enabled);

int __read_mostly secure_boot_enabled;

static int __init setup_noefi(char *arg)
{
	efi_enabled = 0;
	return 0;
}
early_param("noefi", setup_noefi);

static bool efi_no_storage_paranoia;

static int __init setup_storage_paranoia(char *arg)
{
	efi_no_storage_paranoia = true;
	return 0;
}
early_param("efi_no_storage_paranoia", setup_storage_paranoia);

#define call op.u.efi_runtime_call
#define DECLARE_CALL(what) \
	struct xen_platform_op op; \
	op.cmd = XENPF_efi_runtime_call; \
	call.function = XEN_EFI_##what; \
	call.misc = 0

static efi_status_t xen_efi_get_time(efi_time_t *tm, efi_time_cap_t *tc)
{
	int err;
	DECLARE_CALL(get_time);

	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	if (tm) {
		BUILD_BUG_ON(sizeof(*tm) != sizeof(call.u.get_time.time));
		memcpy(tm, &call.u.get_time.time, sizeof(*tm));
	}

	if (tc) {
		tc->resolution = call.u.get_time.resolution;
		tc->accuracy = call.u.get_time.accuracy;
		tc->sets_to_zero = !!(call.misc &
				      XEN_EFI_GET_TIME_SET_CLEARS_NS);
	}

	return call.status;
}

static efi_status_t xen_efi_set_time(efi_time_t *tm)
{
	DECLARE_CALL(set_time);

	BUILD_BUG_ON(sizeof(*tm) != sizeof(call.u.set_time));
	memcpy(&call.u.set_time, tm, sizeof(*tm));

	return HYPERVISOR_platform_op(&op) ? EFI_UNSUPPORTED : call.status;
}

static efi_status_t xen_efi_get_wakeup_time(efi_bool_t *enabled,
					    efi_bool_t *pending,
					    efi_time_t *tm)
{
	int err;
	DECLARE_CALL(get_wakeup_time);

	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	if (tm) {
		BUILD_BUG_ON(sizeof(*tm) != sizeof(call.u.get_wakeup_time));
		memcpy(tm, &call.u.get_wakeup_time, sizeof(*tm));
	}

	if (enabled)
		*enabled = !!(call.misc & XEN_EFI_GET_WAKEUP_TIME_ENABLED);

	if (pending)
		*pending = !!(call.misc & XEN_EFI_GET_WAKEUP_TIME_PENDING);

	return call.status;
}

static efi_status_t xen_efi_set_wakeup_time(efi_bool_t enabled, efi_time_t *tm)
{
	DECLARE_CALL(set_wakeup_time);

	BUILD_BUG_ON(sizeof(*tm) != sizeof(call.u.set_wakeup_time));
	if (enabled)
		call.misc = XEN_EFI_SET_WAKEUP_TIME_ENABLE;
	if (tm)
		memcpy(&call.u.set_wakeup_time, tm, sizeof(*tm));
	else
		call.misc |= XEN_EFI_SET_WAKEUP_TIME_ENABLE_ONLY;

	return HYPERVISOR_platform_op(&op) ? EFI_UNSUPPORTED : call.status;
}

static efi_status_t xen_efi_get_variable(efi_char16_t *name,
					 efi_guid_t *vendor,
					 u32 *attr,
					 unsigned long *data_size,
					 void *data)
{
	int err;
	DECLARE_CALL(get_variable);

	set_xen_guest_handle(call.u.get_variable.name, name);
	BUILD_BUG_ON(sizeof(*vendor) !=
		     sizeof(call.u.get_variable.vendor_guid));
	memcpy(&call.u.get_variable.vendor_guid, vendor, sizeof(*vendor));
	call.u.get_variable.size = *data_size;
	set_xen_guest_handle(call.u.get_variable.data, data);
	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	*data_size = call.u.get_variable.size;
	if (attr)
		*attr = call.misc;

	return call.status;
}

static efi_status_t xen_efi_get_next_variable(unsigned long *name_size,
					      efi_char16_t *name,
					      efi_guid_t *vendor)
{
	int err;
	DECLARE_CALL(get_next_variable_name);

	call.u.get_next_variable_name.size = *name_size;
	set_xen_guest_handle(call.u.get_next_variable_name.name, name);
	BUILD_BUG_ON(sizeof(*vendor) !=
		     sizeof(call.u.get_next_variable_name.vendor_guid));
	memcpy(&call.u.get_next_variable_name.vendor_guid, vendor,
	       sizeof(*vendor));
	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	*name_size = call.u.get_next_variable_name.size;
	memcpy(vendor, &call.u.get_next_variable_name.vendor_guid,
	       sizeof(*vendor));

	return call.status;
}

static efi_status_t xen_efi_set_variable(efi_char16_t *name,
					 efi_guid_t *vendor,
					 u32 attr,
					 unsigned long data_size,
					 void *data)
{
	DECLARE_CALL(set_variable);

	set_xen_guest_handle(call.u.set_variable.name, name);
	call.misc = attr;
	BUILD_BUG_ON(sizeof(*vendor) !=
		     sizeof(call.u.set_variable.vendor_guid));
	memcpy(&call.u.set_variable.vendor_guid, vendor, sizeof(*vendor));
	call.u.set_variable.size = data_size;
	set_xen_guest_handle(call.u.set_variable.data, data);

	return HYPERVISOR_platform_op(&op) ? EFI_UNSUPPORTED : call.status;
}

static efi_status_t xen_efi_query_variable_info(u32 attr,
						u64 *storage_space,
						u64 *remaining_space,
						u64 *max_variable_size)
{
	int err;
	DECLARE_CALL(query_variable_info);

	if (efi.runtime_version < EFI_2_00_SYSTEM_TABLE_REVISION)
		return EFI_UNSUPPORTED;

	call.u.query_variable_info.attr = attr;

	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	*storage_space = call.u.query_variable_info.max_store_size;
	*remaining_space = call.u.query_variable_info.remain_store_size;
	*max_variable_size = call.u.query_variable_info.max_size;

	return call.status;
}

static efi_status_t xen_efi_get_next_high_mono_count(u32 *count)
{
	int err;
	DECLARE_CALL(get_next_high_monotonic_count);

	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	*count = call.misc;

	return call.status;
}

static efi_status_t xen_efi_update_capsule(efi_capsule_header_t **capsules,
					   unsigned long count,
					   unsigned long sg_list)
{
	DECLARE_CALL(update_capsule);

	if (efi.runtime_version < EFI_2_00_SYSTEM_TABLE_REVISION)
		return EFI_UNSUPPORTED;

	set_xen_guest_handle(call.u.update_capsule.capsule_header_array,
			     capsules);
	call.u.update_capsule.capsule_count = count;
	call.u.update_capsule.sg_list = sg_list;

	return HYPERVISOR_platform_op(&op) ? EFI_UNSUPPORTED : call.status;
}

static efi_status_t xen_efi_query_capsule_caps(efi_capsule_header_t **capsules,
					       unsigned long count,
					       u64 *max_size,
					       int *reset_type)
{
	int err;
	DECLARE_CALL(query_capsule_capabilities);

	if (efi.runtime_version < EFI_2_00_SYSTEM_TABLE_REVISION)
		return EFI_UNSUPPORTED;

	set_xen_guest_handle(call.u.query_capsule_capabilities.capsule_header_array,
			     capsules);
	call.u.query_capsule_capabilities.capsule_count = count;

	err = HYPERVISOR_platform_op(&op);
	if (err)
		return EFI_UNSUPPORTED;

	*max_size = call.u.query_capsule_capabilities.max_capsule_size;
	*reset_type = call.u.query_capsule_capabilities.reset_type;

	return call.status;
}

struct efi __read_mostly efi = {
	.mps                      = EFI_INVALID_TABLE_ADDR,
	.acpi                     = EFI_INVALID_TABLE_ADDR,
	.acpi20                   = EFI_INVALID_TABLE_ADDR,
	.smbios                   = EFI_INVALID_TABLE_ADDR,
	.sal_systab               = EFI_INVALID_TABLE_ADDR,
	.boot_info                = EFI_INVALID_TABLE_ADDR,
	.hcdp                     = EFI_INVALID_TABLE_ADDR,
	.uga                      = EFI_INVALID_TABLE_ADDR,
	.uv_systab                = EFI_INVALID_TABLE_ADDR,
	.get_time                 = xen_efi_get_time,
	.set_time                 = xen_efi_set_time,
	.get_wakeup_time          = xen_efi_get_wakeup_time,
	.set_wakeup_time          = xen_efi_set_wakeup_time,
	.get_variable             = xen_efi_get_variable,
	.get_next_variable        = xen_efi_get_next_variable,
	.set_variable             = xen_efi_set_variable,
	.get_next_high_mono_count = xen_efi_get_next_high_mono_count,
	.query_variable_info      = xen_efi_query_variable_info,
	.update_capsule           = xen_efi_update_capsule,
	.query_capsule_caps       = xen_efi_query_capsule_caps,
};
EXPORT_SYMBOL(efi);

int efi_set_rtc_mmss(unsigned long nowtime)
{
	int real_seconds, real_minutes;
	efi_status_t 	status;
	efi_time_t 	eft;
	efi_time_cap_t 	cap;

	status = efi.get_time(&eft, &cap);
	if (status != EFI_SUCCESS) {
		printk(KERN_ERR "Oops: efitime: can't read time!\n");
		return -1;
	}

	real_seconds = nowtime % 60;
	real_minutes = nowtime / 60;
	if (((abs(real_minutes - eft.minute) + 15)/30) & 1)
		real_minutes += 30;
	real_minutes %= 60;
	eft.minute = real_minutes;
	eft.second = real_seconds;

	status = efi.set_time(&eft);
	if (status != EFI_SUCCESS) {
		printk(KERN_ERR "Oops: efitime: can't write time!\n");
		return -1;
	}
	return 0;
}

unsigned long efi_get_time(void)
{
	efi_status_t status;
	efi_time_t eft;
	efi_time_cap_t cap;

	status = efi.get_time(&eft, &cap);
	if (status != EFI_SUCCESS) {
		printk(KERN_ERR "Oops: efitime: can't read time!\n");
		return mach_get_cmos_time();
	}

	return mktime(eft.year, eft.month, eft.day, eft.hour,
		      eft.minute, eft.second);
}

void __init efi_probe(void)
{
	static struct xen_platform_op __initdata op = {
		.cmd = XENPF_firmware_info,
		.u.firmware_info = {
			.type = XEN_FW_EFI_INFO,
			.index = XEN_FW_EFI_CONFIG_TABLE
		}
	};

	if (HYPERVISOR_platform_op(&op) == 0)
		efi_enabled = 1;
}

void __init efi_reserve_boot_services(void) { }

void __init efi_init(void)
{
	efi_config_table_t *config_tables;
	efi_char16_t c16[100];
	char vendor[ARRAY_SIZE(c16)] = "unknown";
	int ret, i;
	struct xen_platform_op op;
	union xenpf_efi_info *info = &op.u.firmware_info.u.efi_info;

	op.cmd = XENPF_firmware_info;
	op.u.firmware_info.type = XEN_FW_EFI_INFO;

	/*
	 * Show what we know for posterity
	 */
	op.u.firmware_info.index = XEN_FW_EFI_VENDOR;
	info->vendor.bufsz = sizeof(c16);
	set_xen_guest_handle(info->vendor.name, c16);
	ret = HYPERVISOR_platform_op(&op);
	if (!ret) {
		for (i = 0; i < sizeof(vendor) - 1 && c16[i]; ++i)
			vendor[i] = c16[i];
		vendor[i] = '\0';
	} else
		printk(KERN_ERR PFX "Could not get the firmware vendor!\n");

	op.u.firmware_info.index = XEN_FW_EFI_VERSION;
	ret = HYPERVISOR_platform_op(&op);
	if (!ret)
		printk(KERN_INFO "EFI v%u.%.02u by %s\n",
		       info->version >> 16,
		       info->version & 0xffff, vendor);
	else
		printk(KERN_ERR PFX "Could not get EFI revision!\n");

	op.u.firmware_info.index = XEN_FW_EFI_RT_VERSION;
	ret = HYPERVISOR_platform_op(&op);
	if (!ret)
		efi.runtime_version = info->version;
	else
		printk(KERN_WARNING "Could not get runtime services revision.\n");

	/*
	 * Let's see what config tables the firmware passed to us.
	 */
	op.u.firmware_info.index = XEN_FW_EFI_CONFIG_TABLE;
	if (HYPERVISOR_platform_op(&op))
		BUG();
	config_tables = early_ioremap(
		info->cfg.addr,
		info->cfg.nent * sizeof(efi_config_table_t));
	if (config_tables == NULL)
		panic("Could not map EFI Configuration Table!\n");

	printk(KERN_INFO);
	for (i = 0; i < info->cfg.nent; i++) {
		if (!efi_guidcmp(config_tables[i].guid, MPS_TABLE_GUID)) {
			efi.mps = config_tables[i].table;
			printk(" MPS=0x%lx ", config_tables[i].table);
		} else if (!efi_guidcmp(config_tables[i].guid,
					ACPI_20_TABLE_GUID)) {
			efi.acpi20 = config_tables[i].table;
			printk(" ACPI 2.0=0x%lx ", config_tables[i].table);
		} else if (!efi_guidcmp(config_tables[i].guid,
					ACPI_TABLE_GUID)) {
			efi.acpi = config_tables[i].table;
			printk(" ACPI=0x%lx ", config_tables[i].table);
		} else if (!efi_guidcmp(config_tables[i].guid,
					SMBIOS_TABLE_GUID)) {
			efi.smbios = config_tables[i].table;
			printk(" SMBIOS=0x%lx ", config_tables[i].table);
		} else if (!efi_guidcmp(config_tables[i].guid,
					HCDP_TABLE_GUID)) {
			efi.hcdp = config_tables[i].table;
			printk(" HCDP=0x%lx ", config_tables[i].table);
		} else if (!efi_guidcmp(config_tables[i].guid,
					UGA_IO_PROTOCOL_GUID)) {
			efi.uga = config_tables[i].table;
			printk(" UGA=0x%lx ", config_tables[i].table);
		}
	}
	printk("\n");
	early_iounmap(config_tables, info->cfg.nent * sizeof(efi_config_table_t));

	x86_platform.get_wallclock = efi_get_time;
}

#undef DECLARE_CALL
#undef call

void __init efi_enter_virtual_mode(void)
{
	/* clean DUMMY object */
	xen_efi_set_variable(efi_dummy_name, &EFI_DUMMY_GUID,
			     EFI_VARIABLE_NON_VOLATILE |
			     EFI_VARIABLE_BOOTSERVICE_ACCESS |
			     EFI_VARIABLE_RUNTIME_ACCESS,
			     0, NULL);
}

static struct platform_device rtc_efi_dev = {
	.name = "rtc-efi",
	.id = -1,
};

static int __init rtc_init(void)
{
	if (efi_enabled && platform_device_register(&rtc_efi_dev) < 0)
		printk(KERN_ERR "unable to register rtc device...\n");

	/* not necessarily an error */
	return 0;
}
arch_initcall(rtc_init);

bool __init efi_get_secure_boot(void)
{
	u8 sb, setup;
	unsigned long datasize = sizeof(sb);
	static efi_guid_t __initdata var_guid = EFI_GLOBAL_VARIABLE_GUID;
	efi_status_t status;

	status = xen_efi_get_variable(L"SecureBoot", &var_guid, NULL,
				      &datasize, &sb);

	if (status != EFI_SUCCESS || !sb)
		return false;

	status = xen_efi_get_variable(L"SetupMode", &var_guid, NULL,
				      &datasize, &setup);

	return status == EFI_SUCCESS && !setup;
}

/*
 * Convenience functions to obtain memory types and attributes
 */
u32 efi_mem_type(unsigned long phys_addr)
{
	struct xen_platform_op op;
	union xenpf_efi_info *info = &op.u.firmware_info.u.efi_info;

	op.cmd = XENPF_firmware_info;
	op.u.firmware_info.type = XEN_FW_EFI_INFO;
	op.u.firmware_info.index = XEN_FW_EFI_MEM_INFO;
	info->mem.addr = phys_addr;
	info->mem.size = 0;
	return HYPERVISOR_platform_op(&op) ? 0 : info->mem.type;
}

u64 efi_mem_attributes(unsigned long phys_addr)
{
	struct xen_platform_op op;
	union xenpf_efi_info *info = &op.u.firmware_info.u.efi_info;

	op.cmd = XENPF_firmware_info;
	op.u.firmware_info.type = XEN_FW_EFI_INFO;
	op.u.firmware_info.index = XEN_FW_EFI_MEM_INFO;
	info->mem.addr = phys_addr;
	info->mem.size = 0;
	return HYPERVISOR_platform_op(&op) ? 0 : info->mem.attr;
}

/*
 * Some firmware has serious problems when using more than 50% of the EFI
 * variable store, i.e. it triggers bugs that can brick machines. Ensure that
 * we never use more than this safe limit.
 *
 * Return EFI_SUCCESS if it is safe to write 'size' bytes to the variable
 * store.
 */
efi_status_t efi_query_variable_store(u32 attributes, unsigned long size)
{
	efi_status_t status;
	u64 storage_size, remaining_size, max_size;

	if (!(attributes & EFI_VARIABLE_NON_VOLATILE))
		return 0;

	status = xen_efi_query_variable_info(attributes, &storage_size,
					     &remaining_size, &max_size);
	if (status != EFI_SUCCESS)
		return status;

	/*
	 * Some firmware implementations refuse to boot if there's insufficient
	 * space in the variable store. We account for that by refusing the
	 * write if permitting it would reduce the available space to under
	 * 5KB. This figure was provided by Samsung, so should be safe.
	 */
	if ((remaining_size - size < EFI_MIN_RESERVE) &&
		!efi_no_storage_paranoia) {

		/*
		 * Triggering garbage collection may require that the firmware
		 * generate a real EFI_OUT_OF_RESOURCES error. We can force
		 * that by attempting to use more space than is available.
		 */
		unsigned long dummy_size = remaining_size + 1024;
		void *dummy = kzalloc(dummy_size, GFP_ATOMIC);

		if (!dummy)
			return EFI_OUT_OF_RESOURCES;

		status = xen_efi_set_variable(efi_dummy_name, &EFI_DUMMY_GUID,
					      EFI_VARIABLE_NON_VOLATILE |
					      EFI_VARIABLE_BOOTSERVICE_ACCESS |
					      EFI_VARIABLE_RUNTIME_ACCESS,
					      dummy_size, dummy);
		kfree(dummy);

		if (status == EFI_SUCCESS) {
			/*
			 * This should have failed, so if it didn't make sure
			 * that we delete it...
			 */
			xen_efi_set_variable(efi_dummy_name, &EFI_DUMMY_GUID,
					     EFI_VARIABLE_NON_VOLATILE |
					     EFI_VARIABLE_BOOTSERVICE_ACCESS |
					     EFI_VARIABLE_RUNTIME_ACCESS,
					     0, NULL);
		}

		/*
		 * The runtime code may now have triggered a garbage collection
		 * run, so check the variable info again
		 */
		status = xen_efi_query_variable_info(attributes, &storage_size,
						     &remaining_size, &max_size);

		if (status != EFI_SUCCESS)
			return status;

		/*
		 * There still isn't enough room, so return an error
		 */
		if (remaining_size - size < EFI_MIN_RESERVE)
			return EFI_OUT_OF_RESOURCES;
	}

	return EFI_SUCCESS;
}
EXPORT_SYMBOL_GPL(efi_query_variable_store);
