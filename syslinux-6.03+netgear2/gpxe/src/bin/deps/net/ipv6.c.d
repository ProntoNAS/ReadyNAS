ipv6_DEPS = net/ipv6.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/ip6.h \
 include/gpxe/ndp.h include/gpxe/icmp6.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/tcpip.h \
 include/gpxe/if_ether.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/ip6.h:

include/gpxe/ndp.h:

include/gpxe/icmp6.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/tcpip.h:

include/gpxe/if_ether.h:

$(BIN)/ipv6.o : net/ipv6.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ipv6.o
 
$(BIN)/ipv6.dbg%.o : net/ipv6.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ipv6.dbg%.o
 
$(BIN)/ipv6.c : net/ipv6.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ipv6.c
 
$(BIN)/ipv6.s : net/ipv6.c $(MAKEDEPS) $(POST_O_DEPS) $(ipv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ipv6.s
 
bin/deps/net/ipv6.c.d : $(ipv6_DEPS)
 
TAGS : $(ipv6_DEPS)

