ib_cm_DEPS = net/infiniband/ib_cm.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/infiniband.h include/gpxe/refcnt.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/ib_mi.h include/gpxe/retry.h \
 include/gpxe/ib_pathrec.h include/gpxe/ib_cm.h

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

include/gpxe/ib_cm.h:

$(BIN)/ib_cm.o : net/infiniband/ib_cm.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_cm.o
 
$(BIN)/ib_cm.dbg%.o : net/infiniband/ib_cm.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_cm.dbg%.o
 
$(BIN)/ib_cm.c : net/infiniband/ib_cm.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_cm.c
 
$(BIN)/ib_cm.s : net/infiniband/ib_cm.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_cm.s
 
bin/deps/net/infiniband/ib_cm.c.d : $(ib_cm_DEPS)
 
TAGS : $(ib_cm_DEPS)

