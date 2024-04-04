hromprefix_DEPS = arch/i386/prefix/hromprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/romprefix.S \
 config/general.h config/defaults.h config/defaults/pcbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/romprefix.S:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

$(BIN)/hromprefix.o : arch/i386/prefix/hromprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(hromprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/hromprefix.o
 
$(BIN)/hromprefix.s : arch/i386/prefix/hromprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(hromprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/hromprefix.s
 
bin/deps/arch/i386/prefix/hromprefix.S.d : $(hromprefix_DEPS)
 
TAGS : $(hromprefix_DEPS)

