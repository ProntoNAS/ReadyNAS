arp_DEPS = net/arp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/if_ether.h include/gpxe/if_arp.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h include/gpxe/netdevice.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/arp.h

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

include/gpxe/if_arp.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/arp.h:

$(BIN)/arp.o : net/arp.c $(MAKEDEPS) $(POST_O_DEPS) $(arp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/arp.o
 
$(BIN)/arp.dbg%.o : net/arp.c $(MAKEDEPS) $(POST_O_DEPS) $(arp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/arp.dbg%.o
 
$(BIN)/arp.c : net/arp.c $(MAKEDEPS) $(POST_O_DEPS) $(arp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/arp.c
 
$(BIN)/arp.s : net/arp.c $(MAKEDEPS) $(POST_O_DEPS) $(arp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/arp.s
 
bin/deps/net/arp.c.d : $(arp_DEPS)
 
TAGS : $(arp_DEPS)

