ramdisk_DEPS = drivers/block/ramdisk.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/blockdev.h \
 include/gpxe/uaccess.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/ramdisk.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/ramdisk.h:

$(BIN)/ramdisk.o : drivers/block/ramdisk.c $(MAKEDEPS) $(POST_O_DEPS) $(ramdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ramdisk.o
 
$(BIN)/ramdisk.dbg%.o : drivers/block/ramdisk.c $(MAKEDEPS) $(POST_O_DEPS) $(ramdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ramdisk.dbg%.o
 
$(BIN)/ramdisk.c : drivers/block/ramdisk.c $(MAKEDEPS) $(POST_O_DEPS) $(ramdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ramdisk.c
 
$(BIN)/ramdisk.s : drivers/block/ramdisk.c $(MAKEDEPS) $(POST_O_DEPS) $(ramdisk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ramdisk.s
 
bin/deps/drivers/block/ramdisk.c.d : $(ramdisk_DEPS)
 
TAGS : $(ramdisk_DEPS)

