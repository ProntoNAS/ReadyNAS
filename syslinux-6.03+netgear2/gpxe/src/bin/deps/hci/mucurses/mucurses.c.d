mucurses_DEPS = hci/mucurses/mucurses.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/console.h \
 include/gpxe/tables.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 hci/mucurses/mucurses.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/console.h:

include/gpxe/tables.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

hci/mucurses/mucurses.h:

$(BIN)/mucurses.o : hci/mucurses/mucurses.c $(MAKEDEPS) $(POST_O_DEPS) $(mucurses_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/mucurses.o
 
$(BIN)/mucurses.dbg%.o : hci/mucurses/mucurses.c $(MAKEDEPS) $(POST_O_DEPS) $(mucurses_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/mucurses.dbg%.o
 
$(BIN)/mucurses.c : hci/mucurses/mucurses.c $(MAKEDEPS) $(POST_O_DEPS) $(mucurses_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/mucurses.c
 
$(BIN)/mucurses.s : hci/mucurses/mucurses.c $(MAKEDEPS) $(POST_O_DEPS) $(mucurses_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/mucurses.s
 
bin/deps/hci/mucurses/mucurses.c.d : $(mucurses_DEPS)
 
TAGS : $(mucurses_DEPS)

