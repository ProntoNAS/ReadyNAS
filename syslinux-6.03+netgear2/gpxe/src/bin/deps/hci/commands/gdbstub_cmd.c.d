gdbstub_cmd_DEPS = hci/commands/gdbstub_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/getopt.h \
 include/stddef.h include/gpxe/command.h include/gpxe/tables.h \
 include/gpxe/gdbstub.h arch/i386/include/gdbmach.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/getopt.h:

include/stddef.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/gpxe/gdbstub.h:

arch/i386/include/gdbmach.h:

$(BIN)/gdbstub_cmd.o : hci/commands/gdbstub_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/gdbstub_cmd.o
 
$(BIN)/gdbstub_cmd.dbg%.o : hci/commands/gdbstub_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/gdbstub_cmd.dbg%.o
 
$(BIN)/gdbstub_cmd.c : hci/commands/gdbstub_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/gdbstub_cmd.c
 
$(BIN)/gdbstub_cmd.s : hci/commands/gdbstub_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/gdbstub_cmd.s
 
bin/deps/hci/commands/gdbstub_cmd.c.d : $(gdbstub_cmd_DEPS)
 
TAGS : $(gdbstub_cmd_DEPS)

