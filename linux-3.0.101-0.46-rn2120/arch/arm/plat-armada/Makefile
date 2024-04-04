ifeq ($(CONFIG_ARCH_FEROCEON),y)
	include $(srctree)/$(MACHINE)/config/mvRules.mk
endif

ifeq ($(CONFIG_ARCH_ARMADA_XP),y)
	include $(srctree)/arch/arm/mach-armadaxp/config/mvRules.mk
endif

# This will never compile, because DUMMY will never by defined.
obj-$(DUMMY)   				:= dummy.o

obj-$(CONFIG_SHEEVA_DEEP_IDLE)		+= cpuidle.o armadaxp_suspend.o
obj-$(CONFIG_HOTPLUG_CPU)               += hotplug.o
obj-$(CONFIG_ARCH_ARMADA_XP)		+= pmu.o
obj-$(CONFIG_PCI_MSI)			+= msi.o
obj-$(CONFIG_ERROR_HANDLIG)		+=error_handling.o
