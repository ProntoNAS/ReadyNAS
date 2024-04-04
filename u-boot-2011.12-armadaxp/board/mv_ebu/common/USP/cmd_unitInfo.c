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

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include <command.h>
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

int do_active_units(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32  cpus, egigas;
#ifdef MV_USB
	MV_U32  usbs;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	MV_U32  satas;
	MV_BOOL  sataActive[MV_SATA_MAX_CHAN];
#endif
	MV_U32  i;
	MV_BOOL  pexActive[MV_PEX_MAX_IF+1];

	/*   CPU	*/
	cpus = mvCtrlGetCpuNum();
	printf("Active: ");
	for (i = 0; i <= cpus; i++) {
		if (i)
			printf(", ");
		printf("cpu%d", i);
	}
	/*   PEX	*/
	mvCtrlGetPexActive(pexActive, MV_PEX_MAX_IF);

	for (i = 0; i <= MV_PEX_MAX_IF; i++) {
		if (pexActive[i])
			printf(", pex%d", i);
	}
	/*   EGIGA	*/
	egigas = mvCtrlEthMaxPortGet();
	for (i = 0; i < egigas; i++) {
		if (MV_FALSE ==  mvBoardIsGbEPortConnected(i))
			continue;
		if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, i))
			continue;
		printf(", egiga%d", i);
	}

#ifdef MV_USB
	/*   USB	*/
	usbs = mvCtrlUsbMaxGet();
	for (i = 0; i < usbs; i++)
		printf(", usb%d", i);
#endif
	/*   SDIO	*/
#ifdef CONFIG_MRVL_MMC
	printf(", mmc0");
#endif
	/*   SATA	*/
#if defined(MV_INCLUDE_INTEG_SATA)
	satas = mvCtrlSataMaxPortGet();
	mvCtrlGetSataActive(sataActive, MV_SATA_MAX_CHAN);
	for (i = 0; i < satas; i++) {
		if (sataActive[i])
			printf(", sata%d", i);
	}
#endif
	/*   SPI	*/
#if defined(MV_INCLUDE_SPI)
	printf(", spi");
#endif

	/*   NAND	*/
#if defined(MV_NAND)
	printf(", nand");
#endif
	/*   NOR	*/
#if defined(MV_INCLUDE_NOR)
	printf(", nor");
#endif
	/*   I2C	*/
#if defined(MV_INCLUDE_TWSI)
	printf(", i2c0");
#endif
	printf("\n");
	return 0;
}



U_BOOT_CMD(
	active_units,	1,	1,	do_active_units,
	"print Board units active ",
	""
);
