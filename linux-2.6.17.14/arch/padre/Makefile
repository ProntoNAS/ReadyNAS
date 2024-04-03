#
# padre/Makefile
#
# Makefile for the architecture dependent flags and dependencies on the
# PADRE.
#
# Copyright (C) 2006 Bin Zhang (zhangbin2000@gmail.com)
#

#
# Uncomment the first CFLAGS if you are doing kgdb source level
# debugging of the kernel to get the proper debugging information.

CROSS_COMPILE	= sparc-linux-
AS              := $(AS) -32
LDFLAGS		:= -m elf32_sparc
# CHECK		:= $(CHECK) -D__sparc__=1
CHECKFLAGS      += -D__sparc__

#CFLAGS := $(CFLAGS) -g -m32 -pipe -mno-fpu -fcall-used-g5 -fcall-used-g7 
CFLAGS := $(CFLAGS) -m32 -pipe -mno-fpu -fcall-used-g5 -fcall-used-g7
AFLAGS := $(AFLAGS) -m32

#  Actual linking is done with "make image".
LDFLAGS_vmlinux = -r

head-y := arch/padre/kernel/head.o arch/padre/kernel/init_task.o
HEAD_Y := $(head-y)

core-y += arch/padre/kernel/ arch/padre/mm/ arch/padre/math-emu/
libs-y += arch/padre/lib/
drivers-y += arch/padre/drivers/
drivers-$(CONFIG_OPROFILE)              += arch/padre/oprofile/


# Export what is needed by arch/padre/boot/Makefile
# Renaming is done to avoid confusing pattern matching rules in 2.5.45 (multy-)
INIT_Y		:= $(patsubst %/, %/built-in.o, $(init-y))
CORE_Y		:= $(core-y)
CORE_Y		+= kernel/ mm/ fs/ ipc/ security/ crypto/ block/
CORE_Y		:= $(patsubst %/, %/built-in.o, $(CORE_Y))
DRIVERS_Y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
NET_Y		:= $(patsubst %/, %/built-in.o, $(net-y))
LIBS_Y1		:= $(patsubst %/, %/lib.a, $(libs-y))
LIBS_Y2		:= $(patsubst %/, %/built-in.o, $(libs-y))
LIBS_Y		:= $(LIBS_Y1) $(LIBS_Y2)

ifdef CONFIG_KALLSYMS
kallsyms.o := .tmp_kallsyms2.o
endif

export INIT_Y CORE_Y DRIVERS_Y NET_Y LIBS_Y HEAD_Y kallsyms.o

# Default target
all: image

boot := arch/padre/boot

image: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

prepare: include/asm-$(ARCH)/asm_offsets.h

arch/$(ARCH)/kernel/asm-offsets.s: include/asm include/linux/version.h \
	include/config/MARKER

include/asm-$(ARCH)/asm_offsets.h: arch/$(ARCH)/kernel/asm-offsets.s
	$(call filechk,gen-asm-offsets)

CLEAN_FILES +=  include/asm-$(ARCH)/asm_offsets.h   \
		arch/$(ARCH)/kernel/asm-offsets.s   \
		arch/$(ARCH)/boot/System.map

# Don't use tabs in echo arguments.
define archhelp
  echo  '* image        - kernel image ($(boot)/image)'
endef
