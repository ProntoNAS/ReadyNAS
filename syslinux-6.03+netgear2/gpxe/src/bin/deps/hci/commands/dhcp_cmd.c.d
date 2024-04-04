dhcp_cmd_DEPS = hci/commands/dhcp_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/stdlib.h \
 include/assert.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/stddef.h include/string.h \
 arch/x86/include/bits/string.h include/getopt.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/command.h include/usr/dhcpmgmt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/stddef.h:

include/string.h:

arch/x86/include/bits/string.h:

include/getopt.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/command.h:

include/usr/dhcpmgmt.h:

$(BIN)/dhcp_cmd.o : hci/commands/dhcp_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dhcp_cmd.o
 
$(BIN)/dhcp_cmd.dbg%.o : hci/commands/dhcp_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dhcp_cmd.dbg%.o
 
$(BIN)/dhcp_cmd.c : hci/commands/dhcp_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dhcp_cmd.c
 
$(BIN)/dhcp_cmd.s : hci/commands/dhcp_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dhcp_cmd.s
 
bin/deps/hci/commands/dhcp_cmd.c.d : $(dhcp_cmd_DEPS)
 
TAGS : $(dhcp_cmd_DEPS)

