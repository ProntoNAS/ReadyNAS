login_cmd_DEPS = hci/commands/login_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/gpxe/command.h include/gpxe/tables.h include/gpxe/login_ui.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/gpxe/login_ui.h:

$(BIN)/login_cmd.o : hci/commands/login_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(login_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/login_cmd.o
 
$(BIN)/login_cmd.dbg%.o : hci/commands/login_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(login_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/login_cmd.dbg%.o
 
$(BIN)/login_cmd.c : hci/commands/login_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(login_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/login_cmd.c
 
$(BIN)/login_cmd.s : hci/commands/login_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(login_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/login_cmd.s
 
bin/deps/hci/commands/login_cmd.c.d : $(login_cmd_DEPS)
 
TAGS : $(login_cmd_DEPS)

