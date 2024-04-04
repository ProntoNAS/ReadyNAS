winattrs_DEPS = hci/mucurses/winattrs.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/curses.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/keys.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/curses.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/keys.h:

$(BIN)/winattrs.o : hci/mucurses/winattrs.c $(MAKEDEPS) $(POST_O_DEPS) $(winattrs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/winattrs.o
 
$(BIN)/winattrs.dbg%.o : hci/mucurses/winattrs.c $(MAKEDEPS) $(POST_O_DEPS) $(winattrs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/winattrs.dbg%.o
 
$(BIN)/winattrs.c : hci/mucurses/winattrs.c $(MAKEDEPS) $(POST_O_DEPS) $(winattrs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/winattrs.c
 
$(BIN)/winattrs.s : hci/mucurses/winattrs.c $(MAKEDEPS) $(POST_O_DEPS) $(winattrs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/winattrs.s
 
bin/deps/hci/mucurses/winattrs.c.d : $(winattrs_DEPS)
 
TAGS : $(winattrs_DEPS)

