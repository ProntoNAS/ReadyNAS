/*
 * arch/arm/plat-armada/cpuidle.c
 *
 * CPU idle implementation for Marvell ARMADA-XP SoCs
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 *
 */
//#define DEBUG
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/cpuidle.h>
#include <asm/io.h>
#include <asm/proc-fns.h>
#include <plat/cache-aurora-l2.h>
#include <mach/smp.h>
#include <asm/vfp.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/pgalloc.h>
#include <asm/sections.h>

#include <../cpuidle.h>
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "mvOs.h"

extern int armadaxp_cpu_resume(void);
extern int axp_secondary_startup(void);
extern int secondary_startup(void);

#ifdef CONFIG_ARMADA_SUPPORT_DEEP_IDLE_FAST_EXIT
extern int armadaxp_deep_idle_exit(void);
extern unsigned char armadaxp_deep_idle_exit_start;
extern unsigned char armadaxp_deep_idle_exit_end;
#endif

#ifdef	CONFIG_CPU_IDLE
static int device_registered;

static void hw_sem_lock(void)
{
	unsigned int cpu = hard_smp_processor_id();

	while(cpu != (readb(INTER_REGS_BASE + MV_CPU_HW_SEM_OFFSET) & 0xf));
}

static void hw_sem_unlock(void)
{
	writeb(0xff, INTER_REGS_BASE + MV_CPU_HW_SEM_OFFSET);
}

unsigned long suspend_phys_addr(void * physaddr)
{
        return virt_to_phys(physaddr);
}

extern u32 identity_page_table_phys;

/*
 * Allocate initial page tables to allow the CPU to
 * enable the MMU safely.  This essentially means a set
 * of our "standard" page tables, with the addition of
 * a 1:1 mapping for the physical address of the kernel.
 */

static int build_identity_page_table(void)
{
	pgd_t *pgd = pgd_alloc(&init_mm);
	if (!pgd)
		return -ENOMEM;

	if (PHYS_OFFSET != PAGE_OFFSET) {
		identity_mapping_add(pgd, __pa(_stext), __pa(_etext));
		identity_mapping_add(pgd, __pa(_sdata), __pa(_edata)); /* is this needed?*/
	}
	identity_page_table_phys = virt_to_phys(*pgd);
	return 0;
}

int pm_mode = DISABLED;
int pm_support = WFI;

static int __init pm_enable_setup(char *str)
{
	if (!strncmp(str, "wfi", 3))
		pm_support = WFI;
	else if (!strncmp(str, "idle", 4))
		pm_support = DEEP_IDLE;
	else if (!strncmp(str, "snooze", 6))
		pm_support = SNOOZE;
	else if (!strncmp(str, "off", 3))
		pm_support = DISABLED;
	
	return 1;
}

__setup("pm_level=", pm_enable_setup);

#define ARMADAXP_IDLE_STATES	3

struct cpuidle_driver armadaxp_idle_driver = {
	.name =         "armadaxp_idle",
	.owner =        THIS_MODULE,
};

DEFINE_PER_CPU(struct cpuidle_device, armadaxp_cpuidle_device);

u32 cib_ctrl_cfg_reg;

extern int armadaxp_cpu_suspend(void);
void armadaxp_fabric_setup_deepIdle(void)
{
	MV_U32  reg;

	/* Enable L2 & Fabric powerdown in Deep-Idle mode - Fabric */
	reg = MV_REG_READ(MV_L2C_NFABRIC_PM_CTRL_CFG_REG);
	reg |= MV_L2C_NFABRIC_PM_CTRL_CFG_PWR_DOWN;
	MV_REG_WRITE(MV_L2C_NFABRIC_PM_CTRL_CFG_REG, reg);

	/* Set the resume control registers to do nothing */
	MV_REG_WRITE(0x20980, 0);
	MV_REG_WRITE(0x20988, 0);
}

#ifdef CONFIG_HOTPLUG_CPU
void armadaxp_fabric_prepare_hotplug(void)
{
	unsigned int processor_id = hard_smp_processor_id();
	MV_U32  reg;

	MV_REG_WRITE(PM_CPU_BOOT_ADDR_REDIRECT(processor_id), virt_to_phys(axp_secondary_startup));
	
	reg = MV_REG_READ(PM_STATUS_AND_MASK_REG(processor_id));
	/* set WaitMask fields */
	reg |= PM_STATUS_AND_MASK_CPU_IDLE_WAIT;
	/* Enable wakeup events */
	reg |= PM_STATUS_AND_MASK_IRQ_WAKEUP | PM_STATUS_AND_MASK_FIQ_WAKEUP;
//	reg |= PM_STATUS_AND_MASK_DBG_WAKEUP;

	/* Mask interrupts */
	reg |= PM_STATUS_AND_MASK_IRQ_MASK | PM_STATUS_AND_MASK_FIQ_MASK;

	MV_REG_WRITE(PM_STATUS_AND_MASK_REG(processor_id), reg);

	/* Disable delivering of other CPU core cache maintenance instruction,
	 * TLB, and Instruction synchronization to the CPU core 
	 */
	/* TODO */
#ifdef CONFIG_CACHE_AURORA_L2
	/* ask HW to power down the L2 Cache if possible */
	reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
	reg |= PM_CONTROL_AND_CONFIG_L2_PWDDN;
	MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);
#endif

	/* request power down */
	reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
	reg |= PM_CONTROL_AND_CONFIG_PWDDN_REQ;
	MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);

#if defined CONFIG_AURORA_IO_CACHE_COHERENCY
	hw_sem_lock();
	/* Disable delivery of snoop requests to the CPU core by setting */
	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	reg &= ~(1 << (24 + processor_id));
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CTRL_REG, reg);
	hw_sem_unlock();
#endif
}
#endif

void armadaxp_fabric_prepare_deepIdle(void)
{
	unsigned int processor_id = hard_smp_processor_id();
	MV_U32  reg;

	MV_REG_WRITE(PM_CPU_BOOT_ADDR_REDIRECT(processor_id), virt_to_phys(armadaxp_cpu_resume));
	
	reg = MV_REG_READ(PM_STATUS_AND_MASK_REG(processor_id));
	/* set WaitMask fields */
	reg |= PM_STATUS_AND_MASK_CPU_IDLE_WAIT;
	/* Enable wakeup events */
	reg |= PM_STATUS_AND_MASK_IRQ_WAKEUP | PM_STATUS_AND_MASK_FIQ_WAKEUP;
//	reg |= PM_STATUS_AND_MASK_DBG_WAKEUP;

	/* Mask interrupts */
	reg |= PM_STATUS_AND_MASK_IRQ_MASK | PM_STATUS_AND_MASK_FIQ_MASK;

	MV_REG_WRITE(PM_STATUS_AND_MASK_REG(processor_id), reg);

	/* Disable delivering of other CPU core cache maintenance instruction,
	 * TLB, and Instruction synchronization to the CPU core 
	 */
	/* TODO */
#ifdef CONFIG_CACHE_AURORA_L2
	if (pm_mode == SNOOZE) {
		/* ask HW to power down the L2 Cache if possible */
		reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
		reg |= PM_CONTROL_AND_CONFIG_L2_PWDDN;
		MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);
	}
#endif
	/* request power down */
	reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
	reg |= PM_CONTROL_AND_CONFIG_PWDDN_REQ;
	MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);
}

void armadaxp_fabric_restore_deepIdle(void)
{
	unsigned int processor_id = hard_smp_processor_id();
	MV_U32  reg;

	/* cancel request power down */
	reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
	reg &= ~PM_CONTROL_AND_CONFIG_PWDDN_REQ;
	MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);

#ifdef CONFIG_CACHE_AURORA_L2
	/* cancel ask HW to power down the L2 Cache if possible */
	reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(processor_id));
	reg &= ~PM_CONTROL_AND_CONFIG_L2_PWDDN;
	MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(processor_id), reg);
#endif
	/* cancel Disable delivering of other CPU core cache maintenance instruction,
	 * TLB, and Instruction synchronization to the CPU core 
	 */
	/* TODO */

	/* cancel Enable wakeup events */
	reg = MV_REG_READ(PM_STATUS_AND_MASK_REG(processor_id));
	reg &= ~(PM_STATUS_AND_MASK_IRQ_WAKEUP | PM_STATUS_AND_MASK_FIQ_WAKEUP);
	reg &= ~PM_STATUS_AND_MASK_CPU_IDLE_WAIT;
	reg &= ~PM_STATUS_AND_MASK_SNP_Q_EMPTY_WAIT;
//	reg &= ~PM_STATUS_AND_MASK_DBG_WAKEUP;

	/* Mask interrupts */
	reg &= ~(PM_STATUS_AND_MASK_IRQ_MASK | PM_STATUS_AND_MASK_FIQ_MASK);
	MV_REG_WRITE(PM_STATUS_AND_MASK_REG(processor_id), reg);

#if defined(CONFIG_AURORA_IO_CACHE_COHERENCY)
	/* cancel Disable delivery of snoop requests to the CPU core by setting */
	hw_sem_lock();
	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	reg |= 1 << (24 + processor_id);
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CTRL_REG, reg);
	MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	hw_sem_unlock();
#endif
}

/*
 * Enter the DEEP IDLE mode (power off CPU only)
 */
void armadaxp_deepidle(int power_state)
{
	pr_debug("armadaxp_deepidle: Entering DEEP IDLE mode.\n");

	pm_mode = power_state;
#ifdef CONFIG_IWMMXT
	/* force any iWMMXt context to ram **/
	if (elf_hwcap & HWCAP_IWMMXT)
		iwmmxt_task_disable(NULL);
#endif

#if defined(CONFIG_VFP)
        vfp_save();
#endif
	aurora_l2_pm_enter();
	/* none zero means deepIdle wasn't entered and regret event happened */
	armadaxp_cpu_suspend();
	cpu_init();
	armadaxp_fabric_restore_deepIdle();

	aurora_l2_pm_exit();
#if defined(CONFIG_VFP)
	vfp_restore();
#endif

	pm_mode = pm_support;

	pr_debug("armadaxp_deepidle: Exiting DEEP IDLE.\n");
}

/* Actual code that puts the SoC in different idle states */
static int armadaxp_enter_idle(struct cpuidle_device *dev,
			       struct cpuidle_state *state)
{
	struct timeval before, after;
	int idle_time;

	local_irq_disable();
	local_fiq_disable();
	do_gettimeofday(&before);
	if (state == &dev->states[0]) {
//		printk(KERN_ERR "armadaxp_enter_idle: WFI \n");
#ifdef CONFIG_SHEEVA_ERRATA_ARM_CPU_BTS61
		/* Deep Idle */
		armadaxp_deepidle(DEEP_IDLE);
#else
		/* Wait for interrupt state */
		cpu_do_idle();
#endif
	} else if (state == &dev->states[1]) {
//		printk(KERN_ERR "armadaxp_enter_idle: Deep Idle \n");
		/* Deep Idle */
		armadaxp_deepidle(DEEP_IDLE);
	} else if (state == &dev->states[2]) {
//		printk(KERN_ERR "armadaxp_enter_idle: Snooze \n");
		/* Snooze */
		armadaxp_deepidle(SNOOZE);
	}
	do_gettimeofday(&after);
	local_fiq_enable();
	local_irq_enable();
	idle_time = (after.tv_sec - before.tv_sec) * USEC_PER_SEC +
			(after.tv_usec - before.tv_usec);
#if 0
	if ( state == &dev->states[1])
	printk(KERN_INFO "%s: state %d idle time %d\n", __func__, state == &dev->states[0]? 0 : 1,
	       idle_time);
#endif
	return idle_time;
}

#ifdef CONFIG_MV_PMU_PROC
struct proc_dir_entry *cpu_idle_proc;

static int mv_cpu_idle_write(struct file *file, const char *buffer,
			     unsigned long count, void *data)
{
	int i;
	unsigned int backup[IRQ_MAIN_INTS_NUM];
	MV_PM_STATES target_power_state;

	struct cpuidle_device *	device = &per_cpu(armadaxp_cpuidle_device, smp_processor_id());

	if (!strncmp (buffer, "enable", strlen("enable"))) {
		for_each_online_cpu(i) {
			device = &per_cpu(armadaxp_cpuidle_device, i);
			if(device_registered == 0) {
				device_registered = 1;
				if (cpuidle_register_device(device)) {
					printk(KERN_ERR "mv_cpu_idle_write: Failed registering\n");
					return -EIO;
				}
			}
			cpuidle_enable_device(device);
		}
	} else if (!strncmp (buffer, "disable", strlen("disable"))) {
		for_each_online_cpu(i) {
			device = &per_cpu(armadaxp_cpuidle_device, i);
			cpuidle_disable_device(device);
		}
	} else if (!strncmp (buffer, "deep", strlen("deep")) || !strncmp (buffer, "snooze", strlen("snooze")) || 
				   !strncmp (buffer, "wfi", strlen("wfi"))) {
		if (!strncmp (buffer, "deep", strlen("deep")))
			target_power_state = DEEP_IDLE;
		else if (!strncmp (buffer, "snooze", strlen("snooze")))
			target_power_state = SNOOZE;
		else		/* WFI */
			target_power_state = WFI;
		
		for (i=0; i<IRQ_MAIN_INTS_NUM; i++) {
			if (i == IRQ_AURORA_UART0)
				continue;

			backup[i] = MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(i));
			MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(i), 0);
		}
		
		printk(KERN_INFO "Processor id = %d, Press any key to leave deep idle:",smp_processor_id());

		if (target_power_state > WFI)
			armadaxp_deepidle(target_power_state);
		else
			cpu_do_idle();

		for (i=0; i<IRQ_MAIN_INTS_NUM; i++) {
			if (i == IRQ_AURORA_UART0)
				continue;
			MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(i), backup[i]);
		}

		pm_mode = pm_support;
	}

	return count;
}

static int mv_cpu_idle_read(char *buffer, char **buffer_location, off_t offset,
			    int buffer_length, int *zero, void *ptr)
{
        if (offset > 0)

                return 0;
        return sprintf(buffer, "enable - Enable CPU Idle framework.\n"
                                "disable - Disable CPU idle framework.\n"
			"wfi - Manually enter CPU WFI state, exit by ket stroke (DEBUG ONLY).\n"
			"deep - Manually enter CPU Idle state, exit by ket stroke (DEBUG ONLY).\n"
			"snooze - Manually enter CPU and Fabric Idle and state, exit by ket stroke (DEBUG ONLY).\n");

}

#endif /* CONFIG_MV_PMU_PROC */

/* 
 * Register Armadaxp IDLE states
 */
int armadaxp_init_cpuidle(void)
{
	struct cpuidle_device *device;
	int i;
	device_registered = 1;

	printk("Initializing Armada-XP CPU power management ");

	if (build_identity_page_table()) {
		printk(KERN_ERR "armadaxp_init_cpuidle: Failed to build identity page table\n");
                return -ENOMEM;
	}

	cpuidle_register_driver(&armadaxp_idle_driver);

	armadaxp_fabric_setup_deepIdle();

#ifdef CONFIG_MV_PMU_PROC
	/* Create proc entry. */
	cpu_idle_proc = create_proc_entry("cpu_idle", 0666, NULL);
	cpu_idle_proc->read_proc = mv_cpu_idle_read;
	cpu_idle_proc->write_proc = mv_cpu_idle_write;
	cpu_idle_proc->nlink = 1;
#endif
	
	if (pm_support == WFI)
		printk(" (WFI)\n");
	else if (pm_support == DEEP_IDLE)
		printk(" (IDLE)\n");
	else if (pm_support == SNOOZE)
		printk(" (SNOOZE)\n");
	else {
		printk(" (DISABLED)\n");
		device_registered = DISABLED;
	}

	pm_mode = pm_support;

	for_each_online_cpu(i) {
		device = &per_cpu(armadaxp_cpuidle_device, i);
		device->cpu = i;

		device->state_count = pm_support;
	
		/* Wait for interrupt state */
		device->states[0].enter = armadaxp_enter_idle;
		device->states[0].exit_latency = 1;		/* Few CPU clock cycles */
		device->states[0].target_residency = 10;
		device->states[0].flags = CPUIDLE_FLAG_TIME_VALID;
		strcpy(device->states[0].name, "WFI");
		strcpy(device->states[0].desc, "Wait for interrupt");
	
		/* Deep Idle Mode */
		device->states[1].enter = armadaxp_enter_idle;
		device->states[1].exit_latency = 100;
		device->states[1].target_residency = 1000;
		device->states[1].flags = CPUIDLE_FLAG_TIME_VALID;
		strcpy(device->states[1].name, "DEEP IDLE");
		strcpy(device->states[1].desc, "Deep Idle");
		
		/* Snooze - Deep Deep Idle Mode */
		device->states[2].enter = armadaxp_enter_idle;
		device->states[2].exit_latency = 1000;
		device->states[2].target_residency = 10000;
		device->states[2].flags = CPUIDLE_FLAG_TIME_VALID;
		strcpy(device->states[2].name, "SNOOZE");
		strcpy(device->states[2].desc, "Snooze");
		
		if(pm_mode) {
			if (cpuidle_register_device(device)) {
				printk(KERN_ERR "armadaxp_init_cpuidle: Failed registering\n");
				return -EIO;
			}
		}
	}

	return 0;
}

device_initcall(armadaxp_init_cpuidle);

#endif
