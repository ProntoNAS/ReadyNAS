/*
 * pm.c
 *
 * Power Management functions for Marvell Dove System On Chip
 *
 * Maintainer: Tawfik Bayouk <tawfik@marvell.com>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/suspend.h>
#include <linux/interrupt.h>

#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"

#ifdef CONFIG_SHEEVA_DEEP_IDLE
extern void armadaxp_deepidle(void);
/*
 * Logical check for Dove valid PM states
 */
static int dove_pm_valid(suspend_state_t state)
{
	return (state == PM_SUSPEND_STANDBY);
}

/*
 * Enter the requested PM state
 */
static int dove_pm_enter(suspend_state_t state)
{
	MV_U32	reg;

	switch (state)	{
	case PM_SUSPEND_STANDBY:
		printk("Entering Wol Mode (Nata IRQ8 is enabled now)...\n");

		/* Reenable the NETA IRQ in order to wake from it */
		reg = MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_GBE0_FIC));
		reg |= 0x1;
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_GBE0_FIC), reg);

		armadaxp_deepidle();

		/* Disable it since it will be re-enabled by the stack */
		reg = MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_GBE0_FIC));
		reg &= ~0x1;
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_GBE0_FIC), reg);

		printk("Exiting Wol Mode (Nata IRQ8 is enabled now)...\n");
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static struct platform_suspend_ops dove_pm_ops = {
	.valid		= dove_pm_valid,
	.enter		= dove_pm_enter,
};

static int __init dove_pm_init(void)
{
	printk("ArmadaXP Power Managament Suspend Operations Initialized\n");
	suspend_set_ops(&dove_pm_ops);
	return 0;
}

__initcall(dove_pm_init);

#else

static int __init dove_pm_init(void)
{
	printk("ArmadaXP Power Managament NOT Initialized (Missing Deep-Idle Support)\n");
	return 0;
}

__initcall(dove_pm_init);

#endif /* CONFIG_SHEEVA_DEEP_IDLE */
