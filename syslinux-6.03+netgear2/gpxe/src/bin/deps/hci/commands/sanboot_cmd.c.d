sanboot_cmd_DEPS = hci/commands/sanboot_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/getopt.h \
 include/gpxe/command.h include/gpxe/tables.h include/usr/autoboot.h \
 include/gpxe/in.h include/gpxe/socket.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/getopt.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/usr/autoboot.h:

include/gpxe/in.h:

include/gpxe/socket.h:

$(BIN)/sanboot_cmd.o : hci/commands/sanboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(sanboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sanboot_cmd.o
 
$(BIN)/sanboot_cmd.dbg%.o : hci/commands/sanboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(sanboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sanboot_cmd.dbg%.o
 
$(BIN)/sanboot_cmd.c : hci/commands/sanboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(sanboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sanboot_cmd.c
 
$(BIN)/sanboot_cmd.s : hci/commands/sanboot_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(sanboot_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sanboot_cmd.s
 
bin/deps/hci/commands/sanboot_cmd.c.d : $(sanboot_cmd_DEPS)
 
TAGS : $(sanboot_cmd_DEPS)

