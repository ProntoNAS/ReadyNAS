config_cmd_DEPS = hci/commands/config_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/gpxe/command.h include/gpxe/tables.h include/gpxe/settings.h \
 include/gpxe/list.h include/assert.h include/gpxe/refcnt.h \
 include/gpxe/settings_ui.h

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

include/gpxe/settings.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/settings_ui.h:

$(BIN)/config_cmd.o : hci/commands/config_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(config_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/config_cmd.o
 
$(BIN)/config_cmd.dbg%.o : hci/commands/config_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(config_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/config_cmd.dbg%.o
 
$(BIN)/config_cmd.c : hci/commands/config_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(config_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/config_cmd.c
 
$(BIN)/config_cmd.s : hci/commands/config_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(config_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/config_cmd.s
 
bin/deps/hci/commands/config_cmd.c.d : $(config_cmd_DEPS)
 
TAGS : $(config_cmd_DEPS)

