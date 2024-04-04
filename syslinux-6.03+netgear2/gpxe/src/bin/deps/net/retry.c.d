retry_DEPS = net/retry.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/list.h include/assert.h include/gpxe/process.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/init.h \
 include/gpxe/retry.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/gpxe/list.h:

include/assert.h:

include/gpxe/process.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/init.h:

include/gpxe/retry.h:

$(BIN)/retry.o : net/retry.c $(MAKEDEPS) $(POST_O_DEPS) $(retry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/retry.o
 
$(BIN)/retry.dbg%.o : net/retry.c $(MAKEDEPS) $(POST_O_DEPS) $(retry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/retry.dbg%.o
 
$(BIN)/retry.c : net/retry.c $(MAKEDEPS) $(POST_O_DEPS) $(retry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/retry.c
 
$(BIN)/retry.s : net/retry.c $(MAKEDEPS) $(POST_O_DEPS) $(retry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/retry.s
 
bin/deps/net/retry.c.d : $(retry_DEPS)
 
TAGS : $(retry_DEPS)

