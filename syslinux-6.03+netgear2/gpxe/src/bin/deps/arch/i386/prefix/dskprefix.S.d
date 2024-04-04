dskprefix_DEPS = arch/i386/prefix/dskprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/dskprefix.o : arch/i386/prefix/dskprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(dskprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/dskprefix.o
 
$(BIN)/dskprefix.s : arch/i386/prefix/dskprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(dskprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/dskprefix.s
 
bin/deps/arch/i386/prefix/dskprefix.S.d : $(dskprefix_DEPS)
 
TAGS : $(dskprefix_DEPS)

