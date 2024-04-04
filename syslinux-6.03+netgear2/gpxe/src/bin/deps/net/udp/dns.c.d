dns_DEPS = net/udp/dns.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/refcnt.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/open.h \
 include/gpxe/tables.h include/gpxe/socket.h include/gpxe/resolv.h \
 include/gpxe/retry.h include/gpxe/tcpip.h include/gpxe/in.h \
 include/gpxe/settings.h include/gpxe/features.h include/gpxe/dhcp.h \
 include/gpxe/uuid.h include/gpxe/netdevice.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/dns.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/refcnt.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/resolv.h:

include/gpxe/retry.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/settings.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/dns.h:

$(BIN)/dns.o : net/udp/dns.c $(MAKEDEPS) $(POST_O_DEPS) $(dns_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dns.o
 
$(BIN)/dns.dbg%.o : net/udp/dns.c $(MAKEDEPS) $(POST_O_DEPS) $(dns_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dns.dbg%.o
 
$(BIN)/dns.c : net/udp/dns.c $(MAKEDEPS) $(POST_O_DEPS) $(dns_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dns.c
 
$(BIN)/dns.s : net/udp/dns.c $(MAKEDEPS) $(POST_O_DEPS) $(dns_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dns.s
 
bin/deps/net/udp/dns.c.d : $(dns_DEPS)
 
TAGS : $(dns_DEPS)

