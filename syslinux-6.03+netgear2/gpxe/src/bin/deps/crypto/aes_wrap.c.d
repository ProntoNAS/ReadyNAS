aes_wrap_DEPS = crypto/aes_wrap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/crypto.h \
 include/gpxe/aes.h crypto/axtls/crypto.h crypto/axtls/bigint.h \
 crypto/axtls/os_port.h include/time.h include/sys/time.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/crypto.h:

include/gpxe/aes.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/time.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

$(BIN)/aes_wrap.o : crypto/aes_wrap.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_wrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/aes_wrap.o
 
$(BIN)/aes_wrap.dbg%.o : crypto/aes_wrap.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_wrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/aes_wrap.dbg%.o
 
$(BIN)/aes_wrap.c : crypto/aes_wrap.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_wrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/aes_wrap.c
 
$(BIN)/aes_wrap.s : crypto/aes_wrap.c $(MAKEDEPS) $(POST_O_DEPS) $(aes_wrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/aes_wrap.s
 
bin/deps/crypto/aes_wrap.c.d : $(aes_wrap_DEPS)
 
TAGS : $(aes_wrap_DEPS)

