/*
 * include/asm-arm/arch-dove/hardware.h
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include "armada370.h"

#define pcibios_assign_all_busses()	1

#define PCIBIOS_MIN_IO			0x1000
#define PCIBIOS_MIN_MEM			0x01000000
#define PCIMEM_BASE			PEX0_MEM_PHYS_BASE /* mem base for VGA */


#endif
