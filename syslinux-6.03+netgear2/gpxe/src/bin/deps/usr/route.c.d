route_DEPS = usr/route.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdio.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/stdarg.h include/gpxe/netdevice.h include/gpxe/list.h \
 include/stddef.h include/assert.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/ip.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/retry.h \
 include/usr/route.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/ip.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/retry.h:

include/usr/route.h:

$(BIN)/route.o : usr/route.c $(MAKEDEPS) $(POST_O_DEPS) $(route_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/route.o
 
$(BIN)/route.dbg%.o : usr/route.c $(MAKEDEPS) $(POST_O_DEPS) $(route_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/route.dbg%.o
 
$(BIN)/route.c : usr/route.c $(MAKEDEPS) $(POST_O_DEPS) $(route_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/route.c
 
$(BIN)/route.s : usr/route.c $(MAKEDEPS) $(POST_O_DEPS) $(route_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/route.s
 
bin/deps/usr/route.c.d : $(route_DEPS)
 
TAGS : $(route_DEPS)

