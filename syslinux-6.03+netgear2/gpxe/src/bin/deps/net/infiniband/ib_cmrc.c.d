ib_cmrc_DEPS = net/infiniband/ib_cmrc.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/xfer.h \
 include/stdarg.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/process.h include/gpxe/tables.h include/gpxe/infiniband.h \
 include/gpxe/device.h include/gpxe/ib_packet.h include/gpxe/ib_mad.h \
 include/gpxe/ib_cm.h include/gpxe/retry.h include/gpxe/ib_cmrc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/process.h:

include/gpxe/tables.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_cm.h:

include/gpxe/retry.h:

include/gpxe/ib_cmrc.h:

$(BIN)/ib_cmrc.o : net/infiniband/ib_cmrc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cmrc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_cmrc.o
 
$(BIN)/ib_cmrc.dbg%.o : net/infiniband/ib_cmrc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cmrc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_cmrc.dbg%.o
 
$(BIN)/ib_cmrc.c : net/infiniband/ib_cmrc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cmrc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_cmrc.c
 
$(BIN)/ib_cmrc.s : net/infiniband/ib_cmrc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_cmrc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_cmrc.s
 
bin/deps/net/infiniband/ib_cmrc.c.d : $(ib_cmrc_DEPS)
 
TAGS : $(ib_cmrc_DEPS)

