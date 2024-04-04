tftp_DEPS = net/udp/tftp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/strings.h \
 arch/i386/include/limits.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/refcnt.h \
 include/gpxe/xfer.h include/gpxe/interface.h include/gpxe/iobuf.h \
 include/gpxe/list.h include/gpxe/open.h include/gpxe/tables.h \
 include/gpxe/socket.h include/gpxe/uri.h include/gpxe/tcpip.h \
 include/gpxe/in.h include/gpxe/retry.h include/gpxe/features.h \
 include/gpxe/dhcp.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/bitmap.h include/gpxe/tftp.h

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

include/gpxe/refcnt.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/uri.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/retry.h:

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

include/gpxe/bitmap.h:

include/gpxe/tftp.h:

$(BIN)/tftp.o : net/udp/tftp.c $(MAKEDEPS) $(POST_O_DEPS) $(tftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tftp.o
 
$(BIN)/tftp.dbg%.o : net/udp/tftp.c $(MAKEDEPS) $(POST_O_DEPS) $(tftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tftp.dbg%.o
 
$(BIN)/tftp.c : net/udp/tftp.c $(MAKEDEPS) $(POST_O_DEPS) $(tftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tftp.c
 
$(BIN)/tftp.s : net/udp/tftp.c $(MAKEDEPS) $(POST_O_DEPS) $(tftp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tftp.s
 
bin/deps/net/udp/tftp.c.d : $(tftp_DEPS)
 
TAGS : $(tftp_DEPS)

