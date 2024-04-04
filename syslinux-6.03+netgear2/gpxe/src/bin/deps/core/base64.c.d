base64_DEPS = core/base64.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/assert.h include/gpxe/base64.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/gpxe/base64.h:

$(BIN)/base64.o : core/base64.c $(MAKEDEPS) $(POST_O_DEPS) $(base64_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/base64.o
 
$(BIN)/base64.dbg%.o : core/base64.c $(MAKEDEPS) $(POST_O_DEPS) $(base64_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/base64.dbg%.o
 
$(BIN)/base64.c : core/base64.c $(MAKEDEPS) $(POST_O_DEPS) $(base64_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/base64.c
 
$(BIN)/base64.s : core/base64.c $(MAKEDEPS) $(POST_O_DEPS) $(base64_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/base64.s
 
bin/deps/core/base64.c.d : $(base64_DEPS)
 
TAGS : $(base64_DEPS)

