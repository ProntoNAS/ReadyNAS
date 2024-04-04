nbiprefix_DEPS = arch/i386/prefix/nbiprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/nbiprefix.o : arch/i386/prefix/nbiprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(nbiprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/nbiprefix.o
 
$(BIN)/nbiprefix.s : arch/i386/prefix/nbiprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(nbiprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/nbiprefix.s
 
bin/deps/arch/i386/prefix/nbiprefix.S.d : $(nbiprefix_DEPS)
 
TAGS : $(nbiprefix_DEPS)

