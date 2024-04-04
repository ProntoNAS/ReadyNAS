axtls_sha1_DEPS = crypto/axtls_sha1.c include/compiler.h \
 arch/i386/include/bits/compiler.h crypto/axtls/crypto.h \
 crypto/axtls/bigint.h crypto/axtls/os_port.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/stdlib.h \
 include/assert.h include/time.h include/sys/time.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/crypto.h include/gpxe/sha1.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

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

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/crypto.h:

include/gpxe/sha1.h:

$(BIN)/axtls_sha1.o : crypto/axtls_sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/axtls_sha1.o
 
$(BIN)/axtls_sha1.dbg%.o : crypto/axtls_sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/axtls_sha1.dbg%.o
 
$(BIN)/axtls_sha1.c : crypto/axtls_sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/axtls_sha1.c
 
$(BIN)/axtls_sha1.s : crypto/axtls_sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(axtls_sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/axtls_sha1.s
 
bin/deps/crypto/axtls_sha1.c.d : $(axtls_sha1_DEPS)
 
TAGS : $(axtls_sha1_DEPS)

