readline_DEPS = hci/readline.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/stdlib.h \
 include/assert.h include/console.h include/gpxe/tables.h \
 include/gpxe/keys.h include/gpxe/editstring.h \
 include/readline/readline.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/keys.h:

include/gpxe/editstring.h:

include/readline/readline.h:

$(BIN)/readline.o : hci/readline.c $(MAKEDEPS) $(POST_O_DEPS) $(readline_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/readline.o
 
$(BIN)/readline.dbg%.o : hci/readline.c $(MAKEDEPS) $(POST_O_DEPS) $(readline_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/readline.dbg%.o
 
$(BIN)/readline.c : hci/readline.c $(MAKEDEPS) $(POST_O_DEPS) $(readline_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/readline.c
 
$(BIN)/readline.s : hci/readline.c $(MAKEDEPS) $(POST_O_DEPS) $(readline_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/readline.s
 
bin/deps/hci/readline.c.d : $(readline_DEPS)
 
TAGS : $(readline_DEPS)

