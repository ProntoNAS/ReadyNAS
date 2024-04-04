3c595_DEPS = drivers/net/3c595.c include/compiler.h \
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
 drivers/net/3c595.h

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

drivers/net/3c595.h:

$(BIN)/3c595.o : drivers/net/3c595.c $(MAKEDEPS) $(POST_O_DEPS) $(3c595_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/3c595.o
 
$(BIN)/3c595.dbg%.o : drivers/net/3c595.c $(MAKEDEPS) $(POST_O_DEPS) $(3c595_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/3c595.dbg%.o
 
$(BIN)/3c595.c : drivers/net/3c595.c $(MAKEDEPS) $(POST_O_DEPS) $(3c595_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/3c595.c
 
$(BIN)/3c595.s : drivers/net/3c595.c $(MAKEDEPS) $(POST_O_DEPS) $(3c595_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/3c595.s
 
bin/deps/drivers/net/3c595.c.d : $(3c595_DEPS)
 
TAGS : $(3c595_DEPS)


# NIC	
# NIC	family	drivers/net/3c595
DRIVERS += 3c595

# NIC	3c590	10b7,5900	3Com590
DRIVER_3c590 = 3c595
ROM_TYPE_3c590 = pci
ROM_DESCRIPTION_3c590 = "3Com590"
PCI_VENDOR_3c590 = 0x10b7
PCI_DEVICE_3c590 = 0x5900
ROMS += 3c590
ROMS_3c595 += 3c590

# NIC	10b75900	10b7,5900	3Com590
DRIVER_10b75900 = 3c595
ROM_TYPE_10b75900 = pci
ROM_DESCRIPTION_10b75900 = "3Com590"
PCI_VENDOR_10b75900 = 0x10b7
PCI_DEVICE_10b75900 = 0x5900
ROMS += 10b75900
ROMS_3c595 += 10b75900

# NIC	3c595	10b7,5950	3Com595
DRIVER_3c595 = 3c595
ROM_TYPE_3c595 = pci
ROM_DESCRIPTION_3c595 = "3Com595"
PCI_VENDOR_3c595 = 0x10b7
PCI_DEVICE_3c595 = 0x5950
ROMS += 3c595
ROMS_3c595 += 3c595

# NIC	10b75950	10b7,5950	3Com595
DRIVER_10b75950 = 3c595
ROM_TYPE_10b75950 = pci
ROM_DESCRIPTION_10b75950 = "3Com595"
PCI_VENDOR_10b75950 = 0x10b7
PCI_DEVICE_10b75950 = 0x5950
ROMS += 10b75950
ROMS_3c595 += 10b75950

# NIC	3c595-1	10b7,5951	3Com595
DRIVER_3c595-1 = 3c595
ROM_TYPE_3c595-1 = pci
ROM_DESCRIPTION_3c595-1 = "3Com595"
PCI_VENDOR_3c595-1 = 0x10b7
PCI_DEVICE_3c595-1 = 0x5951
ROMS += 3c595-1
ROMS_3c595 += 3c595-1

# NIC	10b75951	10b7,5951	3Com595
DRIVER_10b75951 = 3c595
ROM_TYPE_10b75951 = pci
ROM_DESCRIPTION_10b75951 = "3Com595"
PCI_VENDOR_10b75951 = 0x10b7
PCI_DEVICE_10b75951 = 0x5951
ROMS += 10b75951
ROMS_3c595 += 10b75951

# NIC	3c595-2	10b7,5952	3Com595
DRIVER_3c595-2 = 3c595
ROM_TYPE_3c595-2 = pci
ROM_DESCRIPTION_3c595-2 = "3Com595"
PCI_VENDOR_3c595-2 = 0x10b7
PCI_DEVICE_3c595-2 = 0x5952
ROMS += 3c595-2
ROMS_3c595 += 3c595-2

# NIC	10b75952	10b7,5952	3Com595
DRIVER_10b75952 = 3c595
ROM_TYPE_10b75952 = pci
ROM_DESCRIPTION_10b75952 = "3Com595"
PCI_VENDOR_10b75952 = 0x10b7
PCI_DEVICE_10b75952 = 0x5952
ROMS += 10b75952
ROMS_3c595 += 10b75952

# NIC	3c900-tpo	10b7,9000	3Com900-TPO
DRIVER_3c900-tpo = 3c595
ROM_TYPE_3c900-tpo = pci
ROM_DESCRIPTION_3c900-tpo = "3Com900-TPO"
PCI_VENDOR_3c900-tpo = 0x10b7
PCI_DEVICE_3c900-tpo = 0x9000
ROMS += 3c900-tpo
ROMS_3c595 += 3c900-tpo

# NIC	10b79000	10b7,9000	3Com900-TPO
DRIVER_10b79000 = 3c595
ROM_TYPE_10b79000 = pci
ROM_DESCRIPTION_10b79000 = "3Com900-TPO"
PCI_VENDOR_10b79000 = 0x10b7
PCI_DEVICE_10b79000 = 0x9000
ROMS += 10b79000
ROMS_3c595 += 10b79000

# NIC	3c900-t4	10b7,9001	3Com900-Combo
DRIVER_3c900-t4 = 3c595
ROM_TYPE_3c900-t4 = pci
ROM_DESCRIPTION_3c900-t4 = "3Com900-Combo"
PCI_VENDOR_3c900-t4 = 0x10b7
PCI_DEVICE_3c900-t4 = 0x9001
ROMS += 3c900-t4
ROMS_3c595 += 3c900-t4

# NIC	10b79001	10b7,9001	3Com900-Combo
DRIVER_10b79001 = 3c595
ROM_TYPE_10b79001 = pci
ROM_DESCRIPTION_10b79001 = "3Com900-Combo"
PCI_VENDOR_10b79001 = 0x10b7
PCI_DEVICE_10b79001 = 0x9001
ROMS += 10b79001
ROMS_3c595 += 10b79001

# NIC	3c900b-tpo	10b7,9004	3Com900B-TPO
DRIVER_3c900b-tpo = 3c595
ROM_TYPE_3c900b-tpo = pci
ROM_DESCRIPTION_3c900b-tpo = "3Com900B-TPO"
PCI_VENDOR_3c900b-tpo = 0x10b7
PCI_DEVICE_3c900b-tpo = 0x9004
ROMS += 3c900b-tpo
ROMS_3c595 += 3c900b-tpo

# NIC	10b79004	10b7,9004	3Com900B-TPO
DRIVER_10b79004 = 3c595
ROM_TYPE_10b79004 = pci
ROM_DESCRIPTION_10b79004 = "3Com900B-TPO"
PCI_VENDOR_10b79004 = 0x10b7
PCI_DEVICE_10b79004 = 0x9004
ROMS += 10b79004
ROMS_3c595 += 10b79004

# NIC	3c900b-combo	10b7,9005	3Com900B-Combo
DRIVER_3c900b-combo = 3c595
ROM_TYPE_3c900b-combo = pci
ROM_DESCRIPTION_3c900b-combo = "3Com900B-Combo"
PCI_VENDOR_3c900b-combo = 0x10b7
PCI_DEVICE_3c900b-combo = 0x9005
ROMS += 3c900b-combo
ROMS_3c595 += 3c900b-combo

# NIC	10b79005	10b7,9005	3Com900B-Combo
DRIVER_10b79005 = 3c595
ROM_TYPE_10b79005 = pci
ROM_DESCRIPTION_10b79005 = "3Com900B-Combo"
PCI_VENDOR_10b79005 = 0x10b7
PCI_DEVICE_10b79005 = 0x9005
ROMS += 10b79005
ROMS_3c595 += 10b79005

# NIC	3c900b-tpb2	10b7,9006	3Com900B-2/T
DRIVER_3c900b-tpb2 = 3c595
ROM_TYPE_3c900b-tpb2 = pci
ROM_DESCRIPTION_3c900b-tpb2 = "3Com900B-2/T"
PCI_VENDOR_3c900b-tpb2 = 0x10b7
PCI_DEVICE_3c900b-tpb2 = 0x9006
ROMS += 3c900b-tpb2
ROMS_3c595 += 3c900b-tpb2

# NIC	10b79006	10b7,9006	3Com900B-2/T
DRIVER_10b79006 = 3c595
ROM_TYPE_10b79006 = pci
ROM_DESCRIPTION_10b79006 = "3Com900B-2/T"
PCI_VENDOR_10b79006 = 0x10b7
PCI_DEVICE_10b79006 = 0x9006
ROMS += 10b79006
ROMS_3c595 += 10b79006

# NIC	3c900b-fl	10b7,900a	3Com900B-FL
DRIVER_3c900b-fl = 3c595
ROM_TYPE_3c900b-fl = pci
ROM_DESCRIPTION_3c900b-fl = "3Com900B-FL"
PCI_VENDOR_3c900b-fl = 0x10b7
PCI_DEVICE_3c900b-fl = 0x900a
ROMS += 3c900b-fl
ROMS_3c595 += 3c900b-fl

# NIC	10b7900a	10b7,900a	3Com900B-FL
DRIVER_10b7900a = 3c595
ROM_TYPE_10b7900a = pci
ROM_DESCRIPTION_10b7900a = "3Com900B-FL"
PCI_VENDOR_10b7900a = 0x10b7
PCI_DEVICE_10b7900a = 0x900a
ROMS += 10b7900a
ROMS_3c595 += 10b7900a

# NIC	3c980-cyclone-1	10b7,9800	3Com980-Cyclone
DRIVER_3c980-cyclone-1 = 3c595
ROM_TYPE_3c980-cyclone-1 = pci
ROM_DESCRIPTION_3c980-cyclone-1 = "3Com980-Cyclone"
PCI_VENDOR_3c980-cyclone-1 = 0x10b7
PCI_DEVICE_3c980-cyclone-1 = 0x9800
ROMS += 3c980-cyclone-1
ROMS_3c595 += 3c980-cyclone-1

# NIC	10b79800	10b7,9800	3Com980-Cyclone
DRIVER_10b79800 = 3c595
ROM_TYPE_10b79800 = pci
ROM_DESCRIPTION_10b79800 = "3Com980-Cyclone"
PCI_VENDOR_10b79800 = 0x10b7
PCI_DEVICE_10b79800 = 0x9800
ROMS += 10b79800
ROMS_3c595 += 10b79800

# NIC	3c9805-1	10b7,9805	3Com9805
DRIVER_3c9805-1 = 3c595
ROM_TYPE_3c9805-1 = pci
ROM_DESCRIPTION_3c9805-1 = "3Com9805"
PCI_VENDOR_3c9805-1 = 0x10b7
PCI_DEVICE_3c9805-1 = 0x9805
ROMS += 3c9805-1
ROMS_3c595 += 3c9805-1

# NIC	10b79805	10b7,9805	3Com9805
DRIVER_10b79805 = 3c595
ROM_TYPE_10b79805 = pci
ROM_DESCRIPTION_10b79805 = "3Com9805"
PCI_VENDOR_10b79805 = 0x10b7
PCI_DEVICE_10b79805 = 0x9805
ROMS += 10b79805
ROMS_3c595 += 10b79805

# NIC	3csoho100-tx-1	10b7,7646	3CSOHO100-TX
DRIVER_3csoho100-tx-1 = 3c595
ROM_TYPE_3csoho100-tx-1 = pci
ROM_DESCRIPTION_3csoho100-tx-1 = "3CSOHO100-TX"
PCI_VENDOR_3csoho100-tx-1 = 0x10b7
PCI_DEVICE_3csoho100-tx-1 = 0x7646
ROMS += 3csoho100-tx-1
ROMS_3c595 += 3csoho100-tx-1

# NIC	10b77646	10b7,7646	3CSOHO100-TX
DRIVER_10b77646 = 3c595
ROM_TYPE_10b77646 = pci
ROM_DESCRIPTION_10b77646 = "3CSOHO100-TX"
PCI_VENDOR_10b77646 = 0x10b7
PCI_DEVICE_10b77646 = 0x7646
ROMS += 10b77646
ROMS_3c595 += 10b77646

# NIC	3c450-1	10b7,4500	3Com450 HomePNA Tornado
DRIVER_3c450-1 = 3c595
ROM_TYPE_3c450-1 = pci
ROM_DESCRIPTION_3c450-1 = "3Com450 HomePNA Tornado"
PCI_VENDOR_3c450-1 = 0x10b7
PCI_DEVICE_3c450-1 = 0x4500
ROMS += 3c450-1
ROMS_3c595 += 3c450-1

# NIC	10b74500	10b7,4500	3Com450 HomePNA Tornado
DRIVER_10b74500 = 3c595
ROM_TYPE_10b74500 = pci
ROM_DESCRIPTION_10b74500 = "3Com450 HomePNA Tornado"
PCI_VENDOR_10b74500 = 0x10b7
PCI_DEVICE_10b74500 = 0x4500
ROMS += 10b74500
ROMS_3c595 += 10b74500
