iobuf_DEPS = core/iobuf.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/malloc.h include/stdlib.h \
 include/assert.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/malloc.h:

include/stdlib.h:

include/assert.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/stddef.h:

$(BIN)/iobuf.o : core/iobuf.c $(MAKEDEPS) $(POST_O_DEPS) $(iobuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iobuf.o
 
$(BIN)/iobuf.dbg%.o : core/iobuf.c $(MAKEDEPS) $(POST_O_DEPS) $(iobuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iobuf.dbg%.o
 
$(BIN)/iobuf.c : core/iobuf.c $(MAKEDEPS) $(POST_O_DEPS) $(iobuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iobuf.c
 
$(BIN)/iobuf.s : core/iobuf.c $(MAKEDEPS) $(POST_O_DEPS) $(iobuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iobuf.s
 
bin/deps/core/iobuf.c.d : $(iobuf_DEPS)
 
TAGS : $(iobuf_DEPS)

