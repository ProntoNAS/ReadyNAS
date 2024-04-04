spi_DEPS = drivers/nvs/spi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/unistd.h include/stdarg.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/spi.h include/gpxe/nvs.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/gpxe/spi.h:

include/gpxe/nvs.h:

$(BIN)/spi.o : drivers/nvs/spi.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/spi.o
 
$(BIN)/spi.dbg%.o : drivers/nvs/spi.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/spi.dbg%.o
 
$(BIN)/spi.c : drivers/nvs/spi.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/spi.c
 
$(BIN)/spi.s : drivers/nvs/spi.c $(MAKEDEPS) $(POST_O_DEPS) $(spi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/spi.s
 
bin/deps/drivers/nvs/spi.c.d : $(spi_DEPS)
 
TAGS : $(spi_DEPS)

