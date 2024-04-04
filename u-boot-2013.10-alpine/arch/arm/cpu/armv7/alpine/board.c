/*
 * Copyright (C) 2013 Annapurna Labs Ltd.
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
#include <asm/arch/iocc.h>
#include <al_init_sys_fabric.h>
#include <al_hal_iomap.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_ARCH_CPU_INIT
/**
 * Architecture specific CPU related initialization
 *
 * Please notice that at this stage the C environment is limited:
 * - Global data can't be written
 * - BSS yet not cleared
 * - Limited stack size
 */
int arch_cpu_init(void)
{
#ifdef CONFIG_SYS_HW_CACHE_COHERENCY
	struct al_sys_fabric_params params = { .iocc = 1, .dev_ord_relax = 0 };
#endif

#ifdef CONFIG_SYS_HW_CACHE_COHERENCY
	cp15_turn_on_smp_bit();
	al_sys_fabric_init((void __iomem *)AL_NB_BASE, &params);
#endif

	return 0;
}
#endif

void enable_caches(void)
{
#ifndef CONFIG_SYS_DCACHE_OFF
	/* Enable D-cache. I-cache is already enabled in start.S */
	//dcache_enable();
#endif

#ifndef CONFIG_SYS_ICACHE_OFF
	/* Enable D-cache. I-cache is already enabled in start.S */
	icache_enable();
#endif

}

#ifdef CONFIG_SYS_HW_CACHE_COHERENCY
void cpu_cache_initialization(void)
{
	al_sys_fabric_clear_settings((void __iomem *)AL_NB_BASE);
	cp15_turn_off_smp_bit();
}
#endif

