srp_DEPS = drivers/block/srp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/scsi.h include/gpxe/blockdev.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/refcnt.h include/gpxe/xfer.h include/stdarg.h \
 include/gpxe/interface.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/features.h include/gpxe/tables.h include/gpxe/dhcp.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/ib_srp.h \
 include/gpxe/infiniband.h include/gpxe/device.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/srp.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/gpxe/refcnt.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/features.h:

include/gpxe/tables.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/ib_srp.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/srp.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

$(BIN)/srp.o : drivers/block/srp.c $(MAKEDEPS) $(POST_O_DEPS) $(srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/srp.o
 
$(BIN)/srp.dbg%.o : drivers/block/srp.c $(MAKEDEPS) $(POST_O_DEPS) $(srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/srp.dbg%.o
 
$(BIN)/srp.c : drivers/block/srp.c $(MAKEDEPS) $(POST_O_DEPS) $(srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/srp.c
 
$(BIN)/srp.s : drivers/block/srp.c $(MAKEDEPS) $(POST_O_DEPS) $(srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/srp.s
 
bin/deps/drivers/block/srp.c.d : $(srp_DEPS)
 
TAGS : $(srp_DEPS)

