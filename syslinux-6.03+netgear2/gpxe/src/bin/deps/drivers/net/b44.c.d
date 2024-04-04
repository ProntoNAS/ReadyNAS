b44_DEPS = drivers/net/b44.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h include/stdio.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdarg.h \
 include/unistd.h include/stddef.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/io.h config/ioapi.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/mii.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/iobuf.h include/gpxe/malloc.h \
 include/stdlib.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/ethernet.h include/gpxe/if_ether.h include/gpxe/memmap.h \
 drivers/net/b44.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/unistd.h:

include/stddef.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

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

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

include/gpxe/malloc.h:

include/stdlib.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/memmap.h:

drivers/net/b44.h:

$(BIN)/b44.o : drivers/net/b44.c $(MAKEDEPS) $(POST_O_DEPS) $(b44_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/b44.o
 
$(BIN)/b44.dbg%.o : drivers/net/b44.c $(MAKEDEPS) $(POST_O_DEPS) $(b44_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/b44.dbg%.o
 
$(BIN)/b44.c : drivers/net/b44.c $(MAKEDEPS) $(POST_O_DEPS) $(b44_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/b44.c
 
$(BIN)/b44.s : drivers/net/b44.c $(MAKEDEPS) $(POST_O_DEPS) $(b44_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/b44.s
 
bin/deps/drivers/net/b44.c.d : $(b44_DEPS)
 
TAGS : $(b44_DEPS)


# NIC	
# NIC	family	drivers/net/b44
DRIVERS += b44

# NIC	BCM4401	14e4,4401	BCM4401
DRIVER_BCM4401 = b44
ROM_TYPE_BCM4401 = pci
ROM_DESCRIPTION_BCM4401 = "BCM4401"
PCI_VENDOR_BCM4401 = 0x14e4
PCI_DEVICE_BCM4401 = 0x4401
ROMS += BCM4401
ROMS_b44 += BCM4401

# NIC	14e44401	14e4,4401	BCM4401
DRIVER_14e44401 = b44
ROM_TYPE_14e44401 = pci
ROM_DESCRIPTION_14e44401 = "BCM4401"
PCI_VENDOR_14e44401 = 0x14e4
PCI_DEVICE_14e44401 = 0x4401
ROMS += 14e44401
ROMS_b44 += 14e44401

# NIC	BCM4401-B0	14e4,170c	BCM4401-B0
DRIVER_BCM4401-B0 = b44
ROM_TYPE_BCM4401-B0 = pci
ROM_DESCRIPTION_BCM4401-B0 = "BCM4401-B0"
PCI_VENDOR_BCM4401-B0 = 0x14e4
PCI_DEVICE_BCM4401-B0 = 0x170c
ROMS += BCM4401-B0
ROMS_b44 += BCM4401-B0

# NIC	14e4170c	14e4,170c	BCM4401-B0
DRIVER_14e4170c = b44
ROM_TYPE_14e4170c = pci
ROM_DESCRIPTION_14e4170c = "BCM4401-B0"
PCI_VENDOR_14e4170c = 0x14e4
PCI_DEVICE_14e4170c = 0x170c
ROMS += 14e4170c
ROMS_b44 += 14e4170c

# NIC	BCM4401-B1	14e4,4402	BCM4401-B1
DRIVER_BCM4401-B1 = b44
ROM_TYPE_BCM4401-B1 = pci
ROM_DESCRIPTION_BCM4401-B1 = "BCM4401-B1"
PCI_VENDOR_BCM4401-B1 = 0x14e4
PCI_DEVICE_BCM4401-B1 = 0x4402
ROMS += BCM4401-B1
ROMS_b44 += BCM4401-B1

# NIC	14e44402	14e4,4402	BCM4401-B1
DRIVER_14e44402 = b44
ROM_TYPE_14e44402 = pci
ROM_DESCRIPTION_14e44402 = "BCM4401-B1"
PCI_VENDOR_14e44402 = 0x14e4
PCI_DEVICE_14e44402 = 0x4402
ROMS += 14e44402
ROMS_b44 += 14e44402
