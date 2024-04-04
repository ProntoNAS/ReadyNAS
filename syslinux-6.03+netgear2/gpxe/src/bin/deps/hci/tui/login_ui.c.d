login_ui_DEPS = hci/tui/login_ui.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/curses.h include/stdarg.h \
 include/gpxe/keys.h include/console.h include/gpxe/tables.h \
 include/gpxe/settings.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/editbox.h include/gpxe/editstring.h \
 include/gpxe/login_ui.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/curses.h:

include/stdarg.h:

include/gpxe/keys.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/settings.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/editbox.h:

include/gpxe/editstring.h:

include/gpxe/login_ui.h:

$(BIN)/login_ui.o : hci/tui/login_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(login_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/login_ui.o
 
$(BIN)/login_ui.dbg%.o : hci/tui/login_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(login_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/login_ui.dbg%.o
 
$(BIN)/login_ui.c : hci/tui/login_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(login_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/login_ui.c
 
$(BIN)/login_ui.s : hci/tui/login_ui.c $(MAKEDEPS) $(POST_O_DEPS) $(login_ui_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/login_ui.s
 
bin/deps/hci/tui/login_ui.c.d : $(login_ui_DEPS)
 
TAGS : $(login_ui_DEPS)

