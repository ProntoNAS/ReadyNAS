windows_DEPS = hci/mucurses/windows.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 include/stddef.h include/stdlib.h include/assert.h \
 hci/mucurses/mucurses.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

include/stddef.h:

include/stdlib.h:

include/assert.h:

hci/mucurses/mucurses.h:

$(BIN)/windows.o : hci/mucurses/windows.c $(MAKEDEPS) $(POST_O_DEPS) $(windows_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/windows.o
 
$(BIN)/windows.dbg%.o : hci/mucurses/windows.c $(MAKEDEPS) $(POST_O_DEPS) $(windows_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/windows.dbg%.o
 
$(BIN)/windows.c : hci/mucurses/windows.c $(MAKEDEPS) $(POST_O_DEPS) $(windows_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/windows.c
 
$(BIN)/windows.s : hci/mucurses/windows.c $(MAKEDEPS) $(POST_O_DEPS) $(windows_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/windows.s
 
bin/deps/hci/mucurses/windows.c.d : $(windows_DEPS)
 
TAGS : $(windows_DEPS)

