bitops_DEPS = core/bitops.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/strings.h \
 arch/i386/include/limits.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/strings.h:

arch/i386/include/limits.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

$(BIN)/bitops.o : core/bitops.c $(MAKEDEPS) $(POST_O_DEPS) $(bitops_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bitops.o
 
$(BIN)/bitops.dbg%.o : core/bitops.c $(MAKEDEPS) $(POST_O_DEPS) $(bitops_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bitops.dbg%.o
 
$(BIN)/bitops.c : core/bitops.c $(MAKEDEPS) $(POST_O_DEPS) $(bitops_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bitops.c
 
$(BIN)/bitops.s : core/bitops.c $(MAKEDEPS) $(POST_O_DEPS) $(bitops_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bitops.s
 
bin/deps/core/bitops.c.d : $(bitops_DEPS)
 
TAGS : $(bitops_DEPS)

