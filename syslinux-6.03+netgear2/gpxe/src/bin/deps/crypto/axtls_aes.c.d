axtls_aes_DEPS = crypto/axtls_aes.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/crypto.h include/gpxe/cbc.h \
 include/gpxe/aes.h crypto/axtls/crypto.h crypto/axtls/bigint.h \
 crypto/axtls/os_port.h include/stdlib.h include/assert.h include/time.h \
 include/sys/time.h crypto/axtls/bigint_impl.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/crypto.h:

include/gpxe/cbc.h:

include/gpxe/aes.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdlib.h:

include/assert.h:

include/time.h:

include/sys/time.h:

crypto/axtls/bigint_impl.h:

$(BIN)/axtls_aes.o : crypto/axtls_aes.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/axtls_aes.o
 
$(BIN)/axtls_aes.dbg%.o : crypto/axtls_aes.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/axtls_aes.dbg%.o
 
$(BIN)/axtls_aes.c : crypto/axtls_aes.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/axtls_aes.c
 
$(BIN)/axtls_aes.s : crypto/axtls_aes.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_aes_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/axtls_aes.s
 
bin/deps/crypto/axtls_aes.c.d : $(axtls_aes_DEPS)
 
TAGS : $(axtls_aes_DEPS)

