slk_DEPS = hci/mucurses/slk.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 include/stddef.h include/stdlib.h include/assert.h include/string.h \
 arch/x86/include/bits/string.h hci/mucurses/mucurses.h \
 hci/mucurses/cursor.h

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

include/string.h:

arch/x86/include/bits/string.h:

hci/mucurses/mucurses.h:

hci/mucurses/cursor.h:

$(BIN)/slk.o : hci/mucurses/slk.c $(MAKEDEPS) $(POST_O_DEPS) $(slk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/slk.o
 
$(BIN)/slk.dbg%.o : hci/mucurses/slk.c $(MAKEDEPS) $(POST_O_DEPS) $(slk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/slk.dbg%.o
 
$(BIN)/slk.c : hci/mucurses/slk.c $(MAKEDEPS) $(POST_O_DEPS) $(slk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/slk.c
 
$(BIN)/slk.s : hci/mucurses/slk.c $(MAKEDEPS) $(POST_O_DEPS) $(slk_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/slk.s
 
bin/deps/hci/mucurses/slk.c.d : $(slk_DEPS)
 
TAGS : $(slk_DEPS)

