nvo_cmd_DEPS = hci/commands/nvo_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/getopt.h \
 include/gpxe/settings.h include/gpxe/tables.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/command.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/getopt.h:

include/gpxe/settings.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/command.h:

$(BIN)/nvo_cmd.o : hci/commands/nvo_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(nvo_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/nvo_cmd.o
 
$(BIN)/nvo_cmd.dbg%.o : hci/commands/nvo_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(nvo_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/nvo_cmd.dbg%.o
 
$(BIN)/nvo_cmd.c : hci/commands/nvo_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(nvo_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/nvo_cmd.c
 
$(BIN)/nvo_cmd.s : hci/commands/nvo_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(nvo_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/nvo_cmd.s
 
bin/deps/hci/commands/nvo_cmd.c.d : $(nvo_cmd_DEPS)
 
TAGS : $(nvo_cmd_DEPS)

