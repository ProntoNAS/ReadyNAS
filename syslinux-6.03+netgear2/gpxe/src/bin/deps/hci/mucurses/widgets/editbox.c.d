editbox_DEPS = hci/mucurses/widgets/editbox.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/assert.h include/gpxe/editbox.h \
 include/curses.h include/stdarg.h include/gpxe/keys.h \
 include/gpxe/editstring.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/gpxe/editbox.h:

include/curses.h:

include/stdarg.h:

include/gpxe/keys.h:

include/gpxe/editstring.h:

$(BIN)/editbox.o : hci/mucurses/widgets/editbox.c $(MAKEDEPS) $(POST_O_DEPS) $(editbox_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/editbox.o
 
$(BIN)/editbox.dbg%.o : hci/mucurses/widgets/editbox.c $(MAKEDEPS) $(POST_O_DEPS) $(editbox_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/editbox.dbg%.o
 
$(BIN)/editbox.c : hci/mucurses/widgets/editbox.c $(MAKEDEPS) $(POST_O_DEPS) $(editbox_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/editbox.c
 
$(BIN)/editbox.s : hci/mucurses/widgets/editbox.c $(MAKEDEPS) $(POST_O_DEPS) $(editbox_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/editbox.s
 
bin/deps/hci/mucurses/widgets/editbox.c.d : $(editbox_DEPS)
 
TAGS : $(editbox_DEPS)

