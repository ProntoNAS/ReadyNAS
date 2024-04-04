/*
 *  arch/arm/include/asm/mach/marvell.h
 *
 *  Copyright (C) 2013 NETGEAR, Inc.
 *  Copyright (C) 2013 Hiro Sugawara
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This header file describes common definitions for Marvell SoC plarforms.
 */
#ifdef CONFIG_CRYPTO_DEV_MV_CESA
extern void __init mv_crypto_res_init(
		unsigned long reg_base, unsigned reg_window_size,
                unsigned long sram_base, unsigned sram_size, unsigned irq);
#else
static inline void __init mv_crypto_res_init(
		unsigned long reg_base, unsigned reg_window_size,
                unsigned long sram_base, unsigned sram_size, unsigned irq)
{
}
#endif
