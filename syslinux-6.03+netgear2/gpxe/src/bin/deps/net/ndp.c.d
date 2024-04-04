ndp_DEPS = net/ndp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/if_ether.h include/gpxe/iobuf.h include/assert.h \
 include/gpxe/list.h include/gpxe/ndp.h include/gpxe/icmp6.h \
 include/gpxe/ip6.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/netdevice.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/tcpip.h

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

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/ndp.h:

include/gpxe/icmp6.h:

include/gpxe/ip6.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/tcpip.h:

$(BIN)/ndp.o : net/ndp.c $(MAKEDEPS) $(POST_O_DEPS) $(ndp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ndp.o
 
$(BIN)/ndp.dbg%.o : net/ndp.c $(MAKEDEPS) $(POST_O_DEPS) $(ndp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ndp.dbg%.o
 
$(BIN)/ndp.c : net/ndp.c $(MAKEDEPS) $(POST_O_DEPS) $(ndp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ndp.c
 
$(BIN)/ndp.s : net/ndp.c $(MAKEDEPS) $(POST_O_DEPS) $(ndp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ndp.s
 
bin/deps/net/ndp.c.d : $(ndp_DEPS)
 
TAGS : $(ndp_DEPS)

