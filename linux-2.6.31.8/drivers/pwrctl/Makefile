#
# Makefile for the fan control.
#

#ifeq ($(CONFIG_ARCH_FEROCEON_ORION),y)
#        include $(TOPDIR)/arch/arm/mach-feroceon-orion/config/mvRules.mk
#endif
 
ifeq ($(CONFIG_ARCH_FEROCEON_KW),y)
        include $(srctree)/arch/arm/mach-feroceon-kw/config/mvRules.mk
endif

obj-$(CONFIG_USI_POWER_CTL)		+= pwrctl.o
obj-$(CONFIG_USI_POWER_CTL)		+= usiHDDpwrctl.o
obj-$(CONFIG_USI_POWER_CTL)		+= usi_button.o
