mii_DEPS = net/mii.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/mii.h include/gpxe/netdevice.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/mii.h:

include/gpxe/netdevice.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

$(BIN)/mii.o : net/mii.c $(MAKEDEPS) $(POST_O_DEPS) $(mii_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/mii.o
 
$(BIN)/mii.dbg%.o : net/mii.c $(MAKEDEPS) $(POST_O_DEPS) $(mii_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/mii.dbg%.o
 
$(BIN)/mii.c : net/mii.c $(MAKEDEPS) $(POST_O_DEPS) $(mii_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/mii.c
 
$(BIN)/mii.s : net/mii.c $(MAKEDEPS) $(POST_O_DEPS) $(mii_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/mii.s
 
bin/deps/net/mii.c.d : $(mii_DEPS)
 
TAGS : $(mii_DEPS)

