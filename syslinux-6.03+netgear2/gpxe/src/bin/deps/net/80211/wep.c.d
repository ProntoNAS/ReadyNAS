wep_DEPS = net/80211/wep.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/ieee80211.h \
 include/gpxe/if_ether.h include/endian.h arch/i386/include/bits/endian.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/rc80211.h include/gpxe/sec80211.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/crypto.h include/gpxe/arc4.h include/gpxe/crc32.h \
 include/stdlib.h include/string.h arch/x86/include/bits/string.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

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

include/gpxe/sec80211.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/crypto.h:

include/gpxe/arc4.h:

include/gpxe/crc32.h:

include/stdlib.h:

include/string.h:

arch/x86/include/bits/string.h:

$(BIN)/wep.o : net/80211/wep.c $(MAKEDEPS) $(POST_O_DEPS) $(wep_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wep.o
 
$(BIN)/wep.dbg%.o : net/80211/wep.c $(MAKEDEPS) $(POST_O_DEPS) $(wep_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wep.dbg%.o
 
$(BIN)/wep.c : net/80211/wep.c $(MAKEDEPS) $(POST_O_DEPS) $(wep_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wep.c
 
$(BIN)/wep.s : net/80211/wep.c $(MAKEDEPS) $(POST_O_DEPS) $(wep_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wep.s
 
bin/deps/net/80211/wep.c.d : $(wep_DEPS)
 
TAGS : $(wep_DEPS)

