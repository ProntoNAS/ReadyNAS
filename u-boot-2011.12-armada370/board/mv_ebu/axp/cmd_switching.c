/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <common.h>
#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"
#include "ctrlEnv/sys/mvAhbToMbusRegs.h"

int sram_init_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
	MV_U32 baseAddr;
	MV_U32 sramBase;
	MV_U32 waysNum;

	if( argc < 4 ) {
		printf( "Usage:\n%s\n", cmdtp->usage );
		return 0;
	}

	baseAddr = simple_strtoul( argv[1], NULL, 16 );
	sramBase = simple_strtoul( argv[2], NULL, 10 );
	waysNum = simple_strtoul( argv[3], NULL, 10 );

	return mvCtrlSramInit(baseAddr, sramBase, waysNum);
}

U_BOOT_CMD(
	sramInit,      4,     1,      sram_init_cmd,
	"sramInit	- Init SRAM memory\n",
	" regs_base_address - Internal Registers Base Address (to support init of other CPUs' SRAM) \n"
	" sram_base - SRAM base address \n"
	" ways_num - number of SRAM ways to init, allocate the ways from the end od the SRMA. \n"
	"\tInit SRAM memroy. \n"
	);
