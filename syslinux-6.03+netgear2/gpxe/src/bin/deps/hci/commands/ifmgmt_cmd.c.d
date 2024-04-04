ifmgmt_cmd_DEPS = hci/commands/ifmgmt_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/getopt.h \
 include/stddef.h include/gpxe/netdevice.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/command.h include/usr/ifmgmt.h \
 include/hci/ifmgmt_cmd.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/getopt.h:

include/stddef.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/command.h:

include/usr/ifmgmt.h:

include/hci/ifmgmt_cmd.h:

$(BIN)/ifmgmt_cmd.o : hci/commands/ifmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ifmgmt_cmd.o
 
$(BIN)/ifmgmt_cmd.dbg%.o : hci/commands/ifmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ifmgmt_cmd.dbg%.o
 
$(BIN)/ifmgmt_cmd.c : hci/commands/ifmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ifmgmt_cmd.c
 
$(BIN)/ifmgmt_cmd.s : hci/commands/ifmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ifmgmt_cmd.s
 
bin/deps/hci/commands/ifmgmt_cmd.c.d : $(ifmgmt_cmd_DEPS)
 
TAGS : $(ifmgmt_cmd_DEPS)

