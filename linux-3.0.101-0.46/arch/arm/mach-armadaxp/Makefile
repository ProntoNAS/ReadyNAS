#*******************************************************************************
# Marvell GPL License Option
#
# If you received this File from Marvell, you may opt to use, redistribute and/or
# modify this File in accordance with the terms and conditions of the General
# Public License Version 2, June 1991 (the "GPL License"), a copy of which is
# available along with the File in the license.txt file or by writing to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
# on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
#
# THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
# WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
# DISCLAIMED.  The GPL License provides additional details about this warranty
# disclaimer.
#*******************************************************************************/
include 	  $(srctree)/arch/arm/mach-armadaxp/config/mvRules.mk

# Objects list
COMMON_OBJS	= $(COMMON_DIR)/mvDebug.o $(COMMON_DIR)/mvCommon.o $(COMMON_DIR)/mvStack.o $(COMMON_DIR)/mvList.o

OSSERVICES_OBJS	= $(OSSERV_DIR)/mvOs.o

HAL_OBJS	= $(HAL_RTC_DIR)/mvRtc.o					\
                  $(HAL_CNTMR_DIR)/mvCntmr.o					\
		  $(HAL_TWSI_DIR)/mvTwsi.o $(HAL_IF_DIR)/mvSysTwsi.o		\
                  $(HAL_UART_DIR)/mvUart.o $(HAL_GPP_DIR)/mvGpp.o               \
                  $(HAL_DRAM_DIR)/mvDramIf.o					\
		  $(HAL_IF_DIR)/mvSysDdr.o
#		  $(HAL_DRAM_SPD_DIR)/mvSpd.o

KW_FAM_OBJS	= $(BOARD_ENV_DIR)/mvBoardEnvSpec.o $(SOC_ENV_DIR)/mvCtrlEnvLib.o	\
		  $(BOARD_ENV_DIR)/mvBoardEnvLib.o $(SOC_ENV_DIR)/mvCtrlEnvAddrDec.o 	\
		  $(SOC_SYS_DIR)/mvAhbToMbus.o $(SOC_SYS_DIR)/mvCpuIf.o 		\
		  $(SOC_CPU_DIR)/mvCpu.o $(SOC_DEVICE_DIR)/mvDevice.o

QD_OBJS		= $(HAL_QD_DIR)/src/driver/gtDrvConfig.o $(HAL_QD_DIR)/src/driver/gtDrvEvents.o \
                  $(HAL_QD_DIR)/src/driver/gtHwCntl.o $(HAL_QD_DIR)/src/platform/gtMiiSmiIf.o	\
                  $(HAL_QD_DIR)/src/platform/platformDeps.o $(HAL_QD_DIR)/src/platform/gtSem.o	\
                  $(HAL_QD_DIR)/src/platform/gtDebug.o $(HAL_QD_DIR)/src/msapi/gtBrgFdb.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtBrgStp.o $(HAL_QD_DIR)/src/msapi/gtBrgVlan.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtEvents.o $(HAL_QD_DIR)/src/msapi/gtPortCtrl.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtPortStat.o $(HAL_QD_DIR)/src/msapi/gtPortStatus.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtQosMap.o $(HAL_QD_DIR)/src/msapi/gtPIRL.o  		\
                  $(HAL_QD_DIR)/src/msapi/gtPhyCtrl.o $(HAL_QD_DIR)/src/msapi/gtPhyInt.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtSysConfig.o $(HAL_QD_DIR)/src/msapi/gtSysCtrl.o	\
                  $(HAL_QD_DIR)/src/msapi/gtVersion.o $(HAL_QD_DIR)/src/msapi/gtUtils.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtBrgVtu.o $(HAL_QD_DIR)/src/msapi/gtPortRmon.o 	\
                  $(HAL_QD_DIR)/src/msapi/gtSysStatus.o $(HAL_QD_DIR)/src/msapi/gtPortRateCtrl.o\
                  $(HAL_QD_DIR)/src/msapi/gtPortPav.o $(HAL_QD_DIR)/src/msapi/gtVct.o		\
		  $(HAL_QD_DIR)/src/msapi/gtPIRL2.o $(HAL_QD_DIR)/src/msapi/gtCCPVT.o		\
		  $(HAL_QD_DIR)/src/msapi/gtPCSCtrl.o

LSP_OBJS        = core.o irq.o time.o leds.o sysmap.o export.o clock.o

obj-y   				:=  armadaxp.o
armadaxp-objs  				:=$(LSP_OBJS) $(COMMON_OBJS) $(OSSERVICES_OBJS) $(HAL_OBJS) 	\
					  $(KW_FAM_OBJS)

armadaxp-$(CONFIG_MV_INCLUDE_SDIO) 	+= $(HAL_SDMMC_DIR)/mvSdmmcAddrDec.o
armadaxp-$(CONFIG_MV_INCLUDE_XOR) 	+= $(HAL_XOR_DIR)/mvXor.o $(HAL_XOR_DIR)/mvXorAddrDec.o		\
					   $(HAL_IF_DIR)/mvSysXor.o
armadaxp-$(CONFIG_MV_INCLUDE_PEX) 	+= $(HAL_PEX_DIR)/mvPex.o					\
					   $(HAL_IF_DIR)/mvSysPex.o $(HAL_PEX_DIR)/mvPexAddrDec.o
armadaxp-$(CONFIG_MV_INCLUDE_PCI) 	+= $(HAL_PCI_DIR)/mvPci.o $(HAL_IF_DIR)/mvSysPci.o
armadaxp-$(CONFIG_MV_INCLUDE_USB) 	+= $(HAL_USB_DIR)/mvUsb.o $(HAL_USB_DIR)/mvUsbAddrDec.o		\
					   $(HAL_IF_DIR)/mvSysUsb.o
armadaxp-y				+= $(HAL_ETHPHY_DIR)/mvEthPhy.o $(HAL_IF_DIR)/mvSysEthPhy.o

# Legacy Giga driver
ifeq ($(CONFIG_MV_ETH_LEGACY),y)
armadaxp-$(CONFIG_MV_ETH_LEGACY)	+= $(HAL_ETH_GBE_DIR)/mvEth.o $(HAL_ETH_GBE_DIR)/mvEthDebug.o \
					   $(HAL_ETH_GBE_DIR)/mvEthAddrDec.o $(HAL_IF_DIR)/mvSysEth.o
armadaxp-$(CONFIG_MV_ETH_NFP)           += $(HAL_ETH_NFP_DIR)/mvNfp.o
armadaxp-$(CONFIG_MV_ETH_NFP_NAT) 	+= $(HAL_ETH_NFP_DIR)/mvNfpNat.o
armadaxp-$(CONFIG_MV_ETH_NFP_FDB) 	+= $(HAL_ETH_NFP_DIR)/mvNfpFdb.o
armadaxp-$(CONFIG_MV_ETH_NFP_PPP) 	+= $(HAL_ETH_NFP_DIR)/mvNfpPpp.o
armadaxp-$(CONFIG_MV_ETH_NFP_SEC)	+= $(HAL_ETH_NFP_DIR)/mvNfpSec.o

endif

# NETA Giga driver
ifeq ($(CONFIG_MV_ETH_NETA),y)
armadaxp-$(CONFIG_MV_ETH_NETA)	        += $(HAL_ETH_GBE_DIR)/mvNeta.o $(HAL_ETH_GBE_DIR)/mvNetaDebug.o \
					   $(HAL_ETH_GBE_DIR)/mvNetaAddrDec.o $(HAL_IF_DIR)/mvSysNeta.o
armadaxp-$(CONFIG_MV_ETH_PNC)    	+= $(HAL_ETH_PNC_DIR)/mvTcam.o
armadaxp-$(CONFIG_MV_ETH_PNC_PARSER)	+= $(HAL_ETH_PNC_DIR)/mvPnc.o
armadaxp-$(CONFIG_MV_ETH_PNC_WOL)       += $(HAL_ETH_PNC_DIR)/mvPncWol.o
armadaxp-$(CONFIG_MV_ETH_BM) 	        += $(HAL_ETH_BM_DIR)/mvBm.o
armadaxp-$(CONFIG_MV_ETH_PMT)	        += $(HAL_ETH_PMT_DIR)/mvPmt.o
armadaxp-$(CONFIG_MV_ETH_HWF)           += $(HAL_ETH_GBE_DIR)/mvHwf.o
armadaxp-$(CONFIG_MV_ETH_NFP)		+= $(HAL_ETH_NFP_DIR)/mvNfp.o
armadaxp-$(CONFIG_MV_ETH_NFP_NAT) 	+= $(HAL_ETH_NFP_DIR)/mvNfpNat.o
armadaxp-$(CONFIG_MV_ETH_NFP_SWF) 	+= $(HAL_ETH_NFP_DIR)/mvNfpSwf.o
armadaxp-$(CONFIG_MV_ETH_NFP_FIB) 	+= $(HAL_ETH_NFP_DIR)/mvNfpFib.o
armadaxp-$(CONFIG_MV_ETH_NFP_PNC) 	+= $(HAL_ETH_NFP_DIR)/mvNfpPnc.o
endif

armadaxp-$(CONFIG_MV_INCLUDE_CESA) 	+= $(HAL_CESA_DIR)/mvCesa.o $(HAL_CESA_DIR)/mvCesaIf.o         	\
					   $(HAL_CESA_DIR)/mvCesaDebug.o 				\
					   $(HAL_CESA_DIR)/mvCesaAddrDec.o				\
					   $(HAL_CESA_DIR)/mvMD5.o $(HAL_CESA_DIR)/mvSHA1.o 		\
					   $(HAL_CESA_AES_DIR)/mvAesAlg.o $(HAL_CESA_AES_DIR)/mvAesApi.o\
					   $(HAL_IF_DIR)/mvSysCesa.o
armadaxp-$(CONFIG_MV_CESA_TEST) 	+= $(HAL_CESA_DIR)/mvCesaTest.o
armadaxp-$(CONFIG_MV_INCLUDE_INTEG_SATA)+= $(HAL_IF_DIR)/mvSysSata.o $(HAL_SATA_DIR)/mvSataSoc.o	\
					   $(HAL_SATA_DIR)/mvSataAddrDec.o
armadaxp-$(CONFIG_MV_INCLUDE_SPI) 	+= $(HAL_SPI_DIR)/mvSpi.o $(HAL_SPI_DIR)/mvSpiCmnd.o 		\
					   $(HAL_SFLASH_DIR)/mvSFlash.o $(HAL_IF_DIR)/mvSysSFlash.o	\
					   $(HAL_IF_DIR)/mvSysSpi.o
armadaxp-$(CONFIG_MV_INCLUDE_NFC)	+= $(HAL_NFC_DIR)/mvNfc.o
armadaxp-$(CONFIG_MV_INCLUDE_AUDIO) 	+= $(HAL_AUDIO_DIR)/mvAudio.o $(HAL_IF_DIR)/mvSysAudio.o	\
					   $(HAL_AUDIO_DIR)/mvAudioAddrDec.o
armadaxp-$(CONFIG_MV_INCLUDE_TS) 	+= $(HAL_TS_DIR)/mvTsu.o $(HAL_IF_DIR)/mvSysTs.o		\
					   $(HAL_TS_DIR)/mvTsuAddrDec.o
armadaxp-$(CONFIG_MV_CPU_PERF_CNTRS)    += $(HAL_CPU_DIR)/mvCpuCntrs.o $(HAL_CPU_DIR)/pj4/mvPJ4Cntrs.o
armadaxp-$(CONFIG_PCIE_VIRTUAL_BRIDGE_SUPPORT) += $(HAL_PEX_DIR)/mvVrtBrgPex.o
armadaxp-$(CONFIG_MV_CPU_L2_PERF_CNTRS) += $(HAL_CPU_DIR)/mvCpuL2Cntrs.o

obj-$(CONFIG_MV_INCLUDE_SWITCH) 	+= $(QD_OBJS)

# drivers part
# Legacy Giga driver
ifeq ($(CONFIG_MV_ETH_LEGACY),y)
obj-$(CONFIG_MV_ETH_NFP) 	        += $(LSP_NFP_MGR_DIR)/mv_nfp_mgr.o
obj-$(CONFIG_MV_ETH_NFP_SEC)            += $(LSP_NFP_MGR_DIR)/mv_nfp_sec.o
endif

ifeq ($(CONFIG_MV_ETH_NETA),y)
obj-$(CONFIG_MV_ETH_NFP)	        += $(LSP_NFP_MGR_DIR)/mv_nfp_mgr.o $(LSP_NFP_MGR_DIR)/nfp_sysfs.o
obj-$(CONFIG_MV_ETH_PNC)                += $(LSP_PNC_DIR)/pnc_sysfs.o
obj-$(CONFIG_MV_ETH_PNC_WOL)            += $(LSP_PNC_DIR)/wol_sysfs.o
obj-$(CONFIG_MV_ETH_PMT)		+= $(LSP_PMT_DIR)/pmt_sysfs.o
obj-$(CONFIG_MV_ETH_HWF)        	+= $(LSP_HWF_DIR)/hwf_sysfs.o
obj-$(CONFIG_MV_ETH_L2FW)               += $(LSP_L2FW_DIR)/l2fw_sysfs.o $(LSP_L2FW_DIR)/mv_eth_l2fw.o
endif

obj-$(CONFIG_MV_USE_XOR_ENGINE) 	+= $(PLAT_DRIVERS)/mv_xor/
obj-$(CONFIG_MV_CESA) 			+= $(PLAT_DRIVERS)/mv_cesa/
#obj-y					+= $(PLAT_DRIVERS)/mv_btns/
obj-y					+= $(PLAT_DRIVERS)/mv_gpio/
obj-$(CONFIG_MV_INCLUDE_SWITCH)         += $(LSP_SWITCH_DIR)/
obj-$(CONFIG_SENSORS_ARMADA_XP)	+= hwmon.o
# The rest of the drivers are compiled through the driver dir directly.


# LSP part
armadaxp-$(CONFIG_MV_INCLUDE_USB)    	+= usb.o
armadaxp-$(CONFIG_MV_INCLUDE_PCI)	+= pci.o
armadaxp-$(CONFIG_MV_INCLUDE_PEX)	+= pex.o
armadaxp-$(CONFIG_FEROCEON_PROC) 	+= $(PLAT_DRIVERS)/mv_proc/proc.o
armadaxp-$(CONFIG_MV_DBG_TRACE) 	+= dbg-trace.o
armadaxp-$(CONFIG_SMP)			+= platsmp.o headsmp.o
armadaxp-$(CONFIG_PROC_FS)		+= dump_cp15_regs.o
obj-$(CONFIG_FB_DOVE_CLCD)		+= clcd.o
obj-$(CONFIG_PM)			+= pm.o
