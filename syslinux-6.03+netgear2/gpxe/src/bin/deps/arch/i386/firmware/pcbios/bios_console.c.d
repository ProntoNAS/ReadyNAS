bios_console_DEPS = arch/i386/firmware/pcbios/bios_console.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/assert.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/console.h include/gpxe/tables.h \
 include/gpxe/ansiesc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/assert.h:

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/console.h:

include/gpxe/tables.h:

include/gpxe/ansiesc.h:

$(BIN)/bios_console.o : arch/i386/firmware/pcbios/bios_console.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bios_console.o
 
$(BIN)/bios_console.dbg%.o : arch/i386/firmware/pcbios/bios_console.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bios_console.dbg%.o
 
$(BIN)/bios_console.c : arch/i386/firmware/pcbios/bios_console.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bios_console.c
 
$(BIN)/bios_console.s : arch/i386/firmware/pcbios/bios_console.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bios_console.s
 
bin/deps/arch/i386/firmware/pcbios/bios_console.c.d : $(bios_console_DEPS)
 
TAGS : $(bios_console_DEPS)

