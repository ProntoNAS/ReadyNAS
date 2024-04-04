tls_DEPS = net/tls.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/hmac.h include/gpxe/crypto.h \
 include/gpxe/md5.h include/gpxe/sha1.h crypto/axtls/crypto.h \
 crypto/axtls/bigint.h crypto/axtls/os_port.h include/time.h \
 include/sys/time.h crypto/axtls/bigint_impl.h include/gpxe/aes.h \
 include/gpxe/rsa.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/refcnt.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/open.h include/gpxe/tables.h include/gpxe/socket.h \
 include/gpxe/filter.h include/gpxe/asn1.h include/gpxe/x509.h \
 include/gpxe/tls.h include/gpxe/process.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/hmac.h:

include/gpxe/crypto.h:

include/gpxe/md5.h:

include/gpxe/sha1.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/time.h:

include/sys/time.h:

crypto/axtls/bigint_impl.h:

include/gpxe/aes.h:

include/gpxe/rsa.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/filter.h:

include/gpxe/asn1.h:

include/gpxe/x509.h:

include/gpxe/tls.h:

include/gpxe/process.h:

$(BIN)/tls.o : net/tls.c $(MAKEDEPS) $(POST_O_DEPS) $(tls_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tls.o
 
$(BIN)/tls.dbg%.o : net/tls.c $(MAKEDEPS) $(POST_O_DEPS) $(tls_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tls.dbg%.o
 
$(BIN)/tls.c : net/tls.c $(MAKEDEPS) $(POST_O_DEPS) $(tls_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tls.c
 
$(BIN)/tls.s : net/tls.c $(MAKEDEPS) $(POST_O_DEPS) $(tls_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tls.s
 
bin/deps/net/tls.c.d : $(tls_DEPS)
 
TAGS : $(tls_DEPS)

