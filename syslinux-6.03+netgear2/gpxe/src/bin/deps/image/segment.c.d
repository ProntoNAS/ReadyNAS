segment_DEPS = image/segment.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/uaccess.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/memmap.h include/gpxe/errortab.h \
 include/gpxe/tables.h include/gpxe/segment.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/gpxe/memmap.h:

include/gpxe/errortab.h:

include/gpxe/tables.h:

include/gpxe/segment.h:

$(BIN)/segment.o : image/segment.c $(MAKEDEPS) $(POST_O_DEPS) $(segment_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/segment.o
 
$(BIN)/segment.dbg%.o : image/segment.c $(MAKEDEPS) $(POST_O_DEPS) $(segment_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/segment.dbg%.o
 
$(BIN)/segment.c : image/segment.c $(MAKEDEPS) $(POST_O_DEPS) $(segment_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/segment.c
 
$(BIN)/segment.s : image/segment.c $(MAKEDEPS) $(POST_O_DEPS) $(segment_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/segment.s
 
bin/deps/image/segment.c.d : $(segment_DEPS)
 
TAGS : $(segment_DEPS)

