/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

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


#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"

/* For future use */
#define BD_ID_DATA_START_OFFS			0x0
#define BD_DETECT_SEQ_OFFS				0x0
#define BD_SYS_NUM_OFFS					0x4
#define BD_NAME_OFFS					0x8

#define MV_BOARD_DIMM0_I2C_ADDR			0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE 	ADDR7_BIT
#define MV_BOARD_DIMM_I2C_CHANNEL		0x0


/* Board specific configuration */
/* ============================ */

/* boards ID numbers */
#define BOARD_ID_BASE			0x0

/* New board ID numbers */
#define DB_88F6710_BP_ID		(BOARD_ID_BASE)
#define DB_88F6710_PCAC_ID		(DB_88F6710_BP_ID + 1)
#define RD_88F6710_ID                   (DB_88F6710_PCAC_ID + 1)
#define DB_88F6710_BP_ID_RN104		(RD_88F6710_ID + 1)
#define DB_88F6710_BP_ID_S2000		(DB_88F6710_BP_ID_RN104 + 1)
#define MV_MAX_BOARD_ID			(DB_88F6710_BP_ID_S2000 + 1)
unsigned int get_board_type(void);
#define NETGEAR_BD_TYPE_UNKNOWN 0
#define NETGEAR_BD_TYPE_RN102   0x00000001
#define NETGEAR_BD_TYPE_RN104   0x00000002
#define NETGEAR_BD_TYPE_S2000   0x00000004

/******************/
/* DB-88F6710-BP */
/******************/
#define RN104_MPP0_7               0x11111111
#define RN104_MPP8_15              0x11111111
#define RN104_MPP16_23             0x22222111
#define RN104_MPP24_31             0x02222222
#define RN104_MPP32_39             0x11111110
#define RN104_MPP40_47             0x01111111
#define RN104_MPP48_55             0x00000000
#define RN104_MPP56_63             0x00000000
#define RN104_MPP64_67             0x00000000

#define DB_88F6710_MPP0_7          0x00011111
#define DB_88F6710_MPP8_15         0x00000000
#define DB_88F6710_MPP16_23        0x22222110
#define DB_88F6710_MPP24_31        0x02222222
#define DB_88F6710_MPP32_39        0x11111110
#define DB_88F6710_MPP40_47        0x01111111
#define DB_88F6710_MPP48_55        0x00000000
#define DB_88F6710_MPP56_63        0x10000000
#define DB_88F6710_MPP64_67        0x00000011

#define S2000_MPP0_7               0x00011111
#define S2000_MPP8_15              0x00000000
#define S2000_MPP16_23             0x22222110
#define S2000_MPP24_31             0x02222222
#define S2000_MPP32_39             0x11111110
#define S2000_MPP40_47             0x31111111
#define S2000_MPP48_55             0x40033333
#define S2000_MPP56_63             0x00000040
#define S2000_MPP64_67             0x00000001

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
31	Giga_inN		IN
48	USB_Dev_Detect	IN
59	7seg bit0		OUT
60	7seg bit1		OUT
61	7seg bit2		OUT
*/
#define RN104_GPP_OUT_ENA_LOW              (~(BIT31))
#define RN104_GPP_OUT_ENA_MID              (~(BIT15|BIT21|BIT22|BIT23|BIT24|BIT25|BIT26|BIT27|BIT28|BIT29|BIT31))
#define RN104_GPP_OUT_ENA_HIGH             (~(BIT0))

#define RN104_GPP_OUT_VAL_LOW              (BIT31)
#define RN104_GPP_OUT_VAL_MID              (BIT15|BIT22|BIT23|BIT24|BIT25|BIT26|BIT27|BIT28|BIT29)
#define RN104_GPP_OUT_VAL_HIGH             0x0

#define RN104_GPP_POL_LOW                  0x0
#define RN104_GPP_POL_MID                  BIT20
#define RN104_GPP_POL_HIGH                 BIT1

#define DB_88F6710_GPP_OUT_ENA_LOW              (~(BIT8|BIT11|BIT13|BIT14|BIT15|BIT16|BIT31))
#define DB_88F6710_GPP_OUT_ENA_MID              (~(BIT16|BIT17|BIT18|BIT19|BIT20|BIT21|BIT23|BIT24|BIT25|BIT28))
#define DB_88F6710_GPP_OUT_ENA_HIGH             (~(0x0))

#define DB_88F6710_GPP_OUT_VAL_LOW              (BIT8|BIT11|BIT13|BIT14|BIT15|BIT16|BIT31)
#define DB_88F6710_GPP_OUT_VAL_MID              (BIT16|BIT17|BIT18|BIT19|BIT20|BIT21|BIT23|BIT24|BIT28)
#define DB_88F6710_GPP_OUT_VAL_HIGH             0x0

#define DB_88F6710_GPP_POL_LOW                  BIT6
#define DB_88F6710_GPP_POL_MID                  BIT26
#define DB_88F6710_GPP_POL_HIGH                 0x0

#define S2000_GPP_OUT_ENA_LOW              (~(BIT7|BIT8|BIT11|BIT13|BIT14|BIT15|BIT31))
#define S2000_GPP_OUT_ENA_MID              (~(BIT21|BIT24|BIT28))
#define S2000_GPP_OUT_ENA_HIGH             (~(0x0))

#define S2000_GPP_OUT_VAL_LOW              (BIT8|BIT11|BIT13|BIT14|BIT15|BIT31)
#define S2000_GPP_OUT_VAL_MID              (BIT24|BIT28)
#define S2000_GPP_OUT_VAL_HIGH             0x0

#define S2000_GPP_POL_LOW                  BIT6
#define S2000_GPP_POL_MID                  BIT26
#define S2000_GPP_POL_HIGH                 0x0

/*******************/
/* DB-88F6710-PCAC */
/*******************/

#define DB_88F6710_PCAC_MPP0_7			0x00001111
#define DB_88F6710_PCAC_MPP8_15			0x00000000
#define DB_88F6710_PCAC_MPP16_23		0x00000110
#define DB_88F6710_PCAC_MPP24_31		0x10000000
#define DB_88F6710_PCAC_MPP32_39		0x11111111
#define DB_88F6710_PCAC_MPP40_47		0x01111111
#define DB_88F6710_PCAC_MPP48_55		0x00000000
#define DB_88F6710_PCAC_MPP56_63		0x14040000
#define DB_88F6710_PCAC_MPP64_67		0x00000011

/* GPPs
MPP#	NAME			IN/OUT
----------------------------------------------
58	7seg bit0		OUT
59	7seg bit1		OUT
61	7seg bit2		OUT
*/
#define DB_88F6710_PCAC_GPP_OUT_ENA_LOW		(~(0x0))
#define DB_88F6710_PCAC_GPP_OUT_ENA_MID		(~(BIT26 | BIT27 | BIT29))
#define DB_88F6710_PCAC_GPP_OUT_ENA_HIGH	(~(0x0))

#define DB_88F6710_PCAC_GPP_OUT_VAL_LOW		0x0
#define DB_88F6710_PCAC_GPP_OUT_VAL_MID		0x0
#define DB_88F6710_PCAC_GPP_OUT_VAL_HIGH	0x0

#define DB_88F6710_PCAC_GPP_POL_LOW			0x0
#define DB_88F6710_PCAC_GPP_POL_MID			0x0
#define DB_88F6710_PCAC_GPP_POL_HIGH		0x0

/*******************/
/* RD_88F6710 */
/*******************/
//#define RD_88F6710_MPP0_7         0x00011111
#define RD_88F6710_MPP0_7         0x00001111
#define RD_88F6710_MPP8_15        0x33333030
#define RD_88F6710_MPP16_23       0x22222110
#define RD_88F6710_MPP24_31       0x02222222
#define RD_88F6710_MPP32_39       0x11111110
#define RD_88F6710_MPP40_47       0x01111111
#define RD_88F6710_MPP48_55       0x33344444
#define RD_88F6710_MPP56_63       0x03555556
#define RD_88F6710_MPP64_67       0x00000000

/* GPPs ARMADA370
MPP#	NAME			IN/OUT
----------------------------------------------
5       PEX RST#                OUT (1)
6       GPP_PB                  IN
8       Fan power control       OUT (1)
10      SDIO Status             IN
16      SDIO WP                 IN
31      Switch Interrupt        IN
32      User LED                OUT(?)
47      USB Power On            OUT(0)
63      HDD Select              OUT(0)
64      Int HDD Power           OUT(1)
65      Ext HDD Power           OUT(0)

*/
#define RD_88F6710_GPP_OUT_ENA_LOW	(~(BIT5 | BIT8))
#define RD_88F6710_GPP_OUT_ENA_MID	(~(BIT0 | BIT15 | BIT31))
#define RD_88F6710_GPP_OUT_ENA_HIGH	(~(BIT0 | BIT1))

#define RD_88F6710_GPP_OUT_VAL_LOW	(BIT5 | BIT8)
#define RD_88F6710_GPP_OUT_VAL_MID	0x0
#define RD_88F6710_GPP_OUT_VAL_HIGH	0x0


#define RD_88F6710_GPP_POL_LOW		BIT31
#define RD_88F6710_GPP_POL_MID		0x0
#define RD_88F6710_GPP_POL_HIGH		0x0




#endif /* __INCmvBoardEnvSpech */
