rsa_DEPS = crypto/axtls/rsa.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/time.h \
 include/stdlib.h include/assert.h crypto/axtls/crypto.h \
 crypto/axtls/bigint.h crypto/axtls/os_port.h include/sys/time.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/time.h:

include/stdlib.h:

include/assert.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

$(BIN)/rsa.o : crypto/axtls/rsa.c $(MAKEDEPS) $(POST_O_DEPS) $(rsa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rsa.o
 
$(BIN)/rsa.dbg%.o : crypto/axtls/rsa.c $(MAKEDEPS) $(POST_O_DEPS) $(rsa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rsa.dbg%.o
 
$(BIN)/rsa.c : crypto/axtls/rsa.c $(MAKEDEPS) $(POST_O_DEPS) $(rsa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rsa.c
 
$(BIN)/rsa.s : crypto/axtls/rsa.c $(MAKEDEPS) $(POST_O_DEPS) $(rsa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rsa.s
 
bin/deps/crypto/axtls/rsa.c.d : $(rsa_DEPS)
 
TAGS : $(rsa_DEPS)

