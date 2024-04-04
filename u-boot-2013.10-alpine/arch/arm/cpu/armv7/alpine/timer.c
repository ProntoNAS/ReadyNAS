/*
 * arch/arm/cpu/armv7/al/timer.c
 *
 * timer driver for the Annapurna Labs Soc Family using the PBS timer.
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <asm/io.h>

#include "al_hal_timer.h"
#include "al_hal_nb_regs.h"

DECLARE_GLOBAL_DATA_PTR;

#define timer_rate_hz	gd->arch.timer_rate_hz
#define timestamp	gd->arch.tbl
#define lastdec		gd->arch.lastinc

static struct al_timer_subtimer sub_timer;

#define TIMER_LOAD_VAL	0xffffffff

extern unsigned int al_bootstrap_sb_clk_get(void);

/* timer without interrupts */
ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	long tmo = usec * (timer_rate_hz / 1000) / 1000;
	unsigned long now, last = al_timer_value_get(&sub_timer);

	while (tmo > 0) {
		now = al_timer_value_get(&sub_timer);
		if (now > last) /* count up timer overflow */
			tmo -= (TIMER_LOAD_VAL - now) + last;
		else
			tmo -= last - now;
		last = now;
	}
}

ulong get_timer_masked(void)
{
	ulong now;

	/* current tick value */
	now = al_timer_value_get(&sub_timer) / (timer_rate_hz / CONFIG_SYS_HZ);

	if (now > lastdec)	/* normal mode (non roll) */
		/* we have rollover of incrementer */
		timestamp += ((TIMER_LOAD_VAL / (timer_rate_hz / CONFIG_SYS_HZ))
				- now) + lastdec;
	else
		/* move stamp forward with absolute diff ticks */
		timestamp += (lastdec - now);

	debug("get_timer_masked: now %lu last %lu timestamp %lu\n",
		now, lastdec, timestamp);

	lastdec = now;
	return timestamp;
}

unsigned long long get_ticks(void)
{
	return get_timer(0);
}

ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}

int timer_init_r(void)
{
	int ret;

	timer_rate_hz = al_bootstrap_sb_clk_get();

	ret = al_timer_init(&sub_timer, (void *)AL_TIMER_BASE(0, 0), 0, 0);
	if (ret != 0) {
		printf("al_timer_init failed\n");
		return ret;
	}

	al_timer_enable(&sub_timer, AL_FALSE);

	/*
	 * Timer Mode : Free Running
	 * Interrupt : Disabled
	 * Prescale : 8 Stage, Clk/1
	 * Tmr Siz : 32 Bit Counter
	 * Tmr in Wrapping Mode
	 */
	al_timer_config_set(&sub_timer,
				AL_TIMER_SIZE_32,
				AL_TIMER_MODE_FREE_RUN,
				AL_TIMER_SCALE_1);

	al_timer_load_set(&sub_timer, TIMER_LOAD_VAL);
	al_timer_int_enable(&sub_timer, AL_FALSE);
	al_timer_enable(&sub_timer, AL_TRUE);

	/* Reset time */
	lastdec = al_timer_value_get(&sub_timer) /
			(timer_rate_hz / CONFIG_SYS_HZ);

	timestamp = 0;

	debug("timer_rate_hz = %lu\n", timer_rate_hz);

	return 0;
}

uint32_t sys_counter_cnt_get_low(void)
{
	struct al_nb_regs __iomem *nb_regs = (struct al_nb_regs __iomem *)AL_NB_SERVICE_BASE;

	return readl(&nb_regs->system_counter.cnt_low);
}

