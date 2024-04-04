nullprefix_DEPS = arch/i386/prefix/nullprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/nullprefix.o : arch/i386/prefix/nullprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(nullprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/nullprefix.o
 
$(BIN)/nullprefix.s : arch/i386/prefix/nullprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(nullprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/nullprefix.s
 
bin/deps/arch/i386/prefix/nullprefix.S.d : $(nullprefix_DEPS)
 
TAGS : $(nullprefix_DEPS)

