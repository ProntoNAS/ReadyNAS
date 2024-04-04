#
#

include $(TOPDIR)/config.mk

COMMON = ../common

EXTRA_CFLAGS += -I$(TOPDIR)/include/asm/arch/
EXTRA_CFLAGS += -I$(TOPDIR)/board/$(BOARDDIR)/$(COMMON)/

CFLAGS += $(EXTRA_CFLAGS)
CPPFLAGS += $(EXTRA_CFLAGS)

LIB	= $(obj)lib$(BOARD).o

COBJS-y	:= board.o
COBJS-y += lcd.o
COBJS-y += cmd_lcd.o
COBJS-y += pwr_state.o
COBJS-y += $(COMMON)/al_flash_contents.o
COBJS-y += $(COMMON)/cmd_flash_contents.o
COBJS-y += $(COMMON)/eeprom_per_device.o
COBJS-y += $(COMMON)/cmd_eeprom_per_device.o
COBJS-y += $(COMMON)/cmd_cpu_set_speed.o
COBJS-y += $(COMMON)/iodma.o
COBJS-y += $(COMMON)/al_hal_thermal_sensor.o
COBJS-y += $(COMMON)/cmd_thermal.o
COBJS-y += $(COMMON)/shared_params.o
COBJS-y += $(COMMON)/pll_init.o
COBJS-y += $(COMMON)/gpio_board_init.o
COBJS-y += $(COMMON)/al_hal_ddr.o
COBJS-y += $(COMMON)/al_hal_udma_config.o
COBJS-y += $(COMMON)/al_hal_udma_main.o
COBJS-y += $(COMMON)/al_hal_m2m_udma.o
COBJS-y += $(COMMON)/al_hal_udma_iofic.o
COBJS-y += $(COMMON)/al_hal_iofic.o
COBJS-y += $(COMMON)/al_board.o
COBJS-y += $(COMMON)/al_globals.o
COBJS-y += $(COMMON)/cmd_scripth.o
COBJS-y += $(COMMON)/al_hal_bootstrap.o
COBJS-y += $(COMMON)/cmd_dev_info.o
COBJS-y += $(COMMON)/al_hal_pll.o
COBJS-y += $(COMMON)/al_hal_muio_mux.o
COBJS-y += $(COMMON)/cmd_muio_mux.o
COBJS-y += $(COMMON)/al_hal_serdes.o
COBJS-y += $(COMMON)/cmd_serdes.o
COBJS-y += $(COMMON)/al_hal_ssm.o
COBJS-y += $(COMMON)/al_hal_ssm_raid.o
COBJS-y += $(COMMON)/al_hal_nand.o
COBJS-y += $(COMMON)/cmd_iodma.o
COBJS-y += $(COMMON)/cmd_wfi.o
COBJS-y += $(COMMON)/cmd_ddr.o
COBJS-y += $(COMMON)/env_dyn.o
COBJS-y += $(COMMON)/cmd_confirm_msg.o
COBJS-y += $(COMMON)/cmd_eth.o
COBJS-y += $(COMMON)/cmd_dram_margins.o
COBJS-$(CONFIG_AL_NAND_FLASH) += $(COMMON)/al_nand.o
COBJS-$(CONFIG_AL_NAND_SIMULATE) += $(COMMON)/nand_simulation.o
COBJS-$(CONFIG_PCI) += $(COMMON)/pci.o $(COMMON)/pci_skip.o $(COMMON)/al_hal_pcie.o
COBJS-$(CONFIG_CMD_AL_PCI_EXT) += $(COMMON)/cmd_al_pcie_external.o
COBJS-$(CONFIG_AL_ETH) += $(COMMON)/al_hal_eth_main.o
COBJS-$(CONFIG_AL_SPI) += $(COMMON)/al_hal_spi.o

SRCS    := $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS    := $(addprefix $(obj),$(SOBJS-y) $(COBJS-y))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
