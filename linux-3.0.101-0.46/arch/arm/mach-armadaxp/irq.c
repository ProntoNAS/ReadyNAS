/*
 * arch/arm/mach/irq.c
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <asm/mach/arch.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "mvOs.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"

unsigned int  irq_int_type[NR_IRQS];
static DEFINE_SPINLOCK(irq_controller_lock);

static void axp_unmask_fabric_interrupt(int cpu)
{
	u32 val;
	val = MV_REG_READ(CPU_CF_LOCAL_MASK_REG(cpu));
	val |=  (1 << cpu);
	MV_REG_WRITE(CPU_CF_LOCAL_MASK_REG(cpu), val);

#ifdef CONFIG_SMP
	if (cpu > 0) { /*enabled for both cpu */
		val = MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_MP));
		/* FIXME: assuming all 4 cpus */
		val |= 0xf;
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_MP), val);
	}
#endif
}

static void axp_mask_fabric_interrupt(int cpu)
{
	u32 val;
	val = MV_REG_READ(CPU_CF_LOCAL_MASK_REG(cpu));
	val &=  ~(1 << cpu);
	MV_REG_WRITE(CPU_CF_LOCAL_MASK_REG(cpu), val);

#ifdef CONFIG_SMP
	if (cpu > 0) { /*disabled for both cpu */
		val = MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_MP));
		val &= ~0xf;
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(IRQ_AURORA_MP), val);
	}
#endif
}

void axp_irq_mask(u32 irq)
{
	MV_U32 addr, temp, gpio_indx;

	if (irq >= IRQ_AURORA_GPIO_START) {
		/* calculate index in main interrupt */
		gpio_indx = (IRQ_AURORA_GPIO_0_7 + ((irq - IRQ_AURORA_GPIO_START) >> 3));
		/* add 1 because there is a gap between IRQ_AURORA_GPIO_24_31
		   and IRQ_AURORA_GPIO_32_39 */
		if (gpio_indx > IRQ_AURORA_GPIO_24_31)
			gpio_indx++;
		addr = (CPU_INT_SOURCE_CONTROL_REG(gpio_indx));
	}
	else
		addr = (CPU_INT_SOURCE_CONTROL_REG(irq));

	spin_lock(&irq_controller_lock);
	temp = MV_REG_READ(addr);

	if (irq >= IRQ_AURORA_GPIO_START) {
		MV_U32 bitmask = 1 << (irq & (32-1));
		MV_U32 reg = (irq - IRQ_AURORA_GPIO_START) >> 5;
		MV_REG_BIT_RESET(GPP_INT_LVL_REG(reg), bitmask);
	}

	if (irq <= 28) // per CPU
		temp &= ~(1 << smp_processor_id());
	/* for GPIO IRQs , don't disable INTS , they will be disabled in the units mask */
	else if (irq < IRQ_MAIN_INTS_NUM)
		temp &= ~0xf;

	MV_REG_WRITE(addr, temp);
	spin_unlock(&irq_controller_lock);
}

void axp_irq_unmask(u32 irq)
{
	MV_U32 addr, temp, gpio_indx;
	unsigned int map = 0x1;

	if (irq >= IRQ_AURORA_GPIO_START) {
		/* calculate index in main interrupt */
		gpio_indx = IRQ_AURORA_GPIO_0_7 + ((irq - IRQ_AURORA_GPIO_START) >> 3);
		/* add 1 because there is a gap between IRQ_AURORA_GPIO_24_31
		   and IRQ_AURORA_GPIO_32_39 */
		if (gpio_indx > IRQ_AURORA_GPIO_24_31)
			gpio_indx++;
		addr = (CPU_INT_SOURCE_CONTROL_REG(gpio_indx));
	}
	else
		addr = (CPU_INT_SOURCE_CONTROL_REG(irq));

	spin_lock(&irq_controller_lock);
	temp = MV_REG_READ(addr);

	if (irq >= IRQ_AURORA_GPIO_START) {
		MV_U32 bitmask = 1 << (irq & (32-1));
		MV_U32 reg = (irq - IRQ_AURORA_GPIO_START) >> 5;
		MV_REG_BIT_SET(GPP_INT_LVL_REG(reg), bitmask);
	}
#ifdef CONFIG_SMP
	else
		map = *cpus_addr(*irq_desc[irq].affinity);
#endif
	temp &= ~0xf;
	temp |= map;
	temp |= (0x1 << 28); /* Set IntEn for this source */
	MV_REG_WRITE(addr, temp);
	spin_unlock(&irq_controller_lock);
}


#ifdef CONFIG_SMP
int axp_set_affinity(unsigned int irq, const struct cpumask *mask_val)
{
	cpumask_copy(irq_desc[irq].affinity, mask_val);
	axp_irq_unmask(irq);
	return 0;
}

void second_cpu_init(void)
{
	/* open IPI mask */
	MV_REG_WRITE(AXP_IN_DRBEL_MSK, 0x1);
	axp_irq_unmask(IRQ_AURORA_IN_DRBL_LOW);
}
#endif

static struct irq_chip axp_irq_chip = {
	.name		= "axp_irq",
	.mask		= axp_irq_mask,
	.mask_ack	= axp_irq_mask,
	.unmask		= axp_irq_unmask,
	.disable	= axp_irq_mask,
	.enable		= axp_irq_unmask,
#ifdef CONFIG_SMP
	.set_affinity   = axp_set_affinity,
#endif
};


void __init axp_init_irq(void)
{
	u32 irq;

	/* MASK all interrupts */
	/* Enable IRQ in control register */
	for (irq = 0; irq < IRQ_MAIN_INTS_NUM; irq++) {
		axp_irq_mask(irq);
	}

	/*
	 * Register IRQ sources
	 */
	for (irq = 0; irq < NR_IRQS; irq++) {
		set_irq_chip(irq, &axp_irq_chip);
		set_irq_chip_data(irq, 0);
		set_irq_handler(irq, handle_level_irq);
		irq_desc[irq].status |= IRQ_LEVEL;
		set_irq_flags(irq, IRQF_VALID);
	}

#ifdef CONFIG_SMP
	{
		u32/*void __iomem **/addr;
		/* Set the default affinity to the boot cpu. */
		cpumask_clear(irq_default_affinity);
		cpumask_set_cpu(smp_processor_id(), irq_default_affinity);
		/* open IPI mask */
		axp_irq_unmask(IRQ_AURORA_IN_DRBL_LOW);
		addr = /*(void __iomem *)*/(AXP_IN_DRBEL_MSK);
		MV_REG_WRITE(addr, 0x1); // only IPI 0
	}
#endif

}

int pmu_request_irq(int irq, irq_handler_t handler)
{
	int i;
	int ret = request_irq(irq, handler, IRQF_DISABLED | IRQF_NOBALANCING, "armpmu", NULL);
	if (!ret) {
		for_each_online_cpu(i) {
			axp_unmask_fabric_interrupt(i);
		}
	}
	return ret;
}

void pmu_free_irq(int irq)
{
	int i;
	for_each_online_cpu(i) {
		axp_mask_fabric_interrupt(i);
	}
	free_irq(irq, NULL);
}
