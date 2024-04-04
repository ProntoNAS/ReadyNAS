libprefix_DEPS = arch/i386/prefix/libprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/libprefix.o : arch/i386/prefix/libprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(libprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/libprefix.o
 
$(BIN)/libprefix.s : arch/i386/prefix/libprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(libprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/libprefix.s
 
bin/deps/arch/i386/prefix/libprefix.S.d : $(libprefix_DEPS)
 
TAGS : $(libprefix_DEPS)

