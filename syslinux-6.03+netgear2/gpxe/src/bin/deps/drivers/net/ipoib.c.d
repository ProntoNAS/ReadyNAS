ipoib_DEPS = drivers/net/ipoib.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/unistd.h include/stddef.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/string.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/errortab.h include/gpxe/tables.h include/gpxe/if_arp.h \
 include/gpxe/iobuf.h include/assert.h include/gpxe/list.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/infiniband.h include/gpxe/device.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/ib_pathrec.h include/gpxe/ib_mcast.h \
 include/gpxe/ipoib.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/unistd.h:

include/stddef.h:

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

include/string.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/errortab.h:

include/gpxe/tables.h:

include/gpxe/if_arp.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_pathrec.h:

include/gpxe/ib_mcast.h:

include/gpxe/ipoib.h:

$(BIN)/ipoib.o : drivers/net/ipoib.c $(MAKEDEPS) $(POST_O_DEPS) $(ipoib_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ipoib.o
 
$(BIN)/ipoib.dbg%.o : drivers/net/ipoib.c $(MAKEDEPS) $(POST_O_DEPS) $(ipoib_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ipoib.dbg%.o
 
$(BIN)/ipoib.c : drivers/net/ipoib.c $(MAKEDEPS) $(POST_O_DEPS) $(ipoib_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ipoib.c
 
$(BIN)/ipoib.s : drivers/net/ipoib.c $(MAKEDEPS) $(POST_O_DEPS) $(ipoib_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ipoib.s
 
bin/deps/drivers/net/ipoib.c.d : $(ipoib_DEPS)
 
TAGS : $(ipoib_DEPS)

