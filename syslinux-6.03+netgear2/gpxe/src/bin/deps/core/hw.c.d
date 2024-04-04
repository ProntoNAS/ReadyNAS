hw_DEPS = core/hw.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/stdlib.h include/assert.h include/string.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/refcnt.h \
 include/gpxe/process.h include/gpxe/list.h include/gpxe/tables.h \
 include/gpxe/xfer.h include/stdarg.h include/gpxe/interface.h \
 include/gpxe/iobuf.h include/gpxe/open.h include/gpxe/socket.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/refcnt.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/open.h:

include/gpxe/socket.h:

$(BIN)/hw.o : core/hw.c $(MAKEDEPS) $(POST_O_DEPS) $(hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/hw.o
 
$(BIN)/hw.dbg%.o : core/hw.c $(MAKEDEPS) $(POST_O_DEPS) $(hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/hw.dbg%.o
 
$(BIN)/hw.c : core/hw.c $(MAKEDEPS) $(POST_O_DEPS) $(hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/hw.c
 
$(BIN)/hw.s : core/hw.c $(MAKEDEPS) $(POST_O_DEPS) $(hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/hw.s
 
bin/deps/core/hw.c.d : $(hw_DEPS)
 
TAGS : $(hw_DEPS)

