wpa_tkip_DEPS = net/80211/wpa_tkip.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/net80211.h \
 include/gpxe/process.h include/gpxe/list.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/ieee80211.h \
 include/gpxe/if_ether.h include/endian.h arch/i386/include/bits/endian.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/rc80211.h include/gpxe/crypto.h include/gpxe/hmac.h \
 include/gpxe/sha1.h crypto/axtls/crypto.h crypto/axtls/bigint.h \
 crypto/axtls/os_port.h include/stdlib.h include/time.h \
 include/sys/time.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/md5.h include/gpxe/crc32.h \
 include/gpxe/arc4.h include/gpxe/wpa.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h

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

include/gpxe/crypto.h:

include/gpxe/hmac.h:

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

include/gpxe/md5.h:

include/gpxe/crc32.h:

include/gpxe/arc4.h:

include/gpxe/wpa.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

$(BIN)/wpa_tkip.o : net/80211/wpa_tkip.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_tkip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wpa_tkip.o
 
$(BIN)/wpa_tkip.dbg%.o : net/80211/wpa_tkip.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_tkip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wpa_tkip.dbg%.o
 
$(BIN)/wpa_tkip.c : net/80211/wpa_tkip.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_tkip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wpa_tkip.c
 
$(BIN)/wpa_tkip.s : net/80211/wpa_tkip.c $(MAKEDEPS) $(POST_O_DEPS) $(wpa_tkip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wpa_tkip.s
 
bin/deps/net/80211/wpa_tkip.c.d : $(wpa_tkip_DEPS)
 
TAGS : $(wpa_tkip_DEPS)

