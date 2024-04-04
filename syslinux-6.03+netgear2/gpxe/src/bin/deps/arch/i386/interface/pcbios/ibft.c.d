ibft_DEPS = arch/i386/interface/pcbios/ibft.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/list.h include/assert.h include/gpxe/tables.h \
 include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/acpi.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/ethernet.h include/gpxe/dhcp.h include/gpxe/uuid.h \
 include/gpxe/iscsi.h include/gpxe/scsi.h include/gpxe/blockdev.h \
 include/gpxe/chap.h include/gpxe/md5.h include/gpxe/xfer.h \
 include/gpxe/interface.h include/gpxe/iobuf.h include/gpxe/process.h \
 arch/i386/include/gpxe/ibft.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/gpxe/acpi.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/ethernet.h:

include/gpxe/dhcp.h:

include/gpxe/uuid.h:

include/gpxe/iscsi.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

include/gpxe/chap.h:

include/gpxe/md5.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/process.h:

arch/i386/include/gpxe/ibft.h:

$(BIN)/ibft.o : arch/i386/interface/pcbios/ibft.c $(MAKEDEPS) $(POST_O_DEPS) $(ibft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ibft.o
 
$(BIN)/ibft.dbg%.o : arch/i386/interface/pcbios/ibft.c $(MAKEDEPS) $(POST_O_DEPS) $(ibft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ibft.dbg%.o
 
$(BIN)/ibft.c : arch/i386/interface/pcbios/ibft.c $(MAKEDEPS) $(POST_O_DEPS) $(ibft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ibft.c
 
$(BIN)/ibft.s : arch/i386/interface/pcbios/ibft.c $(MAKEDEPS) $(POST_O_DEPS) $(ibft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ibft.s
 
bin/deps/arch/i386/interface/pcbios/ibft.c.d : $(ibft_DEPS)
 
TAGS : $(ibft_DEPS)

