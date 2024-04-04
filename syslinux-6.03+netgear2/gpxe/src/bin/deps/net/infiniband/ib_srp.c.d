ib_srp_DEPS = net/infiniband/ib_srp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/srp.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/stddef.h include/gpxe/xfer.h include/stdarg.h \
 include/gpxe/interface.h include/gpxe/refcnt.h include/gpxe/scsi.h \
 include/gpxe/blockdev.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/infiniband.h \
 include/gpxe/device.h include/gpxe/tables.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/ib_cmrc.h include/gpxe/ib_srp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/srp.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_cmrc.h:

include/gpxe/ib_srp.h:

$(BIN)/ib_srp.o : net/infiniband/ib_srp.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_srp.o
 
$(BIN)/ib_srp.dbg%.o : net/infiniband/ib_srp.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_srp.dbg%.o
 
$(BIN)/ib_srp.c : net/infiniband/ib_srp.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_srp.c
 
$(BIN)/ib_srp.s : net/infiniband/ib_srp.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_srp.s
 
bin/deps/net/infiniband/ib_srp.c.d : $(ib_srp_DEPS)
 
TAGS : $(ib_srp_DEPS)

