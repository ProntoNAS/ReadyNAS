ib_mcast_DEPS = net/infiniband/ib_mcast.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/list.h include/assert.h \
 include/gpxe/infiniband.h include/gpxe/refcnt.h include/gpxe/device.h \
 include/gpxe/tables.h include/gpxe/ib_packet.h include/gpxe/ib_mad.h \
 include/gpxe/ib_mi.h include/gpxe/retry.h include/gpxe/ib_mcast.h

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

include/gpxe/list.h:

include/assert.h:

include/gpxe/infiniband.h:

include/gpxe/refcnt.h:

include/gpxe/device.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_mi.h:

include/gpxe/retry.h:

include/gpxe/ib_mcast.h:

$(BIN)/ib_mcast.o : net/infiniband/ib_mcast.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_mcast_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_mcast.o
 
$(BIN)/ib_mcast.dbg%.o : net/infiniband/ib_mcast.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_mcast_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_mcast.dbg%.o
 
$(BIN)/ib_mcast.c : net/infiniband/ib_mcast.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_mcast_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_mcast.c
 
$(BIN)/ib_mcast.s : net/infiniband/ib_mcast.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_mcast_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_mcast.s
 
bin/deps/net/infiniband/ib_mcast.c.d : $(ib_mcast_DEPS)
 
TAGS : $(ib_mcast_DEPS)

