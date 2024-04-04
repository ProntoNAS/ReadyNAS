shell_DEPS = hci/shell.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h \
 include/readline/readline.h include/gpxe/command.h include/gpxe/tables.h \
 include/gpxe/shell.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/readline/readline.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/gpxe/shell.h:

$(BIN)/shell.o : hci/shell.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/shell.o
 
$(BIN)/shell.dbg%.o : hci/shell.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/shell.dbg%.o
 
$(BIN)/shell.c : hci/shell.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/shell.c
 
$(BIN)/shell.s : hci/shell.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/shell.s
 
bin/deps/hci/shell.c.d : $(shell_DEPS)
 
TAGS : $(shell_DEPS)

