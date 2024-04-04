ib_packet_DEPS = net/infiniband/ib_packet.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/infiniband.h \
 include/gpxe/refcnt.h include/gpxe/device.h include/gpxe/tables.h \
 include/gpxe/ib_packet.h include/gpxe/ib_mad.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

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

include/gpxe/list.h:

include/gpxe/infiniband.h:

include/gpxe/refcnt.h:

include/gpxe/device.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

$(BIN)/ib_packet.o : net/infiniband/ib_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_packet.o
 
$(BIN)/ib_packet.dbg%.o : net/infiniband/ib_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_packet.dbg%.o
 
$(BIN)/ib_packet.c : net/infiniband/ib_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_packet.c
 
$(BIN)/ib_packet.s : net/infiniband/ib_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_packet.s
 
bin/deps/net/infiniband/ib_packet.c.d : $(ib_packet_DEPS)
 
TAGS : $(ib_packet_DEPS)

