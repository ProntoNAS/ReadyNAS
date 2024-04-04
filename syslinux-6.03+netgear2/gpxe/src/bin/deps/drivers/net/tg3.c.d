tg3_DEPS = drivers/net/tg3.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/ethernet.h include/mii.h include/gpxe/netdevice.h \
 include/gpxe/refcnt.h include/gpxe/settings.h drivers/net/tg3.h

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ethernet.h:

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

drivers/net/tg3.h:

$(BIN)/tg3.o : drivers/net/tg3.c $(MAKEDEPS) $(POST_O_DEPS) $(tg3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tg3.o
 
$(BIN)/tg3.dbg%.o : drivers/net/tg3.c $(MAKEDEPS) $(POST_O_DEPS) $(tg3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tg3.dbg%.o
 
$(BIN)/tg3.c : drivers/net/tg3.c $(MAKEDEPS) $(POST_O_DEPS) $(tg3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tg3.c
 
$(BIN)/tg3.s : drivers/net/tg3.c $(MAKEDEPS) $(POST_O_DEPS) $(tg3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tg3.s
 
bin/deps/drivers/net/tg3.c.d : $(tg3_DEPS)
 
TAGS : $(tg3_DEPS)


# NIC	
# NIC	family	drivers/net/tg3
DRIVERS += tg3

# NIC	tg3-5700	14e4,1644	Broadcom Tigon 3 5700
DRIVER_tg3-5700 = tg3
ROM_TYPE_tg3-5700 = pci
ROM_DESCRIPTION_tg3-5700 = "Broadcom Tigon 3 5700"
PCI_VENDOR_tg3-5700 = 0x14e4
PCI_DEVICE_tg3-5700 = 0x1644
ROMS += tg3-5700
ROMS_tg3 += tg3-5700

# NIC	14e41644	14e4,1644	Broadcom Tigon 3 5700
DRIVER_14e41644 = tg3
ROM_TYPE_14e41644 = pci
ROM_DESCRIPTION_14e41644 = "Broadcom Tigon 3 5700"
PCI_VENDOR_14e41644 = 0x14e4
PCI_DEVICE_14e41644 = 0x1644
ROMS += 14e41644
ROMS_tg3 += 14e41644

# NIC	tg3-5701	14e4,1645	Broadcom Tigon 3 5701
DRIVER_tg3-5701 = tg3
ROM_TYPE_tg3-5701 = pci
ROM_DESCRIPTION_tg3-5701 = "Broadcom Tigon 3 5701"
PCI_VENDOR_tg3-5701 = 0x14e4
PCI_DEVICE_tg3-5701 = 0x1645
ROMS += tg3-5701
ROMS_tg3 += tg3-5701

# NIC	14e41645	14e4,1645	Broadcom Tigon 3 5701
DRIVER_14e41645 = tg3
ROM_TYPE_14e41645 = pci
ROM_DESCRIPTION_14e41645 = "Broadcom Tigon 3 5701"
PCI_VENDOR_14e41645 = 0x14e4
PCI_DEVICE_14e41645 = 0x1645
ROMS += 14e41645
ROMS_tg3 += 14e41645

# NIC	tg3-5702	14e4,1646	Broadcom Tigon 3 5702
DRIVER_tg3-5702 = tg3
ROM_TYPE_tg3-5702 = pci
ROM_DESCRIPTION_tg3-5702 = "Broadcom Tigon 3 5702"
PCI_VENDOR_tg3-5702 = 0x14e4
PCI_DEVICE_tg3-5702 = 0x1646
ROMS += tg3-5702
ROMS_tg3 += tg3-5702

# NIC	14e41646	14e4,1646	Broadcom Tigon 3 5702
DRIVER_14e41646 = tg3
ROM_TYPE_14e41646 = pci
ROM_DESCRIPTION_14e41646 = "Broadcom Tigon 3 5702"
PCI_VENDOR_14e41646 = 0x14e4
PCI_DEVICE_14e41646 = 0x1646
ROMS += 14e41646
ROMS_tg3 += 14e41646

# NIC	tg3-5703	14e4,1647	Broadcom Tigon 3 5703
DRIVER_tg3-5703 = tg3
ROM_TYPE_tg3-5703 = pci
ROM_DESCRIPTION_tg3-5703 = "Broadcom Tigon 3 5703"
PCI_VENDOR_tg3-5703 = 0x14e4
PCI_DEVICE_tg3-5703 = 0x1647
ROMS += tg3-5703
ROMS_tg3 += tg3-5703

# NIC	14e41647	14e4,1647	Broadcom Tigon 3 5703
DRIVER_14e41647 = tg3
ROM_TYPE_14e41647 = pci
ROM_DESCRIPTION_14e41647 = "Broadcom Tigon 3 5703"
PCI_VENDOR_14e41647 = 0x14e4
PCI_DEVICE_14e41647 = 0x1647
ROMS += 14e41647
ROMS_tg3 += 14e41647

# NIC	tg3-5704	14e4,1648	Broadcom Tigon 3 5704
DRIVER_tg3-5704 = tg3
ROM_TYPE_tg3-5704 = pci
ROM_DESCRIPTION_tg3-5704 = "Broadcom Tigon 3 5704"
PCI_VENDOR_tg3-5704 = 0x14e4
PCI_DEVICE_tg3-5704 = 0x1648
ROMS += tg3-5704
ROMS_tg3 += tg3-5704

# NIC	14e41648	14e4,1648	Broadcom Tigon 3 5704
DRIVER_14e41648 = tg3
ROM_TYPE_14e41648 = pci
ROM_DESCRIPTION_14e41648 = "Broadcom Tigon 3 5704"
PCI_VENDOR_14e41648 = 0x14e4
PCI_DEVICE_14e41648 = 0x1648
ROMS += 14e41648
ROMS_tg3 += 14e41648

# NIC	tg3-5702FE	14e4,164d	Broadcom Tigon 3 5702FE
DRIVER_tg3-5702FE = tg3
ROM_TYPE_tg3-5702FE = pci
ROM_DESCRIPTION_tg3-5702FE = "Broadcom Tigon 3 5702FE"
PCI_VENDOR_tg3-5702FE = 0x14e4
PCI_DEVICE_tg3-5702FE = 0x164d
ROMS += tg3-5702FE
ROMS_tg3 += tg3-5702FE

# NIC	14e4164d	14e4,164d	Broadcom Tigon 3 5702FE
DRIVER_14e4164d = tg3
ROM_TYPE_14e4164d = pci
ROM_DESCRIPTION_14e4164d = "Broadcom Tigon 3 5702FE"
PCI_VENDOR_14e4164d = 0x14e4
PCI_DEVICE_14e4164d = 0x164d
ROMS += 14e4164d
ROMS_tg3 += 14e4164d

# NIC	tg3-5705	14e4,1653	Broadcom Tigon 3 5705
DRIVER_tg3-5705 = tg3
ROM_TYPE_tg3-5705 = pci
ROM_DESCRIPTION_tg3-5705 = "Broadcom Tigon 3 5705"
PCI_VENDOR_tg3-5705 = 0x14e4
PCI_DEVICE_tg3-5705 = 0x1653
ROMS += tg3-5705
ROMS_tg3 += tg3-5705

# NIC	14e41653	14e4,1653	Broadcom Tigon 3 5705
DRIVER_14e41653 = tg3
ROM_TYPE_14e41653 = pci
ROM_DESCRIPTION_14e41653 = "Broadcom Tigon 3 5705"
PCI_VENDOR_14e41653 = 0x14e4
PCI_DEVICE_14e41653 = 0x1653
ROMS += 14e41653
ROMS_tg3 += 14e41653

# NIC	tg3-5705_2	14e4,1654	Broadcom Tigon 3 5705_2
DRIVER_tg3-5705_2 = tg3
ROM_TYPE_tg3-5705_2 = pci
ROM_DESCRIPTION_tg3-5705_2 = "Broadcom Tigon 3 5705_2"
PCI_VENDOR_tg3-5705_2 = 0x14e4
PCI_DEVICE_tg3-5705_2 = 0x1654
ROMS += tg3-5705_2
ROMS_tg3 += tg3-5705_2

# NIC	14e41654	14e4,1654	Broadcom Tigon 3 5705_2
DRIVER_14e41654 = tg3
ROM_TYPE_14e41654 = pci
ROM_DESCRIPTION_14e41654 = "Broadcom Tigon 3 5705_2"
PCI_VENDOR_14e41654 = 0x14e4
PCI_DEVICE_14e41654 = 0x1654
ROMS += 14e41654
ROMS_tg3 += 14e41654

# NIC	tg3-5721	14e4,1659	Broadcom Tigon 3 5721
DRIVER_tg3-5721 = tg3
ROM_TYPE_tg3-5721 = pci
ROM_DESCRIPTION_tg3-5721 = "Broadcom Tigon 3 5721"
PCI_VENDOR_tg3-5721 = 0x14e4
PCI_DEVICE_tg3-5721 = 0x1659
ROMS += tg3-5721
ROMS_tg3 += tg3-5721

# NIC	14e41659	14e4,1659	Broadcom Tigon 3 5721
DRIVER_14e41659 = tg3
ROM_TYPE_14e41659 = pci
ROM_DESCRIPTION_14e41659 = "Broadcom Tigon 3 5721"
PCI_VENDOR_14e41659 = 0x14e4
PCI_DEVICE_14e41659 = 0x1659
ROMS += 14e41659
ROMS_tg3 += 14e41659

# NIC	tg3-5705M	14e4,165d	Broadcom Tigon 3 5705M
DRIVER_tg3-5705M = tg3
ROM_TYPE_tg3-5705M = pci
ROM_DESCRIPTION_tg3-5705M = "Broadcom Tigon 3 5705M"
PCI_VENDOR_tg3-5705M = 0x14e4
PCI_DEVICE_tg3-5705M = 0x165d
ROMS += tg3-5705M
ROMS_tg3 += tg3-5705M

# NIC	14e4165d	14e4,165d	Broadcom Tigon 3 5705M
DRIVER_14e4165d = tg3
ROM_TYPE_14e4165d = pci
ROM_DESCRIPTION_14e4165d = "Broadcom Tigon 3 5705M"
PCI_VENDOR_14e4165d = 0x14e4
PCI_DEVICE_14e4165d = 0x165d
ROMS += 14e4165d
ROMS_tg3 += 14e4165d

# NIC	tg3-5705M_2	14e4,165e	Broadcom Tigon 3 5705M_2
DRIVER_tg3-5705M_2 = tg3
ROM_TYPE_tg3-5705M_2 = pci
ROM_DESCRIPTION_tg3-5705M_2 = "Broadcom Tigon 3 5705M_2"
PCI_VENDOR_tg3-5705M_2 = 0x14e4
PCI_DEVICE_tg3-5705M_2 = 0x165e
ROMS += tg3-5705M_2
ROMS_tg3 += tg3-5705M_2

# NIC	14e4165e	14e4,165e	Broadcom Tigon 3 5705M_2
DRIVER_14e4165e = tg3
ROM_TYPE_14e4165e = pci
ROM_DESCRIPTION_14e4165e = "Broadcom Tigon 3 5705M_2"
PCI_VENDOR_14e4165e = 0x14e4
PCI_DEVICE_14e4165e = 0x165e
ROMS += 14e4165e
ROMS_tg3 += 14e4165e

# NIC	tg3-5751	14e4,1677	Broadcom Tigon 3 5751
DRIVER_tg3-5751 = tg3
ROM_TYPE_tg3-5751 = pci
ROM_DESCRIPTION_tg3-5751 = "Broadcom Tigon 3 5751"
PCI_VENDOR_tg3-5751 = 0x14e4
PCI_DEVICE_tg3-5751 = 0x1677
ROMS += tg3-5751
ROMS_tg3 += tg3-5751

# NIC	14e41677	14e4,1677	Broadcom Tigon 3 5751
DRIVER_14e41677 = tg3
ROM_TYPE_14e41677 = pci
ROM_DESCRIPTION_14e41677 = "Broadcom Tigon 3 5751"
PCI_VENDOR_14e41677 = 0x14e4
PCI_DEVICE_14e41677 = 0x1677
ROMS += 14e41677
ROMS_tg3 += 14e41677

# NIC	tg3-5754	14e4,167a	Broadcom Tigon 3 5754
DRIVER_tg3-5754 = tg3
ROM_TYPE_tg3-5754 = pci
ROM_DESCRIPTION_tg3-5754 = "Broadcom Tigon 3 5754"
PCI_VENDOR_tg3-5754 = 0x14e4
PCI_DEVICE_tg3-5754 = 0x167a
ROMS += tg3-5754
ROMS_tg3 += tg3-5754

# NIC	14e4167a	14e4,167a	Broadcom Tigon 3 5754
DRIVER_14e4167a = tg3
ROM_TYPE_14e4167a = pci
ROM_DESCRIPTION_14e4167a = "Broadcom Tigon 3 5754"
PCI_VENDOR_14e4167a = 0x14e4
PCI_DEVICE_14e4167a = 0x167a
ROMS += 14e4167a
ROMS_tg3 += 14e4167a

# NIC	tg3-5787	14e4,1693	Broadcom Tigon 3 5787
DRIVER_tg3-5787 = tg3
ROM_TYPE_tg3-5787 = pci
ROM_DESCRIPTION_tg3-5787 = "Broadcom Tigon 3 5787"
PCI_VENDOR_tg3-5787 = 0x14e4
PCI_DEVICE_tg3-5787 = 0x1693
ROMS += tg3-5787
ROMS_tg3 += tg3-5787

# NIC	14e41693	14e4,1693	Broadcom Tigon 3 5787
DRIVER_14e41693 = tg3
ROM_TYPE_14e41693 = pci
ROM_DESCRIPTION_14e41693 = "Broadcom Tigon 3 5787"
PCI_VENDOR_14e41693 = 0x14e4
PCI_DEVICE_14e41693 = 0x1693
ROMS += 14e41693
ROMS_tg3 += 14e41693

# NIC	tg3-5782	14e4,1696	Broadcom Tigon 3 5782
DRIVER_tg3-5782 = tg3
ROM_TYPE_tg3-5782 = pci
ROM_DESCRIPTION_tg3-5782 = "Broadcom Tigon 3 5782"
PCI_VENDOR_tg3-5782 = 0x14e4
PCI_DEVICE_tg3-5782 = 0x1696
ROMS += tg3-5782
ROMS_tg3 += tg3-5782

# NIC	14e41696	14e4,1696	Broadcom Tigon 3 5782
DRIVER_14e41696 = tg3
ROM_TYPE_14e41696 = pci
ROM_DESCRIPTION_14e41696 = "Broadcom Tigon 3 5782"
PCI_VENDOR_14e41696 = 0x14e4
PCI_DEVICE_14e41696 = 0x1696
ROMS += 14e41696
ROMS_tg3 += 14e41696

# NIC	tg3-5786	14e4,169a	Broadcom Tigon 3 5786
DRIVER_tg3-5786 = tg3
ROM_TYPE_tg3-5786 = pci
ROM_DESCRIPTION_tg3-5786 = "Broadcom Tigon 3 5786"
PCI_VENDOR_tg3-5786 = 0x14e4
PCI_DEVICE_tg3-5786 = 0x169a
ROMS += tg3-5786
ROMS_tg3 += tg3-5786

# NIC	14e4169a	14e4,169a	Broadcom Tigon 3 5786
DRIVER_14e4169a = tg3
ROM_TYPE_14e4169a = pci
ROM_DESCRIPTION_14e4169a = "Broadcom Tigon 3 5786"
PCI_VENDOR_14e4169a = 0x14e4
PCI_DEVICE_14e4169a = 0x169a
ROMS += 14e4169a
ROMS_tg3 += 14e4169a

# NIC	tg3-5788	14e4,169c	Broadcom Tigon 3 5788
DRIVER_tg3-5788 = tg3
ROM_TYPE_tg3-5788 = pci
ROM_DESCRIPTION_tg3-5788 = "Broadcom Tigon 3 5788"
PCI_VENDOR_tg3-5788 = 0x14e4
PCI_DEVICE_tg3-5788 = 0x169c
ROMS += tg3-5788
ROMS_tg3 += tg3-5788

# NIC	14e4169c	14e4,169c	Broadcom Tigon 3 5788
DRIVER_14e4169c = tg3
ROM_TYPE_14e4169c = pci
ROM_DESCRIPTION_14e4169c = "Broadcom Tigon 3 5788"
PCI_VENDOR_14e4169c = 0x14e4
PCI_DEVICE_14e4169c = 0x169c
ROMS += 14e4169c
ROMS_tg3 += 14e4169c

# NIC	tg3-5789	14e4,169d	Broadcom Tigon 3 5789
DRIVER_tg3-5789 = tg3
ROM_TYPE_tg3-5789 = pci
ROM_DESCRIPTION_tg3-5789 = "Broadcom Tigon 3 5789"
PCI_VENDOR_tg3-5789 = 0x14e4
PCI_DEVICE_tg3-5789 = 0x169d
ROMS += tg3-5789
ROMS_tg3 += tg3-5789

# NIC	14e4169d	14e4,169d	Broadcom Tigon 3 5789
DRIVER_14e4169d = tg3
ROM_TYPE_14e4169d = pci
ROM_DESCRIPTION_14e4169d = "Broadcom Tigon 3 5789"
PCI_VENDOR_14e4169d = 0x14e4
PCI_DEVICE_14e4169d = 0x169d
ROMS += 14e4169d
ROMS_tg3 += 14e4169d

# NIC	tg3-5702X	14e4,16a6	Broadcom Tigon 3 5702X
DRIVER_tg3-5702X = tg3
ROM_TYPE_tg3-5702X = pci
ROM_DESCRIPTION_tg3-5702X = "Broadcom Tigon 3 5702X"
PCI_VENDOR_tg3-5702X = 0x14e4
PCI_DEVICE_tg3-5702X = 0x16a6
ROMS += tg3-5702X
ROMS_tg3 += tg3-5702X

# NIC	14e416a6	14e4,16a6	Broadcom Tigon 3 5702X
DRIVER_14e416a6 = tg3
ROM_TYPE_14e416a6 = pci
ROM_DESCRIPTION_14e416a6 = "Broadcom Tigon 3 5702X"
PCI_VENDOR_14e416a6 = 0x14e4
PCI_DEVICE_14e416a6 = 0x16a6
ROMS += 14e416a6
ROMS_tg3 += 14e416a6

# NIC	tg3-5703X	14e4,16a7	Broadcom Tigon 3 5703X
DRIVER_tg3-5703X = tg3
ROM_TYPE_tg3-5703X = pci
ROM_DESCRIPTION_tg3-5703X = "Broadcom Tigon 3 5703X"
PCI_VENDOR_tg3-5703X = 0x14e4
PCI_DEVICE_tg3-5703X = 0x16a7
ROMS += tg3-5703X
ROMS_tg3 += tg3-5703X

# NIC	14e416a7	14e4,16a7	Broadcom Tigon 3 5703X
DRIVER_14e416a7 = tg3
ROM_TYPE_14e416a7 = pci
ROM_DESCRIPTION_14e416a7 = "Broadcom Tigon 3 5703X"
PCI_VENDOR_14e416a7 = 0x14e4
PCI_DEVICE_14e416a7 = 0x16a7
ROMS += 14e416a7
ROMS_tg3 += 14e416a7

# NIC	tg3-5704S	14e4,16a8	Broadcom Tigon 3 5704S
DRIVER_tg3-5704S = tg3
ROM_TYPE_tg3-5704S = pci
ROM_DESCRIPTION_tg3-5704S = "Broadcom Tigon 3 5704S"
PCI_VENDOR_tg3-5704S = 0x14e4
PCI_DEVICE_tg3-5704S = 0x16a8
ROMS += tg3-5704S
ROMS_tg3 += tg3-5704S

# NIC	14e416a8	14e4,16a8	Broadcom Tigon 3 5704S
DRIVER_14e416a8 = tg3
ROM_TYPE_14e416a8 = pci
ROM_DESCRIPTION_14e416a8 = "Broadcom Tigon 3 5704S"
PCI_VENDOR_14e416a8 = 0x14e4
PCI_DEVICE_14e416a8 = 0x16a8
ROMS += 14e416a8
ROMS_tg3 += 14e416a8

# NIC	tg3-5702A3	14e4,16c6	Broadcom Tigon 3 5702A3
DRIVER_tg3-5702A3 = tg3
ROM_TYPE_tg3-5702A3 = pci
ROM_DESCRIPTION_tg3-5702A3 = "Broadcom Tigon 3 5702A3"
PCI_VENDOR_tg3-5702A3 = 0x14e4
PCI_DEVICE_tg3-5702A3 = 0x16c6
ROMS += tg3-5702A3
ROMS_tg3 += tg3-5702A3

# NIC	14e416c6	14e4,16c6	Broadcom Tigon 3 5702A3
DRIVER_14e416c6 = tg3
ROM_TYPE_14e416c6 = pci
ROM_DESCRIPTION_14e416c6 = "Broadcom Tigon 3 5702A3"
PCI_VENDOR_14e416c6 = 0x14e4
PCI_DEVICE_14e416c6 = 0x16c6
ROMS += 14e416c6
ROMS_tg3 += 14e416c6

# NIC	tg3-5703A3	14e4,16c7	Broadcom Tigon 3 5703A3
DRIVER_tg3-5703A3 = tg3
ROM_TYPE_tg3-5703A3 = pci
ROM_DESCRIPTION_tg3-5703A3 = "Broadcom Tigon 3 5703A3"
PCI_VENDOR_tg3-5703A3 = 0x14e4
PCI_DEVICE_tg3-5703A3 = 0x16c7
ROMS += tg3-5703A3
ROMS_tg3 += tg3-5703A3

# NIC	14e416c7	14e4,16c7	Broadcom Tigon 3 5703A3
DRIVER_14e416c7 = tg3
ROM_TYPE_14e416c7 = pci
ROM_DESCRIPTION_14e416c7 = "Broadcom Tigon 3 5703A3"
PCI_VENDOR_14e416c7 = 0x14e4
PCI_DEVICE_14e416c7 = 0x16c7
ROMS += 14e416c7
ROMS_tg3 += 14e416c7

# NIC	tg3-5901	14e4,170d	Broadcom Tigon 3 5901
DRIVER_tg3-5901 = tg3
ROM_TYPE_tg3-5901 = pci
ROM_DESCRIPTION_tg3-5901 = "Broadcom Tigon 3 5901"
PCI_VENDOR_tg3-5901 = 0x14e4
PCI_DEVICE_tg3-5901 = 0x170d
ROMS += tg3-5901
ROMS_tg3 += tg3-5901

# NIC	14e4170d	14e4,170d	Broadcom Tigon 3 5901
DRIVER_14e4170d = tg3
ROM_TYPE_14e4170d = pci
ROM_DESCRIPTION_14e4170d = "Broadcom Tigon 3 5901"
PCI_VENDOR_14e4170d = 0x14e4
PCI_DEVICE_14e4170d = 0x170d
ROMS += 14e4170d
ROMS_tg3 += 14e4170d

# NIC	tg3-5901_2	14e4,170e	Broadcom Tigon 3 5901_2
DRIVER_tg3-5901_2 = tg3
ROM_TYPE_tg3-5901_2 = pci
ROM_DESCRIPTION_tg3-5901_2 = "Broadcom Tigon 3 5901_2"
PCI_VENDOR_tg3-5901_2 = 0x14e4
PCI_DEVICE_tg3-5901_2 = 0x170e
ROMS += tg3-5901_2
ROMS_tg3 += tg3-5901_2

# NIC	14e4170e	14e4,170e	Broadcom Tigon 3 5901_2
DRIVER_14e4170e = tg3
ROM_TYPE_14e4170e = pci
ROM_DESCRIPTION_14e4170e = "Broadcom Tigon 3 5901_2"
PCI_VENDOR_14e4170e = 0x14e4
PCI_DEVICE_14e4170e = 0x170e
ROMS += 14e4170e
ROMS_tg3 += 14e4170e

# NIC	tg3-9DXX	1148,4400	Syskonnect 9DXX
DRIVER_tg3-9DXX = tg3
ROM_TYPE_tg3-9DXX = pci
ROM_DESCRIPTION_tg3-9DXX = "Syskonnect 9DXX"
PCI_VENDOR_tg3-9DXX = 0x1148
PCI_DEVICE_tg3-9DXX = 0x4400
ROMS += tg3-9DXX
ROMS_tg3 += tg3-9DXX

# NIC	11484400	1148,4400	Syskonnect 9DXX
DRIVER_11484400 = tg3
ROM_TYPE_11484400 = pci
ROM_DESCRIPTION_11484400 = "Syskonnect 9DXX"
PCI_VENDOR_11484400 = 0x1148
PCI_DEVICE_11484400 = 0x4400
ROMS += 11484400
ROMS_tg3 += 11484400

# NIC	tg3-9MXX	1148,4500	Syskonnect 9MXX
DRIVER_tg3-9MXX = tg3
ROM_TYPE_tg3-9MXX = pci
ROM_DESCRIPTION_tg3-9MXX = "Syskonnect 9MXX"
PCI_VENDOR_tg3-9MXX = 0x1148
PCI_DEVICE_tg3-9MXX = 0x4500
ROMS += tg3-9MXX
ROMS_tg3 += tg3-9MXX

# NIC	11484500	1148,4500	Syskonnect 9MXX
DRIVER_11484500 = tg3
ROM_TYPE_11484500 = pci
ROM_DESCRIPTION_11484500 = "Syskonnect 9MXX"
PCI_VENDOR_11484500 = 0x1148
PCI_DEVICE_11484500 = 0x4500
ROMS += 11484500
ROMS_tg3 += 11484500

# NIC	tg3-ac1000	173b,03e8	Altima AC1000
DRIVER_tg3-ac1000 = tg3
ROM_TYPE_tg3-ac1000 = pci
ROM_DESCRIPTION_tg3-ac1000 = "Altima AC1000"
PCI_VENDOR_tg3-ac1000 = 0x173b
PCI_DEVICE_tg3-ac1000 = 0x03e8
ROMS += tg3-ac1000
ROMS_tg3 += tg3-ac1000

# NIC	173b03e8	173b,03e8	Altima AC1000
DRIVER_173b03e8 = tg3
ROM_TYPE_173b03e8 = pci
ROM_DESCRIPTION_173b03e8 = "Altima AC1000"
PCI_VENDOR_173b03e8 = 0x173b
PCI_DEVICE_173b03e8 = 0x03e8
ROMS += 173b03e8
ROMS_tg3 += 173b03e8

# NIC	tg3-ac1001	173b,03e9	Altima AC1001
DRIVER_tg3-ac1001 = tg3
ROM_TYPE_tg3-ac1001 = pci
ROM_DESCRIPTION_tg3-ac1001 = "Altima AC1001"
PCI_VENDOR_tg3-ac1001 = 0x173b
PCI_DEVICE_tg3-ac1001 = 0x03e9
ROMS += tg3-ac1001
ROMS_tg3 += tg3-ac1001

# NIC	173b03e9	173b,03e9	Altima AC1001
DRIVER_173b03e9 = tg3
ROM_TYPE_173b03e9 = pci
ROM_DESCRIPTION_173b03e9 = "Altima AC1001"
PCI_VENDOR_173b03e9 = 0x173b
PCI_DEVICE_173b03e9 = 0x03e9
ROMS += 173b03e9
ROMS_tg3 += 173b03e9

# NIC	tg3-ac9100	173b,03ea	Altima AC9100
DRIVER_tg3-ac9100 = tg3
ROM_TYPE_tg3-ac9100 = pci
ROM_DESCRIPTION_tg3-ac9100 = "Altima AC9100"
PCI_VENDOR_tg3-ac9100 = 0x173b
PCI_DEVICE_tg3-ac9100 = 0x03ea
ROMS += tg3-ac9100
ROMS_tg3 += tg3-ac9100

# NIC	173b03ea	173b,03ea	Altima AC9100
DRIVER_173b03ea = tg3
ROM_TYPE_173b03ea = pci
ROM_DESCRIPTION_173b03ea = "Altima AC9100"
PCI_VENDOR_173b03ea = 0x173b
PCI_DEVICE_173b03ea = 0x03ea
ROMS += 173b03ea
ROMS_tg3 += 173b03ea

# NIC	tg3-ac1003	173b,03eb	Altima AC1003
DRIVER_tg3-ac1003 = tg3
ROM_TYPE_tg3-ac1003 = pci
ROM_DESCRIPTION_tg3-ac1003 = "Altima AC1003"
PCI_VENDOR_tg3-ac1003 = 0x173b
PCI_DEVICE_tg3-ac1003 = 0x03eb
ROMS += tg3-ac1003
ROMS_tg3 += tg3-ac1003

# NIC	173b03eb	173b,03eb	Altima AC1003
DRIVER_173b03eb = tg3
ROM_TYPE_173b03eb = pci
ROM_DESCRIPTION_173b03eb = "Altima AC1003"
PCI_VENDOR_173b03eb = 0x173b
PCI_DEVICE_173b03eb = 0x03eb
ROMS += 173b03eb
ROMS_tg3 += 173b03eb

# NIC	tg3-hp	0e11,00ca	HP Tigon 3
DRIVER_tg3-hp = tg3
ROM_TYPE_tg3-hp = pci
ROM_DESCRIPTION_tg3-hp = "HP Tigon 3"
PCI_VENDOR_tg3-hp = 0x0e11
PCI_DEVICE_tg3-hp = 0x00ca
ROMS += tg3-hp
ROMS_tg3 += tg3-hp

# NIC	0e1100ca	0e11,00ca	HP Tigon 3
DRIVER_0e1100ca = tg3
ROM_TYPE_0e1100ca = pci
ROM_DESCRIPTION_0e1100ca = "HP Tigon 3"
PCI_VENDOR_0e1100ca = 0x0e11
PCI_DEVICE_0e1100ca = 0x00ca
ROMS += 0e1100ca
ROMS_tg3 += 0e1100ca
