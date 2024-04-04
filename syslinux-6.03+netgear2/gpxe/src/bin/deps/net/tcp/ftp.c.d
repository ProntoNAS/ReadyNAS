ftp_DEPS = net/tcp/ftp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/socket.h include/gpxe/tcpip.h include/gpxe/in.h \
 include/gpxe/tables.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/refcnt.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/open.h include/gpxe/uri.h include/gpxe/features.h \
 include/gpxe/dhcp.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/ftp.h

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/socket.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/tables.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/uri.h:

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

include/gpxe/ftp.h:

$(BIN)/ftp.o : net/tcp/ftp.c $(MAKEDEPS) $(POST_O_DEPS) $(ftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ftp.o
 
$(BIN)/ftp.dbg%.o : net/tcp/ftp.c $(MAKEDEPS) $(POST_O_DEPS) $(ftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ftp.dbg%.o
 
$(BIN)/ftp.c : net/tcp/ftp.c $(MAKEDEPS) $(POST_O_DEPS) $(ftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ftp.c
 
$(BIN)/ftp.s : net/tcp/ftp.c $(MAKEDEPS) $(POST_O_DEPS) $(ftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ftp.s
 
bin/deps/net/tcp/ftp.c.d : $(ftp_DEPS)
 
TAGS : $(ftp_DEPS)

