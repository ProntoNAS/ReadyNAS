time_cmd_DEPS = hci/commands/time_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/stdlib.h \
 include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/command.h include/gpxe/tables.h include/gpxe/nap.h \
 config/nap.h include/gpxe/null_nap.h arch/i386/include/bits/nap.h \
 arch/i386/include/gpxe/bios_nap.h arch/x86/include/gpxe/efi/efix86_nap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

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

include/gpxe/command.h:

include/gpxe/tables.h:

include/gpxe/nap.h:

config/nap.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

$(BIN)/time_cmd.o : hci/commands/time_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(time_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/time_cmd.o
 
$(BIN)/time_cmd.dbg%.o : hci/commands/time_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(time_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/time_cmd.dbg%.o
 
$(BIN)/time_cmd.c : hci/commands/time_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(time_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/time_cmd.c
 
$(BIN)/time_cmd.s : hci/commands/time_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(time_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/time_cmd.s
 
bin/deps/hci/commands/time_cmd.c.d : $(time_cmd_DEPS)
 
TAGS : $(time_cmd_DEPS)

