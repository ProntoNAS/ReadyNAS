 /*
   * board/annapurna-labs/common/cmd_cpu_set_speed.c
   *
   * Thie file contains a U-Boot command for setting CPU speed
   *
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
#include <command.h>

#include "al_globals.h"
#include "al_hal_pll.h"

int do_cpu_set_speed(
	cmd_tbl_t *cmdtp,
	int flag,
	int argc,
	char *const argv[])
{
	enum al_pll_freq pll_freq;
	unsigned int pll_freq_val;
	struct al_pll_obj obj;
	unsigned int speed_khz;
	int err = 0;

	if (argc != 2) {
		printf("Syntax error - invalid number of parameters!\n\n");
		return -1;
	}

	speed_khz = 1000 * simple_strtol(argv[1], NULL, 10);

	/* Init SB PLL object */
	err = al_pll_init(
		(void __iomem *)AL_PLL_BASE(AL_PLL_CPU),
		"CPU PLL",
		(al_globals.bootstraps.pll_ref_clk_freq == 25000000) ?
		AL_PLL_REF_CLK_FREQ_25_MHZ :
		AL_PLL_REF_CLK_FREQ_125_MHZ,
		&obj);
	if (err) {
		printf("%s: al_pll_init failed!\n", __func__);
		return -1;
	}

	/* Obtain PLL current frequency */
	err = al_pll_freq_get(
		&obj,
		&pll_freq,
		&pll_freq_val);
	if (err) {
		printf("%s: al_pll_freq_get failed!\n", __func__);
		return err;
	}

	/*
	 * Check if the required frequency can be derived from the
	 * PLL frequency
	 */
	if (pll_freq_val % speed_khz) {
		printf("%s: PLL freq (%u) not suitable for %dMHz channel!\n",
			__func__, pll_freq_val, speed_khz / 1000);
		return -1;
	}

	/* Set the channel's PLL divider */
	err = al_pll_channel_div_set(
		&obj,
		0,
		pll_freq_val / speed_khz,
		0,
		0,	/* No reset */
		1,	/* Apply on last channel */
		1000);	/* 1ms timeout to settle */
	if (err) {
		printf("%s: al_pll_channel_div_set failed!\n",
				__func__);
		return -1;
	}

	return 0;
}

U_BOOT_CMD(
	cpu_set_speed, 2, 0, do_cpu_set_speed,
	"Set CPU speed",
	"cpu_set_speed <speed - MHz>\n\n");

