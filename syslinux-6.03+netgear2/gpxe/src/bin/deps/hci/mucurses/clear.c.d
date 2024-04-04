clear_DEPS = hci/mucurses/clear.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 hci/mucurses/mucurses.h hci/mucurses/cursor.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

hci/mucurses/mucurses.h:

hci/mucurses/cursor.h:

$(BIN)/clear.o : hci/mucurses/clear.c $(MAKEDEPS) $(POST_O_DEPS) $(clear_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/clear.o
 
$(BIN)/clear.dbg%.o : hci/mucurses/clear.c $(MAKEDEPS) $(POST_O_DEPS) $(clear_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/clear.dbg%.o
 
$(BIN)/clear.c : hci/mucurses/clear.c $(MAKEDEPS) $(POST_O_DEPS) $(clear_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/clear.c
 
$(BIN)/clear.s : hci/mucurses/clear.c $(MAKEDEPS) $(POST_O_DEPS) $(clear_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/clear.s
 
bin/deps/hci/mucurses/clear.c.d : $(clear_DEPS)
 
TAGS : $(clear_DEPS)

