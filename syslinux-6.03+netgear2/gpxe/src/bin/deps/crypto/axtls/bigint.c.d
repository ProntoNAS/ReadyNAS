bigint_DEPS = crypto/axtls/bigint.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h \
 arch/i386/include/limits.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/time.h crypto/axtls/bigint.h crypto/axtls/os_port.h \
 include/sys/time.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h crypto/axtls/bigint_impl.h crypto/axtls/crypto.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

arch/i386/include/limits.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/time.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

crypto/axtls/crypto.h:

$(BIN)/bigint.o : crypto/axtls/bigint.c $(MAKEDEPS) $(POST_O_DEPS) $(bigint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bigint.o
 
$(BIN)/bigint.dbg%.o : crypto/axtls/bigint.c $(MAKEDEPS) $(POST_O_DEPS) $(bigint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bigint.dbg%.o
 
$(BIN)/bigint.c : crypto/axtls/bigint.c $(MAKEDEPS) $(POST_O_DEPS) $(bigint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bigint.c
 
$(BIN)/bigint.s : crypto/axtls/bigint.c $(MAKEDEPS) $(POST_O_DEPS) $(bigint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bigint.s
 
bin/deps/crypto/axtls/bigint.c.d : $(bigint_DEPS)
 
TAGS : $(bigint_DEPS)

