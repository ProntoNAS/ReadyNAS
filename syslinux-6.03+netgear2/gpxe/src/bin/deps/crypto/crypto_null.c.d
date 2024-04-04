crypto_null_DEPS = crypto/crypto_null.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/crypto.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/crypto.h:

$(BIN)/crypto_null.o : crypto/crypto_null.c $(MAKEDEPS) $(POST_O_DEPS) $(crypto_null_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/crypto_null.o
 
$(BIN)/crypto_null.dbg%.o : crypto/crypto_null.c $(MAKEDEPS) $(POST_O_DEPS) $(crypto_null_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/crypto_null.dbg%.o
 
$(BIN)/crypto_null.c : crypto/crypto_null.c $(MAKEDEPS) $(POST_O_DEPS) $(crypto_null_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/crypto_null.c
 
$(BIN)/crypto_null.s : crypto/crypto_null.c $(MAKEDEPS) $(POST_O_DEPS) $(crypto_null_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/crypto_null.s
 
bin/deps/crypto/crypto_null.c.d : $(crypto_null_DEPS)
 
TAGS : $(crypto_null_DEPS)

