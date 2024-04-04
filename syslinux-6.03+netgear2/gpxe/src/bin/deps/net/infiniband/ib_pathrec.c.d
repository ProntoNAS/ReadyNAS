ib_pathrec_DEPS = net/infiniband/ib_pathrec.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/infiniband.h include/gpxe/refcnt.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/ib_mi.h include/gpxe/retry.h \
 include/gpxe/ib_pathrec.h

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

include/gpxe/infiniband.h:

include/gpxe/refcnt.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_mi.h:

include/gpxe/retry.h:

include/gpxe/ib_pathrec.h:

$(BIN)/ib_pathrec.o : net/infiniband/ib_pathrec.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_pathrec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_pathrec.o
 
$(BIN)/ib_pathrec.dbg%.o : net/infiniband/ib_pathrec.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_pathrec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_pathrec.dbg%.o
 
$(BIN)/ib_pathrec.c : net/infiniband/ib_pathrec.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_pathrec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_pathrec.c
 
$(BIN)/ib_pathrec.s : net/infiniband/ib_pathrec.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_pathrec_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_pathrec.s
 
bin/deps/net/infiniband/ib_pathrec.c.d : $(ib_pathrec_DEPS)
 
TAGS : $(ib_pathrec_DEPS)

