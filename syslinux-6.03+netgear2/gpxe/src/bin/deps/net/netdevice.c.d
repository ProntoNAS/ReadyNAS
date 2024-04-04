netdevice_DEPS = net/netdevice.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/if_ether.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/tables.h \
 include/gpxe/process.h include/gpxe/refcnt.h include/gpxe/init.h \
 include/gpxe/device.h include/gpxe/errortab.h include/gpxe/netdevice.h \
 include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/process.h:

include/gpxe/refcnt.h:

include/gpxe/init.h:

include/gpxe/device.h:

include/gpxe/errortab.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

$(BIN)/netdevice.o : net/netdevice.c $(MAKEDEPS) $(POST_O_DEPS) $(netdevice_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/netdevice.o
 
$(BIN)/netdevice.dbg%.o : net/netdevice.c $(MAKEDEPS) $(POST_O_DEPS) $(netdevice_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/netdevice.dbg%.o
 
$(BIN)/netdevice.c : net/netdevice.c $(MAKEDEPS) $(POST_O_DEPS) $(netdevice_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/netdevice.c
 
$(BIN)/netdevice.s : net/netdevice.c $(MAKEDEPS) $(POST_O_DEPS) $(netdevice_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/netdevice.s
 
bin/deps/net/netdevice.c.d : $(netdevice_DEPS)
 
TAGS : $(netdevice_DEPS)

