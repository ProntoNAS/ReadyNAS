ansi_screen_DEPS = hci/mucurses/ansi_screen.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/curses.h \
 include/gpxe/keys.h include/console.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/curses.h:

include/gpxe/keys.h:

include/console.h:

include/gpxe/tables.h:

$(BIN)/ansi_screen.o : hci/mucurses/ansi_screen.c $(MAKEDEPS) $(POST_O_DEPS) $(ansi_screen_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ansi_screen.o
 
$(BIN)/ansi_screen.dbg%.o : hci/mucurses/ansi_screen.c $(MAKEDEPS) $(POST_O_DEPS) $(ansi_screen_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ansi_screen.dbg%.o
 
$(BIN)/ansi_screen.c : hci/mucurses/ansi_screen.c $(MAKEDEPS) $(POST_O_DEPS) $(ansi_screen_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ansi_screen.c
 
$(BIN)/ansi_screen.s : hci/mucurses/ansi_screen.c $(MAKEDEPS) $(POST_O_DEPS) $(ansi_screen_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ansi_screen.s
 
bin/deps/hci/mucurses/ansi_screen.c.d : $(ansi_screen_DEPS)
 
TAGS : $(ansi_screen_DEPS)

