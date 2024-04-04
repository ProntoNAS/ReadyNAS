forcedeth_DEPS = drivers/net/forcedeth.c include/compiler.h \
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

$(BIN)/forcedeth.o : drivers/net/forcedeth.c $(MAKEDEPS) $(POST_O_DEPS) $(forcedeth_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/forcedeth.o
 
$(BIN)/forcedeth.dbg%.o : drivers/net/forcedeth.c $(MAKEDEPS) $(POST_O_DEPS) $(forcedeth_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/forcedeth.dbg%.o
 
$(BIN)/forcedeth.c : drivers/net/forcedeth.c $(MAKEDEPS) $(POST_O_DEPS) $(forcedeth_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/forcedeth.c
 
$(BIN)/forcedeth.s : drivers/net/forcedeth.c $(MAKEDEPS) $(POST_O_DEPS) $(forcedeth_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/forcedeth.s
 
bin/deps/drivers/net/forcedeth.c.d : $(forcedeth_DEPS)
 
TAGS : $(forcedeth_DEPS)


# NIC	
# NIC	family	drivers/net/forcedeth
DRIVERS += forcedeth

# NIC	nforce	10de,01c3	nForce NVENET_1 Ethernet Controller
DRIVER_nforce = forcedeth
ROM_TYPE_nforce = pci
ROM_DESCRIPTION_nforce = "nForce NVENET_1 Ethernet Controller"
PCI_VENDOR_nforce = 0x10de
PCI_DEVICE_nforce = 0x01c3
ROMS += nforce
ROMS_forcedeth += nforce

# NIC	10de01c3	10de,01c3	nForce NVENET_1 Ethernet Controller
DRIVER_10de01c3 = forcedeth
ROM_TYPE_10de01c3 = pci
ROM_DESCRIPTION_10de01c3 = "nForce NVENET_1 Ethernet Controller"
PCI_VENDOR_10de01c3 = 0x10de
PCI_DEVICE_10de01c3 = 0x01c3
ROMS += 10de01c3
ROMS_forcedeth += 10de01c3

# NIC	nforce2	10de,0066	nForce NVENET_2 Ethernet Controller
DRIVER_nforce2 = forcedeth
ROM_TYPE_nforce2 = pci
ROM_DESCRIPTION_nforce2 = "nForce NVENET_2 Ethernet Controller"
PCI_VENDOR_nforce2 = 0x10de
PCI_DEVICE_nforce2 = 0x0066
ROMS += nforce2
ROMS_forcedeth += nforce2

# NIC	10de0066	10de,0066	nForce NVENET_2 Ethernet Controller
DRIVER_10de0066 = forcedeth
ROM_TYPE_10de0066 = pci
ROM_DESCRIPTION_10de0066 = "nForce NVENET_2 Ethernet Controller"
PCI_VENDOR_10de0066 = 0x10de
PCI_DEVICE_10de0066 = 0x0066
ROMS += 10de0066
ROMS_forcedeth += 10de0066

# NIC	nforce3	10de,00d6	nForce NVENET_3 Ethernet Controller
DRIVER_nforce3 = forcedeth
ROM_TYPE_nforce3 = pci
ROM_DESCRIPTION_nforce3 = "nForce NVENET_3 Ethernet Controller"
PCI_VENDOR_nforce3 = 0x10de
PCI_DEVICE_nforce3 = 0x00d6
ROMS += nforce3
ROMS_forcedeth += nforce3

# NIC	10de00d6	10de,00d6	nForce NVENET_3 Ethernet Controller
DRIVER_10de00d6 = forcedeth
ROM_TYPE_10de00d6 = pci
ROM_DESCRIPTION_10de00d6 = "nForce NVENET_3 Ethernet Controller"
PCI_VENDOR_10de00d6 = 0x10de
PCI_DEVICE_10de00d6 = 0x00d6
ROMS += 10de00d6
ROMS_forcedeth += 10de00d6

# NIC	nforce4	10de,0086	nForce NVENET_4 Ethernet Controller
DRIVER_nforce4 = forcedeth
ROM_TYPE_nforce4 = pci
ROM_DESCRIPTION_nforce4 = "nForce NVENET_4 Ethernet Controller"
PCI_VENDOR_nforce4 = 0x10de
PCI_DEVICE_nforce4 = 0x0086
ROMS += nforce4
ROMS_forcedeth += nforce4

# NIC	10de0086	10de,0086	nForce NVENET_4 Ethernet Controller
DRIVER_10de0086 = forcedeth
ROM_TYPE_10de0086 = pci
ROM_DESCRIPTION_10de0086 = "nForce NVENET_4 Ethernet Controller"
PCI_VENDOR_10de0086 = 0x10de
PCI_DEVICE_10de0086 = 0x0086
ROMS += 10de0086
ROMS_forcedeth += 10de0086

# NIC	nforce5	10de,008c	nForce NVENET_5 Ethernet Controller
DRIVER_nforce5 = forcedeth
ROM_TYPE_nforce5 = pci
ROM_DESCRIPTION_nforce5 = "nForce NVENET_5 Ethernet Controller"
PCI_VENDOR_nforce5 = 0x10de
PCI_DEVICE_nforce5 = 0x008c
ROMS += nforce5
ROMS_forcedeth += nforce5

# NIC	10de008c	10de,008c	nForce NVENET_5 Ethernet Controller
DRIVER_10de008c = forcedeth
ROM_TYPE_10de008c = pci
ROM_DESCRIPTION_10de008c = "nForce NVENET_5 Ethernet Controller"
PCI_VENDOR_10de008c = 0x10de
PCI_DEVICE_10de008c = 0x008c
ROMS += 10de008c
ROMS_forcedeth += 10de008c

# NIC	nforce6	10de,00e6	nForce NVENET_6 Ethernet Controller
DRIVER_nforce6 = forcedeth
ROM_TYPE_nforce6 = pci
ROM_DESCRIPTION_nforce6 = "nForce NVENET_6 Ethernet Controller"
PCI_VENDOR_nforce6 = 0x10de
PCI_DEVICE_nforce6 = 0x00e6
ROMS += nforce6
ROMS_forcedeth += nforce6

# NIC	10de00e6	10de,00e6	nForce NVENET_6 Ethernet Controller
DRIVER_10de00e6 = forcedeth
ROM_TYPE_10de00e6 = pci
ROM_DESCRIPTION_10de00e6 = "nForce NVENET_6 Ethernet Controller"
PCI_VENDOR_10de00e6 = 0x10de
PCI_DEVICE_10de00e6 = 0x00e6
ROMS += 10de00e6
ROMS_forcedeth += 10de00e6

# NIC	nforce7	10de,00df	nForce NVENET_7 Ethernet Controller
DRIVER_nforce7 = forcedeth
ROM_TYPE_nforce7 = pci
ROM_DESCRIPTION_nforce7 = "nForce NVENET_7 Ethernet Controller"
PCI_VENDOR_nforce7 = 0x10de
PCI_DEVICE_nforce7 = 0x00df
ROMS += nforce7
ROMS_forcedeth += nforce7

# NIC	10de00df	10de,00df	nForce NVENET_7 Ethernet Controller
DRIVER_10de00df = forcedeth
ROM_TYPE_10de00df = pci
ROM_DESCRIPTION_10de00df = "nForce NVENET_7 Ethernet Controller"
PCI_VENDOR_10de00df = 0x10de
PCI_DEVICE_10de00df = 0x00df
ROMS += 10de00df
ROMS_forcedeth += 10de00df

# NIC	nforce8	10de,0056	nForce NVENET_8 Ethernet Controller
DRIVER_nforce8 = forcedeth
ROM_TYPE_nforce8 = pci
ROM_DESCRIPTION_nforce8 = "nForce NVENET_8 Ethernet Controller"
PCI_VENDOR_nforce8 = 0x10de
PCI_DEVICE_nforce8 = 0x0056
ROMS += nforce8
ROMS_forcedeth += nforce8

# NIC	10de0056	10de,0056	nForce NVENET_8 Ethernet Controller
DRIVER_10de0056 = forcedeth
ROM_TYPE_10de0056 = pci
ROM_DESCRIPTION_10de0056 = "nForce NVENET_8 Ethernet Controller"
PCI_VENDOR_10de0056 = 0x10de
PCI_DEVICE_10de0056 = 0x0056
ROMS += 10de0056
ROMS_forcedeth += 10de0056

# NIC	nforce9	10de,0057	nForce NVENET_9 Ethernet Controller
DRIVER_nforce9 = forcedeth
ROM_TYPE_nforce9 = pci
ROM_DESCRIPTION_nforce9 = "nForce NVENET_9 Ethernet Controller"
PCI_VENDOR_nforce9 = 0x10de
PCI_DEVICE_nforce9 = 0x0057
ROMS += nforce9
ROMS_forcedeth += nforce9

# NIC	10de0057	10de,0057	nForce NVENET_9 Ethernet Controller
DRIVER_10de0057 = forcedeth
ROM_TYPE_10de0057 = pci
ROM_DESCRIPTION_10de0057 = "nForce NVENET_9 Ethernet Controller"
PCI_VENDOR_10de0057 = 0x10de
PCI_DEVICE_10de0057 = 0x0057
ROMS += 10de0057
ROMS_forcedeth += 10de0057

# NIC	nforce10	10de,0037	nForce NVENET_10 Ethernet Controller
DRIVER_nforce10 = forcedeth
ROM_TYPE_nforce10 = pci
ROM_DESCRIPTION_nforce10 = "nForce NVENET_10 Ethernet Controller"
PCI_VENDOR_nforce10 = 0x10de
PCI_DEVICE_nforce10 = 0x0037
ROMS += nforce10
ROMS_forcedeth += nforce10

# NIC	10de0037	10de,0037	nForce NVENET_10 Ethernet Controller
DRIVER_10de0037 = forcedeth
ROM_TYPE_10de0037 = pci
ROM_DESCRIPTION_10de0037 = "nForce NVENET_10 Ethernet Controller"
PCI_VENDOR_10de0037 = 0x10de
PCI_DEVICE_10de0037 = 0x0037
ROMS += 10de0037
ROMS_forcedeth += 10de0037

# NIC	nforce11	10de,0038	nForce NVENET_11 Ethernet Controller
DRIVER_nforce11 = forcedeth
ROM_TYPE_nforce11 = pci
ROM_DESCRIPTION_nforce11 = "nForce NVENET_11 Ethernet Controller"
PCI_VENDOR_nforce11 = 0x10de
PCI_DEVICE_nforce11 = 0x0038
ROMS += nforce11
ROMS_forcedeth += nforce11

# NIC	10de0038	10de,0038	nForce NVENET_11 Ethernet Controller
DRIVER_10de0038 = forcedeth
ROM_TYPE_10de0038 = pci
ROM_DESCRIPTION_10de0038 = "nForce NVENET_11 Ethernet Controller"
PCI_VENDOR_10de0038 = 0x10de
PCI_DEVICE_10de0038 = 0x0038
ROMS += 10de0038
ROMS_forcedeth += 10de0038

# NIC	nforce15	10de,0373	nForce NVENET_15 Ethernet Controller
DRIVER_nforce15 = forcedeth
ROM_TYPE_nforce15 = pci
ROM_DESCRIPTION_nforce15 = "nForce NVENET_15 Ethernet Controller"
PCI_VENDOR_nforce15 = 0x10de
PCI_DEVICE_nforce15 = 0x0373
ROMS += nforce15
ROMS_forcedeth += nforce15

# NIC	10de0373	10de,0373	nForce NVENET_15 Ethernet Controller
DRIVER_10de0373 = forcedeth
ROM_TYPE_10de0373 = pci
ROM_DESCRIPTION_10de0373 = "nForce NVENET_15 Ethernet Controller"
PCI_VENDOR_10de0373 = 0x10de
PCI_DEVICE_10de0373 = 0x0373
ROMS += 10de0373
ROMS_forcedeth += 10de0373

# NIC	nforce16	10de,0269	nForce NVENET_16 Ethernet Controller
DRIVER_nforce16 = forcedeth
ROM_TYPE_nforce16 = pci
ROM_DESCRIPTION_nforce16 = "nForce NVENET_16 Ethernet Controller"
PCI_VENDOR_nforce16 = 0x10de
PCI_DEVICE_nforce16 = 0x0269
ROMS += nforce16
ROMS_forcedeth += nforce16

# NIC	10de0269	10de,0269	nForce NVENET_16 Ethernet Controller
DRIVER_10de0269 = forcedeth
ROM_TYPE_10de0269 = pci
ROM_DESCRIPTION_10de0269 = "nForce NVENET_16 Ethernet Controller"
PCI_VENDOR_10de0269 = 0x10de
PCI_DEVICE_10de0269 = 0x0269
ROMS += 10de0269
ROMS_forcedeth += 10de0269

# NIC	nforce17	10de,0760	nForce NVENET_17 Ethernet Controller
DRIVER_nforce17 = forcedeth
ROM_TYPE_nforce17 = pci
ROM_DESCRIPTION_nforce17 = "nForce NVENET_17 Ethernet Controller"
PCI_VENDOR_nforce17 = 0x10de
PCI_DEVICE_nforce17 = 0x0760
ROMS += nforce17
ROMS_forcedeth += nforce17

# NIC	10de0760	10de,0760	nForce NVENET_17 Ethernet Controller
DRIVER_10de0760 = forcedeth
ROM_TYPE_10de0760 = pci
ROM_DESCRIPTION_10de0760 = "nForce NVENET_17 Ethernet Controller"
PCI_VENDOR_10de0760 = 0x10de
PCI_DEVICE_10de0760 = 0x0760
ROMS += 10de0760
ROMS_forcedeth += 10de0760

# NIC	nforce67	10de,054c	nForce NVENET_67 Ethernet Controller
DRIVER_nforce67 = forcedeth
ROM_TYPE_nforce67 = pci
ROM_DESCRIPTION_nforce67 = "nForce NVENET_67 Ethernet Controller"
PCI_VENDOR_nforce67 = 0x10de
PCI_DEVICE_nforce67 = 0x054c
ROMS += nforce67
ROMS_forcedeth += nforce67

# NIC	10de054c	10de,054c	nForce NVENET_67 Ethernet Controller
DRIVER_10de054c = forcedeth
ROM_TYPE_10de054c = pci
ROM_DESCRIPTION_10de054c = "nForce NVENET_67 Ethernet Controller"
PCI_VENDOR_10de054c = 0x10de
PCI_DEVICE_10de054c = 0x054c
ROMS += 10de054c
ROMS_forcedeth += 10de054c
