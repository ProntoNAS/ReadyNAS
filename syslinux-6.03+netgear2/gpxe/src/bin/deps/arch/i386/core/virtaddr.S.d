virtaddr_DEPS = arch/i386/core/virtaddr.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/librm.h:

$(BIN)/virtaddr.o : arch/i386/core/virtaddr.S $(MAKEDEPS) $(POST_O_DEPS) $(virtaddr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/virtaddr.o
 
$(BIN)/virtaddr.s : arch/i386/core/virtaddr.S $(MAKEDEPS) $(POST_O_DEPS) $(virtaddr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/virtaddr.s
 
bin/deps/arch/i386/core/virtaddr.S.d : $(virtaddr_DEPS)
 
TAGS : $(virtaddr_DEPS)

