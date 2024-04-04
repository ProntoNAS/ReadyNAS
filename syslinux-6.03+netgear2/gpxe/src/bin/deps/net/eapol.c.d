eapol_DEPS = net/eapol.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/gpxe/netdevice.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/if_ether.h \
 include/gpxe/eapol.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/netdevice.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/if_ether.h:

include/gpxe/eapol.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

$(BIN)/eapol.o : net/eapol.c $(MAKEDEPS) $(POST_O_DEPS) $(eapol_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/eapol.o
 
$(BIN)/eapol.dbg%.o : net/eapol.c $(MAKEDEPS) $(POST_O_DEPS) $(eapol_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/eapol.dbg%.o
 
$(BIN)/eapol.c : net/eapol.c $(MAKEDEPS) $(POST_O_DEPS) $(eapol_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/eapol.c
 
$(BIN)/eapol.s : net/eapol.c $(MAKEDEPS) $(POST_O_DEPS) $(eapol_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/eapol.s
 
bin/deps/net/eapol.c.d : $(eapol_DEPS)
 
TAGS : $(eapol_DEPS)

