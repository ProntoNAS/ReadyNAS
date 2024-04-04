linebuf_DEPS = core/linebuf.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/linebuf.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/linebuf.h:

$(BIN)/linebuf.o : core/linebuf.c $(MAKEDEPS) $(POST_O_DEPS) $(linebuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/linebuf.o
 
$(BIN)/linebuf.dbg%.o : core/linebuf.c $(MAKEDEPS) $(POST_O_DEPS) $(linebuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/linebuf.dbg%.o
 
$(BIN)/linebuf.c : core/linebuf.c $(MAKEDEPS) $(POST_O_DEPS) $(linebuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/linebuf.c
 
$(BIN)/linebuf.s : core/linebuf.c $(MAKEDEPS) $(POST_O_DEPS) $(linebuf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/linebuf.s
 
bin/deps/core/linebuf.c.d : $(linebuf_DEPS)
 
TAGS : $(linebuf_DEPS)

