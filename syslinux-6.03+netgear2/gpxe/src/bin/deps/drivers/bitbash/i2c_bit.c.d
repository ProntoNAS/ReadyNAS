i2c_bit_DEPS = drivers/bitbash/i2c_bit.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/string.h \
 arch/x86/include/bits/string.h include/assert.h include/unistd.h \
 include/stdarg.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/bitbash.h include/gpxe/i2c.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/string.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/unistd.h:

include/stdarg.h:

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

include/gpxe/bitbash.h:

include/gpxe/i2c.h:

$(BIN)/i2c_bit.o : drivers/bitbash/i2c_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(i2c_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/i2c_bit.o
 
$(BIN)/i2c_bit.dbg%.o : drivers/bitbash/i2c_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(i2c_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/i2c_bit.dbg%.o
 
$(BIN)/i2c_bit.c : drivers/bitbash/i2c_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(i2c_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/i2c_bit.c
 
$(BIN)/i2c_bit.s : drivers/bitbash/i2c_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(i2c_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/i2c_bit.s
 
bin/deps/drivers/bitbash/i2c_bit.c.d : $(i2c_bit_DEPS)
 
TAGS : $(i2c_bit_DEPS)

