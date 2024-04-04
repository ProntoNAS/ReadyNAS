threewire_DEPS = drivers/nvs/threewire.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/assert.h include/unistd.h \
 include/stdarg.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/threewire.h include/gpxe/spi.h include/gpxe/nvs.h \
 arch/i386/include/limits.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/gpxe/threewire.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

arch/i386/include/limits.h:

$(BIN)/threewire.o : drivers/nvs/threewire.c $(MAKEDEPS) $(POST_O_DEPS) $(threewire_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/threewire.o
 
$(BIN)/threewire.dbg%.o : drivers/nvs/threewire.c $(MAKEDEPS) $(POST_O_DEPS) $(threewire_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/threewire.dbg%.o
 
$(BIN)/threewire.c : drivers/nvs/threewire.c $(MAKEDEPS) $(POST_O_DEPS) $(threewire_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/threewire.c
 
$(BIN)/threewire.s : drivers/nvs/threewire.c $(MAKEDEPS) $(POST_O_DEPS) $(threewire_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/threewire.s
 
bin/deps/drivers/nvs/threewire.c.d : $(threewire_DEPS)
 
TAGS : $(threewire_DEPS)

