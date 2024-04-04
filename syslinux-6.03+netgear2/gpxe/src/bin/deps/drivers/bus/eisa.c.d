eisa_DEPS = drivers/bus/eisa.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/unistd.h include/gpxe/timer.h \
 config/timer.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/eisa.h include/gpxe/isa_ids.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/device.h include/gpxe/list.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/unistd.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/eisa.h:

include/gpxe/isa_ids.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

$(BIN)/eisa.o : drivers/bus/eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/eisa.o
 
$(BIN)/eisa.dbg%.o : drivers/bus/eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/eisa.dbg%.o
 
$(BIN)/eisa.c : drivers/bus/eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/eisa.c
 
$(BIN)/eisa.s : drivers/bus/eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/eisa.s
 
bin/deps/drivers/bus/eisa.c.d : $(eisa_DEPS)
 
TAGS : $(eisa_DEPS)

