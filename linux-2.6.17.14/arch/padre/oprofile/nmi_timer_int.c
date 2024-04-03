/**
 * @file nmi_timer_int.c
 *
 * @remark Copyright 2003 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Zwane Mwaikambo <zwane@linuxpower.ca>
 */
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/oprofile.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/timer.h>
#include <asm/padre0.h>
#include <asm/ptrace.h>
#include <asm/io.h>
 
static void * dev_id = NULL;
static unsigned long tm_0 = PADRE_TM_CTRL_AUTO_BIT(0) | PADRE_TM_CTRL_LOAD_BIT(0) | PADRE_TM_CTRL_INTEN_BIT(0) | PADRE_TM_CTRL_SEL_BIT(0);
static unsigned long tm_1 = PADRE_TM_CTRL_AUTO_BIT(1) | PADRE_TM_CTRL_LOAD_BIT(1) | PADRE_TM_CTRL_INTEN_BIT(1) | PADRE_TM_CTRL_SEL_BIT(1);

static unsigned int op_counter;
static irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	op_counter ++;
#if 0
	if((op_counter % 1024) == 1023)
		printk(KERN_ERR "timer_interrupt: irq = %d\n", irq);
#endif
	oprofile_add_sample(regs, 0);
	return IRQ_HANDLED;
}

static int timer_start(void)
{
	int ret;
	
	ret = request_irq(PADRE_IRQ_TIMER0, timer_interrupt, SA_INTERRUPT, 
			  "PADRE PMU", (void *)dev_id);

	if (ret < 0) {
                printk(KERN_ERR "oprofile: unable to request IRQ%d for PADRE PMU\n",
		       PADRE_IRQ_TIMER0);
                return ret;
        }


	writel(PADRE_TIMER_COUNT, PADRE_TM_TIMER0);
	writel(tm_0 + tm_1, PADRE_TM_CTRL);

	return 0;
}


static void timer_stop(void)
{
	// tm_1 is the standard timer, restore to tm_1 only
	writel(tm_1, PADRE_TM_CTRL);

	free_irq(PADRE_IRQ_TIMER0, (void *)dev_id);
}


int __init op_timer_init(struct oprofile_operations * ops)
{
	ops->start = timer_start;
	ops->stop = timer_stop;
	ops->cpu_type = "timer";
	printk(KERN_INFO "oprofile: using timer interrupt.\n");
	return 0;
}
