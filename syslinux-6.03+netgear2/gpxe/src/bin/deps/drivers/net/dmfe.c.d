dmfe_DEPS = drivers/net/dmfe.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h include/gpxe/ethernet.h

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

$(BIN)/dmfe.o : drivers/net/dmfe.c $(MAKEDEPS) $(POST_O_DEPS) $(dmfe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dmfe.o
 
$(BIN)/dmfe.dbg%.o : drivers/net/dmfe.c $(MAKEDEPS) $(POST_O_DEPS) $(dmfe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dmfe.dbg%.o
 
$(BIN)/dmfe.c : drivers/net/dmfe.c $(MAKEDEPS) $(POST_O_DEPS) $(dmfe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dmfe.c
 
$(BIN)/dmfe.s : drivers/net/dmfe.c $(MAKEDEPS) $(POST_O_DEPS) $(dmfe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dmfe.s
 
bin/deps/drivers/net/dmfe.c.d : $(dmfe_DEPS)
 
TAGS : $(dmfe_DEPS)


# NIC	
# NIC	family	drivers/net/dmfe
DRIVERS += dmfe

# NIC	dmfe9100	1282,9100	Davicom 9100
DRIVER_dmfe9100 = dmfe
ROM_TYPE_dmfe9100 = pci
ROM_DESCRIPTION_dmfe9100 = "Davicom 9100"
PCI_VENDOR_dmfe9100 = 0x1282
PCI_DEVICE_dmfe9100 = 0x9100
ROMS += dmfe9100
ROMS_dmfe += dmfe9100

# NIC	12829100	1282,9100	Davicom 9100
DRIVER_12829100 = dmfe
ROM_TYPE_12829100 = pci
ROM_DESCRIPTION_12829100 = "Davicom 9100"
PCI_VENDOR_12829100 = 0x1282
PCI_DEVICE_12829100 = 0x9100
ROMS += 12829100
ROMS_dmfe += 12829100

# NIC	dmfe9102	1282,9102	Davicom 9102
DRIVER_dmfe9102 = dmfe
ROM_TYPE_dmfe9102 = pci
ROM_DESCRIPTION_dmfe9102 = "Davicom 9102"
PCI_VENDOR_dmfe9102 = 0x1282
PCI_DEVICE_dmfe9102 = 0x9102
ROMS += dmfe9102
ROMS_dmfe += dmfe9102

# NIC	12829102	1282,9102	Davicom 9102
DRIVER_12829102 = dmfe
ROM_TYPE_12829102 = pci
ROM_DESCRIPTION_12829102 = "Davicom 9102"
PCI_VENDOR_12829102 = 0x1282
PCI_DEVICE_12829102 = 0x9102
ROMS += 12829102
ROMS_dmfe += 12829102

# NIC	dmfe9009	1282,9009	Davicom 9009
DRIVER_dmfe9009 = dmfe
ROM_TYPE_dmfe9009 = pci
ROM_DESCRIPTION_dmfe9009 = "Davicom 9009"
PCI_VENDOR_dmfe9009 = 0x1282
PCI_DEVICE_dmfe9009 = 0x9009
ROMS += dmfe9009
ROMS_dmfe += dmfe9009

# NIC	12829009	1282,9009	Davicom 9009
DRIVER_12829009 = dmfe
ROM_TYPE_12829009 = pci
ROM_DESCRIPTION_12829009 = "Davicom 9009"
PCI_VENDOR_12829009 = 0x1282
PCI_DEVICE_12829009 = 0x9009
ROMS += 12829009
ROMS_dmfe += 12829009

# NIC	dmfe9132	1282,9132	Davicom 9132
DRIVER_dmfe9132 = dmfe
ROM_TYPE_dmfe9132 = pci
ROM_DESCRIPTION_dmfe9132 = "Davicom 9132"
PCI_VENDOR_dmfe9132 = 0x1282
PCI_DEVICE_dmfe9132 = 0x9132
ROMS += dmfe9132
ROMS_dmfe += dmfe9132

# NIC	12829132	1282,9132	Davicom 9132
DRIVER_12829132 = dmfe
ROM_TYPE_12829132 = pci
ROM_DESCRIPTION_12829132 = "Davicom 9132"
PCI_VENDOR_12829132 = 0x1282
PCI_DEVICE_12829132 = 0x9132
ROMS += 12829132
ROMS_dmfe += 12829132
