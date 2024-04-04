open_DEPS = core/open.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdarg.h include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/xfer.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/iobuf.h include/assert.h include/gpxe/list.h \
 include/gpxe/uri.h include/stdlib.h include/gpxe/socket.h \
 include/gpxe/open.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/uri.h:

include/stdlib.h:

include/gpxe/socket.h:

include/gpxe/open.h:

include/gpxe/tables.h:

$(BIN)/open.o : core/open.c $(MAKEDEPS) $(POST_O_DEPS) $(open_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/open.o
 
$(BIN)/open.dbg%.o : core/open.c $(MAKEDEPS) $(POST_O_DEPS) $(open_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/open.dbg%.o
 
$(BIN)/open.c : core/open.c $(MAKEDEPS) $(POST_O_DEPS) $(open_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/open.c
 
$(BIN)/open.s : core/open.c $(MAKEDEPS) $(POST_O_DEPS) $(open_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/open.s
 
bin/deps/core/open.c.d : $(open_DEPS)
 
TAGS : $(open_DEPS)

