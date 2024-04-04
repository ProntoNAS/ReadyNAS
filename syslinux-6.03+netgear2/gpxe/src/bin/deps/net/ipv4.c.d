ipv4_DEPS = net/ipv4.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/stdlib.h include/assert.h include/stdio.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/list.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/arp.h include/gpxe/tables.h include/gpxe/if_ether.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/ip.h include/gpxe/retry.h \
 include/gpxe/tcpip.h include/gpxe/dhcp.h include/gpxe/uuid.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/list.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/arp.h:

include/gpxe/tables.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/ip.h:

include/gpxe/retry.h:

include/gpxe/tcpip.h:

include/gpxe/dhcp.h:

include/gpxe/uuid.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/ipv4.o : net/ipv4.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ipv4.o
 
$(BIN)/ipv4.dbg%.o : net/ipv4.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ipv4.dbg%.o
 
$(BIN)/ipv4.c : net/ipv4.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ipv4.c
 
$(BIN)/ipv4.s : net/ipv4.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ipv4.s
 
bin/deps/net/ipv4.c.d : $(ipv4_DEPS)
 
TAGS : $(ipv4_DEPS)

