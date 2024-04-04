spi_bit_DEPS = drivers/bitbash/spi_bit.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h include/unistd.h \
 include/stdarg.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/bitbash.h include/gpxe/spi_bit.h include/gpxe/spi.h \
 include/gpxe/nvs.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/gpxe/spi_bit.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

$(BIN)/spi_bit.o : drivers/bitbash/spi_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/spi_bit.o
 
$(BIN)/spi_bit.dbg%.o : drivers/bitbash/spi_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/spi_bit.dbg%.o
 
$(BIN)/spi_bit.c : drivers/bitbash/spi_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/spi_bit.c
 
$(BIN)/spi_bit.s : drivers/bitbash/spi_bit.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_bit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/spi_bit.s
 
bin/deps/drivers/bitbash/spi_bit.c.d : $(spi_bit_DEPS)
 
TAGS : $(spi_bit_DEPS)

