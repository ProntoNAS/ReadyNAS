print_nadv_DEPS = hci/mucurses/print_nadv.c include/compiler.h \
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

$(BIN)/print_nadv.o : hci/mucurses/print_nadv.c $(MAKEDEPS) $(POST_O_DEPS) $(print_nadv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/print_nadv.o
 
$(BIN)/print_nadv.dbg%.o : hci/mucurses/print_nadv.c $(MAKEDEPS) $(POST_O_DEPS) $(print_nadv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/print_nadv.dbg%.o
 
$(BIN)/print_nadv.c : hci/mucurses/print_nadv.c $(MAKEDEPS) $(POST_O_DEPS) $(print_nadv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/print_nadv.c
 
$(BIN)/print_nadv.s : hci/mucurses/print_nadv.c $(MAKEDEPS) $(POST_O_DEPS) $(print_nadv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/print_nadv.s
 
bin/deps/hci/mucurses/print_nadv.c.d : $(print_nadv_DEPS)
 
TAGS : $(print_nadv_DEPS)

