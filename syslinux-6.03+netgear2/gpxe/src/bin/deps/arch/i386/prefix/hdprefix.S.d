hdprefix_DEPS = arch/i386/prefix/hdprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/bootpart.S

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/bootpart.S:

$(BIN)/hdprefix.o : arch/i386/prefix/hdprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(hdprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/hdprefix.o
 
$(BIN)/hdprefix.s : arch/i386/prefix/hdprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(hdprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/hdprefix.s
 
bin/deps/arch/i386/prefix/hdprefix.S.d : $(hdprefix_DEPS)
 
TAGS : $(hdprefix_DEPS)

