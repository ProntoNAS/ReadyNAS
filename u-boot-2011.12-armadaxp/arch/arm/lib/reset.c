/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2004 Texas Insturments
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include "mvOs.h"
#include "cntmr/mvCntmr.h"
#include "cntmr/mvCntmrRegs.h"
#include "cpu/mvCpu.h"
int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int reg_data;
	puts ("resetting ...\n");
	//PCIE reset signal
	puts ("PCIE_RESETOUT\n");
	reg_data = MV_REG_READ(0x18140) & ~0x800;
	MV_REG_WRITE(0x18140, reg_data);
	udelay (100000);			/* wait 100 ms */
	disable_interrupts();
	reset_cpu(0);

	/*NOTREACHED*/
	return 0;
}
