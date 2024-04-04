print_DEPS = hci/mucurses/print.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 include/stdio.h include/stddef.h include/gpxe/vsprintf.h \
 hci/mucurses/mucurses.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

include/stdio.h:

include/stddef.h:

include/gpxe/vsprintf.h:

hci/mucurses/mucurses.h:

$(BIN)/print.o : hci/mucurses/print.c $(MAKEDEPS) $(POST_O_DEPS) $(print_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/print.o
 
$(BIN)/print.dbg%.o : hci/mucurses/print.c $(MAKEDEPS) $(POST_O_DEPS) $(print_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/print.dbg%.o
 
$(BIN)/print.c : hci/mucurses/print.c $(MAKEDEPS) $(POST_O_DEPS) $(print_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/print.c
 
$(BIN)/print.s : hci/mucurses/print.c $(MAKEDEPS) $(POST_O_DEPS) $(print_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/print.s
 
bin/deps/hci/mucurses/print.c.d : $(print_DEPS)
 
TAGS : $(print_DEPS)

