mtd80x_DEPS = drivers/net/mtd80x.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/etherboot.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/unistd.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/strings.h arch/i386/include/limits.h include/string.h \
 arch/x86/include/bits/string.h include/console.h include/gpxe/tables.h \
 include/gpxe/if_arp.h include/gpxe/if_ether.h include/nic.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/pci_io.h config/ioapi.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/isapnp.h include/gpxe/isa_ids.h include/gpxe/isa.h \
 include/gpxe/eisa.h include/gpxe/mca.h include/gpxe/ethernet.h \
 include/mii.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/etherboot.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/unistd.h:

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

include/strings.h:

arch/i386/include/limits.h:

include/string.h:

arch/x86/include/bits/string.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/if_arp.h:

include/gpxe/if_ether.h:

include/nic.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

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

include/gpxe/isapnp.h:

include/gpxe/isa_ids.h:

include/gpxe/isa.h:

include/gpxe/eisa.h:

include/gpxe/mca.h:

include/gpxe/ethernet.h:

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

$(BIN)/mtd80x.o : drivers/net/mtd80x.c $(MAKEDEPS) $(POST_O_DEPS) $(mtd80x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/mtd80x.o
 
$(BIN)/mtd80x.dbg%.o : drivers/net/mtd80x.c $(MAKEDEPS) $(POST_O_DEPS) $(mtd80x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/mtd80x.dbg%.o
 
$(BIN)/mtd80x.c : drivers/net/mtd80x.c $(MAKEDEPS) $(POST_O_DEPS) $(mtd80x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/mtd80x.c
 
$(BIN)/mtd80x.s : drivers/net/mtd80x.c $(MAKEDEPS) $(POST_O_DEPS) $(mtd80x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/mtd80x.s
 
bin/deps/drivers/net/mtd80x.c.d : $(mtd80x_DEPS)
 
TAGS : $(mtd80x_DEPS)


# NIC	
# NIC	family	drivers/net/mtd80x
DRIVERS += mtd80x

# NIC	MTD800	1516,0800	Myson MTD800
DRIVER_MTD800 = mtd80x
ROM_TYPE_MTD800 = pci
ROM_DESCRIPTION_MTD800 = "Myson MTD800"
PCI_VENDOR_MTD800 = 0x1516
PCI_DEVICE_MTD800 = 0x0800
ROMS += MTD800
ROMS_mtd80x += MTD800

# NIC	15160800	1516,0800	Myson MTD800
DRIVER_15160800 = mtd80x
ROM_TYPE_15160800 = pci
ROM_DESCRIPTION_15160800 = "Myson MTD800"
PCI_VENDOR_15160800 = 0x1516
PCI_DEVICE_15160800 = 0x0800
ROMS += 15160800
ROMS_mtd80x += 15160800

# NIC	MTD803	1516,0803	Surecom EP-320X
DRIVER_MTD803 = mtd80x
ROM_TYPE_MTD803 = pci
ROM_DESCRIPTION_MTD803 = "Surecom EP-320X"
PCI_VENDOR_MTD803 = 0x1516
PCI_DEVICE_MTD803 = 0x0803
ROMS += MTD803
ROMS_mtd80x += MTD803

# NIC	15160803	1516,0803	Surecom EP-320X
DRIVER_15160803 = mtd80x
ROM_TYPE_15160803 = pci
ROM_DESCRIPTION_15160803 = "Surecom EP-320X"
PCI_VENDOR_15160803 = 0x1516
PCI_DEVICE_15160803 = 0x0803
ROMS += 15160803
ROMS_mtd80x += 15160803

# NIC	MTD891	1516,0891	Myson MTD891
DRIVER_MTD891 = mtd80x
ROM_TYPE_MTD891 = pci
ROM_DESCRIPTION_MTD891 = "Myson MTD891"
PCI_VENDOR_MTD891 = 0x1516
PCI_DEVICE_MTD891 = 0x0891
ROMS += MTD891
ROMS_mtd80x += MTD891

# NIC	15160891	1516,0891	Myson MTD891
DRIVER_15160891 = mtd80x
ROM_TYPE_15160891 = pci
ROM_DESCRIPTION_15160891 = "Myson MTD891"
PCI_VENDOR_15160891 = 0x1516
PCI_DEVICE_15160891 = 0x0891
ROMS += 15160891
ROMS_mtd80x += 15160891
