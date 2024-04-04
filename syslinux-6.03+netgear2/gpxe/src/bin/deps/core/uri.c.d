uri_DEPS = core/uri.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/libgen.h include/ctype.h \
 include/gpxe/vsprintf.h include/stdarg.h include/stdio.h \
 include/gpxe/uri.h include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/libgen.h:

include/ctype.h:

include/gpxe/vsprintf.h:

include/stdarg.h:

include/stdio.h:

include/gpxe/uri.h:

include/gpxe/refcnt.h:

$(BIN)/uri.o : core/uri.c $(MAKEDEPS) $(POST_O_DEPS) $(uri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/uri.o
 
$(BIN)/uri.dbg%.o : core/uri.c $(MAKEDEPS) $(POST_O_DEPS) $(uri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/uri.dbg%.o
 
$(BIN)/uri.c : core/uri.c $(MAKEDEPS) $(POST_O_DEPS) $(uri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/uri.c
 
$(BIN)/uri.s : core/uri.c $(MAKEDEPS) $(POST_O_DEPS) $(uri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/uri.s
 
bin/deps/core/uri.c.d : $(uri_DEPS)
 
TAGS : $(uri_DEPS)

