/*  $Id: irq.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 *  arch/sparc/kernel/irq.c:  Interrupt request handling routines. On the
 *                            Sparc the IRQ's are basically 'cast in stone'
 *                            and you are supposed to probe the prom's device
 *                            node trees to find out who's got which IRQ.
 *
 *  Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *  Copyright (C) 1995 Miguel de Icaza (miguel@nuclecu.unam.mx)
 *  Copyright (C) 1995 Pete A. Zaitcev (zaitcev@yahoo.com)
 *  Copyright (C) 1996 Davi Redman (djhr@tadpole.co.uk)
 *  Copyright (C) 1998-2000 Anton Blanchard (anton@samba.org)
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/errno.h>
#include <linux/linkage.h>
#include <linux/kernel_stat.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/delay.h>
#include <linux/threads.h>
#include <linux/spinlock.h>
#include <linux/irq.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>


#include <asm/ptrace.h>
#include <asm/processor.h>
#include <asm/system.h>
#include <asm/psr.h>
#include <asm/smp.h>
#include <asm/traps.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
//#include <asm/hardirq.h>


#ifdef CONFIG_PADRE_CPU1
#define PADRE_INTC_MASK_H PADRE_INTC1_MASK_H
#define PADRE_INTC_MASK_L PADRE_INTC1_MASK_L
#define PADRE_INTC_ICLR_H PADRE_INTC1_ICLR_H
#define PADRE_INTC_ICLR_L PADRE_INTC1_ICLR_L
#define PADRE_INTC_ICAUSE_H PADRE_INTC1_ICAUSE_H
#define PADRE_INTC_ICAUSE_L PADRE_INTC1_ICAUSE_L
#else
#define PADRE_INTC_MASK_H PADRE_INTC0_MASK_H
#define PADRE_INTC_MASK_L PADRE_INTC0_MASK_L
#define PADRE_INTC_ICLR_H PADRE_INTC0_ICLR_H
#define PADRE_INTC_ICLR_L PADRE_INTC0_ICLR_L
#define PADRE_INTC_ICAUSE_H PADRE_INTC0_ICAUSE_H
#define PADRE_INTC_ICAUSE_L PADRE_INTC0_ICAUSE_L
#endif

unsigned long __ic3call __local_irq_save(void)
{
        unsigned long retval;
        unsigned long tmp;

        __asm__ __volatile__(
                "rd     %%psr, %0\n\t"
                "or     %0, %2, %1\n\t"
                "wr     %1, 0, %%psr\n\t"
                : "=&r" (retval), "=r" (tmp)
                : "i" (PSR_PIL)
                : "memory");

        return retval;
}

void __ic3call local_irq_enable(void)
{
        unsigned long tmp;

        __asm__ __volatile__(
                "rd     %%psr, %0\n\t"
                "andn   %0, %1, %0\n\t"
                "wr     %0, 0, %%psr\n\t"
                : "=&r" (tmp)
                : "i" (PSR_PIL)
                : "memory");
}
void __ic3call local_irq_restore(unsigned long old_psr)
{
        unsigned long tmp;

        __asm__ __volatile__(
                "rd     %%psr, %0\n\t"
                "and    %2, %1, %2\n\t"
                "andn   %0, %1, %0\n\t"
                "wr     %0, %2, %%psr\n\t"
                : "=&r" (tmp)
                : "i" (PSR_PIL), "r" (old_psr)
                : "memory");
}


EXPORT_SYMBOL(__local_irq_save);
EXPORT_SYMBOL(local_irq_enable);
EXPORT_SYMBOL(local_irq_restore);


/*
 * Controller mappings for all interrupt sources:
 */
extern struct hw_interrupt_type padre_irq_type;
irq_desc_t irq_desc[NR_IRQS] __cacheline_aligned = { 
	[0 ... NR_IRQS-1] = {
		.handler = &padre_irq_type,
		.lock = SPIN_LOCK_UNLOCKED,
	}
};

spinlock_t padre_lock = SPIN_LOCK_UNLOCKED;
unsigned int cached_irq_mask_l = 0x00000000;
unsigned int cached_irq_mask_h = 0x0000;

#ifndef CONFIG_PADRE_TRAP10_BUG
#define FIX_BUG_505 	writel(0, 0x66660000)
#else
#define FIX_BUG_505
#endif



static void  disable_padre_irq(unsigned int irq)
{
    unsigned int mask =  ~(1 << (irq & 31));
	unsigned long flags;
	
    spin_lock_irqsave(&padre_lock, flags);    	
	if( irq > 31 ) {
		cached_irq_mask_h &= mask;
		writel(cached_irq_mask_h,PADRE_INTC_MASK_H);
	}
	else {
		cached_irq_mask_l &= mask;
		writel(cached_irq_mask_l,PADRE_INTC_MASK_L);
	}
	// bug 505
	FIX_BUG_505;
	spin_unlock_irqrestore(&padre_lock, flags);     
		
		
}

static void enable_padre_irq(unsigned int irq)
{
    unsigned int mask =  1 << (irq & 31);
    unsigned long flags;
 
    spin_lock_irqsave(&padre_lock, flags);
    if( irq > 31 ) {
	    cached_irq_mask_h |= mask;
		writel(cached_irq_mask_h,PADRE_INTC_MASK_H);
	}
    else {
		cached_irq_mask_l |= mask;
		writel(cached_irq_mask_l,PADRE_INTC_MASK_L);
	}
	// bug 505
	FIX_BUG_505;
    spin_unlock_irqrestore(&padre_lock, flags);	
}

static unsigned int startup_padre_irq(unsigned int irq)
{
	enable_padre_irq(irq);
	return 0;
}

#if 0
static void ack_padre_irq(unsigned int irq)
{
}
#endif

static void end_padre_irq(unsigned int irq)
{
#if 1
    if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		enable_padre_irq(irq);	
#endif
}

static void end_padre_irq_lv(unsigned int irq)
{
    unsigned int mask =  1 << (irq & 31);
    unsigned long flags;
 
    spin_lock_irqsave(&padre_lock, flags);

    if( irq > 31 ) {
		writel(mask,PADRE_INTC_ICLR_H);
		if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))) {
	    	cached_irq_mask_h |= mask;
			writel(cached_irq_mask_h,PADRE_INTC_MASK_H);
			// bug 505
			FIX_BUG_505;
		}
	}
    else {
		writel(mask,PADRE_INTC_ICLR_L);
		if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))) {
			cached_irq_mask_l |= mask;
			writel(cached_irq_mask_l,PADRE_INTC_MASK_L);
			// bug 505
			FIX_BUG_505;
		}
	}
	
    spin_unlock_irqrestore(&padre_lock, flags);	
}

static void mask_padre_irq_lv(unsigned int irq)
{
    unsigned int mask =  ~(1 << (irq & 31));
	unsigned long flags;
	
    spin_lock_irqsave(&padre_lock, flags);    	
	if( irq > 31 ) {
		cached_irq_mask_h &= mask;
		writel(cached_irq_mask_h,PADRE_INTC_MASK_H);
	}
	else {
		cached_irq_mask_l &= mask;
		writel(cached_irq_mask_l,PADRE_INTC_MASK_L);
	}
	// bug 505
	FIX_BUG_505;
	spin_unlock_irqrestore(&padre_lock, flags);     
}

#define shutdown_padre_irq	disable_padre_irq

static struct hw_interrupt_type padre_irq_type = {
	"PADRE",
	startup_padre_irq,
	shutdown_padre_irq,
	enable_padre_irq,
	disable_padre_irq,
	//ack_padre_irq,
	mask_padre_irq_lv,
	end_padre_irq,
	NULL
};

static struct hw_interrupt_type padre_irq_lv_type = {
	"PADRE_LEVEL",
	startup_padre_irq,
	shutdown_padre_irq,
	enable_padre_irq,
	disable_padre_irq,
	mask_padre_irq_lv,
	end_padre_irq_lv,
	NULL
};
/*
 * Generic enable/disable code: this just calls
 * down into the PIC-specific version for the actual
 * hardware disable after having gotten the irq
 * controller lock.
 */
	 
/**
 * disable_irq_nosync - disable an irq without waiting
 * @irq: Interrupt to disable
 *
 * Disable the selected interrupt line. Disables of an interrupt
 * stack. Unlike disable_irq(), this function does not ensure existing
 * instances of the IRQ handler have completed before returning.
 * 
 * This function may be called from IRQ context.
 */

void inline disable_irq_nosync(unsigned int irq)
{
    irq_desc_t *desc = irq_desc + irq;
	unsigned long flags;
		 
	spin_lock_irqsave(&desc->lock, flags);
	if (!desc->depth++) {
		desc->status |= IRQ_DISABLED;
		desc->handler->disable(irq);
	}
	spin_unlock_irqrestore(&desc->lock, flags);
}

/**
 * disable_irq - disable an irq and wait for completion
 * @irq: Interrupt to disable
 *
 * Disable the selected interrupt line. Disables of an interrupt
 * stack. That is for two disables you need two enables. This
 * function waits for any pending IRQ handlers for this interrupt
 * to complete before returning. If you use this function while
 * holding a resource the IRQ handler may need you will deadlock.
 *
 * This function may be called - with care - from IRQ context.
 */
 
void disable_irq(unsigned int irq)
{
    irq_desc_t *desc = irq_desc + irq;
    disable_irq_nosync(irq);
    if (desc->action)
		synchronize_irq(irq);
	
}

/*
 * enable_irq - enable interrupt handling on an irq
 * @irq: Interrupt to enable
 *
 * Re-enables the processing of interrupts on this IRQ line
 * providing no disable_irq calls are now in effect.
 *
 * This function may be called from IRQ context.
 */
 
void enable_irq(unsigned int irq)
{
	irq_desc_t *desc = irq_desc + irq;
	unsigned long flags;
			 
	spin_lock_irqsave(&desc->lock, flags);
	switch (desc->depth) {
	case 1: {
		unsigned int status = desc->status & ~IRQ_DISABLED;
		desc->status = status;
		if ((status & (IRQ_PENDING | IRQ_REPLAY)) == IRQ_PENDING) {
			desc->status = status | IRQ_REPLAY;
            //hw_resend_irq(desc->handler,irq);
        }
        desc->handler->enable(irq);
		/* fall-through */
	}
	default:
	    desc->depth--;
		break;
	case 0:
		printk("enable_irq(%u) unbalanced from %p\n", irq,
				 __builtin_return_address(0));
    }
	spin_unlock_irqrestore(&desc->lock, flags);
}

static void register_irq_proc (unsigned int irq);

/* this was setup_x86_irq but it seems pretty generic */
int setup_irq(unsigned int irq, struct irqaction * new)
{
	int shared = 0;
	unsigned long flags;
	struct irqaction *old, **p;

	irq_desc_t *desc = irq_desc + irq;
					 
	/*
	 * Some drivers like serial.c use request_irq() heavily,
	* so we have to be careful not to interfere with a
	* running system.
	*/
	if (new->flags & SA_SAMPLE_RANDOM) {
		/*
		 * This function might sleep, we want to call it first,
	 	* outside of the atomic block.
	 	* Yes, this might clear the entropy pool if the wrong
	 	* driver is attempted to be loaded, without actually
	 	* installing a new handler, but is this really a problem,
	 	* only the sysadmin is able to do this.
	 	*/
		rand_initialize_irq(irq);
	}
	
	/*
	 * The following block of code has to be executed atomically
	 */
	spin_lock_irqsave(&desc->lock,flags);

	p = &desc->action;
	if ((old = *p) != NULL) {
		/* Can't share interrupts unless both agree to */
		if (!(old->flags & new->flags & SA_SHIRQ)) {
			spin_unlock_irqrestore(&desc->lock,flags);
			return -EBUSY;
		}
						 
		/* add new interrupt at end of irq queue */
		do {
			p = &old->next;
			old = *p;
		} while (old);
		shared = 1;
	}
			 
	*p = new;
				 
	if (!shared) {
		desc->depth = 0;
		desc->status &= ~(IRQ_DISABLED | IRQ_AUTODETECT | IRQ_WAITING | IRQ_INPROGRESS);
		desc->handler->startup(irq);
	}
	spin_unlock_irqrestore(&desc->lock,flags);
						 
	register_irq_proc(irq);
	return 0;
}

/**
 * request_irq - allocate an interrupt line
 * @irq: Interrupt line to allocate
 * @handler: Function to be called when the IRQ occurs
 * @irqflags: Interrupt type flags
 * @devname: An ascii name for the claiming device
 * @dev_id: A cookie passed back to the handler function
 *
 * This call allocates interrupt resources and enables the
 * interrupt line and IRQ handling. From the point this
 * call is made your handler function may be invoked. Since
 * your handler function must clear any interrupt the board
 * raises, you must take care both to initialise your hardware
 * and to set up the interrupt handler in the right order.
 *  
 * Dev_id must be globally unique. Normally the address of the
 * device data structure is used as the cookie. Since the handler
 * receives this value it makes sense to use it.
 *           
 * If your interrupt is shared you must pass a non NULL dev_id
 * as this is required when freeing the interrupt.
 *              
 * Flags:
 *
 * SA_SHIRQ        Interrupt is shared
 *
 * SA_INTERRUPT        Disable local interrupts while processing
 * 
 * SA_SAMPLE_RANDOM    The interrupt can be used for entropy
 * 
 */

int request_irq(unsigned int irq,
		irqreturn_t (*handler)(int, void *, struct pt_regs *),
		unsigned long irqflags,
		const char * devname,
		void *dev_id)
{
	    int retval;
		struct irqaction * action;
			 
	    /*
	     * Sanity-check: shared interrupts should REALLY pass in
	     * a real dev-ID, otherwise we'll have trouble later trying
	     * to figure out which interrupt is which (messes up the
	     * interrupt freeing logic etc).
	     */
	    if (irqflags & SA_SHIRQ) {
		    if (!dev_id)
				printk("Bad boy: %s (at 0x%x) called us without a dev_id!\n", devname, (&irq)[-1]);
		}
	   
		if (irq >= NR_IRQS)
			return -EINVAL;
	    if (!handler)
		    return -EINVAL;
						 
	    action = (struct irqaction *)
	            kmalloc(sizeof(struct irqaction), GFP_KERNEL);
	    if (!action)
		    return -ENOMEM;
								
		action->handler = handler;
	    action->flags = irqflags;
		action->mask = CPU_MASK_NONE;
		action->name = devname;
		action->next = NULL;
		action->dev_id = dev_id;
						 
		retval = setup_irq(irq, action);
		if (retval)
			kfree(action);
	   	return retval;
}

EXPORT_SYMBOL(request_irq);

/**
 *  free_irq - free an interrupt
 *  @irq: Interrupt line to free
 *  @dev_id: Device identity to free
 *
 *  Remove an interrupt handler. The handler is removed and if the
 *  interrupt line is no longer in use by any driver it is disabled.
 *  On a shared IRQ the caller must ensure the interrupt is disabled
 *  on the card it drives before calling this function. The function
 *  does not return until any executing interrupts for this IRQ
 *  have completed.
 *
 *  This function may be called from interrupt context.
 *
 *  Bugs: Attempting to free an irq in a handler for the same irq hangs
 *        the machine.
 */

void free_irq(unsigned int irq, void *dev_id)
{
	irq_desc_t *desc;
	struct irqaction **p;
	unsigned long flags;

	if (irq >= NR_IRQS)
		return;

	desc = irq_desc + irq;
	spin_lock_irqsave(&desc->lock,flags);
	p = &desc->action;
	for (;;) {
		struct irqaction * action = *p;
		if (action) {
			struct irqaction **pp = p; 
			p = &action->next;
			if (action->dev_id != dev_id)
				continue;

			/* Found it - now remove it from the list of entries */
			*pp = action->next;
			if (!desc->action) {
				desc->status |= IRQ_DISABLED;
				desc->handler->shutdown(irq);
			}
			spin_unlock_irqrestore(&desc->lock,flags);

			kfree(action);
			return;
		}
		printk("Trying to free free IRQ%d\n",irq);
		spin_unlock_irqrestore(&desc->lock,flags);
		return;
	}
}

EXPORT_SYMBOL(free_irq);

int show_interrupts(struct seq_file *p, void *v)
{
    int i = *(loff_t *) v, j;
    struct irqaction * action;
    unsigned long flags;

    if (i == 0) {
        seq_printf(p, "           ");
        for (j=0; j<NR_CPUS; j++)
            if (cpu_online(j))
                seq_printf(p, "CPU%d       ",j);
	
		seq_putc(p, '\n');
    }

    if (i < NR_IRQS) {
  		spin_lock_irqsave(&irq_desc[i].lock, flags);
		action = irq_desc[i].action;
		if (!action)
			goto skip;
		seq_printf(p, "%3d: ",i);
#ifndef CONFIG_SMP
        seq_printf(p, "%10u ", kstat_irqs(i));
#else
        for (j = 0; j < NR_CPUS; j++)
	        if (cpu_online(j))
		        seq_printf(p, "%10u ", kstat_cpu(j).irqs[i]);
#endif
		seq_printf(p, " %14s", irq_desc[i].handler->typename);
		seq_printf(p, "  %s", action->name);
        for (action=action->next; action; action = action->next)
	        seq_printf(p, ", %s", action->name);
        seq_putc(p, '\n');
skip:
        spin_unlock_irqrestore(&irq_desc[i].lock, flags);
	} else if (i == NR_IRQS) {
#if 0
	    seq_printf(p, "NMI: ");
	    for (j = 0; j < NR_CPUS; j++)
		    if (cpu_online(j))
			    seq_printf(p, "%10u ", nmi_count(j));
		seq_putc(p, '\n');
        seq_printf(p, "ERR: %10u\n", atomic_read(&irq_err_count));
#endif
    }
	return 0;
}



/*
 * This should really return information about whether
 * we should do bottom half handling etc. Right now we
 * end up _always_ checking the bottom half, which is a
 * waste of time and is not what some drivers would
 * prefer.
 */
asmlinkage int __ic3call handle_IRQ_event(unsigned int irq, 
		struct pt_regs * regs, struct irqaction *action)
{
    int status = 1;
	int retval = 0;
	 
					 
    if (!(action->flags & SA_INTERRUPT)) 
		local_irq_enable();
						 
	do {
		status |= action->flags;
		retval |= action->handler(irq, action->dev_id, regs);
		action = action->next;
	} while (action);
	if (status & SA_SAMPLE_RANDOM)
		add_interrupt_randomness(irq);
	local_irq_disable();
								 
	return retval;
}

static void __report_bad_irq(int irq, irq_desc_t *desc, irqreturn_t action_ret)
{
	    struct irqaction *action;

		if (action_ret != IRQ_HANDLED && action_ret != IRQ_NONE) {
	        printk(KERN_ERR "irq event %d: bogus return value %x\n",
	                irq, action_ret);
	    } else {
	        printk(KERN_ERR "irq %d: nobody cared!\n", irq);
	    }
	    dump_stack();
	    printk(KERN_ERR "handlers:\n");
	    action = desc->action;
	    do {
	        printk(KERN_ERR "[<%p>]", action->handler);
	        print_symbol(" (%s)",
		    (unsigned long)action->handler);
		    printk("\n");
		    action = action->next;
		} while (action);
}

static void report_bad_irq(int irq, irq_desc_t *desc, irqreturn_t action_ret)
{
    static int count = 100;

    if (count) {
        count--;
        __report_bad_irq(irq, desc, action_ret);
    }
}

static int noirqdebug;

static int __init noirqdebug_setup(char *str)
{
    noirqdebug = 1;
    printk("IRQ lockup detection disabled\n");
    return 1;
}

__setup("noirqdebug", noirqdebug_setup);

static void note_interrupt(int irq, irq_desc_t *desc, irqreturn_t action_ret)
{
    if (action_ret != IRQ_HANDLED) {
        desc->irqs_unhandled++;
    	if (action_ret != IRQ_NONE)
           report_bad_irq(irq, desc, action_ret);
    }
    desc->irq_count++;
    if (desc->irq_count < 100000)
	    return;
    desc->irq_count = 0;
    if (desc->irqs_unhandled > 99900) {
	    /*
	     * The interrupt is stuck
	     */
		__report_bad_irq(irq, desc, action_ret);
		/*
		 * Now kill the IRQ
		 */
		printk(KERN_EMERG "Disabling IRQ #%d\n", irq);
		desc->status |= IRQ_DISABLED;
		desc->handler->disable(irq);
	}
    desc->irqs_unhandled = 0;
}
				

asmlinkage void do_IRQ(int level, struct pt_regs *regs)
{
	unsigned long i_cause, count;
	int irq,i;
    irq_desc_t *desc;
	struct irqaction * action;
	unsigned int status;

	irq_enter();
	  
	if( level > 4 ) {
		i_cause = readl(PADRE_INTC_ICAUSE_H);
		count = 16;
		irq = 32;
	}
	else {
		i_cause = readl(PADRE_INTC_ICAUSE_L);
		count = 32;
		irq = 0;
	}

	for( i = 0 ; i < count ; i++, irq++) {
		if( i_cause & (0x1 << i) ) {
			desc = irq_desc + irq ;
   			kstat_this_cpu.irqs[irq]++;
    		spin_lock(&desc->lock);
     		desc->handler->ack(irq);
 	 
			status = desc->status;
    		//status = desc->status & ~(IRQ_REPLAY | IRQ_WAITING);
    		status |= IRQ_PENDING; /* we _want_ to handle it */	
    		/*
			 * If the IRQ is disabled for whatever reason, we cannot
	     	 * use the action we have.
	         */
    		action = NULL;
    		if (!(status & (IRQ_DISABLED | IRQ_INPROGRESS))) {
	        	action = desc->action;
		        status &= ~IRQ_PENDING; /* we commit to handling */
			    status |= IRQ_INPROGRESS; /* we are handling it */
			}
    		desc->status = status;	
			
        	if (!action ) 
				goto out;
			
			for(;;) {
	            irqreturn_t action_ret;
					   
				spin_unlock(&desc->lock);
		    	action_ret = handle_IRQ_event(irq, regs, action);
				spin_lock(&desc->lock);
        		if (!noirqdebug)
            		note_interrupt(irq, desc, action_ret);
		
				if (!(desc->status & IRQ_PENDING))
					break;
				desc->status &= ~IRQ_PENDING;
			}
			desc->status &= ~(IRQ_INPROGRESS);
out:
			desc->handler->end(irq);
			spin_unlock(&desc->lock);
		}
	}
	
   	irq_exit();
}

/* We really don't need these at all on the Sparc.  We only have
 * stubs here because they are exported to modules.
 */
unsigned long probe_irq_on(void)
{
	return 0;
}

EXPORT_SYMBOL(probe_irq_on);

int probe_irq_off(unsigned long mask)
{
	return 0;
}

EXPORT_SYMBOL(probe_irq_off);

#define GPIO_INTERRUPT_0		16
#define PCI_INTERRUPT_PINTA     27
#define PCI_INTERRUPT_PINTB     26
#define GMAC0_M4_IRQ			41
#define GMAC1_M4_IRQ			40
#define LP0_IRQ					44
#define LP1_IRQ					43

extern unsigned short fpga_date,fpga_month,fpga_year,fpga_build,fpga_config;

void __init init_IRQ(void)
{
	int i;

	for( i = 0 ; i < NR_IRQS ; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
		if( (i >= GPIO_INTERRUPT_0) && (i <= PCI_INTERRUPT_PINTA))
			irq_desc[i].handler = &padre_irq_lv_type;
	}
        irq_desc[10].handler = &padre_irq_lv_type;
        irq_desc[11].handler = &padre_irq_lv_type;
	// M4 always level trig now
	irq_desc[GMAC0_M4_IRQ].handler = &padre_irq_lv_type;
	irq_desc[GMAC1_M4_IRQ].handler = &padre_irq_lv_type;

	irq_desc[LP0_IRQ].handler = &padre_irq_lv_type;
	irq_desc[LP1_IRQ].handler = &padre_irq_lv_type;


	writel(cached_irq_mask_l,PADRE_INTC_MASK_L);
	writel(cached_irq_mask_h,PADRE_INTC_MASK_H);
}


static struct proc_dir_entry * root_irq_dir;
static struct proc_dir_entry * irq_dir [NR_IRQS];


static int prof_cpu_mask_read_proc (char *page, char **start, off_t off,
		            int count, int *eof, void *data)
{
    int len = cpumask_scnprintf(page, count, *(cpumask_t *)data);
    if (count - len < 2)
        return -EINVAL;
    len += sprintf(page + len, "\n");
	    return len;
}


static int prof_cpu_mask_write_proc (struct file *file, const char __user *buffer,
		                    unsigned long count, void *data)
{
    cpumask_t *mask = (cpumask_t *)data;
    unsigned long full_count = count, err;
    cpumask_t new_value;

    err = cpumask_parse(buffer, count, new_value);
    if (err)
        return err;

    *mask = new_value;
    return full_count;
}



#define MAX_NAMELEN 10

static void register_irq_proc (unsigned int irq)
{
    char name [MAX_NAMELEN];

    if (!root_irq_dir || irq_dir[irq])
        return;

    memset(name, 0, MAX_NAMELEN);
    sprintf(name, "%d", irq);

    /* create /proc/irq/1234 */
    irq_dir[irq] = proc_mkdir(name, root_irq_dir);
}


unsigned long prof_cpu_mask = -1;

void init_irq_proc(void)
{
    struct proc_dir_entry *entry;
    int i;

    /* create /proc/irq */
    root_irq_dir = proc_mkdir("irq", 0);

    /* create /proc/irq/prof_cpu_mask */
    entry = create_proc_entry("prof_cpu_mask", 0600, root_irq_dir);

    if (!entry)
	        return;

    entry->nlink = 1;
    entry->data = (void *)&prof_cpu_mask;
    entry->read_proc = prof_cpu_mask_read_proc;
    entry->write_proc = prof_cpu_mask_write_proc;

    /*
	 * Create entries for all existing IRQs.
	 */
    for (i = 0; i < NR_IRQS; i++)
	        register_irq_proc(i);
}

