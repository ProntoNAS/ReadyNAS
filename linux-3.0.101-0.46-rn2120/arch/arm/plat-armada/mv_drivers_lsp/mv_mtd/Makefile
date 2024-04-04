
ifeq ($(CONFIG_ARCH_FEROCEON),y)
	include $(srctree)/$(MACHINE)/config/mvRules.mk
endif

ifeq ($(CONFIG_ARCH_ARMADA_XP),y)
	include $(srctree)/arch/arm/mach-armadaxp/config/mvRules.mk
endif
 
obj-$(CONFIG_MV_INCLUDE_SFLASH_MTD) 	+= sflash.o
obj-$(CONFIG_MV_INCLUDE_MFLASH_MTD) 	+= mflash.o
obj-$(CONFIG_MTD_NAND_LNC)		+= nand_lnc.o
obj-$(CONFIG_MTD_NAND_NFC)		+= nand_nfc.o
