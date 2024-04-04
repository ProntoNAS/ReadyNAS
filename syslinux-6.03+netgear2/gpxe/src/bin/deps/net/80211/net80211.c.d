net80211_DEPS = net/80211/net80211.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/stdlib.h include/assert.h \
 include/gpxe/settings.h include/gpxe/tables.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/if_arp.h include/gpxe/ethernet.h \
 include/gpxe/ieee80211.h include/gpxe/if_ether.h include/endian.h \
 include/gpxe/netdevice.h include/gpxe/net80211.h include/gpxe/process.h \
 include/gpxe/iobuf.h include/gpxe/rc80211.h include/gpxe/sec80211.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/nap.h config/nap.h include/gpxe/null_nap.h \
 arch/i386/include/bits/nap.h arch/i386/include/gpxe/bios_nap.h \
 arch/x86/include/gpxe/efi/efix86_nap.h include/unistd.h include/stdarg.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/stdlib.h:

include/assert.h:

include/gpxe/settings.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/if_arp.h:

include/gpxe/ethernet.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

include/gpxe/netdevice.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/iobuf.h:

include/gpxe/rc80211.h:

include/gpxe/sec80211.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/nap.h:

config/nap.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

include/unistd.h:

include/stdarg.h:

$(BIN)/net80211.o : net/80211/net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/net80211.o
 
$(BIN)/net80211.dbg%.o : net/80211/net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/net80211.dbg%.o
 
$(BIN)/net80211.c : net/80211/net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/net80211.c
 
$(BIN)/net80211.s : net/80211/net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/net80211.s
 
bin/deps/net/80211/net80211.c.d : $(net80211_DEPS)
 
TAGS : $(net80211_DEPS)

