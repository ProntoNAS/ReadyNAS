# $Id: Makefile,v 1.1.1.1 2006-08-28 17:52:12 bzhang Exp $
# Makefile for the Sparc boot stuff.
#
# Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
# Copyright (C) 1997,1998 Jakub Jelinek (jj@ultra.linux.cz)

targets		:= image vmlinux.bin

BTOBJS := $(HEAD_Y) $(INIT_Y)
BTLIBS := $(CORE_Y) $(LIBS_Y) $(DRIVERS_Y) $(NET_Y)
LDFLAGS_image := -T arch/padre/kernel/vmlinux.lds $(BTOBJS) --start-group $(BTLIBS) --end-group $(kallsyms.o)

# Actual linking
$(obj)/image: $(obj)/tmp.o FORCE
	$(call if_changed,ld)
	$(NM) $@ | grep -v  '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $(obj)/System.map
	$(OBJCOPY) -O binary $@ $(obj)/vmlinux.bin
	rm -f $(obj)/tmp.S $(obj)/tmp.o

$(obj)/tmp.S: vmlinux FORCE
	touch $(obj)/tmp.S

