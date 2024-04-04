cbc_DEPS = crypto/cbc.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/assert.h include/gpxe/crypto.h include/gpxe/cbc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/gpxe/crypto.h:

include/gpxe/cbc.h:

$(BIN)/cbc.o : crypto/cbc.c $(MAKEDEPS) $(POST_O_DEPS) $(cbc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/cbc.o
 
$(BIN)/cbc.dbg%.o : crypto/cbc.c $(MAKEDEPS) $(POST_O_DEPS) $(cbc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/cbc.dbg%.o
 
$(BIN)/cbc.c : crypto/cbc.c $(MAKEDEPS) $(POST_O_DEPS) $(cbc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/cbc.c
 
$(BIN)/cbc.s : crypto/cbc.c $(MAKEDEPS) $(POST_O_DEPS) $(cbc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/cbc.s
 
bin/deps/crypto/cbc.c.d : $(cbc_DEPS)
 
TAGS : $(cbc_DEPS)

