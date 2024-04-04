resolv_DEPS = core/resolv.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/xfer.h \
 include/stdarg.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/open.h \
 include/gpxe/tables.h include/gpxe/process.h include/gpxe/resolv.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/process.h:

include/gpxe/resolv.h:

$(BIN)/resolv.o : core/resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/resolv.o
 
$(BIN)/resolv.dbg%.o : core/resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/resolv.dbg%.o
 
$(BIN)/resolv.c : core/resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/resolv.c
 
$(BIN)/resolv.s : core/resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/resolv.s
 
bin/deps/core/resolv.c.d : $(resolv_DEPS)
 
TAGS : $(resolv_DEPS)

