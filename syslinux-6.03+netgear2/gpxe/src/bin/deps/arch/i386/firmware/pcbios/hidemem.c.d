hidemem_DEPS = arch/i386/firmware/pcbios/hidemem.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/assert.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/biosint.h \
 arch/i386/include/basemem.h arch/i386/include/bios.h \
 arch/i386/include/fakee820.h include/gpxe/init.h include/gpxe/tables.h \
 include/gpxe/memmap.h include/gpxe/hidemem.h

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

arch/i386/include/biosint.h:

arch/i386/include/basemem.h:

arch/i386/include/bios.h:

arch/i386/include/fakee820.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/memmap.h:

include/gpxe/hidemem.h:

$(BIN)/hidemem.o : arch/i386/firmware/pcbios/hidemem.c $(MAKEDEPS) $(POST_O_DEPS) $(hidemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/hidemem.o
 
$(BIN)/hidemem.dbg%.o : arch/i386/firmware/pcbios/hidemem.c $(MAKEDEPS) $(POST_O_DEPS) $(hidemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/hidemem.dbg%.o
 
$(BIN)/hidemem.c : arch/i386/firmware/pcbios/hidemem.c $(MAKEDEPS) $(POST_O_DEPS) $(hidemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/hidemem.c
 
$(BIN)/hidemem.s : arch/i386/firmware/pcbios/hidemem.c $(MAKEDEPS) $(POST_O_DEPS) $(hidemem_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/hidemem.s
 
bin/deps/arch/i386/firmware/pcbios/hidemem.c.d : $(hidemem_DEPS)
 
TAGS : $(hidemem_DEPS)

