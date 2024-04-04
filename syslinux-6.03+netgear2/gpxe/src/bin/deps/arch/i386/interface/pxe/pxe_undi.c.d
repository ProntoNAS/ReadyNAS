pxe_undi_DEPS = arch/i386/interface/pxe/pxe_undi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 arch/i386/include/basemem_packet.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/netdevice.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/device.h \
 include/gpxe/pci.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/if_ether.h include/gpxe/ip.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/retry.h include/gpxe/arp.h \
 include/gpxe/rarp.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

arch/i386/include/basemem_packet.h:

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

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/device.h:

include/gpxe/pci.h:

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

include/gpxe/if_ether.h:

include/gpxe/ip.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/retry.h:

include/gpxe/arp.h:

include/gpxe/rarp.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

$(BIN)/pxe_undi.o : arch/i386/interface/pxe/pxe_undi.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_undi.o
 
$(BIN)/pxe_undi.dbg%.o : arch/i386/interface/pxe/pxe_undi.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_undi.dbg%.o
 
$(BIN)/pxe_undi.c : arch/i386/interface/pxe/pxe_undi.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_undi.c
 
$(BIN)/pxe_undi.s : arch/i386/interface/pxe/pxe_undi.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_undi.s
 
bin/deps/arch/i386/interface/pxe/pxe_undi.c.d : $(pxe_undi_DEPS)
 
TAGS : $(pxe_undi_DEPS)

