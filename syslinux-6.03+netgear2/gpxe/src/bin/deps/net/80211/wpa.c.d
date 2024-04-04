wpa_DEPS = net/80211/wpa.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/ieee80211.h \
 include/gpxe/if_ether.h include/endian.h arch/i386/include/bits/endian.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/rc80211.h include/gpxe/sec80211.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/wpa.h include/gpxe/eapol.h include/gpxe/crypto.h \
 include/gpxe/arc4.h include/gpxe/crc32.h include/gpxe/sha1.h \
 crypto/axtls/crypto.h crypto/axtls/bigint.h crypto/axtls/os_port.h \
 include/stdlib.h include/time.h include/sys/time.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h crypto/axtls/bigint_impl.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/hmac.h \
 include/gpxe/ethernet.h

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

include/gpxe/wpa.h:

include/gpxe/eapol.h:

include/gpxe/crypto.h:

include/gpxe/arc4.h:

include/gpxe/crc32.h:

include/gpxe/sha1.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdlib.h:

include/time.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/hmac.h:

include/gpxe/ethernet.h:

$(BIN)/wpa.o : net/80211/wpa.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wpa.o
 
$(BIN)/wpa.dbg%.o : net/80211/wpa.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wpa.dbg%.o
 
$(BIN)/wpa.c : net/80211/wpa.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wpa.c
 
$(BIN)/wpa.s : net/80211/wpa.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wpa.s
 
bin/deps/net/80211/wpa.c.d : $(wpa_DEPS)
 
TAGS : $(wpa_DEPS)

