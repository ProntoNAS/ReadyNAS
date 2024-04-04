http_DEPS = net/tcp/http.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/strings.h \
 arch/i386/include/limits.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/uri.h \
 include/gpxe/refcnt.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/xfer.h include/gpxe/interface.h include/gpxe/open.h \
 include/gpxe/tables.h include/gpxe/socket.h include/gpxe/tcpip.h \
 include/gpxe/in.h include/gpxe/process.h include/gpxe/linebuf.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/base64.h include/gpxe/http.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/strings.h:

arch/i386/include/limits.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/uri.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/process.h:

include/gpxe/linebuf.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

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

include/gpxe/base64.h:

include/gpxe/http.h:

$(BIN)/http.o : net/tcp/http.c $(MAKEDEPS) $(POST_O_DEPS) $(http_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/http.o
 
$(BIN)/http.dbg%.o : net/tcp/http.c $(MAKEDEPS) $(POST_O_DEPS) $(http_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/http.dbg%.o
 
$(BIN)/http.c : net/tcp/http.c $(MAKEDEPS) $(POST_O_DEPS) $(http_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/http.c
 
$(BIN)/http.s : net/tcp/http.c $(MAKEDEPS) $(POST_O_DEPS) $(http_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/http.s
 
bin/deps/net/tcp/http.c.d : $(http_DEPS)
 
TAGS : $(http_DEPS)

