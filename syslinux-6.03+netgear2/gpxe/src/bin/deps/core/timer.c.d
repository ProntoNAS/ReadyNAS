timer_DEPS = core/timer.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/unistd.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdarg.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/unistd.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

$(BIN)/timer.o : core/timer.c $(MAKEDEPS) $(POST_O_DEPS) $(timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/timer.o
 
$(BIN)/timer.dbg%.o : core/timer.c $(MAKEDEPS) $(POST_O_DEPS) $(timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/timer.dbg%.o
 
$(BIN)/timer.c : core/timer.c $(MAKEDEPS) $(POST_O_DEPS) $(timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/timer.c
 
$(BIN)/timer.s : core/timer.c $(MAKEDEPS) $(POST_O_DEPS) $(timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/timer.s
 
bin/deps/core/timer.c.d : $(timer_DEPS)
 
TAGS : $(timer_DEPS)

