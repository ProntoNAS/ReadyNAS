iscsi_DEPS = net/tcp/iscsi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/vsprintf.h include/gpxe/socket.h \
 include/gpxe/xfer.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/open.h \
 include/gpxe/tables.h include/gpxe/scsi.h include/gpxe/blockdev.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/process.h include/gpxe/tcpip.h \
 include/gpxe/in.h include/gpxe/settings.h include/gpxe/features.h \
 include/gpxe/dhcp.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/iscsi.h include/gpxe/chap.h include/gpxe/md5.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/vsprintf.h:

include/gpxe/socket.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/process.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/settings.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/iscsi.h:

include/gpxe/chap.h:

include/gpxe/md5.h:

$(BIN)/iscsi.o : net/tcp/iscsi.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iscsi.o
 
$(BIN)/iscsi.dbg%.o : net/tcp/iscsi.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iscsi.dbg%.o
 
$(BIN)/iscsi.c : net/tcp/iscsi.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iscsi.c
 
$(BIN)/iscsi.s : net/tcp/iscsi.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iscsi.s
 
bin/deps/net/tcp/iscsi.c.d : $(iscsi_DEPS)
 
TAGS : $(iscsi_DEPS)

