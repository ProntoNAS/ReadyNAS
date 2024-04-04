/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 or V3 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/**
 * @defgroup group_addr_map Address Map
 * @ingroup group_pbs
 *  The Address Map HAL can be used to adjust the Physical Address Slave
 *  Window (PASW) configuration.
 *
 *  @{
 * @file   al_hal_addr_map.h
 *
 * @brief  includes Address Map API
 *
 */

#ifndef __AL_HAL_ADDR_MAP_H__
#define __AL_HAL_ADDR_MAP_H__

#include "al_hal_common.h"

/* enum definitions */

/* Address map physical address slave windows */
enum al_addr_map_pasw {
	AL_ADDR_MAP_PASW_NB_DRAM0, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM1, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM2, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_DRAM3, /**< DRAM (system fabric) */
	AL_ADDR_MAP_PASW_NB_MSIX, /**< MSI-X Target Address region, to which
					all the MSI-X messages are forwarded.
					(system fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM0, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM1, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM2, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_DRAM3, /**< DRAM (io fabric) */
	AL_ADDR_MAP_PASW_SB_MSIX, /**< MSI-X Target Address region, to which
					all the MSI-X messages are forwarded.
					(io fabric)*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM0, /**< PCI Express interface 1 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM1,	/**< PCI Express interface 2 memory
						mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_MEM2, /**< PCI Express interface 3 memory
					mapped space*/
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM0, /**< PCI Express interface 1 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM1, /**< PCI Express interface 2 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_EXT_ECAM2, /**< PCI Express interface 3 – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PBS_SPI, /**< PBS SPI */
	AL_ADDR_MAP_PASW_PBS_NAND, /**< PBS NAND */
	AL_ADDR_MAP_PASW_PBS_INT_MEM, /**< PBS Boot ROM and SRAM */
	AL_ADDR_MAP_PASW_PBS_BOOT, /**< Boot address as specified by ARM
				architecture*/
	AL_ADDR_MAP_PASW_NB_INT, /**< System Fabric Internal registers */
	AL_ADDR_MAP_PASW_NB_STM, /**< System Fabric System Trace Module (STM) */
	AL_ADDR_MAP_PASW_PCIE_INT_ECAM, /**< Internal virtual PCIe bus – ECAM,
				i.e. memory-mapped configuration space */
	AL_ADDR_MAP_PASW_PCIE_INT_MEM, /**< Internal virtual PCIe bus
				memory-mapped address space */
	AL_ADDR_MAP_PASW_SB_INT, /**< IO Fabric Internal registers */
};

/**
 * Update Physical Slave Address Window (PASW) in system address map
 *
 * This function is used to update a certain PASW in the system address map
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	pasw
 *		The PASW to update
 * @param	base
 *		Base address for the PASW
 * @param	log2size
 *		Log2 of the PASW size, has to be >=15 and <=40.
 *		If log2size==15, the PASW is disabled.
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_pasw_set(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t base,
		uint32_t log2size);

/**
 * Get Physical Slave Address Window (PASW) on the system address map
 *
 * This function is used to get a certain PASW on the system address map
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	pasw
 *		Required PASW
 * @param	*base
 *		returned base address
 * @param	*log2size
 *		returned Log2 of the PASW size
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_pasw_get(
		void __iomem *pbs_regs_base,
		enum al_addr_map_pasw pasw,
		al_phys_addr_t *base,
		uint32_t *log2size);

/**
 * Set DRAM remapping
 *
 * This function is used for remapping the DRAM.
 * By default the 4th DRAM GB is accessed through
 * system addresses 2_4000_0000 - 2_8000_0000.
 * This function can be used for changing this default behavior.
 *
 * @param	pbs_regs_base
 *		The address of the PBS Regfile
 * @param	dram_remap_base
 *		Remaping source DRAM area base address.
 *		Source DRAM area should be any area beginning from 0x20000000
 *		and up 40Bit addressing
 *		Any area below this address will be undestood by system
 *		as area that begins from address 0, that defined as system area
 * @param	dram_remap_transl_base
 *		Remaping destination DRAM translated area base address.
 *		Destination DRAM area should be any area beginning from 0x20000000
 *		and up 40Bit addressing
 *		Any area below this address will be undestood by system
 *		as area that begins from address 0, that defined as system area
 *		Destination area depends from system installed memory (DRAM)
 * @param	window_size
 * 		This field determines the remaping window size.
 * 		Maximum allowed to remaping window size is of 2GByte.
 * 		Following values should be used to define window size:
 * 			0 - window is disabled.
 * 			1 - Window size is 512MByte.
 * 			2 - Window size is 1GByte.
 * 			3 - Window size is 2GByte.
 *		Any other values are prohibited
 *
 * @return	0 if finished successfully
 *		<0 if an error occurred
 */
int al_addr_map_dram_remap_set(
		void __iomem *pbs_regs_base,
		al_phys_addr_t dram_remap_base,
		al_phys_addr_t dram_remap_transl_base,
		uint8_t window_size);

#endif /*__AL_HAL_ADDR_MAP_H__ */

/** @} end of Address Map group */
