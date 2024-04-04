libkir_DEPS = arch/i386/transitions/libkir.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/libkir.o : arch/i386/transitions/libkir.S $(MAKEDEPS) $(POST_O_DEPS) $(libkir_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/libkir.o
 
$(BIN)/libkir.s : arch/i386/transitions/libkir.S $(MAKEDEPS) $(POST_O_DEPS) $(libkir_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/libkir.s
 
bin/deps/arch/i386/transitions/libkir.S.d : $(libkir_DEPS)
 
TAGS : $(libkir_DEPS)

