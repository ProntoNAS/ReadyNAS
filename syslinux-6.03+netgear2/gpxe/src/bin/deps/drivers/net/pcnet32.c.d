pcnet32_DEPS = drivers/net/pcnet32.c include/compiler.h \
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

$(BIN)/pcnet32.o : drivers/net/pcnet32.c $(MAKEDEPS) $(POST_O_DEPS) $(pcnet32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pcnet32.o
 
$(BIN)/pcnet32.dbg%.o : drivers/net/pcnet32.c $(MAKEDEPS) $(POST_O_DEPS) $(pcnet32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pcnet32.dbg%.o
 
$(BIN)/pcnet32.c : drivers/net/pcnet32.c $(MAKEDEPS) $(POST_O_DEPS) $(pcnet32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pcnet32.c
 
$(BIN)/pcnet32.s : drivers/net/pcnet32.c $(MAKEDEPS) $(POST_O_DEPS) $(pcnet32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pcnet32.s
 
bin/deps/drivers/net/pcnet32.c.d : $(pcnet32_DEPS)
 
TAGS : $(pcnet32_DEPS)


# NIC	
# NIC	family	drivers/net/pcnet32
DRIVERS += pcnet32

# NIC	pcnet32	1022,2000	AMD PCnet/PCI
DRIVER_pcnet32 = pcnet32
ROM_TYPE_pcnet32 = pci
ROM_DESCRIPTION_pcnet32 = "AMD PCnet/PCI"
PCI_VENDOR_pcnet32 = 0x1022
PCI_DEVICE_pcnet32 = 0x2000
ROMS += pcnet32
ROMS_pcnet32 += pcnet32

# NIC	10222000	1022,2000	AMD PCnet/PCI
DRIVER_10222000 = pcnet32
ROM_TYPE_10222000 = pci
ROM_DESCRIPTION_10222000 = "AMD PCnet/PCI"
PCI_VENDOR_10222000 = 0x1022
PCI_DEVICE_10222000 = 0x2000
ROMS += 10222000
ROMS_pcnet32 += 10222000

# NIC	pcnetfastiii	1022,2625	AMD PCNet FAST III
DRIVER_pcnetfastiii = pcnet32
ROM_TYPE_pcnetfastiii = pci
ROM_DESCRIPTION_pcnetfastiii = "AMD PCNet FAST III"
PCI_VENDOR_pcnetfastiii = 0x1022
PCI_DEVICE_pcnetfastiii = 0x2625
ROMS += pcnetfastiii
ROMS_pcnet32 += pcnetfastiii

# NIC	10222625	1022,2625	AMD PCNet FAST III
DRIVER_10222625 = pcnet32
ROM_TYPE_10222625 = pci
ROM_DESCRIPTION_10222625 = "AMD PCNet FAST III"
PCI_VENDOR_10222625 = 0x1022
PCI_DEVICE_10222625 = 0x2625
ROMS += 10222625
ROMS_pcnet32 += 10222625

# NIC	amdhomepna	1022,2001	AMD PCnet/HomePNA
DRIVER_amdhomepna = pcnet32
ROM_TYPE_amdhomepna = pci
ROM_DESCRIPTION_amdhomepna = "AMD PCnet/HomePNA"
PCI_VENDOR_amdhomepna = 0x1022
PCI_DEVICE_amdhomepna = 0x2001
ROMS += amdhomepna
ROMS_pcnet32 += amdhomepna

# NIC	10222001	1022,2001	AMD PCnet/HomePNA
DRIVER_10222001 = pcnet32
ROM_TYPE_10222001 = pci
ROM_DESCRIPTION_10222001 = "AMD PCnet/HomePNA"
PCI_VENDOR_10222001 = 0x1022
PCI_DEVICE_10222001 = 0x2001
ROMS += 10222001
ROMS_pcnet32 += 10222001
