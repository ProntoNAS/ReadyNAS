usbdisk_DEPS = arch/i386/prefix/usbdisk.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/mbr.S \
 arch/i386/prefix/bootpart.S

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/mbr.S:

arch/i386/prefix/bootpart.S:

$(BIN)/usbdisk.o : arch/i386/prefix/usbdisk.S $(MAKEDEPS) $(POST_O_DEPS) $(usbdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/usbdisk.o
 
$(BIN)/usbdisk.s : arch/i386/prefix/usbdisk.S $(MAKEDEPS) $(POST_O_DEPS) $(usbdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/usbdisk.s
 
bin/deps/arch/i386/prefix/usbdisk.S.d : $(usbdisk_DEPS)
 
TAGS : $(usbdisk_DEPS)

