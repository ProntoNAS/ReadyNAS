/*
 * include/asm-arm/arch-dove/io.h
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __ASM_ARCH_IO_H
#define __ASM_ARCH_IO_H

#include "armadaxp.h"

#define IO_SPACE_LIMIT		0xffffffff
#define IO_SPACE_REMAP 		PEX0_IO_PHYS_BASE

#define __io(a)			((a) + PEX0_IO_VIRT_BASE)
#define __mem_pci(a)		((unsigned long)(a))
#define __mem_isa(a)		(a)

/*#define aurora_setbits(r, mask)	writel(readl(r) | (mask), (r))
#define aurora_clrbits(r, mask)	writel(readl(r) & ~(mask), (r))*/
#endif
