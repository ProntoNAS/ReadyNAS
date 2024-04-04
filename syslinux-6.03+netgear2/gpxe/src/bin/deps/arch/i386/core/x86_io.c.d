x86_io_DEPS = arch/i386/core/x86_io.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/io.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/io.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

$(BIN)/x86_io.o : arch/i386/core/x86_io.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/x86_io.o
 
$(BIN)/x86_io.dbg%.o : arch/i386/core/x86_io.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/x86_io.dbg%.o
 
$(BIN)/x86_io.c : arch/i386/core/x86_io.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/x86_io.c
 
$(BIN)/x86_io.s : arch/i386/core/x86_io.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/x86_io.s
 
bin/deps/arch/i386/core/x86_io.c.d : $(x86_io_DEPS)
 
TAGS : $(x86_io_DEPS)

