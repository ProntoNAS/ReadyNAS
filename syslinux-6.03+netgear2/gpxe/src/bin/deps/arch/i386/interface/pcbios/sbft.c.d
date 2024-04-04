sbft_DEPS = arch/i386/interface/pcbios/sbft.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/assert.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/srp.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/xfer.h \
 include/stdarg.h include/gpxe/interface.h include/gpxe/refcnt.h \
 include/gpxe/scsi.h include/gpxe/blockdev.h include/gpxe/ib_srp.h \
 include/gpxe/infiniband.h include/gpxe/device.h include/gpxe/tables.h \
 include/gpxe/ib_packet.h include/gpxe/ib_mad.h include/gpxe/acpi.h \
 arch/i386/include/gpxe/sbft.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/assert.h:

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/srp.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

include/gpxe/ib_srp.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/acpi.h:

arch/i386/include/gpxe/sbft.h:

$(BIN)/sbft.o : arch/i386/interface/pcbios/sbft.c $(MAKEDEPS) $(POST_O_DEPS) $(sbft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sbft.o
 
$(BIN)/sbft.dbg%.o : arch/i386/interface/pcbios/sbft.c $(MAKEDEPS) $(POST_O_DEPS) $(sbft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sbft.dbg%.o
 
$(BIN)/sbft.c : arch/i386/interface/pcbios/sbft.c $(MAKEDEPS) $(POST_O_DEPS) $(sbft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sbft.c
 
$(BIN)/sbft.s : arch/i386/interface/pcbios/sbft.c $(MAKEDEPS) $(POST_O_DEPS) $(sbft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sbft.s
 
bin/deps/arch/i386/interface/pcbios/sbft.c.d : $(sbft_DEPS)
 
TAGS : $(sbft_DEPS)

