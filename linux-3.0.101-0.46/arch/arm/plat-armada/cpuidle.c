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

#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "mvOs.h"

static MV_AHB_TO_MBUS_DEC_WIN ahbAddrDecWin[MAX_AHB_TO_MBUS_WINS];
static MV_ADDR_WIN ahbAddrWinRemap[MAX_AHB_TO_MBUS_WINS];

extern int armadaxp_cpu_resume(void);
#ifdef CONFIG_ARMADA_SUPPORT_DEEP_IDLE_FAST_EXIT
extern int armadaxp_deep_idle_exit(void);
extern unsigned char armadaxp_deep_idle_exit_start;
extern unsigned char armadaxp_deep_idle_exit_end;
#endif

unsigned long suspend_phys_addr(void * physaddr)
{
        return virt_to_phys(physaddr);
}

extern u32 identity_page_table_phys;

#if 0
unsigned long suspend_phys_addr(void * physaddr)
{
	return virt_to_phys(physaddr);
}
 
static inline void identity_mapping_add(pgd_t *pgd, unsigned long start,
					unsigned long end)
{
        unsigned long addr, prot;
        pmd_t *pmd;

        prot = PMD_TYPE_SECT | PMD_SECT_AP_WRITE;

        for (addr = start & PGDIR_MASK; addr < end;) {
                pmd = pmd_offset(pgd + pgd_index(addr), addr);
                pmd[0] = __pmd(addr | prot);
                addr += SECTION_SIZE;
                pmd[1] = __pmd(addr | prot);
                addr += SECTION_SIZE;
                flush_pmd_entry(pmd);
                outer_clean_range(__pa(pmd), __pa(pmd + 1));
        }
}
static inline void identity_mapping_del(pgd_t *pgd, unsigned long start,
					unsigned long end)
{
        unsigned long addr;
        pmd_t *pmd;

        for (addr = start & PGDIR_MASK; addr < end; addr += PGDIR_SIZE) {
                pmd = pmd_offset(pgd + pgd_index(addr), addr);
                pmd[0] = __pmd(0);
                pmd[1] = __pmd(0);
                clean_pmd_entry(pmd);
                outer_clean_range(__pa(pmd), __pa(pmd + 1));
        }
}
#endif
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

int pm_disable = 1;
static int __init pm_enable_setup(char *__unused)
{
	pm_disable = 0;
	return 1;
}

__setup("pm_enable", pm_enable_setup);


#define ARMADAXP_IDLE_STATES	2

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
	MV_U32	i;

	reg = MV_REG_READ(MV_L2C_NFABRIC_PM_CTRL_CFG_REG);
	reg |= MV_L2C_NFABRIC_PM_CTRL_CFG_PWR_DOWN;
	MV_REG_WRITE(MV_L2C_NFABRIC_PM_CTRL_CFG_REG, reg);

#ifdef  CONFIG_ARCH_ARMADA_XP
	for (i=0; i<4; i++) {
#else
	for (i=0; i<1; i++) {
#endif
		/* Enable L2 & Fabric powerdown in Deep-Idle mode */
		reg = MV_REG_READ(PM_CONTROL_AND_CONFIG_REG(i));
		reg |= PM_CONTROL_AND_CONFIG_L2_PWDDN;
		MV_REG_WRITE(PM_CONTROL_AND_CONFIG_REG(i), reg);
	}

#ifdef CONFIG_ARMADA_XP_Z1_DEEP_IDLE_WA
	/* Configure CPU_DivClk_Control0 */
	reg = MV_REG_READ(0x18700);
	reg &= ~0xFFFF00;
	reg |= 0x10EF00;
	MV_REG_WRITE(0x18700, reg);

	/* Configure  PMU_DFS_Control_1 */
	reg = MV_REG_READ(0x1C054);
	reg &= 0xFF000000;
	reg >>= 24;
	reg = (reg << 24) | ((reg + 1) << 16) | 0x10404;
	MV_REG_WRITE(0x1C054, reg);

	/* Configure  PMU Program registers */
	MV_REG_WRITE(0x1C270, 0x00c108a8);
	MV_REG_WRITE(0x1C274, 0x0000005a);
	MV_REG_WRITE(0x1C278, 0x00000000);
	MV_REG_WRITE(0x1C27c, 0x195b0000);
	MV_REG_WRITE(0x1C280, 0x00ff0014);

#endif
#ifdef CONFIG_ARMADA_XP_DEEP_IDLE_L2_WA
	/* disable HW L2C Flush configure 0x22008:
	 *  Set bit 4 -> Will skip HW L2C flush triggering.
	 *  Set bit 5 -> Will skip waiting for HW L2C flush done indication.
	 */
	reg = MV_REG_READ(MV_L2C_NFABRIC_PWR_DOWN_FLOW_CTRL_REG);
	reg |= 3 << 4;

	/* Configure skiping the RA & WA Disable */
	reg |= 1;
	/* Configure skiping the Sharing Disable */
	reg |= 1 << 8;
	/* Configure skiping the CIB Ack Disable */
	reg |= 1 << 6;
	/* Configure skiping the RA & WA Resume */
	reg |= 1 << 12;
	/* Configure skiping the CIB Ack Resume */
	reg |= 1 << 15;
	/* Configure skiping the Sharing Resume */
	reg |= 1 << 14;

	MV_REG_WRITE(MV_L2C_NFABRIC_PWR_DOWN_FLOW_CTRL_REG, reg);
#endif

#ifdef CONFIG_ARMADA_XP_DEEP_IDLE_L2_WA
	/* neet to restore this register on resume */
	cib_ctrl_cfg_reg = MV_REG_READ(MV_CIB_CTRL_CFG_REG);
#endif

	/* Set the resume control registers to do nothing */
	MV_REG_WRITE(0x20980, 0);
	MV_REG_WRITE(0x20988, 0);
#if 0
	/* configure the MPP29 used for CPU0+L2+Fabric power control*/
	reg = MV_REG_READ(MPP_CONTROL_REG(3));
	reg &= ~0x00F00000;
	reg |= 0x00500000;
	MV_REG_WRITE(MPP_CONTROL_REG(3), reg);

	/* configure the MPP40 used for CPU1 power control*/
	reg = MV_REG_READ(MPP_CONTROL_REG(5));
	reg &= ~0x0000000F;
	reg |= 0x00000003;
	MV_REG_WRITE(MPP_CONTROL_REG(5), reg);

	/* configure the MPP57 used for CPU2+3 power control*/
	reg = MV_REG_READ(MPP_CONTROL_REG(7));
	reg &= ~0x000000F0;
	reg |= 0x00000020;
	MV_REG_WRITE(MPP_CONTROL_REG(7), reg);
#endif
}

void mv_cpuidle_restore_cpu_win_state(void)
{
	u32 i;

	/* Save CPU windows state, and enable access for Bootrom	*
	** according to SoC default address decoding windows.		*/
	for(i = 0; i < MAX_AHB_TO_MBUS_WINS; i++) {
		mvAhbToMbusWinSet(i, &ahbAddrDecWin[i]);
		mvAhbToMbusWinRemap(i, &ahbAddrWinRemap[i]);
	}

}

void mv_cpuidle_reset_cpu_win_state(void)
{
	u32 i;
	MV_AHB_TO_MBUS_DEC_WIN	winInfo;

#ifdef CONFIG_ARMADA_SUPPORT_DEEP_IDLE_FAST_EXIT
	u32 length = &armadaxp_deep_idle_exit_end - &armadaxp_deep_idle_exit_start;
	memcpy((void *)CRYPT_ENG_VIRT_BASE(0), &armadaxp_deep_idle_exit_start, length);
	mvOsCacheFlush(NULL, CRYPT_ENG_VIRT_BASE(0), length);
#endif
	/* Save CPU windows state, and enable access for Bootrom	*
	** according to SoC default address decoding windows.		*/
	for(i = 0; i < MAX_AHB_TO_MBUS_WINS; i++) {
		mvAhbToMbusWinGet(i, &ahbAddrDecWin[i]);
		mvAhbToMbusWinRemapGet(i, &ahbAddrWinRemap[i]);

		/* Disable the window */
		mvAhbToMbusWinEnable(i, MV_FALSE);
	}

	/* Open default windows for Bootrom, PnC and internal regs.	*/
	/* Bootrom */
#ifndef CONFIG_ARMADA_SUPPORT_DEEP_IDLE_FAST_EXIT
	winInfo.target = BOOT_ROM_CS;
	winInfo.addrWin.baseLow = 0xF8000000;
	winInfo.addrWin.baseHigh = 0x0;
	winInfo.addrWin.size = _128M;
	winInfo.enable = MV_TRUE;
	mvAhbToMbusWinSet(13, &winInfo);

	winInfo.target = CRYPT0_ENG;
	winInfo.addrWin.baseLow = 0xC8010000;
	winInfo.addrWin.baseHigh = 0x0;
	winInfo.addrWin.size = _64K;
	winInfo.enable = MV_TRUE;
	mvAhbToMbusWinSet(8, &winInfo);
#else
	/* Cesa SRAM */
	winInfo.target = CRYPT0_ENG;
	winInfo.addrWin.baseLow = 0xFFFF0000;
	winInfo.addrWin.baseHigh = 0x0;
	winInfo.addrWin.size = _64K;
	winInfo.enable = MV_TRUE;
	mvAhbToMbusWinSet(13, &winInfo);
#endif



}

void armadaxp_fabric_prepare_deepIdle(void)
{
	unsigned int processor_id = hard_smp_processor_id();
	MV_U32  reg;

	MV_REG_WRITE(PM_CPU_BOOT_ADDR_REDIRECT(processor_id), virt_to_phys(armadaxp_cpu_resume));

#ifdef CONFIG_AURORA_IO_CACHE_COHERENCY
	/* Disable delivery of snoop requests to the CPU core by setting */
	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	reg &= ~(1 << (24 + processor_id));
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CTRL_REG, reg);
#endif

	reg = MV_REG_READ(PM_STATUS_AND_MASK_REG(processor_id));
	/* set WaitMask fields */
	reg |= PM_STATUS_AND_MASK_CPU_IDLE_WAIT;
	reg |= PM_STATUS_AND_MASK_SNP_Q_EMPTY_WAIT;
	/* Enable wakeup events */
	reg |= PM_STATUS_AND_MASK_IRQ_WAKEUP | PM_STATUS_AND_MASK_FIQ_WAKEUP;
//	reg |= PM_STATUS_AND_MASK_DBG_WAKEUP;

#ifdef CONFIG_ARMADA_XP_DEEP_IDLE_UNMASK_INTS_WA
	/* don't mask interrupts due to known issue */
#else
	/* Mask interrupts */
	reg |= PM_STATUS_AND_MASK_IRQ_MASK | PM_STATUS_AND_MASK_FIQ_MASK;
#endif
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

#ifdef CONFIG_ARMADA_XP_DEEP_IDLE_L2_WA
	/* Disable RA & WA allocate */
	reg = MV_REG_READ(MV_CIB_CTRL_CFG_REG);
	reg &= ~0x1E;
	reg |= 0x12;
	MV_REG_WRITE(MV_CIB_CTRL_CFG_REG, reg);

	auroraL2_flush_all();

	/* Disable CIB Ack */
	reg = MV_REG_READ(MV_CIB_CTRL_CFG_REG);
	reg |= 1 << 9;
	MV_REG_WRITE(MV_CIB_CTRL_CFG_REG, reg);

	/* wait for CIB empty */
	udelay(1);

	/* Disable CIB Sharing */
	reg = MV_REG_READ(MV_CIB_CTRL_CFG_REG);
	reg &=  ~(3 << 10);
	reg |= 0x2 << 10;
	MV_REG_WRITE(MV_CIB_CTRL_CFG_REG, reg);
#endif
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
#ifdef CONFIG_AURORA_IO_CACHE_COHERENCY
	/* cancel Disable delivery of snoop requests to the CPU core by setting */
	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	reg |= 1 << (24 + processor_id);
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CTRL_REG, reg);
#endif
#ifdef CONFIG_ARMADA_XP_DEEP_IDLE_L2_WA
	/* restore CIB  Control and Configuration register */
	MV_REG_WRITE(MV_CIB_CTRL_CFG_REG, cib_ctrl_cfg_reg);
#endif

}

/*
 * Enter the DEEP IDLE mode (power off CPU only)
 */
void armadaxp_deepidle(void)
{
	pr_debug("armadaxp_deepidle: Entering DEEP IDLE mode.\n");

#ifdef CONFIG_IWMMXT
	/* force any iWMMXt context to ram **/
	if (elf_hwcap & HWCAP_IWMMXT)
		iwmmxt_task_disable(NULL);
#endif

#if defined(CONFIG_VFP)
        vfp_save();
#endif
	aurora_l2_pm_enter();
//	armadaxp_fabric_prepare_deepIdle();

	/* none zero means deepIdle wasn't entered and regret event happened */
	mv_cpuidle_reset_cpu_win_state();
	armadaxp_cpu_suspend();
	cpu_init();

	armadaxp_fabric_restore_deepIdle();
	mv_cpuidle_restore_cpu_win_state();

	aurora_l2_pm_exit();
#if defined(CONFIG_VFP)
	vfp_restore();
#endif
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
#ifdef CONFIG_SHEEVA_ERRATA_ARM_CPU_BTS61
	/* Deep Idle */
		armadaxp_deepidle();
#else
		/* Wait for interrupt state */
		cpu_do_idle();
#endif
	}
	else if (state == &dev->states[1]) {
		/* Deep Idle */
		armadaxp_deepidle();
	}
	do_gettimeofday(&after);
	local_fiq_enable();
	local_irq_enable();
	idle_time = (after.tv_sec - before.tv_sec) * USEC_PER_SEC +
			(after.tv_usec - before.tv_usec);
	return idle_time;
}

#ifdef CONFIG_MV_PMU_PROC
static int device_registered;
struct proc_dir_entry *cpu_idle_proc;

static int mv_cpu_idle_write(struct file *file, const char *buffer,
			     unsigned long count, void *data)
{
	struct cpuidle_device *	device = &per_cpu(armadaxp_cpuidle_device, smp_processor_id());

        if (!strncmp (buffer, "enable", strlen("enable"))) {
                if(device_registered == 0) {
                        device_registered = 1;
                        if (cpuidle_register_device(device)) {
                                printk(KERN_ERR "mv_cpu_idle_write: Failed registering\n");
                                return -EIO;
                        }
                }
                cpuidle_enable_device(device);
        } else if (!strncmp (buffer, "disable", strlen("disable"))) {
                cpuidle_disable_device(device);
        } else if (!strncmp (buffer, "test", strlen("test"))) {


//for debug
		disable_irq(IRQ_AURORA_TIMER0);
		disable_irq(IRQ_AURORA_GBE0_FIC);
		disable_irq(IRQ_AURORA_SDIO);
		disable_irq(IRQ_AURORA_SATA0);
//		disable_irq(IRQ_AURORA_UART0);

                printk(KERN_INFO "Press any key to leave deep idle:");
//		cpu_do_idle();
		armadaxp_deepidle();


		enable_irq(IRQ_AURORA_TIMER0);
		enable_irq(IRQ_AURORA_GBE0_FIC);
		enable_irq(IRQ_AURORA_SDIO);
		enable_irq(IRQ_AURORA_SATA0);
//		enable_irq(IRQ_AURORA_UART0);

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
		       "test - Manually enter CPU Idle state, exit by ket stroke (DEBUG ONLY).\n");

}

#endif /* CONFIG_MV_PMU_PROC */

/*
 * Register Armadaxp IDLE states
 */
int armadaxp_init_cpuidle(void)
{
	struct cpuidle_device *device;

	printk("Initializing Armada-XP CPU power management ");

	cpuidle_register_driver(&armadaxp_idle_driver);
	device = &per_cpu(armadaxp_cpuidle_device, smp_processor_id());
	device->state_count = ARMADAXP_IDLE_STATES;

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

	if (build_identity_page_table()) {
		printk(KERN_ERR "armadaxp_init_cpuidle: Failed to build identity page table\n");
                return -ENOMEM;
        }
	armadaxp_fabric_setup_deepIdle();

#ifdef CONFIG_MV_PMU_PROC
        /* Create proc entry. */
        cpu_idle_proc = create_proc_entry("cpu_idle", 0666, NULL);
        cpu_idle_proc->read_proc = mv_cpu_idle_read;
        cpu_idle_proc->write_proc = mv_cpu_idle_write;
        cpu_idle_proc->nlink = 1;
#endif

	if (pm_disable) {
		printk(" (DISABLED)\n");
		return 0;
	}

	if (cpuidle_register_device(device)) {
		printk(KERN_ERR "armadaxp_init_cpuidle: Failed registering\n");
		return -EIO;
	}

	printk("\n");

	return 0;
}

device_initcall(armadaxp_init_cpuidle);
