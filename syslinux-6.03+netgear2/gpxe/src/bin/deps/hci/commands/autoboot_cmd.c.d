autoboot_cmd_DEPS = hci/commands/autoboot_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/command.h \
 include/gpxe/tables.h include/usr/autoboot.h include/gpxe/in.h \
 include/gpxe/socket.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/usr/autoboot.h:

include/gpxe/in.h:

include/gpxe/socket.h:

$(BIN)/autoboot_cmd.o : hci/commands/autoboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/autoboot_cmd.o
 
$(BIN)/autoboot_cmd.dbg%.o : hci/commands/autoboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/autoboot_cmd.dbg%.o
 
$(BIN)/autoboot_cmd.c : hci/commands/autoboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/autoboot_cmd.c
 
$(BIN)/autoboot_cmd.s : hci/commands/autoboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/autoboot_cmd.s
 
bin/deps/hci/commands/autoboot_cmd.c.d : $(autoboot_cmd_DEPS)
 
TAGS : $(autoboot_cmd_DEPS)

