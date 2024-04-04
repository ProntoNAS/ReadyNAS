undiisr_DEPS = arch/i386/drivers/net/undiisr.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/undiisr.o : arch/i386/drivers/net/undiisr.S $(MAKEDEPS) $(POST_O_DEPS) $(undiisr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/undiisr.o
 
$(BIN)/undiisr.s : arch/i386/drivers/net/undiisr.S $(MAKEDEPS) $(POST_O_DEPS) $(undiisr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/undiisr.s
 
bin/deps/arch/i386/drivers/net/undiisr.S.d : $(undiisr_DEPS)
 
TAGS : $(undiisr_DEPS)

