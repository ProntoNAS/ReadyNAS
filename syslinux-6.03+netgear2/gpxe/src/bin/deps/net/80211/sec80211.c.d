sec80211_DEPS = net/80211/sec80211.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/ieee80211.h include/gpxe/if_ether.h include/endian.h \
 arch/i386/include/bits/endian.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/tables.h include/gpxe/iobuf.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/rc80211.h include/gpxe/sec80211.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

arch/i386/include/bits/endian.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/rc80211.h:

include/gpxe/sec80211.h:

$(BIN)/sec80211.o : net/80211/sec80211.c $(MAKEDEPS) $(POST_O_DEPS) $(sec80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sec80211.o
 
$(BIN)/sec80211.dbg%.o : net/80211/sec80211.c $(MAKEDEPS) $(POST_O_DEPS) $(sec80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sec80211.dbg%.o
 
$(BIN)/sec80211.c : net/80211/sec80211.c $(MAKEDEPS) $(POST_O_DEPS) $(sec80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sec80211.c
 
$(BIN)/sec80211.s : net/80211/sec80211.c $(MAKEDEPS) $(POST_O_DEPS) $(sec80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sec80211.s
 
bin/deps/net/80211/sec80211.c.d : $(sec80211_DEPS)
 
TAGS : $(sec80211_DEPS)

