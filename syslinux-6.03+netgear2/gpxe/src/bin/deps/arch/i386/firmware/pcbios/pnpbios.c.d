pnpbios_DEPS = arch/i386/firmware/pcbios/pnpbios.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/pnpbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

arch/i386/include/pnpbios.h:

$(BIN)/pnpbios.o : arch/i386/firmware/pcbios/pnpbios.c $(MAKEDEPS) $(POST_O_DEPS) $(pnpbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pnpbios.o
 
$(BIN)/pnpbios.dbg%.o : arch/i386/firmware/pcbios/pnpbios.c $(MAKEDEPS) $(POST_O_DEPS) $(pnpbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pnpbios.dbg%.o
 
$(BIN)/pnpbios.c : arch/i386/firmware/pcbios/pnpbios.c $(MAKEDEPS) $(POST_O_DEPS) $(pnpbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pnpbios.c
 
$(BIN)/pnpbios.s : arch/i386/firmware/pcbios/pnpbios.c $(MAKEDEPS) $(POST_O_DEPS) $(pnpbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pnpbios.s
 
bin/deps/arch/i386/firmware/pcbios/pnpbios.c.d : $(pnpbios_DEPS)
 
TAGS : $(pnpbios_DEPS)

