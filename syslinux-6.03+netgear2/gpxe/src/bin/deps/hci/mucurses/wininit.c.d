wininit_DEPS = hci/mucurses/wininit.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/curses.h include/stdarg.h \
 include/gpxe/keys.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/curses.h:

include/stdarg.h:

include/gpxe/keys.h:

$(BIN)/wininit.o : hci/mucurses/wininit.c $(MAKEDEPS) $(POST_O_DEPS) $(wininit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wininit.o
 
$(BIN)/wininit.dbg%.o : hci/mucurses/wininit.c $(MAKEDEPS) $(POST_O_DEPS) $(wininit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wininit.dbg%.o
 
$(BIN)/wininit.c : hci/mucurses/wininit.c $(MAKEDEPS) $(POST_O_DEPS) $(wininit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wininit.c
 
$(BIN)/wininit.s : hci/mucurses/wininit.c $(MAKEDEPS) $(POST_O_DEPS) $(wininit_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wininit.s
 
bin/deps/hci/mucurses/wininit.c.d : $(wininit_DEPS)
 
TAGS : $(wininit_DEPS)

