iwmgmt_DEPS = usr/iwmgmt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/console.h \
 include/gpxe/tables.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/ieee80211.h include/gpxe/if_ether.h \
 include/endian.h arch/i386/include/bits/endian.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/rc80211.h \
 include/gpxe/ethernet.h include/usr/ifmgmt.h include/usr/iwmgmt.h \
 include/gpxe/errortab.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/console.h:

include/gpxe/tables.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

arch/i386/include/bits/endian.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/rc80211.h:

include/gpxe/ethernet.h:

include/usr/ifmgmt.h:

include/usr/iwmgmt.h:

include/gpxe/errortab.h:

$(BIN)/iwmgmt.o : usr/iwmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iwmgmt.o
 
$(BIN)/iwmgmt.dbg%.o : usr/iwmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iwmgmt.dbg%.o
 
$(BIN)/iwmgmt.c : usr/iwmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iwmgmt.c
 
$(BIN)/iwmgmt.s : usr/iwmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(iwmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iwmgmt.s
 
bin/deps/usr/iwmgmt.c.d : $(iwmgmt_DEPS)
 
TAGS : $(iwmgmt_DEPS)

