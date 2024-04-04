stringextra_DEPS = core/stringextra.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/ctype.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/ctype.h:

$(BIN)/stringextra.o : core/stringextra.c $(MAKEDEPS) $(POST_O_DEPS) $(stringextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/stringextra.o
 
$(BIN)/stringextra.dbg%.o : core/stringextra.c $(MAKEDEPS) $(POST_O_DEPS) $(stringextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/stringextra.dbg%.o
 
$(BIN)/stringextra.c : core/stringextra.c $(MAKEDEPS) $(POST_O_DEPS) $(stringextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/stringextra.c
 
$(BIN)/stringextra.s : core/stringextra.c $(MAKEDEPS) $(POST_O_DEPS) $(stringextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/stringextra.s
 
bin/deps/core/stringextra.c.d : $(stringextra_DEPS)
 
TAGS : $(stringextra_DEPS)

