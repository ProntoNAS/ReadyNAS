crandom_DEPS = crypto/crandom.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/crypto.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/stdlib.h \
 include/assert.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/crypto.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/stdlib.h:

include/assert.h:

$(BIN)/crandom.o : crypto/crandom.c $(MAKEDEPS) $(POST_O_DEPS) $(crandom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/crandom.o
 
$(BIN)/crandom.dbg%.o : crypto/crandom.c $(MAKEDEPS) $(POST_O_DEPS) $(crandom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/crandom.dbg%.o
 
$(BIN)/crandom.c : crypto/crandom.c $(MAKEDEPS) $(POST_O_DEPS) $(crandom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/crandom.c
 
$(BIN)/crandom.s : crypto/crandom.c $(MAKEDEPS) $(POST_O_DEPS) $(crandom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/crandom.s
 
bin/deps/crypto/crandom.c.d : $(crandom_DEPS)
 
TAGS : $(crandom_DEPS)

