x86_string_DEPS = arch/x86/core/x86_string.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

$(BIN)/x86_string.o : arch/x86/core/x86_string.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/x86_string.o
 
$(BIN)/x86_string.dbg%.o : arch/x86/core/x86_string.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/x86_string.dbg%.o
 
$(BIN)/x86_string.c : arch/x86/core/x86_string.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/x86_string.c
 
$(BIN)/x86_string.s : arch/x86/core/x86_string.c $(MAKEDEPS) $(POST_O_DEPS) $(x86_string_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/x86_string.s
 
bin/deps/arch/x86/core/x86_string.c.d : $(x86_string_DEPS)
 
TAGS : $(x86_string_DEPS)

