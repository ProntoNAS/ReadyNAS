pic8259_DEPS = arch/i386/core/pic8259.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/io.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 arch/i386/include/pic8259.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h

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

arch/i386/include/pic8259.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/pic8259.o : arch/i386/core/pic8259.c $(MAKEDEPS) $(POST_O_DEPS) $(pic8259_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pic8259.o
 
$(BIN)/pic8259.dbg%.o : arch/i386/core/pic8259.c $(MAKEDEPS) $(POST_O_DEPS) $(pic8259_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pic8259.dbg%.o
 
$(BIN)/pic8259.c : arch/i386/core/pic8259.c $(MAKEDEPS) $(POST_O_DEPS) $(pic8259_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pic8259.c
 
$(BIN)/pic8259.s : arch/i386/core/pic8259.c $(MAKEDEPS) $(POST_O_DEPS) $(pic8259_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pic8259.s
 
bin/deps/arch/i386/core/pic8259.c.d : $(pic8259_DEPS)
 
TAGS : $(pic8259_DEPS)

