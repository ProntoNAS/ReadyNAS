tcpip_DEPS = net/tcpip.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/iobuf.h include/assert.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/tcpip.h include/gpxe/socket.h \
 include/gpxe/in.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/tcpip.h:

include/gpxe/socket.h:

include/gpxe/in.h:

$(BIN)/tcpip.o : net/tcpip.c $(MAKEDEPS) $(POST_O_DEPS) $(tcpip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tcpip.o
 
$(BIN)/tcpip.dbg%.o : net/tcpip.c $(MAKEDEPS) $(POST_O_DEPS) $(tcpip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tcpip.dbg%.o
 
$(BIN)/tcpip.c : net/tcpip.c $(MAKEDEPS) $(POST_O_DEPS) $(tcpip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tcpip.c
 
$(BIN)/tcpip.s : net/tcpip.c $(MAKEDEPS) $(POST_O_DEPS) $(tcpip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tcpip.s
 
bin/deps/net/tcpip.c.d : $(tcpip_DEPS)
 
TAGS : $(tcpip_DEPS)

