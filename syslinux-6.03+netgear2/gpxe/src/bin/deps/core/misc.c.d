misc_DEPS = core/misc.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdlib.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/assert.h include/ctype.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/ctype.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

$(BIN)/misc.o : core/misc.c $(MAKEDEPS) $(POST_O_DEPS) $(misc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/misc.o
 
$(BIN)/misc.dbg%.o : core/misc.c $(MAKEDEPS) $(POST_O_DEPS) $(misc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/misc.dbg%.o
 
$(BIN)/misc.c : core/misc.c $(MAKEDEPS) $(POST_O_DEPS) $(misc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/misc.c
 
$(BIN)/misc.s : core/misc.c $(MAKEDEPS) $(POST_O_DEPS) $(misc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/misc.s
 
bin/deps/core/misc.c.d : $(misc_DEPS)
 
TAGS : $(misc_DEPS)

