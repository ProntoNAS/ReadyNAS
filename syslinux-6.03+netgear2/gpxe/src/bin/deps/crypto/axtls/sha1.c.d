sha1_DEPS = crypto/axtls/sha1.c include/compiler.h \
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

$(BIN)/sha1.o : crypto/axtls/sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sha1.o
 
$(BIN)/sha1.dbg%.o : crypto/axtls/sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sha1.dbg%.o
 
$(BIN)/sha1.c : crypto/axtls/sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sha1.c
 
$(BIN)/sha1.s : crypto/axtls/sha1.c $(MAKEDEPS) $(POST_O_DEPS) $(sha1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sha1.s
 
bin/deps/crypto/axtls/sha1.c.d : $(sha1_DEPS)
 
TAGS : $(sha1_DEPS)

