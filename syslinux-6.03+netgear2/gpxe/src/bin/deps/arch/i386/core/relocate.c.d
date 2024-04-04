relocate_DEPS = arch/i386/core/relocate.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/io.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 arch/i386/include/registers.h include/gpxe/memmap.h

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

arch/i386/include/registers.h:

include/gpxe/memmap.h:

$(BIN)/relocate.o : arch/i386/core/relocate.c $(MAKEDEPS) $(POST_O_DEPS) $(relocate_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/relocate.o
 
$(BIN)/relocate.dbg%.o : arch/i386/core/relocate.c $(MAKEDEPS) $(POST_O_DEPS) $(relocate_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/relocate.dbg%.o
 
$(BIN)/relocate.c : arch/i386/core/relocate.c $(MAKEDEPS) $(POST_O_DEPS) $(relocate_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/relocate.c
 
$(BIN)/relocate.s : arch/i386/core/relocate.c $(MAKEDEPS) $(POST_O_DEPS) $(relocate_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/relocate.s
 
bin/deps/arch/i386/core/relocate.c.d : $(relocate_DEPS)
 
TAGS : $(relocate_DEPS)

