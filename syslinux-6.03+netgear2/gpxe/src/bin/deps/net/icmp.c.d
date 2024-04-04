icmp_DEPS = net/icmp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/iobuf.h include/assert.h include/gpxe/list.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/tcpip.h \
 include/gpxe/tables.h include/gpxe/icmp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/tcpip.h:

include/gpxe/tables.h:

include/gpxe/icmp.h:

$(BIN)/icmp.o : net/icmp.c $(MAKEDEPS) $(POST_O_DEPS) $(icmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/icmp.o
 
$(BIN)/icmp.dbg%.o : net/icmp.c $(MAKEDEPS) $(POST_O_DEPS) $(icmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/icmp.dbg%.o
 
$(BIN)/icmp.c : net/icmp.c $(MAKEDEPS) $(POST_O_DEPS) $(icmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/icmp.c
 
$(BIN)/icmp.s : net/icmp.c $(MAKEDEPS) $(POST_O_DEPS) $(icmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/icmp.s
 
bin/deps/net/icmp.c.d : $(icmp_DEPS)
 
TAGS : $(icmp_DEPS)

