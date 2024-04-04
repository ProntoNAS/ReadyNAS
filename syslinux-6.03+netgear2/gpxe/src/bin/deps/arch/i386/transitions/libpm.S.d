libpm_DEPS = arch/i386/transitions/libpm.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/libpm.o : arch/i386/transitions/libpm.S $(MAKEDEPS) $(POST_O_DEPS) $(libpm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/libpm.o
 
$(BIN)/libpm.s : arch/i386/transitions/libpm.S $(MAKEDEPS) $(POST_O_DEPS) $(libpm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/libpm.s
 
bin/deps/arch/i386/transitions/libpm.S.d : $(libpm_DEPS)
 
TAGS : $(libpm_DEPS)

