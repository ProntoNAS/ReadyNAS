setjmp_DEPS = arch/i386/core/setjmp.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/setjmp.o : arch/i386/core/setjmp.S $(MAKEDEPS) $(POST_O_DEPS) $(setjmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/setjmp.o
 
$(BIN)/setjmp.s : arch/i386/core/setjmp.S $(MAKEDEPS) $(POST_O_DEPS) $(setjmp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/setjmp.s
 
bin/deps/arch/i386/core/setjmp.S.d : $(setjmp_DEPS)
 
TAGS : $(setjmp_DEPS)

