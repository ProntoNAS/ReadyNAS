iwmgmt_cmd_DEPS = hci/commands/iwmgmt_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/netdevice.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/gpxe/list.h \
 include/stddef.h include/assert.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/ieee80211.h include/gpxe/if_ether.h \
 include/endian.h arch/i386/include/bits/endian.h include/gpxe/iobuf.h \
 include/gpxe/rc80211.h include/gpxe/command.h include/usr/iwmgmt.h \
 include/hci/ifmgmt_cmd.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/netdevice.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

arch/i386/include/bits/endian.h:

include/gpxe/iobuf.h:

include/gpxe/rc80211.h:

include/gpxe/command.h:

include/usr/iwmgmt.h:

include/hci/ifmgmt_cmd.h:

$(BIN)/iwmgmt_cmd.o : hci/commands/iwmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iwmgmt_cmd.o
 
$(BIN)/iwmgmt_cmd.dbg%.o : hci/commands/iwmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iwmgmt_cmd.dbg%.o
 
$(BIN)/iwmgmt_cmd.c : hci/commands/iwmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iwmgmt_cmd.c
 
$(BIN)/iwmgmt_cmd.s : hci/commands/iwmgmt_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iwmgmt_cmd.s
 
bin/deps/hci/commands/iwmgmt_cmd.c.d : $(iwmgmt_cmd_DEPS)
 
TAGS : $(iwmgmt_cmd_DEPS)

