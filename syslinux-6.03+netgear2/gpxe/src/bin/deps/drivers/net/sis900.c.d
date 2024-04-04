sis900_DEPS = drivers/net/sis900.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/etherboot.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/unistd.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/strings.h arch/i386/include/limits.h include/string.h \
 arch/x86/include/bits/string.h include/console.h include/gpxe/tables.h \
 include/gpxe/if_arp.h include/gpxe/if_ether.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/list.h include/gpxe/pci_io.h \
 config/ioapi.h include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h include/nic.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/isapnp.h include/gpxe/isa_ids.h include/gpxe/isa.h \
 include/gpxe/eisa.h include/gpxe/mca.h drivers/net/sis900.h

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

include/nic.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/isapnp.h:

include/gpxe/isa_ids.h:

include/gpxe/isa.h:

include/gpxe/eisa.h:

include/gpxe/mca.h:

drivers/net/sis900.h:

$(BIN)/sis900.o : drivers/net/sis900.c $(MAKEDEPS) $(POST_O_DEPS) $(sis900_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sis900.o
 
$(BIN)/sis900.dbg%.o : drivers/net/sis900.c $(MAKEDEPS) $(POST_O_DEPS) $(sis900_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sis900.dbg%.o
 
$(BIN)/sis900.c : drivers/net/sis900.c $(MAKEDEPS) $(POST_O_DEPS) $(sis900_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sis900.c
 
$(BIN)/sis900.s : drivers/net/sis900.c $(MAKEDEPS) $(POST_O_DEPS) $(sis900_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sis900.s
 
bin/deps/drivers/net/sis900.c.d : $(sis900_DEPS)
 
TAGS : $(sis900_DEPS)


# NIC	
# NIC	family	drivers/net/sis900
DRIVERS += sis900

# NIC	sis900	1039,0900	SIS900
DRIVER_sis900 = sis900
ROM_TYPE_sis900 = pci
ROM_DESCRIPTION_sis900 = "SIS900"
PCI_VENDOR_sis900 = 0x1039
PCI_DEVICE_sis900 = 0x0900
ROMS += sis900
ROMS_sis900 += sis900

# NIC	10390900	1039,0900	SIS900
DRIVER_10390900 = sis900
ROM_TYPE_10390900 = pci
ROM_DESCRIPTION_10390900 = "SIS900"
PCI_VENDOR_10390900 = 0x1039
PCI_DEVICE_10390900 = 0x0900
ROMS += 10390900
ROMS_sis900 += 10390900

# NIC	sis7016	1039,7016	SIS7016
DRIVER_sis7016 = sis900
ROM_TYPE_sis7016 = pci
ROM_DESCRIPTION_sis7016 = "SIS7016"
PCI_VENDOR_sis7016 = 0x1039
PCI_DEVICE_sis7016 = 0x7016
ROMS += sis7016
ROMS_sis900 += sis7016

# NIC	10397016	1039,7016	SIS7016
DRIVER_10397016 = sis900
ROM_TYPE_10397016 = pci
ROM_DESCRIPTION_10397016 = "SIS7016"
PCI_VENDOR_10397016 = 0x1039
PCI_DEVICE_10397016 = 0x7016
ROMS += 10397016
ROMS_sis900 += 10397016
