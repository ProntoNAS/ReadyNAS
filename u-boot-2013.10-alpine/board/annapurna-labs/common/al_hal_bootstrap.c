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
*  Bootstrap
*  @{
* @file   al_hal_bootstrap.c
*
* @brief  includes bootstrap HAL implementation
*
*/

#include "al_hal_bootstrap.h"
#include "al_hal_bootstrap_map.h"

/**
 * The following al_bootstrap_get_* functions are all implemented the same way:
 * Update the appropriate field on the bootstrap struct, according to the
 * register value.
 *
 * Note that in case of an error, no value will be written to the bootstrap
 * struct - so any uninitialized value should be ignored.
 *
 * @param [in]	bootstrap_reg_val
 *		The value of the bootstrap register
 * @param [in]	bypass_freq
 *		Frequency to be returned in case bypass mode is requested
 * @param [out]	bootstrap
 *		The bootstrap struct we are setting
 */
static inline void al_bootstrap_cpu_pll_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap)
{
	uint32_t field, parsed_field = 2000000000U;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_MASK,
			AL_HAL_BOOTSTRAP_CPU_PLL_FREQ_SHIFT);

	if (bypass_freq == 25000000) {
		switch (field) {
		case 0x0:
			parsed_field = bypass_freq;
			break;
		case 0x1:
			parsed_field = 1250000000U;
			break;
		case 0x2:
			parsed_field = 1500000000U;
			break;
		case 0x3:
			parsed_field = 1625000000U;
			break;
		case 0x4:
			parsed_field = 1750000000U;
			break;
		case 0x5:
			parsed_field = 2125000000U;
			break;
		case 0x6:
			parsed_field = 2250000000U;
			break;
		case 0x7:
			parsed_field = 2375000000U;
			break;
		case 0x8:
			parsed_field = 2500000000U;
			break;
		case 0x9:
			parsed_field = 2625000000U;
			break;
		case 0xA:
			parsed_field = 2750000000U;
			break;
		case 0xB:
			parsed_field = 2875000000U;
			break;
		case 0xC:
			parsed_field = 3000000000U;
			break;
		case 0xF:
			parsed_field = 2000000000U;
			break;
		default:
			al_assert(0);
			break;
		}
	} else {
		switch (field) {
		case 0x0:
			parsed_field = bypass_freq;
			break;
		case 0x1:
			parsed_field = 1000000000U;
			break;
		case 0x2:
			parsed_field = 1400000000U;
			break;
		case 0x3:
			parsed_field = 1500000000U;
			break;
		case 0x4:
			parsed_field = 1600000000U;
			break;
		case 0x5:
			parsed_field = 1700000000U;
			break;
		case 0x6:
			parsed_field = 1800000000U;
			break;
		case 0x7:
			parsed_field = 1900000000U;
			break;
		case 0x8:
			parsed_field = 2100000000U;
			break;
		case 0x9:
			parsed_field = 2200000000U;
			break;
		case 0xA:
			parsed_field = 2300000000U;
			break;
		case 0xB:
			parsed_field = 2400000000U;
			break;
		case 0xC:
			parsed_field = 2500000000U;
			break;
		case 0xF:
			parsed_field = 2000000000U;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->cpu_pll_freq = parsed_field;
}

static inline void al_bootstrap_nb_pll_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap)
{
	uint32_t field, parsed_field = 800000000;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_NB_PLL_FREQ_MASK,
			AL_HAL_BOOTSTRAP_NB_PLL_FREQ_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = bypass_freq;
		break;
	case 0x1:
		parsed_field = 533333333;
		break;
	case 0x2:
		parsed_field = 666666666;
		break;
	case 0x3:
		parsed_field = 916666666;
		break;
	case 0x4:
		parsed_field = 933333333;
		break;
	case 0x5:
		parsed_field = 1066000000;
		break;
	case 0x7:
		parsed_field = 800000000;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->nb_pll_freq = parsed_field;
}

/* This set function sets both the sb_pll_freq and sb_clk_freq, because the pll
 * affects the clk when its set to bypass mode */
static inline void al_bootstrap_sb_pll_and_clk_freq_get(
		uint32_t bootstrap_reg_val,
		uint32_t bypass_freq,
		struct al_bootstrap *bootstrap)
{
	uint32_t pll_field, pll_parsed_field = 1500000000;
	uint32_t clk_field, clk_parsed_field = 375000000;
	int bypass_flag = 0;

	pll_field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SB_PLL_FREQ_MASK,
			AL_HAL_BOOTSTRAP_SB_PLL_FREQ_SHIFT);
	if (pll_field) {
		pll_parsed_field = 1500000000;
	} else {
		pll_parsed_field = bypass_freq;
		bypass_flag = 1;
	}

	bootstrap->sb_pll_freq = pll_parsed_field;

	if (bypass_flag)
		/* if pll is on bypass mode, clk_freq = bypass_freq */
		clk_parsed_field = bypass_freq;
	else {
		clk_field = AL_REG_FIELD_GET(bootstrap_reg_val,
				AL_HAL_BOOTSTRAP_SB_CLK_FREQ_MASK,
				AL_HAL_BOOTSTRAP_SB_CLK_FREQ_SHIFT);
		switch (clk_field) {
		case 0x0:
			clk_parsed_field = 250000000;
			break;
		case 0x1:
			clk_parsed_field = 300000000;
			break;
		case 0x2:
			clk_parsed_field = 500000000;
			break;
		case 0x3:
			clk_parsed_field = 375000000;
			break;
		default:
			al_assert(0);
			break;
		}
	}

	bootstrap->sb_clk_freq = clk_parsed_field;
}

static inline void al_bootstrap_pll_ref_clk_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum pll_ref_clk parsed_field = PLL_REF_CLK_CMOS;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_MASK,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = PLL_REF_CLK_HSTL;
		break;
	case 0x1:
		parsed_field = PLL_REF_CLK_CMOS;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->pll_ref_clk = parsed_field;
}

static inline void al_bootstrap_i2c_preload_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum i2c_preload parsed_field = I2C_PRELOAD_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = I2C_PRELOAD_ENABLED;
		break;
	case 0x1:
		parsed_field = I2C_PRELOAD_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->i2c_preload = parsed_field;
}

static inline void al_bootstrap_spis_preload_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum spis_preload parsed_field = SPIS_PRELOAD_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_MASK,
			AL_HAL_BOOTSTRAP_SPIS_PRELOAD_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = SPIS_PRELOAD_ENABLED;
		break;
	case 0x1:
		parsed_field = SPIS_PRELOAD_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->spis_preload = parsed_field;
}

static inline void al_bootstrap_boot_rom_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum boot_rom parsed_field = BOOT_ROM_ENABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_BOOT_ROM_MASK,
			AL_HAL_BOOTSTRAP_BOOT_ROM_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = BOOT_ROM_DISABLED;
		break;
	case 0x1:
		parsed_field = BOOT_ROM_ENABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->boot_rom = parsed_field;
}

static inline void al_bootstrap_boot_device_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum boot_device parsed_field = BOOT_DEVICE_SPI_MODE_0;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_BOOT_DEVICE_MASK,
			AL_HAL_BOOTSTRAP_BOOT_DEVICE_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = BOOT_DEVICE_NOR_8BIT;
		break;
	case 0x1:
		parsed_field = BOOT_DEVICE_NOR_16BIT;
		break;
	case 0x2:
		parsed_field = BOOT_DEVICE_NAND_8BIT;
		break;
	case 0x3:
		parsed_field = BOOT_DEVICE_NAND_16BIT;
		break;
	case 0x4:
		parsed_field = BOOT_DEVICE_UART;
		break;
	case 0x5:
		parsed_field = BOOT_DEVICE_SPI_MODE_3;
		break;
	case 0x7:
		parsed_field = BOOT_DEVICE_SPI_MODE_0;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->boot_device = parsed_field;
}

static inline void al_bootstrap_debug_mode_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum debug_mode parsed_field = DEBUG_MODE_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_DEBUG_MODE_MASK,
			AL_HAL_BOOTSTRAP_DEBUG_MODE_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = DEBUG_MODE_ENABLED;
		break;
	case 0x1:
		parsed_field = DEBUG_MODE_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->debug_mode = parsed_field;
}

static inline void al_bootstrap_pll_ref_clk_freq_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field, parsed_field = 25000000;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_MASK,
			AL_HAL_BOOTSTRAP_PLL_REF_CLK_FREQ_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = 25000000;
		break;
	case 0x1:
		parsed_field = 100000000;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->pll_ref_clk_freq = parsed_field;
}

static inline void al_bootstrap_cpu_exist_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum cpu_exist parsed_field = CPU_EXIST_0;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_CPU_EXIST_MASK,
			AL_HAL_BOOTSTRAP_CPU_EXIST_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = CPU_EXIST_0;
		break;
	case 0x1:
		parsed_field = CPU_EXIST_0_1;
		break;
	case 0x3:
		parsed_field = CPU_EXIST_0_1_2_3;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->cpu_exist = parsed_field;
}

static inline void al_bootstrap_secure_boot_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	enum secure_boot parsed_field = SECURE_BOOT_DISABLED;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_SECURE_BOOT_MASK,
			AL_HAL_BOOTSTRAP_SECURE_BOOT_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = SECURE_BOOT_ENABLED;
		break;
	case 0x1:
		parsed_field = SECURE_BOOT_DISABLED;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->secure_boot = parsed_field;
}


static inline void al_bootstrap_i2c_preload_addr_get(
		uint32_t bootstrap_reg_val,
		struct al_bootstrap *bootstrap)
{
	uint32_t field;
	uint8_t parsed_field = 0x50;

	field = AL_REG_FIELD_GET(bootstrap_reg_val,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_MASK,
			AL_HAL_BOOTSTRAP_I2C_PRELOAD_ADDR_SHIFT);
	switch (field) {
	case 0x0:
		parsed_field = 0x57;
		break;
	case 0x1:
		parsed_field = 0x50;
		break;
	default:
		al_assert(0);
		break;
	}

	bootstrap->i2c_preload_addr = parsed_field;
}


int al_bootstrap_parse(
		void __iomem *pbs_regfile_ptr,
		struct al_bootstrap *bootstrap)
{
	struct al_pbs_regs *pbs_regfile;
	uint32_t bootstrap_reg_val;

	/* Read the value of the bootstrap register */
	pbs_regfile = pbs_regfile_ptr;
	bootstrap_reg_val = al_reg_read32(&pbs_regfile->unit.boot_strap);

	/* Parse the bootstrap register into the al_bootstrap struct
	 * The following code parse each field seperately. In case of multiple
	 * errors, the function will return the first error it encounters.
	 */
	al_bootstrap_pll_ref_clk_freq_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_cpu_pll_freq_get(bootstrap_reg_val,
			bootstrap->pll_ref_clk_freq,
			bootstrap);
	al_bootstrap_nb_pll_freq_get(bootstrap_reg_val,
			bootstrap->pll_ref_clk_freq,
			bootstrap);
	al_bootstrap_sb_pll_and_clk_freq_get(bootstrap_reg_val,
			bootstrap->pll_ref_clk_freq,
			bootstrap);
	al_bootstrap_pll_ref_clk_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_i2c_preload_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_spis_preload_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_boot_rom_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_boot_device_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_debug_mode_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_cpu_exist_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_secure_boot_get(bootstrap_reg_val, bootstrap);
	al_bootstrap_i2c_preload_addr_get(bootstrap_reg_val, bootstrap);

	return 0;
}


/** @} end of Bootstrap group */


