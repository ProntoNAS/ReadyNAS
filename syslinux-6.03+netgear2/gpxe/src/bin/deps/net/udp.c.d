udp_DEPS = net/udp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/tcpip.h \
 include/gpxe/socket.h include/gpxe/in.h include/gpxe/tables.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/xfer.h \
 include/stdarg.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/open.h include/gpxe/uri.h include/gpxe/udp.h \
 include/gpxe/if_ether.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

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

include/gpxe/tcpip.h:

include/gpxe/socket.h:

include/gpxe/in.h:

include/gpxe/tables.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/open.h:

include/gpxe/uri.h:

include/gpxe/udp.h:

include/gpxe/if_ether.h:

$(BIN)/udp.o : net/udp.c $(MAKEDEPS) $(POST_O_DEPS) $(udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/udp.o
 
$(BIN)/udp.dbg%.o : net/udp.c $(MAKEDEPS) $(POST_O_DEPS) $(udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/udp.dbg%.o
 
$(BIN)/udp.c : net/udp.c $(MAKEDEPS) $(POST_O_DEPS) $(udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/udp.c
 
$(BIN)/udp.s : net/udp.c $(MAKEDEPS) $(POST_O_DEPS) $(udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/udp.s
 
bin/deps/net/udp.c.d : $(udp_DEPS)
 
TAGS : $(udp_DEPS)

