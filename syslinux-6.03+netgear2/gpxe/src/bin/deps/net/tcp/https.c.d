https_DEPS = net/tcp/https.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/open.h include/stdarg.h \
 include/gpxe/tables.h include/gpxe/socket.h include/gpxe/tls.h \
 include/gpxe/refcnt.h include/gpxe/filter.h include/gpxe/xfer.h \
 include/gpxe/interface.h include/gpxe/iobuf.h include/assert.h \
 include/gpxe/list.h include/gpxe/process.h include/gpxe/crypto.h \
 include/gpxe/md5.h include/gpxe/sha1.h crypto/axtls/crypto.h \
 crypto/axtls/bigint.h crypto/axtls/os_port.h include/stdlib.h \
 include/time.h include/sys/time.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h crypto/axtls/bigint_impl.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/x509.h include/gpxe/http.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/uuid.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/open.h:

include/stdarg.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/tls.h:

include/gpxe/refcnt.h:

include/gpxe/filter.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/process.h:

include/gpxe/crypto.h:

include/gpxe/md5.h:

include/gpxe/sha1.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdlib.h:

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

include/gpxe/x509.h:

include/gpxe/http.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/https.o : net/tcp/https.c $(MAKEDEPS) $(POST_O_DEPS) $(https_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/https.o
 
$(BIN)/https.dbg%.o : net/tcp/https.c $(MAKEDEPS) $(POST_O_DEPS) $(https_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/https.dbg%.o
 
$(BIN)/https.c : net/tcp/https.c $(MAKEDEPS) $(POST_O_DEPS) $(https_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/https.c
 
$(BIN)/https.s : net/tcp/https.c $(MAKEDEPS) $(POST_O_DEPS) $(https_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/https.s
 
bin/deps/net/tcp/https.c.d : $(https_DEPS)
 
TAGS : $(https_DEPS)

