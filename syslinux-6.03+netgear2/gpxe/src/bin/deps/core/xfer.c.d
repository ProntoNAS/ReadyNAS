xfer_DEPS = core/xfer.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/xfer.h \
 include/gpxe/interface.h include/gpxe/refcnt.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

$(BIN)/xfer.o : core/xfer.c $(MAKEDEPS) $(POST_O_DEPS) $(xfer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/xfer.o
 
$(BIN)/xfer.dbg%.o : core/xfer.c $(MAKEDEPS) $(POST_O_DEPS) $(xfer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/xfer.dbg%.o
 
$(BIN)/xfer.c : core/xfer.c $(MAKEDEPS) $(POST_O_DEPS) $(xfer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/xfer.c
 
$(BIN)/xfer.s : core/xfer.c $(MAKEDEPS) $(POST_O_DEPS) $(xfer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/xfer.s
 
bin/deps/core/xfer.c.d : $(xfer_DEPS)
 
TAGS : $(xfer_DEPS)

