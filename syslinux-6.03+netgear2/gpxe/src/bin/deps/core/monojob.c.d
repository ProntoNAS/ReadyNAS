monojob_DEPS = core/monojob.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/process.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/console.h \
 include/gpxe/keys.h include/gpxe/job.h include/gpxe/interface.h \
 include/gpxe/monojob.h include/gpxe/timer.h include/gpxe/api.h \
 config/timer.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/console.h:

include/gpxe/keys.h:

include/gpxe/job.h:

include/gpxe/interface.h:

include/gpxe/monojob.h:

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

$(BIN)/monojob.o : core/monojob.c $(MAKEDEPS) $(POST_O_DEPS) $(monojob_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/monojob.o
 
$(BIN)/monojob.dbg%.o : core/monojob.c $(MAKEDEPS) $(POST_O_DEPS) $(monojob_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/monojob.dbg%.o
 
$(BIN)/monojob.c : core/monojob.c $(MAKEDEPS) $(POST_O_DEPS) $(monojob_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/monojob.c
 
$(BIN)/monojob.s : core/monojob.c $(MAKEDEPS) $(POST_O_DEPS) $(monojob_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/monojob.s
 
bin/deps/core/monojob.c.d : $(monojob_DEPS)
 
TAGS : $(monojob_DEPS)

