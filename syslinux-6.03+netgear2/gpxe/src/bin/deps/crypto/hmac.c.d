hmac_DEPS = crypto/hmac.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/assert.h include/gpxe/crypto.h \
 include/gpxe/hmac.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/gpxe/crypto.h:

include/gpxe/hmac.h:

$(BIN)/hmac.o : crypto/hmac.c $(MAKEDEPS) $(POST_O_DEPS) $(hmac_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/hmac.o
 
$(BIN)/hmac.dbg%.o : crypto/hmac.c $(MAKEDEPS) $(POST_O_DEPS) $(hmac_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/hmac.dbg%.o
 
$(BIN)/hmac.c : crypto/hmac.c $(MAKEDEPS) $(POST_O_DEPS) $(hmac_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/hmac.c
 
$(BIN)/hmac.s : crypto/hmac.c $(MAKEDEPS) $(POST_O_DEPS) $(hmac_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/hmac.s
 
bin/deps/crypto/hmac.c.d : $(hmac_DEPS)
 
TAGS : $(hmac_DEPS)

