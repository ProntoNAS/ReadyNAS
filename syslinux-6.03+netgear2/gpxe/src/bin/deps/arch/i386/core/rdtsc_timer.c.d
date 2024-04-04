rdtsc_timer_DEPS = arch/i386/core/rdtsc_timer.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/assert.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/assert.h:

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

$(BIN)/rdtsc_timer.o : arch/i386/core/rdtsc_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(rdtsc_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rdtsc_timer.o
 
$(BIN)/rdtsc_timer.dbg%.o : arch/i386/core/rdtsc_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(rdtsc_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rdtsc_timer.dbg%.o
 
$(BIN)/rdtsc_timer.c : arch/i386/core/rdtsc_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(rdtsc_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rdtsc_timer.c
 
$(BIN)/rdtsc_timer.s : arch/i386/core/rdtsc_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(rdtsc_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rdtsc_timer.s
 
bin/deps/arch/i386/core/rdtsc_timer.c.d : $(rdtsc_timer_DEPS)
 
TAGS : $(rdtsc_timer_DEPS)

