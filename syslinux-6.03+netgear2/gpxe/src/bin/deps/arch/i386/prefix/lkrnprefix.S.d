lkrnprefix_DEPS = arch/i386/prefix/lkrnprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/lkrnprefix.o : arch/i386/prefix/lkrnprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(lkrnprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/lkrnprefix.o
 
$(BIN)/lkrnprefix.s : arch/i386/prefix/lkrnprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(lkrnprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/lkrnprefix.s
 
bin/deps/arch/i386/prefix/lkrnprefix.S.d : $(lkrnprefix_DEPS)
 
TAGS : $(lkrnprefix_DEPS)

