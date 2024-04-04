phantom_DEPS = drivers/net/phantom/phantom.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/unistd.h include/stdarg.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/pci_io.h config/ioapi.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/malloc.h include/gpxe/iobuf.h include/gpxe/netdevice.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/if_ether.h \
 include/gpxe/ethernet.h include/gpxe/spi.h include/gpxe/nvs.h \
 drivers/net/phantom/phantom.h drivers/net/phantom/nx_bitops.h \
 drivers/net/phantom/phantom_hw.h \
 drivers/net/phantom/nxhal_nic_interface.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/unistd.h:

include/stdarg.h:

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

config/ioapi.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/gpxe/malloc.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

drivers/net/phantom/phantom.h:

drivers/net/phantom/nx_bitops.h:

drivers/net/phantom/phantom_hw.h:

drivers/net/phantom/nxhal_nic_interface.h:

$(BIN)/phantom.o : drivers/net/phantom/phantom.c $(MAKEDEPS) $(POST_O_DEPS) $(phantom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/phantom.o
 
$(BIN)/phantom.dbg%.o : drivers/net/phantom/phantom.c $(MAKEDEPS) $(POST_O_DEPS) $(phantom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/phantom.dbg%.o
 
$(BIN)/phantom.c : drivers/net/phantom/phantom.c $(MAKEDEPS) $(POST_O_DEPS) $(phantom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/phantom.c
 
$(BIN)/phantom.s : drivers/net/phantom/phantom.c $(MAKEDEPS) $(POST_O_DEPS) $(phantom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/phantom.s
 
bin/deps/drivers/net/phantom/phantom.c.d : $(phantom_DEPS)
 
TAGS : $(phantom_DEPS)


# NIC	
# NIC	family	drivers/net/phantom/phantom
DRIVERS += phantom

# NIC	nx	4040,0100	NX
DRIVER_nx = phantom
ROM_TYPE_nx = pci
ROM_DESCRIPTION_nx = "NX"
PCI_VENDOR_nx = 0x4040
PCI_DEVICE_nx = 0x0100
ROMS += nx
ROMS_phantom += nx

# NIC	40400100	4040,0100	NX
DRIVER_40400100 = phantom
ROM_TYPE_40400100 = pci
ROM_DESCRIPTION_40400100 = "NX"
PCI_VENDOR_40400100 = 0x4040
PCI_DEVICE_40400100 = 0x0100
ROMS += 40400100
ROMS_phantom += 40400100
