video_subr_DEPS = arch/i386/core/video_subr.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/console.h include/gpxe/tables.h \
 include/gpxe/init.h arch/i386/include/vga.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/init.h:

arch/i386/include/vga.h:

$(BIN)/video_subr.o : arch/i386/core/video_subr.c $(MAKEDEPS) $(POST_O_DEPS) $(video_subr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/video_subr.o
 
$(BIN)/video_subr.dbg%.o : arch/i386/core/video_subr.c $(MAKEDEPS) $(POST_O_DEPS) $(video_subr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/video_subr.dbg%.o
 
$(BIN)/video_subr.c : arch/i386/core/video_subr.c $(MAKEDEPS) $(POST_O_DEPS) $(video_subr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/video_subr.c
 
$(BIN)/video_subr.s : arch/i386/core/video_subr.c $(MAKEDEPS) $(POST_O_DEPS) $(video_subr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/video_subr.s
 
bin/deps/arch/i386/core/video_subr.c.d : $(video_subr_DEPS)
 
TAGS : $(video_subr_DEPS)

