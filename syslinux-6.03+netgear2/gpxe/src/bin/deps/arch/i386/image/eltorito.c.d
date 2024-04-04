eltorito_DEPS = arch/i386/image/eltorito.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/bootsector.h \
 arch/i386/include/int13.h include/gpxe/list.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/segment.h \
 include/gpxe/ramdisk.h include/gpxe/blockdev.h include/gpxe/init.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

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

arch/i386/include/bootsector.h:

arch/i386/include/int13.h:

include/gpxe/list.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/segment.h:

include/gpxe/ramdisk.h:

include/gpxe/blockdev.h:

include/gpxe/init.h:

$(BIN)/eltorito.o : arch/i386/image/eltorito.c $(MAKEDEPS) $(POST_O_DEPS) $(eltorito_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/eltorito.o
 
$(BIN)/eltorito.dbg%.o : arch/i386/image/eltorito.c $(MAKEDEPS) $(POST_O_DEPS) $(eltorito_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/eltorito.dbg%.o
 
$(BIN)/eltorito.c : arch/i386/image/eltorito.c $(MAKEDEPS) $(POST_O_DEPS) $(eltorito_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/eltorito.c
 
$(BIN)/eltorito.s : arch/i386/image/eltorito.c $(MAKEDEPS) $(POST_O_DEPS) $(eltorito_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/eltorito.s
 
bin/deps/arch/i386/image/eltorito.c.d : $(eltorito_DEPS)
 
TAGS : $(eltorito_DEPS)

