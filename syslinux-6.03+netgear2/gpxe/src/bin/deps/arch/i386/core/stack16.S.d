stack16_DEPS = arch/i386/core/stack16.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/stack16.o : arch/i386/core/stack16.S $(MAKEDEPS) $(POST_O_DEPS) $(stack16_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/stack16.o
 
$(BIN)/stack16.s : arch/i386/core/stack16.S $(MAKEDEPS) $(POST_O_DEPS) $(stack16_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/stack16.s
 
bin/deps/arch/i386/core/stack16.S.d : $(stack16_DEPS)
 
TAGS : $(stack16_DEPS)

