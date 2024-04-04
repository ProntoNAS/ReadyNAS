nullnet_DEPS = net/nullnet.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/iobuf.h include/assert.h \
 include/gpxe/list.h include/stddef.h include/gpxe/netdevice.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

$(BIN)/nullnet.o : net/nullnet.c $(MAKEDEPS) $(POST_O_DEPS) $(nullnet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/nullnet.o
 
$(BIN)/nullnet.dbg%.o : net/nullnet.c $(MAKEDEPS) $(POST_O_DEPS) $(nullnet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/nullnet.dbg%.o
 
$(BIN)/nullnet.c : net/nullnet.c $(MAKEDEPS) $(POST_O_DEPS) $(nullnet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/nullnet.c
 
$(BIN)/nullnet.s : net/nullnet.c $(MAKEDEPS) $(POST_O_DEPS) $(nullnet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/nullnet.s
 
bin/deps/net/nullnet.c.d : $(nullnet_DEPS)
 
TAGS : $(nullnet_DEPS)

