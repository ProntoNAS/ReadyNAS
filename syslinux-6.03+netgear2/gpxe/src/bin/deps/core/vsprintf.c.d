vsprintf_DEPS = core/vsprintf.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/stdio.h \
 include/console.h include/gpxe/tables.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/vsprintf.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/stdio.h:

include/console.h:

include/gpxe/tables.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/vsprintf.h:

$(BIN)/vsprintf.o : core/vsprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(vsprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/vsprintf.o
 
$(BIN)/vsprintf.dbg%.o : core/vsprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(vsprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/vsprintf.dbg%.o
 
$(BIN)/vsprintf.c : core/vsprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(vsprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/vsprintf.c
 
$(BIN)/vsprintf.s : core/vsprintf.c $(MAKEDEPS) $(POST_O_DEPS) $(vsprintf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/vsprintf.s
 
bin/deps/core/vsprintf.c.d : $(vsprintf_DEPS)
 
TAGS : $(vsprintf_DEPS)

