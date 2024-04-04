kb_DEPS = hci/mucurses/kb.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 include/stddef.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 hci/mucurses/mucurses.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

include/stddef.h:

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

hci/mucurses/mucurses.h:

$(BIN)/kb.o : hci/mucurses/kb.c $(MAKEDEPS) $(POST_O_DEPS) $(kb_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/kb.o
 
$(BIN)/kb.dbg%.o : hci/mucurses/kb.c $(MAKEDEPS) $(POST_O_DEPS) $(kb_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/kb.dbg%.o
 
$(BIN)/kb.c : hci/mucurses/kb.c $(MAKEDEPS) $(POST_O_DEPS) $(kb_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/kb.c
 
$(BIN)/kb.s : hci/mucurses/kb.c $(MAKEDEPS) $(POST_O_DEPS) $(kb_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/kb.s
 
bin/deps/hci/mucurses/kb.c.d : $(kb_DEPS)
 
TAGS : $(kb_DEPS)

