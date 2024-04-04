exec_DEPS = core/exec.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/ctype.h \
 include/unistd.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/getopt.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/tables.h \
 include/gpxe/command.h include/gpxe/settings.h include/gpxe/list.h \
 include/gpxe/refcnt.h

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

include/ctype.h:

include/unistd.h:

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

include/getopt.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/tables.h:

include/gpxe/command.h:

include/gpxe/settings.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

$(BIN)/exec.o : core/exec.c $(MAKEDEPS) $(POST_O_DEPS) $(exec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/exec.o
 
$(BIN)/exec.dbg%.o : core/exec.c $(MAKEDEPS) $(POST_O_DEPS) $(exec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/exec.dbg%.o
 
$(BIN)/exec.c : core/exec.c $(MAKEDEPS) $(POST_O_DEPS) $(exec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/exec.c
 
$(BIN)/exec.s : core/exec.c $(MAKEDEPS) $(POST_O_DEPS) $(exec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/exec.s
 
bin/deps/core/exec.c.d : $(exec_DEPS)
 
TAGS : $(exec_DEPS)

