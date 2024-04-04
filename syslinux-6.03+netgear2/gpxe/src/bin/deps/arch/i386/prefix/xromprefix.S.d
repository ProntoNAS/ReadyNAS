xromprefix_DEPS = arch/i386/prefix/xromprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/romprefix.S \
 config/general.h config/defaults.h config/defaults/pcbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/romprefix.S:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

$(BIN)/xromprefix.o : arch/i386/prefix/xromprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(xromprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/xromprefix.o
 
$(BIN)/xromprefix.s : arch/i386/prefix/xromprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(xromprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/xromprefix.s
 
bin/deps/arch/i386/prefix/xromprefix.S.d : $(xromprefix_DEPS)
 
TAGS : $(xromprefix_DEPS)

