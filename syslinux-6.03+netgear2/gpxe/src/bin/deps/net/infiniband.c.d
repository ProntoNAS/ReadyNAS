infiniband_DEPS = net/infiniband.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/list.h include/gpxe/errortab.h include/gpxe/tables.h \
 include/gpxe/if_arp.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/ipoib.h \
 include/gpxe/infiniband.h include/gpxe/device.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/process.h include/gpxe/ib_mi.h \
 include/gpxe/retry.h include/gpxe/ib_sma.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/unistd.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/list.h:

include/gpxe/errortab.h:

include/gpxe/tables.h:

include/gpxe/if_arp.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/ipoib.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/process.h:

include/gpxe/ib_mi.h:

include/gpxe/retry.h:

include/gpxe/ib_sma.h:

$(BIN)/infiniband.o : net/infiniband.c $(MAKEDEPS) $(POST_O_DEPS) $(infiniband_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/infiniband.o
 
$(BIN)/infiniband.dbg%.o : net/infiniband.c $(MAKEDEPS) $(POST_O_DEPS) $(infiniband_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/infiniband.dbg%.o
 
$(BIN)/infiniband.c : net/infiniband.c $(MAKEDEPS) $(POST_O_DEPS) $(infiniband_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/infiniband.c
 
$(BIN)/infiniband.s : net/infiniband.c $(MAKEDEPS) $(POST_O_DEPS) $(infiniband_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/infiniband.s
 
bin/deps/net/infiniband.c.d : $(infiniband_DEPS)
 
TAGS : $(infiniband_DEPS)

