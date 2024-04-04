stack_DEPS = arch/i386/core/stack.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/stack.o : arch/i386/core/stack.S $(MAKEDEPS) $(POST_O_DEPS) $(stack_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/stack.o
 
$(BIN)/stack.s : arch/i386/core/stack.S $(MAKEDEPS) $(POST_O_DEPS) $(stack_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/stack.s
 
bin/deps/arch/i386/core/stack.S.d : $(stack_DEPS)
 
TAGS : $(stack_DEPS)

