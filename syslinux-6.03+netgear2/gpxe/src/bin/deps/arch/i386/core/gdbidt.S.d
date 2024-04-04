gdbidt_DEPS = arch/i386/core/gdbidt.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/librm.h:

$(BIN)/gdbidt.o : arch/i386/core/gdbidt.S $(MAKEDEPS) $(POST_O_DEPS) $(gdbidt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/gdbidt.o
 
$(BIN)/gdbidt.s : arch/i386/core/gdbidt.S $(MAKEDEPS) $(POST_O_DEPS) $(gdbidt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/gdbidt.s
 
bin/deps/arch/i386/core/gdbidt.S.d : $(gdbidt_DEPS)
 
TAGS : $(gdbidt_DEPS)

