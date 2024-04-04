fakee820_DEPS = arch/i386/firmware/pcbios/fakee820.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/realmode.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/biosint.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

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

arch/i386/include/biosint.h:

$(BIN)/fakee820.o : arch/i386/firmware/pcbios/fakee820.c $(MAKEDEPS) $(POST_O_DEPS) $(fakee820_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/fakee820.o
 
$(BIN)/fakee820.dbg%.o : arch/i386/firmware/pcbios/fakee820.c $(MAKEDEPS) $(POST_O_DEPS) $(fakee820_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/fakee820.dbg%.o
 
$(BIN)/fakee820.c : arch/i386/firmware/pcbios/fakee820.c $(MAKEDEPS) $(POST_O_DEPS) $(fakee820_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/fakee820.c
 
$(BIN)/fakee820.s : arch/i386/firmware/pcbios/fakee820.c $(MAKEDEPS) $(POST_O_DEPS) $(fakee820_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/fakee820.s
 
bin/deps/arch/i386/firmware/pcbios/fakee820.c.d : $(fakee820_DEPS)
 
TAGS : $(fakee820_DEPS)

