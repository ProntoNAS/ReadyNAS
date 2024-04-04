settings_ui_DEPS = hci/tui/settings_ui.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/unistd.h \
 include/stddef.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/string.h arch/x86/include/bits/string.h include/curses.h \
 include/gpxe/keys.h include/console.h include/gpxe/tables.h \
 include/gpxe/settings.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/editbox.h include/gpxe/editstring.h \
 include/gpxe/settings_ui.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/unistd.h:

include/stddef.h:

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

include/string.h:

arch/x86/include/bits/string.h:

include/curses.h:

include/gpxe/keys.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/settings.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/editbox.h:

include/gpxe/editstring.h:

include/gpxe/settings_ui.h:

$(BIN)/settings_ui.o : hci/tui/settings_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/settings_ui.o
 
$(BIN)/settings_ui.dbg%.o : hci/tui/settings_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/settings_ui.dbg%.o
 
$(BIN)/settings_ui.c : hci/tui/settings_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/settings_ui.c
 
$(BIN)/settings_ui.s : hci/tui/settings_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/settings_ui.s
 
bin/deps/hci/tui/settings_ui.c.d : $(settings_ui_DEPS)
 
TAGS : $(settings_ui_DEPS)

