romprefix_DEPS = arch/i386/prefix/romprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h config/general.h config/defaults.h \
 config/defaults/pcbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

$(BIN)/romprefix.o : arch/i386/prefix/romprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/romprefix.o
 
$(BIN)/romprefix.s : arch/i386/prefix/romprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/romprefix.s
 
bin/deps/arch/i386/prefix/romprefix.S.d : $(romprefix_DEPS)
 
TAGS : $(romprefix_DEPS)

