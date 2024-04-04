e820mangler_DEPS = arch/i386/firmware/pcbios/e820mangler.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/e820mangler.o : arch/i386/firmware/pcbios/e820mangler.S $(MAKEDEPS) $(POST_O_DEPS) $(e820mangler_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/e820mangler.o
 
$(BIN)/e820mangler.s : arch/i386/firmware/pcbios/e820mangler.S $(MAKEDEPS) $(POST_O_DEPS) $(e820mangler_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/e820mangler.s
 
bin/deps/arch/i386/firmware/pcbios/e820mangler.S.d : $(e820mangler_DEPS)
 
TAGS : $(e820mangler_DEPS)

