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
 * @defgroup group_bootstrap Bootstrap
 * @ingroup group_pbs
 *  Bootstrap
 *  @{
 * @file   al_hal_bootstrap.h
 *
 * @brief  includes bootstrap API
 *
 */

#ifndef __AL_HAL_BOOTSTRAP_H__
#define __AL_HAL_BOOTSTRAP_H__

#include "al_hal_common.h"
#include "al_hal_pbs_regs.h"

/* enum definitions */

/* Reference clock type */
enum pll_ref_clk {
	PLL_REF_CLK_HSTL,
	PLL_REF_CLK_CMOS
};

/* I2C preload enabled/disabled */
enum i2c_preload {
	I2C_PRELOAD_ENABLED,
	I2C_PRELOAD_DISABLED
};

/* SPI preload enabled/disabled - Not supported */
enum spis_preload {
	SPIS_PRELOAD_ENABLED,
	SPIS_PRELOAD_DISABLED
};

/* Boot through boot ROM */
enum boot_rom {
	BOOT_ROM_DISABLED,
	BOOT_ROM_ENABLED
};

/* Boot device */
enum boot_device {
	BOOT_DEVICE_NOR_8BIT,		/* Not supported */
	BOOT_DEVICE_NOR_16BIT,		/* Not supported */
	BOOT_DEVICE_NAND_8BIT,
	BOOT_DEVICE_NAND_16BIT,		/* Not supported */
	BOOT_DEVICE_UART,
	BOOT_DEVICE_SPI_MODE_3,
	BOOT_DEVICE_SPI_MODE_0
};

/* Boot ROM debug mode enabled/disabled */
enum debug_mode {
	DEBUG_MODE_ENABLED,
	DEBUG_MODE_DISABLED
};

/* CPU existance indication */
enum cpu_exist {
	CPU_EXIST_0,
	CPU_EXIST_0_1,
	CPU_EXIST_0_1_2_3
};

/* Secure boot enabled/disabled */
enum secure_boot {
	SECURE_BOOT_ENABLED,
	SECURE_BOOT_DISABLED
};


/* struct definitions */

/* struct al_bootstrap
 * All freq values are measured in Hz units
 */
struct al_bootstrap {
	/* CPU PLL frequency - CPU clock has the same frequency */
	uint32_t		cpu_pll_freq;

	/* NB PLL frequency - DDR clock has the same frequency */
	uint32_t		nb_pll_freq;

	/* SB PLL frequency */
	uint32_t		sb_pll_freq;

	/* SB main clock frequency */
	uint32_t		sb_clk_freq;

	/* Reference clock frequency */
	uint32_t		pll_ref_clk_freq;

	enum pll_ref_clk	pll_ref_clk;
	enum i2c_preload	i2c_preload;
	enum spis_preload	spis_preload;
	enum boot_rom		boot_rom;
	enum boot_device	boot_device;
	enum debug_mode		debug_mode;
	enum cpu_exist		cpu_exist;
	enum secure_boot	secure_boot;
	uint8_t			i2c_preload_addr;
};

/* API definitions */

/**
 * Set the boot parameters according to the bootstrap register
 *
 * This function reads the Bootstrap register, parses its value, and
 * updates the bootstrap struct accordingly. The parsing is achieved by
 * using a per-parameter static functions.
 *
 * @param	pbs_regfile_ptr
 *		The address of the PBS Regfile
 * @param bootstrap
 *              The boot options struct we are initializing, should be allocated
 *		by the function's caller
 */
int al_bootstrap_parse(
		void __iomem *pbs_regfile_ptr,
		struct al_bootstrap *bootstrap);


#endif

/** @} end of Bootstrap group */

