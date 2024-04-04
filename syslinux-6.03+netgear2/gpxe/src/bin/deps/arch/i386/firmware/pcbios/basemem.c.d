basemem_DEPS = arch/i386/firmware/pcbios/basemem.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/bios.h \
 arch/i386/include/basemem.h include/gpxe/hidemem.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

arch/i386/include/bios.h:

arch/i386/include/basemem.h:

include/gpxe/hidemem.h:

$(BIN)/basemem.o : arch/i386/firmware/pcbios/basemem.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/basemem.o
 
$(BIN)/basemem.dbg%.o : arch/i386/firmware/pcbios/basemem.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/basemem.dbg%.o
 
$(BIN)/basemem.c : arch/i386/firmware/pcbios/basemem.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/basemem.c
 
$(BIN)/basemem.s : arch/i386/firmware/pcbios/basemem.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/basemem.s
 
bin/deps/arch/i386/firmware/pcbios/basemem.c.d : $(basemem_DEPS)
 
TAGS : $(basemem_DEPS)

