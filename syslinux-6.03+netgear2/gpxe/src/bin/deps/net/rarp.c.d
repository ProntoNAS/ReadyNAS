rarp_DEPS = net/rarp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/if_ether.h \
 include/gpxe/rarp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/if_ether.h:

include/gpxe/rarp.h:

$(BIN)/rarp.o : net/rarp.c $(MAKEDEPS) $(POST_O_DEPS) $(rarp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rarp.o
 
$(BIN)/rarp.dbg%.o : net/rarp.c $(MAKEDEPS) $(POST_O_DEPS) $(rarp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rarp.dbg%.o
 
$(BIN)/rarp.c : net/rarp.c $(MAKEDEPS) $(POST_O_DEPS) $(rarp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rarp.c
 
$(BIN)/rarp.s : net/rarp.c $(MAKEDEPS) $(POST_O_DEPS) $(rarp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rarp.s
 
bin/deps/net/rarp.c.d : $(rarp_DEPS)
 
TAGS : $(rarp_DEPS)

