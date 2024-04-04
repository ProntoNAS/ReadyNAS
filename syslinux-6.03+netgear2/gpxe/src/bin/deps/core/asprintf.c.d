asprintf_DEPS = core/asprintf.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

$(BIN)/asprintf.o : core/asprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(asprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/asprintf.o
 
$(BIN)/asprintf.dbg%.o : core/asprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(asprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/asprintf.dbg%.o
 
$(BIN)/asprintf.c : core/asprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(asprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/asprintf.c
 
$(BIN)/asprintf.s : core/asprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(asprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/asprintf.s
 
bin/deps/core/asprintf.c.d : $(asprintf_DEPS)
 
TAGS : $(asprintf_DEPS)

