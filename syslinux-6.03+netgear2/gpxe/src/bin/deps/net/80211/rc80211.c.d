rc80211_DEPS = net/80211/rc80211.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/stddef.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/ieee80211.h \
 include/gpxe/if_ether.h include/endian.h arch/i386/include/bits/endian.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/rc80211.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

arch/i386/include/bits/endian.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/rc80211.h:

$(BIN)/rc80211.o : net/80211/rc80211.c $(MAKEDEPS) $(POST_O_DEPS) $(rc80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rc80211.o
 
$(BIN)/rc80211.dbg%.o : net/80211/rc80211.c $(MAKEDEPS) $(POST_O_DEPS) $(rc80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rc80211.dbg%.o
 
$(BIN)/rc80211.c : net/80211/rc80211.c $(MAKEDEPS) $(POST_O_DEPS) $(rc80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rc80211.c
 
$(BIN)/rc80211.s : net/80211/rc80211.c $(MAKEDEPS) $(POST_O_DEPS) $(rc80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rc80211.s
 
bin/deps/net/80211/rc80211.c.d : $(rc80211_DEPS)
 
TAGS : $(rc80211_DEPS)

