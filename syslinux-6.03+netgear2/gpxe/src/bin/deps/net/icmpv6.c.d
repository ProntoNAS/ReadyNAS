icmpv6_DEPS = net/icmpv6.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/ip6.h include/gpxe/if_ether.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h include/gpxe/ndp.h \
 include/gpxe/icmp6.h include/gpxe/netdevice.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/tcpip.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/ip6.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/ndp.h:

include/gpxe/icmp6.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/tcpip.h:

$(BIN)/icmpv6.o : net/icmpv6.c $(MAKEDEPS) $(POST_O_DEPS) $(icmpv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/icmpv6.o
 
$(BIN)/icmpv6.dbg%.o : net/icmpv6.c $(MAKEDEPS) $(POST_O_DEPS) $(icmpv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/icmpv6.dbg%.o
 
$(BIN)/icmpv6.c : net/icmpv6.c $(MAKEDEPS) $(POST_O_DEPS) $(icmpv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/icmpv6.c
 
$(BIN)/icmpv6.s : net/icmpv6.c $(MAKEDEPS) $(POST_O_DEPS) $(icmpv6_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/icmpv6.s
 
bin/deps/net/icmpv6.c.d : $(icmpv6_DEPS)
 
TAGS : $(icmpv6_DEPS)

