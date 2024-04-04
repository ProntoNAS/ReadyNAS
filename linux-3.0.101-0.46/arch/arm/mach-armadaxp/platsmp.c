/*
 *  linux/arch/arm/mach-armadaxp/platsmp.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>

#include <asm/cacheflush.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/unified.h>

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"

extern void axp_secondary_startup(void);
extern void second_cpu_init(void);
extern MV_CPU_DEC_WIN* mv_sys_map(void);

/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen"
 */
volatile int __cpuinitdata pen_release = -1;

static unsigned int __init get_core_count(void)
{
#ifdef CONFIG_MACH_ARMADA_XP_FPGA
	return 2;
#else
	/* Read the number of availabe CPUs in the SoC */
	return ((MV_REG_READ(SOC_COHERENCY_FABRIC_CFG_REG) & 0xF) + 1);
#endif
}

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
	trace_hardirqs_off();

	/*
	 * if any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */
	second_cpu_init();

	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	pen_release = -1;
	smp_wmb();

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	flush_cache_all();
	pen_release = cpu;
	flush_cache_all();

	timeout = jiffies + (10 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		dmac_map_area((const void *)&pen_release, 32, DMA_BIDIRECTIONAL);
		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	//printk("pen_release %d \n",pen_release);
	return pen_release != -1 ? -ENOSYS : 0;
}

#define AXP_CPU_DIVCLK_CTRL0			0x18700
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL0	0x18708
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1	0x1870C

static void __init wakeup_cpus(void)
{
	MV_U32 val = 0;
	MV_U32 ncores = get_core_count();
#ifndef CONFIG_MACH_ARMADA_XP_FPGA
	/* Scale up CPU#1 clock to max */
	if (ncores > 1) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL0);
		val &= ~(0xFF000000); 	/* cpu1 clkdiv ratio; cpu0 based on SAR */
		val |= 0x1 << 24;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL0, val);
	}

	/* Scale up CPU#2 clock to max */
	if (ncores > 2) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
		val &= ~0x00FF0000;	/* cpus 2 clkdiv ratios */
		val |= 0x1 << 16;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);
	}

	/* Scale up CPU#3 clock to max */
	if (ncores > 3) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
		val &= ~0xFF000000;	/* cpus 3 clkdiv ratios */
		val |= 0x1 << 24;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);
	}

	/* Set clock devider reload smooth bit mask */
	val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
	val |= ((0x1 << (ncores-1)) - 1) << 21;
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

	/* Request clock devider reload */
	val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
	val |= 1 << 24;
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

	/* Wait for clocks to settle down then release reload request */
	udelay(100);
	val &= ~(0xf << 21);
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);
	udelay(100);
#endif
	/* Set resume control and address */
	MV_REG_WRITE(AXP_CPU_RESUME_CTRL_REG, 0x0);
	if (ncores > 1) MV_REG_WRITE(AXP_CPU_RESUME_ADDR_REG(1), virt_to_phys(axp_secondary_startup));
	if (ncores > 2) MV_REG_WRITE(AXP_CPU_RESUME_ADDR_REG(2), virt_to_phys(axp_secondary_startup));
	if (ncores > 3) MV_REG_WRITE(AXP_CPU_RESUME_ADDR_REG(3), virt_to_phys(axp_secondary_startup));

	/* nobody is to be released from the pen yet */
	pen_release = -1;

	/* Kick secondary CPUs */
	if (ncores > 1) MV_REG_WRITE(AXP_CPU_RESET_REG(1), 0x0);
	if (ncores > 2)	MV_REG_WRITE(AXP_CPU_RESET_REG(2), 0x0);
	if (ncores > 3) MV_REG_WRITE(AXP_CPU_RESET_REG(3), 0x0);

	mb();
	udelay(10);
}

static void __init initialize_bridge(void)
{
	MV_U32 reg;
	MV_U32 ncores = get_core_count();

	/* Associate all available CPUs to SMP group 0 */
	reg = MV_REG_READ(SOC_COHERENCY_FABRIC_CFG_REG);
	reg |= (((0x1 << ncores) - 1) << 24);
	MV_REG_WRITE(SOC_COHERENCY_FABRIC_CFG_REG, reg);

	/* enable CPUs in SMP group on Fabric coherency */
	reg = MV_REG_READ(SOC_COHERENCY_FABRIC_CTRL_REG);
	reg |= (((0x1 << ncores) - 1) << 24);
	MV_REG_WRITE(SOC_COHERENCY_FABRIC_CTRL_REG, reg);
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init smp_init_cpus(void)
{
	MV_U32 i;
	MV_U32 ncores = get_core_count();

	printk("SMP: init cpus\n");
	/* Set CPU address decoding */
	if( mvCpuIfInit(mv_sys_map())) {
		printk( "Cpu Interface initialization failed.\n" );
		return;
	}
	mvCpuIfAddDecShow();

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int ncores = get_core_count();
	unsigned int cpu = smp_processor_id();
	int i;

	printk("SMP: prepare CPUs (%d cores)\n", ncores);
	/* sanity check */
	if (ncores == 0) {
		printk(KERN_ERR
		       "strange CM count of 0? Default to 1\n");

		ncores = 1;
	}

	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
		       "no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	smp_store_cpu_info(cpu);

	/*
	 * are we trying to boot more cores than exist?
	 */
	if (max_cpus > ncores)
		max_cpus = ncores;

#ifdef CONFIG_LOCAL_TIMERS
	/*
	 * Enable the local timer for primary CPU. If the device is
	 * dummy (!CONFIG_LOCAL_TIMERS), it was already registers in
	 * aurora_time_init
	 */
//	local_timer_setup();
#endif

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	if (max_cpus > 1) {
		flush_cache_all();
		wakeup_cpus();
	}
	initialize_bridge();
}
