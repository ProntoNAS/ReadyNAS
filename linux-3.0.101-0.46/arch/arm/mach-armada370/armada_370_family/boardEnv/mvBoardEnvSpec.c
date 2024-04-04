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
#include "mvCommon.h"
#include "mvBoardEnvLib.h"
#include "mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPexRegs.h"

#define ARRSZ(x)	(sizeof(x)/sizeof(x[0]))

/***********************/
/* ARMADA-370 DB BOARD */
/***********************/

#define DB_88F6710_BOARD_NOR_READ_PARAMS	0x403E07CF
#define DB_88F6710_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8	db88f6710InfoBoardDebugLedIf[] = {59, 60, 61};

MV_BOARD_TWSI_INFO	db88f6710InfoBoardTwsiDev[] = {
#if 0
	/* {{MV_BOARD_TWSI_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
#endif
};

MV_BOARD_MAC_INFO db88f6710InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
        {BOARD_MAC_SPEED_AUTO, 0x0},
        {BOARD_MAC_SPEED_AUTO, 0x0},
};

MV_BOARD_MAC_INFO db88f6710InfoBoardMacInfo_rn104[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
        {BOARD_MAC_SPEED_AUTO, 0x0},
        {BOARD_MAC_SPEED_AUTO, 0x1},
};

MV_BOARD_SWITCH_INFO db88f6710InfoBoardSwitchValue[] = {
	{
		.switchIrq = (31 + 128),	/* set to -1 for timer operation. 128 is the base IRQ number for GPP interrupts */
		.switchPort = {0, 1, 2, 3, 4},
		.cpuPort = 6,
		.connectedPort = {-1, 6},
		.smiScanMode = 2,
		.quadPhyAddr = 0,
		.forceLinkMask = 0x0
	}
};

MV_BOARD_MODULE_TYPE_INFO db88f6710InfoBoardModTypeInfo[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_RGMII1,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
	}
};

MV_BOARD_MODULE_TYPE_INFO db88f6710InfoBoardModTypeInfo_rn104[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_RGMII1 | MV_BOARD_RGMII0,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
	}
};

MV_BOARD_MODULE_TYPE_INFO db88f6710InfoBoardModTypeInfo_s2000[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_RGMII1 | MV_BOARD_SDIO,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
	}
};

MV_BOARD_GPP_INFO db88f6710InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 48} /* from MPP map */
};

MV_BOARD_GPP_INFO S2000BoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 48}, /* from MPP map */
	{BOARD_GPP_SDIO_DETECT, 63}, /* from MPP map */
	{BOARD_GPP_SDIO_WP, 16} /* from MPP map */
};

MV_DEV_CS_INFO db88f6710InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6710InfoBoardMppConfigValue[] = {
	{ {
		DB_88F6710_MPP0_7,
		DB_88F6710_MPP8_15,
		DB_88F6710_MPP16_23,
		DB_88F6710_MPP24_31,
		DB_88F6710_MPP32_39,
		DB_88F6710_MPP40_47,
		DB_88F6710_MPP48_55,
		DB_88F6710_MPP56_63,
		DB_88F6710_MPP64_67,
	} }
};

MV_BOARD_MPP_INFO db88f6710InfoBoardMppConfigValue_rn104[] = {
	{ {
		RN104_MPP0_7,
		RN104_MPP8_15,
		RN104_MPP16_23,
		RN104_MPP24_31,
		RN104_MPP32_39,
		RN104_MPP40_47,
		RN104_MPP48_55,
		RN104_MPP56_63,
		RN104_MPP64_67,
	} }
};

MV_BOARD_MPP_INFO db88f6710InfoBoardMppConfigValue_s2000[] = {
	{ {
		S2000_MPP0_7,
		S2000_MPP8_15,
		S2000_MPP16_23,
		S2000_MPP24_31,
		S2000_MPP32_39,
		S2000_MPP40_47,
		S2000_MPP48_55,
		S2000_MPP56_63,
		S2000_MPP64_67,
	} }
};

MV_SERDES_CFG db88f6710InfoBoardSerdesConfigValue[] = {
	/* Default - Auto detection */
	{ SRDS_MOD_AUTO,  0x0 },
	/* DB6710 Basic Modules Setup */
	{ SRDS_MOD_PCIE0_LANE0 | SRDS_MOD_PCIE1_LANE1 | SRDS_MOD_SATA0_LANE2 | SRDS_MOD_SATA1_LANE3, 0 }
};

MV_SERDES_CFG db88f6710InfoBoardSerdesConfigValue_s2000[] = {
	/* Default - Auto detection */
	{ SRDS_MOD_AUTO,  0x0 },
	/* DB6710 Basic Modules Setup */
	{ SRDS_MOD_PCIE0_LANE0 | SRDS_MOD_SATA0_LANE2 | SRDS_MOD_SATA1_LANE3, 0 }
};

MV_BOARD_TDM_INFO	db88f6710Tdm880[]	= { {0} }; /* SPI Cs */

MV_BOARD_TDM_SPI_INFO db88f6710TdmSpiInfo[] = { {1} }; /* SPI controller ID */

MV_BOARD_INFO db88f6710Info = {
	.boardName				= "RN102",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710InfoBoardModTypeInfo),
	.pBoardModTypeValue			= db88f6710InfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f6710InfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f6710InfoBoardMppConfigValue,
	.pBoardSerdesConfigValue	= db88f6710InfoBoardSerdesConfigValue,
	//.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskLow				= 0,
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710InfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(db88f6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= db88f6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(db88f6710InfoBoardMacInfo),
	.pBoardMacInfo				= db88f6710InfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(db88f6710InfoBoardGppInfo),
	.pBoardGppInfo				= db88f6710InfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710InfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 16000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6710_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6710_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6710_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6710_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6710_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6710_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6710_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6710_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6710_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6710InfoBoardSwitchValue),

	/* TDM configuration */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f6710Tdm880},
	.boardTdmInfoIndex		= 0,
	.pBoardTdmSpiInfo 		= db88f6710TdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= DB_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F6710_BOARD_NOR_WRITE_PARAMS
};

MV_BOARD_INFO db88f6710Info_rn104 = {
	.boardName				= "RN104",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710InfoBoardModTypeInfo_rn104),
	.pBoardModTypeValue			= db88f6710InfoBoardModTypeInfo_rn104,
	.numBoardMppConfigValue		= ARRSZ(db88f6710InfoBoardMppConfigValue_rn104),
	.pBoardMppConfigValue		= db88f6710InfoBoardMppConfigValue_rn104,
	.pBoardSerdesConfigValue	= db88f6710InfoBoardSerdesConfigValue,
	//.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskLow				= 0,
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710InfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(db88f6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= db88f6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(db88f6710InfoBoardMacInfo_rn104),
	.pBoardMacInfo				= db88f6710InfoBoardMacInfo_rn104,
	.numBoardGppInfo			= ARRSZ(db88f6710InfoBoardGppInfo),
	.pBoardGppInfo				= db88f6710InfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710InfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 16000,

	/* GPP values */
	.gppOutEnValLow			= RN104_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RN104_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RN104_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RN104_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RN104_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RN104_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RN104_GPP_POL_LOW,
	.gppPolarityValMid		= RN104_GPP_POL_MID,
	.gppPolarityValHigh		= RN104_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6710InfoBoardSwitchValue),

	/* TDM configuration */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f6710Tdm880},
	.boardTdmInfoIndex		= 0,
	.pBoardTdmSpiInfo 		= db88f6710TdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= DB_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F6710_BOARD_NOR_WRITE_PARAMS
};

MV_BOARD_INFO db88f6710Info_s2000 = {
	.boardName				= "S2000",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710InfoBoardModTypeInfo_s2000),
	.pBoardModTypeValue			= db88f6710InfoBoardModTypeInfo_s2000,
	.numBoardMppConfigValue		= ARRSZ(db88f6710InfoBoardMppConfigValue_s2000),
	.pBoardMppConfigValue		= db88f6710InfoBoardMppConfigValue_s2000,
	.pBoardSerdesConfigValue	= db88f6710InfoBoardSerdesConfigValue_s2000,
	//.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskLow				= 0,
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710InfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(db88f6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= db88f6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(db88f6710InfoBoardMacInfo),
	.pBoardMacInfo				= db88f6710InfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(S2000BoardGppInfo),
	.pBoardGppInfo				= S2000BoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710InfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 16000,

	/* GPP values */
	.gppOutEnValLow			= S2000_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= S2000_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= S2000_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= S2000_GPP_OUT_VAL_LOW,
	.gppOutValMid			= S2000_GPP_OUT_VAL_MID,
	.gppOutValHigh			= S2000_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= S2000_GPP_POL_LOW,
	.gppPolarityValMid		= S2000_GPP_POL_MID,
	.gppPolarityValHigh		= S2000_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(db88f6710InfoBoardSwitchValue),

	/* TDM configuration */
	.numBoardTdmInfo		= {1},
	.pBoardTdmInt2CsInfo		= {db88f6710Tdm880},
	.boardTdmInfoIndex		= 0,
	.pBoardTdmSpiInfo 		= db88f6710TdmSpiInfo,

	/* NOR init params */
	.norFlashReadParams		= DB_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= DB_88F6710_BOARD_NOR_WRITE_PARAMS
};
/*************************/
/* ARMADA-370 PCAC BOARD */
/*************************/

MV_U8	db88f6710pcacInfoBoardDebugLedIf[] = {58, 59, 61};

MV_BOARD_MAC_INFO db88f6710pcacInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0},
	{BOARD_MAC_SPEED_AUTO, 0x1},
};

MV_BOARD_MODULE_TYPE_INFO db88f6710pcacInfoBoardModTypeInfo[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_AUTO,
		.boardMppGrp2Mod	= MV_BOARD_AUTO
	}
};

MV_BOARD_GPP_INFO db88f6710pcacInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 24} /* from MPP map */
};

MV_DEV_CS_INFO db88f6710pcacInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO db88f6710pcacInfoBoardMppConfigValue[] = {
	{ {
		DB_88F6710_PCAC_MPP0_7,
		DB_88F6710_PCAC_MPP8_15,
		DB_88F6710_PCAC_MPP16_23,
		DB_88F6710_PCAC_MPP24_31,
		DB_88F6710_PCAC_MPP32_39,
		DB_88F6710_PCAC_MPP40_47,
		DB_88F6710_PCAC_MPP48_55,
		DB_88F6710_PCAC_MPP56_63,
		DB_88F6710_PCAC_MPP64_67,
	} }
};

MV_SERDES_CFG db88f6710pcacInfoBoardSerdesConfigValue[] = {
	/* Default - Auto detection */
	{ SRDS_MOD_AUTO,  0x0 },
	/* DB6710 PCAC Serdes static configuration */
	{ SRDS_MOD_PCIE0_LANE0 | SRDS_MOD_SGMII0_LANE2, 0 }
};

MV_BOARD_INFO db88f6710pcacInfo = {
	.boardName					= "DB-88F6710-PCAC",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue		= ARRSZ(db88f6710pcacInfoBoardModTypeInfo),
	.pBoardModTypeValue			= db88f6710pcacInfoBoardModTypeInfo,
	.numBoardMppConfigValue		= ARRSZ(db88f6710pcacInfoBoardMppConfigValue),
	.pBoardMppConfigValue		= db88f6710pcacInfoBoardMppConfigValue,
	.pBoardSerdesConfigValue	= db88f6710pcacInfoBoardSerdesConfigValue,
	.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(db88f6710pcacInfoBoardDeCsInfo),
	.pDevCsInfo					= db88f6710pcacInfoBoardDeCsInfo,
	.numBoardTwsiDev			= 0,
	.pBoardTwsiDev				= NULL,
	.numBoardMacInfo			= ARRSZ(db88f6710pcacInfoBoardMacInfo),
	.pBoardMacInfo				= db88f6710pcacInfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(db88f6710pcacInfoBoardGppInfo),
	.pBoardGppInfo				= db88f6710pcacInfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(db88f6710pcacInfoBoardDebugLedIf),
	.pLedGppPin					= db88f6710pcacInfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow			= DB_88F6710_PCAC_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= DB_88F6710_PCAC_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= DB_88F6710_PCAC_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= DB_88F6710_PCAC_GPP_OUT_VAL_LOW,
	.gppOutValMid			= DB_88F6710_PCAC_GPP_OUT_VAL_MID,
	.gppOutValHigh			= DB_88F6710_PCAC_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= DB_88F6710_PCAC_GPP_POL_LOW,
	.gppPolarityValMid		= DB_88F6710_PCAC_GPP_POL_MID,
	.gppPolarityValHigh		= DB_88F6710_PCAC_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	.numBoardTdmInfo	= { 0 },
	.pBoardTdmInt2CsInfo	= { NULL },
	.boardTdmInfoIndex	= -1,

	/* NOR init params */
	.norFlashReadParams	= 0,
	.norFlashWriteParams	= 0,
};

/*************************/
/* ARMADA-370 RD BOARD */
/*************************/
#define RD_88F6710_BOARD_NOR_READ_PARAMS	0x403E07CF
#define RD_88F6710_BOARD_NOR_WRITE_PARAMS	0x000F0F0F

MV_U8	rd88F6710InfoBoardDebugLedIf[] = {32};

MV_BOARD_TWSI_INFO	rd88F6710InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_TWSI_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x50, ADDR7_BIT},
};

MV_BOARD_MAC_INFO rd88F6710InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED	boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x0},
	{BOARD_MAC_SPEED_1000M, 0x10},
};

MV_BOARD_SWITCH_INFO rd88F6710InfoBoardSwitchValue[] = {
	{
		.switchIrq = (31 + 128),	/* set to -1 for timer operation. 128 is the base IRQ number for GPP interrupts */
		.switchPort = {0, 1, 2, 3, -1},
		.cpuPort = 5,
		.connectedPort = {-1, 5},
		.smiScanMode = 2,
		.quadPhyAddr = 0,
		.forceLinkMask = 0x0
	}
};

MV_BOARD_MODULE_TYPE_INFO rd88F6710InfoBoardModTypeInfo[] = {
	{
		.boardMppGrp1Mod	= MV_BOARD_SDIO | MV_BOARD_RGMII1,
		.boardMppGrp2Mod	= MV_BOARD_TDM
	}
};

MV_BOARD_GPP_INFO rd88F6710InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS	devClass, MV_U8	gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 24} /* from MPP map */
};

MV_DEV_CS_INFO rd88F6710InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth, busWidth }*/
#if defined(MV_INCLUDE_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8, 8}, /* SPI DEV */
#endif
#if defined(MV_INCLUDE_NOR)
	{DEV_BOOCS, N_A, BOARD_DEV_NOR_FLASH, 16, 16} /* NOR DEV */
#endif
};

MV_BOARD_MPP_INFO rd88F6710InfoBoardMppConfigValue[] = {
	{ {
		RD_88F6710_MPP0_7,
		RD_88F6710_MPP8_15,
		RD_88F6710_MPP16_23,
		RD_88F6710_MPP24_31,
		RD_88F6710_MPP32_39,
		RD_88F6710_MPP40_47,
		RD_88F6710_MPP48_55,
		RD_88F6710_MPP56_63,
		RD_88F6710_MPP64_67,
	} }
};

MV_SERDES_CFG rd88F6710InfoBoardSerdesConfigValue[] = {
	/* Default - Auto detection */
	{ SRDS_MOD_AUTO,  0x0 },
	/* RD_88F6710 Basic Modules Setup */
	{ SRDS_MOD_PCIE0_LANE0 | SRDS_MOD_PCIE1_LANE1 | SRDS_MOD_SGMII0_LANE2 | SRDS_MOD_SATA1_LANE3, 0 }
};

MV_BOARD_TDM_INFO	rd88F6710Tdm880[]	= { {1}, {2} };
MV_BOARD_TDM_INFO	rd88F6710Tdm792[]	= { {1}, {2}, {3}, {4}, {6}, {7} };
MV_BOARD_TDM_INFO	rd88F6710Tdm3215[]	= { {1} };

MV_BOARD_INFO rd88F6710Info = {
	.boardName				= "RD-88F6710",
	.enableModuleScan 			= MV_FALSE,
	.numBoardMppTypeValue			= ARRSZ(rd88F6710InfoBoardModTypeInfo),
	.pBoardModTypeValue			= rd88F6710InfoBoardModTypeInfo,
	.numBoardMppConfigValue			= ARRSZ(rd88F6710InfoBoardMppConfigValue),
	.pBoardMppConfigValue			= rd88F6710InfoBoardMppConfigValue,
	.pBoardSerdesConfigValue		= rd88F6710InfoBoardSerdesConfigValue,
	.intsGppMaskLow				= BIT31,	/* for Switch link interrupt */
	.intsGppMaskMid				= 0,
	.intsGppMaskHigh			= 0,
	.numBoardDeviceIf			= ARRSZ(rd88F6710InfoBoardDeCsInfo),
	.pDevCsInfo				= rd88F6710InfoBoardDeCsInfo,
	.numBoardTwsiDev			= ARRSZ(rd88F6710InfoBoardTwsiDev),
	.pBoardTwsiDev				= rd88F6710InfoBoardTwsiDev,
	.numBoardMacInfo			= ARRSZ(rd88F6710InfoBoardMacInfo),
	.pBoardMacInfo				= rd88F6710InfoBoardMacInfo,
	.numBoardGppInfo			= ARRSZ(rd88F6710InfoBoardGppInfo),
	.pBoardGppInfo				= rd88F6710InfoBoardGppInfo,
	.activeLedsNumber			= ARRSZ(rd88F6710InfoBoardDebugLedIf),
	.pLedGppPin				= rd88F6710InfoBoardDebugLedIf,
	.ledsPolarity				= 0,

	/* PMU Power */
	.pmuPwrUpPolarity			= 0,
	.pmuPwrUpDelay				= 80000,

	/* GPP values */
	.gppOutEnValLow			= RD_88F6710_GPP_OUT_ENA_LOW,
	.gppOutEnValMid			= RD_88F6710_GPP_OUT_ENA_MID,
	.gppOutEnValHigh		= RD_88F6710_GPP_OUT_ENA_HIGH,
	.gppOutValLow			= RD_88F6710_GPP_OUT_VAL_LOW,
	.gppOutValMid			= RD_88F6710_GPP_OUT_VAL_MID,
	.gppOutValHigh			= RD_88F6710_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow		= RD_88F6710_GPP_POL_LOW,
	.gppPolarityValMid		= RD_88F6710_GPP_POL_MID,
	.gppPolarityValHigh		= RD_88F6710_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = rd88F6710InfoBoardSwitchValue,
	.switchInfoNum = ARRSZ(rd88F6710InfoBoardSwitchValue),

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	** can be connected to board.
	** When modules are scanned, then we select the index of the relevant
	** slic's information array.
	** For RD and Customers boards we only need to initialize a single
	** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	*/
	.numBoardTdmInfo		= {2, 6, 1},
	.pBoardTdmInt2CsInfo		= {rd88F6710Tdm880, rd88F6710Tdm792, rd88F6710Tdm3215},
	.boardTdmInfoIndex		= -1,

	/* NOR init params */
	.norFlashReadParams		= RD_88F6710_BOARD_NOR_READ_PARAMS,
	.norFlashWriteParams	= RD_88F6710_BOARD_NOR_WRITE_PARAMS
};

MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f6710Info,
	&db88f6710pcacInfo,
	&rd88F6710Info,
	&db88f6710Info_rn104,
	&db88f6710Info_s2000,
};
