/* $Id: time.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 * linux/arch/sparc/kernel/time.c
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1996 Thomas K. Dyas (tdyas@eden.rutgers.edu)
 *
 * Chris Davis (cdavis@cois.on.ca) 03/27/1998
 * Added support for the intersil on the sun4/4200
 *
 * Gleb Raiko (rajko@mech.math.msu.su) 08/18/1998
 * Support for MicroSPARC-IIep, PCI CPU.
 *
 * This file handles the Sparc specific time handling details.
 *
 * 1997-09-10	Updated NTP code according to technical memorandum Jan '96
 *		"A Kernel Model for Precision Timekeeping" by Dave Mills
 */
#include <linux/config.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/profile.h>

#include <asm/segment.h>
#include <asm/timer.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/padre0.h>
#include <asm/ds1337.h>

extern unsigned long wall_jiffies;

DEFINE_SPINLOCK(rtc_lock);
static int set_rtc_mmss(unsigned long);

DEFINE_SPINLOCK(ticker_lock);

unsigned long profile_pc(struct pt_regs *regs)
{
        extern char __copy_user_begin[], __copy_user_end[];
        extern char __atomic_begin[], __atomic_end[];
        extern char __bzero_begin[], __bzero_end[];
        extern char __bitops_begin[], __bitops_end[];

        unsigned long pc = regs->pc;

        if (in_lock_functions(pc) ||
            (pc >= (unsigned long) __copy_user_begin &&
             pc < (unsigned long) __copy_user_end) ||
            (pc >= (unsigned long) __atomic_begin &&
             pc < (unsigned long) __atomic_end) ||
            (pc >= (unsigned long) __bzero_begin &&
             pc < (unsigned long) __bzero_end) ||
            (pc >= (unsigned long) __bitops_begin &&
             pc < (unsigned long) __bitops_end))
                pc = regs->u_regs[UREG_RETPC];
        return pc;
}

/*
 * timer_interrupt() needs to keep up the real-time clock,
 * as well as call the "do_timer()" routine every clocktick
 */

#define TICK_SIZE (tick_nsec / 1000)
extern void padre_profile_tick(unsigned long pc);

irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	/* last time the cmos clock got updated */
	static long last_rtc_update;

	profile_tick(CPU_PROFILING, regs);

	/* Protect counter clear so that do_gettimeoffset works */
#if 0   /* BINZHANG SPEEDUP */
	write_seqlock(&xtime_lock);
#endif
	
	do_timer(regs);
#ifndef CONFIG_SMP
        update_process_times(user_mode(regs));
#endif

	/* Determine when to update the Mostek clock. */
	if ((time_status & STA_UNSYNC) == 0 &&
	    xtime.tv_sec > last_rtc_update + 660 &&
	    (xtime.tv_nsec / 1000) >= 500000 - ((unsigned) TICK_SIZE) / 2 &&
	    (xtime.tv_nsec / 1000) <= 500000 + ((unsigned) TICK_SIZE) / 2) {
	  if (set_rtc_mmss(xtime.tv_sec) == 0)
	    last_rtc_update = xtime.tv_sec;
	  else
	    last_rtc_update = xtime.tv_sec - 600; /* do it again in 60 s */
	}

#if 0   /* BINZHANG SPEEDUP */
	write_sequnlock(&xtime_lock);
#endif

	return IRQ_HANDLED;
}

static struct irqaction irq_timer = {
	.handler =      timer_interrupt,
	.flags =        SA_INTERRUPT,
	.name =         "timer",
};

static unsigned long get_rts_time(void)
{
	return mktime(2003, 12, 1, 12, 0, 0);
}

void __init time_init(void)
{
	unsigned long timer = 1;

    xtime.tv_sec = get_rts_time();
	xtime.tv_nsec = (INITIAL_JIFFIES % HZ) * (NSEC_PER_SEC / HZ);
    set_normalized_timespec(&wall_to_monotonic,	-xtime.tv_sec, -xtime.tv_nsec);
			
	setup_irq(PADRE_IRQ_TIMER1, &irq_timer);
    writel(PADRE_TM_CTRL_STP,PADRE_TM_CTRL);
    writel(PADRE_PRESCALE_VALUE, PADRE_TM_PS);
    writel(PADRE_TIMER_COUNT,PADRE_TM_TIMER);
    writel(PADRE_TM_CTRL_AUTO_BIT(timer) | PADRE_TM_CTRL_LOAD_BIT(timer) |
           PADRE_TM_CTRL_INTEN_BIT(timer) | PADRE_TM_CTRL_SEL_BIT(timer),PADRE_TM_CTRL);
	
}

extern __inline__ unsigned long do_gettimeoffset(void)
{
	unsigned long old_time, current_time = 0;

	current_time = readl(PADRE_TM_CNTDIAG);
	do {
		old_time = current_time;
		current_time = readl(PADRE_TM_CNTDIAG);
	} while(current_time != old_time);
	
	return ((PADRE_TIMER_COUNT-current_time)*1000000/HZ) / PADRE_TIMER_COUNT;
}

/*
 * Returns nanoseconds
 * XXX This is a suboptimal implementation.
 */
unsigned long long __ic3call sched_clock(void)
{
	return (unsigned long long)jiffies * (1000000000 / HZ);
}

/* Ok, my cute asm atomicity trick doesn't work anymore.
 * There are just too many variables that need to be protected
 * now (both members of xtime, wall_jiffies, et al.)
 */
void do_gettimeofday(struct timeval *tv)
{
	unsigned long flags;
	unsigned long seq;
	unsigned long usec, sec;
	unsigned long max_ntp_tick = tick_usec - tickadj;

	do {
		unsigned long lost;

		seq = read_seqbegin_irqsave(&xtime_lock, flags);
		usec = do_gettimeoffset();
		lost = jiffies - wall_jiffies;

		/*
		 * If time_adjust is negative then NTP is slowing the clock
		 * so make sure not to go into next possible interval.
		 * Better to lose some accuracy than have time go backwards..
		 */
		if (unlikely(time_adjust < 0)) {
			usec = min(usec, max_ntp_tick);

			if (lost)
				usec += lost * max_ntp_tick;
		}
		else if (unlikely(lost))
			usec += lost * tick_usec;

		sec = xtime.tv_sec;
		usec += (xtime.tv_nsec / 1000);
	} while (read_seqretry_irqrestore(&xtime_lock, seq, flags));

	while (usec >= 1000000) {
		usec -= 1000000;
		sec++;
	}

	tv->tv_sec = sec;
	tv->tv_usec = usec;
}

EXPORT_SYMBOL(do_gettimeofday);

int do_settimeofday(struct timespec *tv)
{
	time_t wtm_sec, sec = tv->tv_sec;
	long wtm_nsec, nsec = tv->tv_nsec;
		  

	if ((unsigned long)tv->tv_nsec >= NSEC_PER_SEC)
		return -EINVAL;
	
	write_seqlock_irq(&xtime_lock);

   	nsec -= do_gettimeoffset() * NSEC_PER_USEC;
    nsec -= (jiffies - wall_jiffies) * TICK_NSEC;
 	
   	wtm_sec  = wall_to_monotonic.tv_sec + (xtime.tv_sec - sec);
    wtm_nsec = wall_to_monotonic.tv_nsec + (xtime.tv_nsec - nsec);

    set_normalized_timespec(&xtime, sec, nsec);
    set_normalized_timespec(&wall_to_monotonic, wtm_sec, wtm_nsec);

    time_adjust = 0;        /* stop active adjtime() */
    time_status |= STA_UNSYNC;
    time_maxerror = NTP_PHASE_LIMIT;
    time_esterror = NTP_PHASE_LIMIT;
    write_sequnlock_irq(&xtime_lock);
    clock_was_set();
    return 0;
}

EXPORT_SYMBOL(do_settimeofday);

/*
 * BUG: This routine does not handle hour overflow properly; it just
 *      sets the minutes. Usually you won't notice until after reboot!
 */
static int set_rtc_mmss(unsigned long nowtime)
{
	return 0;
}
