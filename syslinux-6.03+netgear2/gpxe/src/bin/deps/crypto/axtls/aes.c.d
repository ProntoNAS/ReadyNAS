aes_DEPS = crypto/axtls/aes.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h crypto/axtls/crypto.h \
 crypto/axtls/bigint.h crypto/axtls/os_port.h include/stdlib.h \
 include/assert.h include/time.h include/sys/time.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdlib.h:

include/assert.h:

include/time.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

$(BIN)/aes.o : crypto/axtls/aes.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/aes.o
 
$(BIN)/aes.dbg%.o : crypto/axtls/aes.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/aes.dbg%.o
 
$(BIN)/aes.c : crypto/axtls/aes.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/aes.c
 
$(BIN)/aes.s : crypto/axtls/aes.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/aes.s
 
bin/deps/crypto/axtls/aes.c.d : $(aes_DEPS)
 
TAGS : $(aes_DEPS)

