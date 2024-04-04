int13_DEPS = arch/i386/interface/pcbios/int13.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/limits.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h include/assert.h \
 include/gpxe/list.h include/stddef.h include/gpxe/blockdev.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/memmap.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h arch/i386/include/bios.h \
 arch/i386/include/biosint.h arch/i386/include/bootsector.h \
 arch/i386/include/int13.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/limits.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/memmap.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

arch/i386/include/bios.h:

arch/i386/include/biosint.h:

arch/i386/include/bootsector.h:

arch/i386/include/int13.h:

$(BIN)/int13.o : arch/i386/interface/pcbios/int13.c $(MAKEDEPS) $(POST_O_DEPS) $(int13_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/int13.o
 
$(BIN)/int13.dbg%.o : arch/i386/interface/pcbios/int13.c $(MAKEDEPS) $(POST_O_DEPS) $(int13_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/int13.dbg%.o
 
$(BIN)/int13.c : arch/i386/interface/pcbios/int13.c $(MAKEDEPS) $(POST_O_DEPS) $(int13_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/int13.c
 
$(BIN)/int13.s : arch/i386/interface/pcbios/int13.c $(MAKEDEPS) $(POST_O_DEPS) $(int13_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/int13.s
 
bin/deps/arch/i386/interface/pcbios/int13.c.d : $(int13_DEPS)
 
TAGS : $(int13_DEPS)

