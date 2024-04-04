edging_DEPS = hci/mucurses/edging.c include/compiler.h \
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

$(BIN)/edging.o : hci/mucurses/edging.c $(MAKEDEPS) $(POST_O_DEPS) $(edging_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/edging.o
 
$(BIN)/edging.dbg%.o : hci/mucurses/edging.c $(MAKEDEPS) $(POST_O_DEPS) $(edging_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/edging.dbg%.o
 
$(BIN)/edging.c : hci/mucurses/edging.c $(MAKEDEPS) $(POST_O_DEPS) $(edging_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/edging.c
 
$(BIN)/edging.s : hci/mucurses/edging.c $(MAKEDEPS) $(POST_O_DEPS) $(edging_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/edging.s
 
bin/deps/hci/mucurses/edging.c.d : $(edging_DEPS)
 
TAGS : $(edging_DEPS)

