etherfabric_DEPS = drivers/net/etherfabric.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/unistd.h include/stddef.h include/stdarg.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/console.h include/gpxe/tables.h include/gpxe/io.h config/ioapi.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/malloc.h include/gpxe/ethernet.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/mii.h drivers/net/etherfabric.h drivers/net/etherfabric_nic.h \
 include/gpxe/bitbash.h include/gpxe/i2c.h include/gpxe/spi.h \
 include/gpxe/nvs.h include/gpxe/nvo.h include/gpxe/dhcpopts.h \
 include/gpxe/if_ether.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/unistd.h:

include/stddef.h:

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

include/console.h:

include/gpxe/tables.h:

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/malloc.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/mii.h:

drivers/net/etherfabric.h:

drivers/net/etherfabric_nic.h:

include/gpxe/bitbash.h:

include/gpxe/i2c.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

include/gpxe/nvo.h:

include/gpxe/dhcpopts.h:

include/gpxe/if_ether.h:

$(BIN)/etherfabric.o : drivers/net/etherfabric.c $(MAKEDEPS) $(POST_O_DEPS) $(etherfabric_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/etherfabric.o
 
$(BIN)/etherfabric.dbg%.o : drivers/net/etherfabric.c $(MAKEDEPS) $(POST_O_DEPS) $(etherfabric_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/etherfabric.dbg%.o
 
$(BIN)/etherfabric.c : drivers/net/etherfabric.c $(MAKEDEPS) $(POST_O_DEPS) $(etherfabric_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/etherfabric.c
 
$(BIN)/etherfabric.s : drivers/net/etherfabric.c $(MAKEDEPS) $(POST_O_DEPS) $(etherfabric_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/etherfabric.s
 
bin/deps/drivers/net/etherfabric.c.d : $(etherfabric_DEPS)
 
TAGS : $(etherfabric_DEPS)


# NIC	
# NIC	family	drivers/net/etherfabric
DRIVERS += etherfabric

# NIC	falcon	1924,0703	EtherFabric Falcon
DRIVER_falcon = etherfabric
ROM_TYPE_falcon = pci
ROM_DESCRIPTION_falcon = "EtherFabric Falcon"
PCI_VENDOR_falcon = 0x1924
PCI_DEVICE_falcon = 0x0703
ROMS += falcon
ROMS_etherfabric += falcon

# NIC	19240703	1924,0703	EtherFabric Falcon
DRIVER_19240703 = etherfabric
ROM_TYPE_19240703 = pci
ROM_DESCRIPTION_19240703 = "EtherFabric Falcon"
PCI_VENDOR_19240703 = 0x1924
PCI_DEVICE_19240703 = 0x0703
ROMS += 19240703
ROMS_etherfabric += 19240703

# NIC	falconb0	1924,0710	EtherFabric FalconB0
DRIVER_falconb0 = etherfabric
ROM_TYPE_falconb0 = pci
ROM_DESCRIPTION_falconb0 = "EtherFabric FalconB0"
PCI_VENDOR_falconb0 = 0x1924
PCI_DEVICE_falconb0 = 0x0710
ROMS += falconb0
ROMS_etherfabric += falconb0

# NIC	19240710	1924,0710	EtherFabric FalconB0
DRIVER_19240710 = etherfabric
ROM_TYPE_19240710 = pci
ROM_DESCRIPTION_19240710 = "EtherFabric FalconB0"
PCI_VENDOR_19240710 = 0x1924
PCI_DEVICE_19240710 = 0x0710
ROMS += 19240710
ROMS_etherfabric += 19240710
