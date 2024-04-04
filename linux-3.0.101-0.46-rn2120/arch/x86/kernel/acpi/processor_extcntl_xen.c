/*
 * processor_extcntl_xen.c - interface to notify Xen
 *
 *  Copyright (C) 2008, Intel corporation
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/acpi.h>
#include <linux/pm.h>
#include <linux/cpu.h>

#include <linux/cpufreq.h>
#include <acpi/processor.h>
#include <asm/hypercall.h>

static int xen_cx_notifier(struct acpi_processor *pr, int action)
{
	int ret, count = 0, i;
	xen_platform_op_t op = {
		.cmd			= XENPF_set_processor_pminfo,
		.interface_version	= XENPF_INTERFACE_VERSION,
		.u.set_pminfo.id	= pr->acpi_id,
		.u.set_pminfo.type	= XEN_PM_CX,
	};
	struct xen_processor_cx *data, *buf;
	struct acpi_processor_cx *cx;

	/* Convert to Xen defined structure and hypercall */
	buf = kzalloc(pr->power.count * sizeof(struct xen_processor_cx),
			GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	data = buf;
	for (i = 1; i <= pr->power.count; i++) {
		cx = &pr->power.states[i];
		/* Skip invalid cstate entry */
		if (!cx->valid)
			continue;

		data->type = cx->type;
		data->latency = cx->latency;
		data->power = cx->power;
		data->reg.space_id = cx->reg.space_id;
		data->reg.bit_width = cx->reg.bit_width;
		data->reg.bit_offset = cx->reg.bit_offset;
		data->reg.access_size = cx->reg.access_size;
		data->reg.address = cx->reg.address;

		/* Get dependency relationships */
		if (cx->csd_count) {
			pr_warning("_CSD found: Not supported for now!\n");
			kfree(buf);
			return -EINVAL;
		} else {
			data->dpcnt = 0;
			set_xen_guest_handle(data->dp, NULL);
		}

		data++;
		count++;
	}

	if (!count) {
		pr_info("No available Cx info for cpu %d\n", pr->acpi_id);
		kfree(buf);
		return -EINVAL;
	}

	op.u.set_pminfo.u.power.count = count;
	op.u.set_pminfo.u.power.flags.bm_control = pr->flags.bm_control;
	op.u.set_pminfo.u.power.flags.bm_check = pr->flags.bm_check;
	op.u.set_pminfo.u.power.flags.has_cst = pr->flags.has_cst;
	op.u.set_pminfo.u.power.flags.power_setup_done = pr->flags.power_setup_done;

	set_xen_guest_handle(op.u.set_pminfo.u.power.states, buf);
	ret = HYPERVISOR_platform_op(&op);
	kfree(buf);
	return ret;
}

static int xen_px_notifier(struct acpi_processor *pr, int action)
{
	int ret = -EINVAL;
	xen_platform_op_t op = {
		.cmd			= XENPF_set_processor_pminfo,
		.interface_version	= XENPF_INTERFACE_VERSION,
		.u.set_pminfo.id	= pr->acpi_id,
		.u.set_pminfo.type	= XEN_PM_PX,
	};
	struct xen_processor_performance *perf;
	struct xen_processor_px *states = NULL;
	struct acpi_processor_performance *px;
	struct acpi_psd_package *pdomain;

	if (!pr)
		return -EINVAL;

	perf = &op.u.set_pminfo.u.perf;
	px = pr->performance;
	if (!px)
		return -EINVAL;

	switch(action) {
	case PROCESSOR_PM_CHANGE:
		/* ppc dynamic handle */
		perf->flags = XEN_PX_PPC;
		perf->platform_limit = pr->performance_platform_limit;

		ret = HYPERVISOR_platform_op(&op);
		break;

	case PROCESSOR_PM_INIT:
		/* px normal init */
		perf->flags = XEN_PX_PPC | 
			      XEN_PX_PCT | 
			      XEN_PX_PSS | 
			      XEN_PX_PSD;

		/* ppc */
		perf->platform_limit = pr->performance_platform_limit;

		/* pct */
		xen_convert_pct_reg(&perf->control_register, &px->control_register);
		xen_convert_pct_reg(&perf->status_register, &px->status_register);

		/* pss */
		perf->state_count = px->state_count;
		states = kzalloc(px->state_count*sizeof(xen_processor_px_t),GFP_KERNEL);
		if (!states)
			return -ENOMEM;
		xen_convert_pss_states(states, px->states, px->state_count);
		set_xen_guest_handle(perf->states, states);

		/* psd */
		pdomain = &px->domain_info;
		xen_convert_psd_pack(&perf->domain_info, pdomain);
		if (pdomain->coord_type == DOMAIN_COORD_TYPE_SW_ALL)
			perf->shared_type = CPUFREQ_SHARED_TYPE_ALL;
		else if (pdomain->coord_type == DOMAIN_COORD_TYPE_SW_ANY)
			perf->shared_type = CPUFREQ_SHARED_TYPE_ANY;
		else if (pdomain->coord_type == DOMAIN_COORD_TYPE_HW_ALL)
			perf->shared_type = CPUFREQ_SHARED_TYPE_HW;
		else {
			ret = -ENODEV;
			kfree(states);
			break;
		}

		ret = HYPERVISOR_platform_op(&op);
		kfree(states);
		break;

	default:
		break;
	}

	return ret;
}

static int xen_tx_notifier(struct acpi_processor *pr, int action)
{
	return -EINVAL;
}

static int xen_hotplug_notifier(struct acpi_processor *pr, int event)
{
	int ret = -EINVAL;
#ifdef CONFIG_ACPI_HOTPLUG_CPU
	acpi_status status = 0;
	acpi_object_type type;
	uint32_t apic_id;
	int device_decl = 0;
	unsigned long long pxm;
	xen_platform_op_t op;

	status = acpi_get_type(pr->handle, &type);
	if (ACPI_FAILURE(status)) {
		pr_warn("can't get object type for acpi_id %#x\n",
			pr->acpi_id);
		return -ENXIO;
	}

	switch (type) {
	case ACPI_TYPE_PROCESSOR:
		break;
	case ACPI_TYPE_DEVICE:
		device_decl = 1;
		break;
	default:
		pr_warn("unsupported object type %#x for acpi_id %#x\n",
			type, pr->acpi_id);
		return -EOPNOTSUPP;
	}

	apic_id = acpi_get_cpuid(pr->handle, ~device_decl, pr->acpi_id);
	if (apic_id == -1) {
		pr_warn("can't get apic_id for acpi_id %#x\n", pr->acpi_id);
		return -ENODATA;
	}

	status = acpi_evaluate_integer(pr->handle, "_PXM", NULL, &pxm);
	if (ACPI_FAILURE(status)) {
		pr_warn("can't get pxm for acpi_id %#x\n", pr->acpi_id);
		return -ENODATA;
	}

	switch (event) {
	case HOTPLUG_TYPE_ADD:
		op.cmd = XENPF_cpu_hotadd;
		op.u.cpu_add.apic_id = apic_id;
		op.u.cpu_add.acpi_id = pr->acpi_id;
		op.u.cpu_add.pxm = pxm;
		ret = HYPERVISOR_platform_op(&op);
		break;
	case HOTPLUG_TYPE_REMOVE:
		pr_warn("Xen doesn't support CPU hot remove\n");
		ret = -EOPNOTSUPP;
		break;
	}
#endif

	return ret;
}

static struct processor_extcntl_ops xen_extcntl_ops = {
	.hotplug		= xen_hotplug_notifier,
};

static int __init init_extcntl(void)
{
	unsigned int pmbits = (xen_start_info->flags & SIF_PM_MASK) >> 8;

#ifndef CONFIG_ACPI_HOTPLUG_CPU
	if (!pmbits)
		return 0;
#endif
	if (pmbits & XEN_PROCESSOR_PM_CX)
		xen_extcntl_ops.pm_ops[PM_TYPE_IDLE] = xen_cx_notifier;
	if (pmbits & XEN_PROCESSOR_PM_PX)
		xen_extcntl_ops.pm_ops[PM_TYPE_PERF] = xen_px_notifier;
	if (pmbits & XEN_PROCESSOR_PM_TX)
		xen_extcntl_ops.pm_ops[PM_TYPE_THR] = xen_tx_notifier;

	processor_extcntl_ops = &xen_extcntl_ops;

	return 0;
}
arch_initcall(init_extcntl);

unsigned int cpufreq_quick_get(unsigned int cpu)
{
	xen_platform_op_t op;

	op.cmd = XENPF_get_cpu_freq;
	op.u.get_cpu_freq.vcpu = cpu;
	return HYPERVISOR_platform_op(&op) == 0 ? op.u.get_cpu_freq.freq : 0;
}

unsigned int cpufreq_quick_get_max(unsigned int cpu)
{
	xen_platform_op_t op;

	op.cmd = XENPF_get_cpu_freq_max;
	op.u.get_cpu_freq.vcpu = cpu;
	return HYPERVISOR_platform_op(&op) == 0 ? op.u.get_cpu_freq.freq : 0;
}
EXPORT_SYMBOL(cpufreq_quick_get_max);
