route_cmd_DEPS = hci/commands/route_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/getopt.h \
 include/stddef.h include/gpxe/command.h include/gpxe/tables.h \
 include/usr/route.h

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

include/usr/route.h:

$(BIN)/route_cmd.o : hci/commands/route_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(route_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/route_cmd.o
 
$(BIN)/route_cmd.dbg%.o : hci/commands/route_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(route_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/route_cmd.dbg%.o
 
$(BIN)/route_cmd.c : hci/commands/route_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(route_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/route_cmd.c
 
$(BIN)/route_cmd.s : hci/commands/route_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(route_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/route_cmd.s
 
bin/deps/hci/commands/route_cmd.c.d : $(route_cmd_DEPS)
 
TAGS : $(route_cmd_DEPS)

