dhcp_DEPS = net/udp/dhcp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/ctype.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/if_ether.h include/gpxe/netdevice.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/device.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/iobuf.h include/gpxe/open.h include/gpxe/socket.h \
 include/gpxe/job.h include/gpxe/retry.h include/gpxe/tcpip.h \
 include/gpxe/in.h include/gpxe/ip.h include/gpxe/uuid.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/dhcp.h include/gpxe/uaccess.h config/ioapi.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/dhcpopts.h include/gpxe/dhcppkt.h \
 include/gpxe/features.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/ctype.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/if_ether.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/device.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/open.h:

include/gpxe/socket.h:

include/gpxe/job.h:

include/gpxe/retry.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/ip.h:

include/gpxe/uuid.h:

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

include/gpxe/dhcp.h:

include/gpxe/uaccess.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/dhcpopts.h:

include/gpxe/dhcppkt.h:

include/gpxe/features.h:

$(BIN)/dhcp.o : net/udp/dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dhcp.o
 
$(BIN)/dhcp.dbg%.o : net/udp/dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dhcp.dbg%.o
 
$(BIN)/dhcp.c : net/udp/dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dhcp.c
 
$(BIN)/dhcp.s : net/udp/dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dhcp.s
 
bin/deps/net/udp/dhcp.c.d : $(dhcp_DEPS)
 
TAGS : $(dhcp_DEPS)

