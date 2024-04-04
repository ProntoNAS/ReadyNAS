colour_DEPS = hci/mucurses/colour.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

$(BIN)/colour.o : hci/mucurses/colour.c $(MAKEDEPS) $(POST_O_DEPS) $(colour_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/colour.o
 
$(BIN)/colour.dbg%.o : hci/mucurses/colour.c $(MAKEDEPS) $(POST_O_DEPS) $(colour_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/colour.dbg%.o
 
$(BIN)/colour.c : hci/mucurses/colour.c $(MAKEDEPS) $(POST_O_DEPS) $(colour_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/colour.c
 
$(BIN)/colour.s : hci/mucurses/colour.c $(MAKEDEPS) $(POST_O_DEPS) $(colour_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/colour.s
 
bin/deps/hci/mucurses/colour.c.d : $(colour_DEPS)
 
TAGS : $(colour_DEPS)

