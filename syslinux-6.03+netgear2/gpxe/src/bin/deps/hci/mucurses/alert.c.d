alert_DEPS = hci/mucurses/alert.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h \
 include/stdio.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

include/stdio.h:

$(BIN)/alert.o : hci/mucurses/alert.c $(MAKEDEPS) $(POST_O_DEPS) $(alert_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/alert.o
 
$(BIN)/alert.dbg%.o : hci/mucurses/alert.c $(MAKEDEPS) $(POST_O_DEPS) $(alert_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/alert.dbg%.o
 
$(BIN)/alert.c : hci/mucurses/alert.c $(MAKEDEPS) $(POST_O_DEPS) $(alert_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/alert.c
 
$(BIN)/alert.s : hci/mucurses/alert.c $(MAKEDEPS) $(POST_O_DEPS) $(alert_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/alert.s
 
bin/deps/hci/mucurses/alert.c.d : $(alert_DEPS)
 
TAGS : $(alert_DEPS)

