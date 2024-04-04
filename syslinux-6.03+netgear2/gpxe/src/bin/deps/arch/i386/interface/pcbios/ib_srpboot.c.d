ib_srpboot_DEPS = arch/i386/interface/pcbios/ib_srpboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/sanboot.h \
 include/gpxe/tables.h arch/i386/include/int13.h include/gpxe/list.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/srp.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/iobuf.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/refcnt.h include/gpxe/scsi.h include/gpxe/blockdev.h \
 arch/i386/include/gpxe/sbft.h include/gpxe/acpi.h include/gpxe/ib_srp.h \
 include/gpxe/infiniband.h include/gpxe/device.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/sanboot.h:

include/gpxe/tables.h:

arch/i386/include/int13.h:

include/gpxe/list.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

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

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

arch/i386/include/gpxe/sbft.h:

include/gpxe/acpi.h:

include/gpxe/ib_srp.h:

include/gpxe/infiniband.h:

include/gpxe/device.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

$(BIN)/ib_srpboot.o : arch/i386/interface/pcbios/ib_srpboot.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srpboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_srpboot.o
 
$(BIN)/ib_srpboot.dbg%.o : arch/i386/interface/pcbios/ib_srpboot.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srpboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_srpboot.dbg%.o
 
$(BIN)/ib_srpboot.c : arch/i386/interface/pcbios/ib_srpboot.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srpboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_srpboot.c
 
$(BIN)/ib_srpboot.s : arch/i386/interface/pcbios/ib_srpboot.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_srpboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_srpboot.s
 
bin/deps/arch/i386/interface/pcbios/ib_srpboot.c.d : $(ib_srpboot_DEPS)
 
TAGS : $(ib_srpboot_DEPS)

