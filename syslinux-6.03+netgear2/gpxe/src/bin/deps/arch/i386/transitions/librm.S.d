librm_DEPS = arch/i386/transitions/librm.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/librm.h:

$(BIN)/librm.o : arch/i386/transitions/librm.S $(MAKEDEPS) $(POST_O_DEPS) $(librm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/librm.o
 
$(BIN)/librm.s : arch/i386/transitions/librm.S $(MAKEDEPS) $(POST_O_DEPS) $(librm_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/librm.s
 
bin/deps/arch/i386/transitions/librm.S.d : $(librm_DEPS)
 
TAGS : $(librm_DEPS)

