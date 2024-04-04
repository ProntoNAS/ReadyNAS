ethernet_DEPS = net/ethernet.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h include/assert.h \
 include/gpxe/if_arp.h include/gpxe/if_ether.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/netdevice.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/iobuf.h include/gpxe/ethernet.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

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

include/assert.h:

include/gpxe/if_arp.h:

include/gpxe/if_ether.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/ethernet.h:

$(BIN)/ethernet.o : net/ethernet.c $(MAKEDEPS) $(POST_O_DEPS) $(ethernet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ethernet.o
 
$(BIN)/ethernet.dbg%.o : net/ethernet.c $(MAKEDEPS) $(POST_O_DEPS) $(ethernet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ethernet.dbg%.o
 
$(BIN)/ethernet.c : net/ethernet.c $(MAKEDEPS) $(POST_O_DEPS) $(ethernet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ethernet.c
 
$(BIN)/ethernet.s : net/ethernet.c $(MAKEDEPS) $(POST_O_DEPS) $(ethernet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ethernet.s
 
bin/deps/net/ethernet.c.d : $(ethernet_DEPS)
 
TAGS : $(ethernet_DEPS)

