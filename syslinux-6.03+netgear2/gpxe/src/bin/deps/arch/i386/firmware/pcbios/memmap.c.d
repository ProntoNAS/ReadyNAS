memmap_DEPS = arch/i386/firmware/pcbios/memmap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/bios.h \
 arch/i386/include/memsizes.h arch/i386/include/basemem.h \
 include/gpxe/memmap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

arch/i386/include/memsizes.h:

arch/i386/include/basemem.h:

include/gpxe/memmap.h:

$(BIN)/memmap.o : arch/i386/firmware/pcbios/memmap.c $(MAKEDEPS) $(POST_O_DEPS) $(memmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/memmap.o
 
$(BIN)/memmap.dbg%.o : arch/i386/firmware/pcbios/memmap.c $(MAKEDEPS) $(POST_O_DEPS) $(memmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/memmap.dbg%.o
 
$(BIN)/memmap.c : arch/i386/firmware/pcbios/memmap.c $(MAKEDEPS) $(POST_O_DEPS) $(memmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/memmap.c
 
$(BIN)/memmap.s : arch/i386/firmware/pcbios/memmap.c $(MAKEDEPS) $(POST_O_DEPS) $(memmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/memmap.s
 
bin/deps/arch/i386/firmware/pcbios/memmap.c.d : $(memmap_DEPS)
 
TAGS : $(memmap_DEPS)

