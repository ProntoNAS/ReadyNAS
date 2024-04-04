string_DEPS = core/string.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/ctype.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/ctype.h:

$(BIN)/string.o : core/string.c $(MAKEDEPS) $(POST_O_DEPS) $(string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/string.o
 
$(BIN)/string.dbg%.o : core/string.c $(MAKEDEPS) $(POST_O_DEPS) $(string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/string.dbg%.o
 
$(BIN)/string.c : core/string.c $(MAKEDEPS) $(POST_O_DEPS) $(string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/string.c
 
$(BIN)/string.s : core/string.c $(MAKEDEPS) $(POST_O_DEPS) $(string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/string.s
 
bin/deps/core/string.c.d : $(string_DEPS)
 
TAGS : $(string_DEPS)

