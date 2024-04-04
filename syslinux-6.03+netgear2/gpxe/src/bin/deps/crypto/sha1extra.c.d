sha1extra_DEPS = crypto/sha1extra.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/crypto.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/gpxe/sha1.h \
 crypto/axtls/crypto.h crypto/axtls/bigint.h crypto/axtls/os_port.h \
 include/stdlib.h include/assert.h include/time.h include/sys/time.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 crypto/axtls/bigint_impl.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/hmac.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/crypto.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/gpxe/sha1.h:

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

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/hmac.h:

$(BIN)/sha1extra.o : crypto/sha1extra.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1extra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sha1extra.o
 
$(BIN)/sha1extra.dbg%.o : crypto/sha1extra.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1extra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sha1extra.dbg%.o
 
$(BIN)/sha1extra.c : crypto/sha1extra.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1extra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sha1extra.c
 
$(BIN)/sha1extra.s : crypto/sha1extra.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1extra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sha1extra.s
 
bin/deps/crypto/sha1extra.c.d : $(sha1extra_DEPS)
 
TAGS : $(sha1extra_DEPS)

