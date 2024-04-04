#
#

include $(TOPDIR)/config.mk

EXTRA_INCLUDES += -I$(TOPDIR)/include/asm/arch/
EXTRA_INCLUDES += -I$(TOPDIR)/board/$(BOARDDIR)/../common/ -I./

CFLAGS += $(EXTRA_INCLUDES)
CPPFLAGS += $(EXTRA_INCLUDES)
AFLAGS += $(EXTRA_INCLUDES)

LIB	=  $(obj)lib$(SOC).o

SOBJS	:= lowlevel_init.o al_init_l2cache.o al_init_non_secure.o al_init_sys_fabric_percpu.o

COBJS-y	:= board.o sys_info.o al_hal_timer.o timer.o al_init_sys_fabric_security.o al_init_sys_fabric.o

COBJS	:= $(COBJS-y)
SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

all:	 $(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
