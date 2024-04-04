ns8390_DEPS = drivers/net/ns8390.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h drivers/net/ns8390.h \
 include/gpxe/ethernet.h

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

drivers/net/ns8390.h:

include/gpxe/ethernet.h:

$(BIN)/ns8390.o : drivers/net/ns8390.c $(MAKEDEPS) $(POST_O_DEPS) $(ns8390_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ns8390.o
 
$(BIN)/ns8390.dbg%.o : drivers/net/ns8390.c $(MAKEDEPS) $(POST_O_DEPS) $(ns8390_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ns8390.dbg%.o
 
$(BIN)/ns8390.c : drivers/net/ns8390.c $(MAKEDEPS) $(POST_O_DEPS) $(ns8390_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ns8390.c
 
$(BIN)/ns8390.s : drivers/net/ns8390.c $(MAKEDEPS) $(POST_O_DEPS) $(ns8390_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ns8390.s
 
bin/deps/drivers/net/ns8390.c.d : $(ns8390_DEPS)
 
TAGS : $(ns8390_DEPS)


# NIC	
# NIC	family	drivers/net/ns8390
DRIVERS += ns8390

# NIC	wd	-	WD8003/8013, SMC8216/8416, SMC 83c790 (EtherEZ)
DRIVER_wd = ns8390
ROM_TYPE_wd = isa
ROM_DESCRIPTION_wd = "WD8003/8013, SMC8216/8416, SMC 83c790 (EtherEZ)"
ROMS += wd
ROMS_ns8390 += wd

# NIC	3c503	-	3Com503, Etherlink II[/16]
DRIVER_3c503 = ns8390
ROM_TYPE_3c503 = isa
ROM_DESCRIPTION_3c503 = "3Com503, Etherlink II[/16]"
ROMS += 3c503
ROMS_ns8390 += 3c503

# NIC	ne	-	NE1000/2000 and clones
DRIVER_ne = ns8390
ROM_TYPE_ne = isa
ROM_DESCRIPTION_ne = "NE1000/2000 and clones"
ROMS += ne
ROMS_ns8390 += ne

# NIC	rtl8029	10ec,8029	Realtek 8029
DRIVER_rtl8029 = ns8390
ROM_TYPE_rtl8029 = pci
ROM_DESCRIPTION_rtl8029 = "Realtek 8029"
PCI_VENDOR_rtl8029 = 0x10ec
PCI_DEVICE_rtl8029 = 0x8029
ROMS += rtl8029
ROMS_ns8390 += rtl8029

# NIC	10ec8029	10ec,8029	Realtek 8029
DRIVER_10ec8029 = ns8390
ROM_TYPE_10ec8029 = pci
ROM_DESCRIPTION_10ec8029 = "Realtek 8029"
PCI_VENDOR_10ec8029 = 0x10ec
PCI_DEVICE_10ec8029 = 0x8029
ROMS += 10ec8029
ROMS_ns8390 += 10ec8029

# NIC	dlink-528	1186,0300	D-Link DE-528
DRIVER_dlink-528 = ns8390
ROM_TYPE_dlink-528 = pci
ROM_DESCRIPTION_dlink-528 = "D-Link DE-528"
PCI_VENDOR_dlink-528 = 0x1186
PCI_DEVICE_dlink-528 = 0x0300
ROMS += dlink-528
ROMS_ns8390 += dlink-528

# NIC	11860300	1186,0300	D-Link DE-528
DRIVER_11860300 = ns8390
ROM_TYPE_11860300 = pci
ROM_DESCRIPTION_11860300 = "D-Link DE-528"
PCI_VENDOR_11860300 = 0x1186
PCI_DEVICE_11860300 = 0x0300
ROMS += 11860300
ROMS_ns8390 += 11860300

# NIC	winbond940	1050,0940	Winbond NE2000-PCI
DRIVER_winbond940 = ns8390
ROM_TYPE_winbond940 = pci
ROM_DESCRIPTION_winbond940 = "Winbond NE2000-PCI"
PCI_VENDOR_winbond940 = 0x1050
PCI_DEVICE_winbond940 = 0x0940
ROMS += winbond940
ROMS_ns8390 += winbond940

# NIC	10500940	1050,0940	Winbond NE2000-PCI
DRIVER_10500940 = ns8390
ROM_TYPE_10500940 = pci
ROM_DESCRIPTION_10500940 = "Winbond NE2000-PCI"
PCI_VENDOR_10500940 = 0x1050
PCI_DEVICE_10500940 = 0x0940
ROMS += 10500940
ROMS_ns8390 += 10500940

# NIC	winbond940f	1050,5a5a	Winbond W89c940F
DRIVER_winbond940f = ns8390
ROM_TYPE_winbond940f = pci
ROM_DESCRIPTION_winbond940f = "Winbond W89c940F"
PCI_VENDOR_winbond940f = 0x1050
PCI_DEVICE_winbond940f = 0x5a5a
ROMS += winbond940f
ROMS_ns8390 += winbond940f

# NIC	10505a5a	1050,5a5a	Winbond W89c940F
DRIVER_10505a5a = ns8390
ROM_TYPE_10505a5a = pci
ROM_DESCRIPTION_10505a5a = "Winbond W89c940F"
PCI_VENDOR_10505a5a = 0x1050
PCI_DEVICE_10505a5a = 0x5a5a
ROMS += 10505a5a
ROMS_ns8390 += 10505a5a

# NIC	compexrl2000	11f6,1401	Compex ReadyLink 2000
DRIVER_compexrl2000 = ns8390
ROM_TYPE_compexrl2000 = pci
ROM_DESCRIPTION_compexrl2000 = "Compex ReadyLink 2000"
PCI_VENDOR_compexrl2000 = 0x11f6
PCI_DEVICE_compexrl2000 = 0x1401
ROMS += compexrl2000
ROMS_ns8390 += compexrl2000

# NIC	11f61401	11f6,1401	Compex ReadyLink 2000
DRIVER_11f61401 = ns8390
ROM_TYPE_11f61401 = pci
ROM_DESCRIPTION_11f61401 = "Compex ReadyLink 2000"
PCI_VENDOR_11f61401 = 0x11f6
PCI_DEVICE_11f61401 = 0x1401
ROMS += 11f61401
ROMS_ns8390 += 11f61401

# NIC	ktiet32p2	8e2e,3000	KTI ET32P2
DRIVER_ktiet32p2 = ns8390
ROM_TYPE_ktiet32p2 = pci
ROM_DESCRIPTION_ktiet32p2 = "KTI ET32P2"
PCI_VENDOR_ktiet32p2 = 0x8e2e
PCI_DEVICE_ktiet32p2 = 0x3000
ROMS += ktiet32p2
ROMS_ns8390 += ktiet32p2

# NIC	8e2e3000	8e2e,3000	KTI ET32P2
DRIVER_8e2e3000 = ns8390
ROM_TYPE_8e2e3000 = pci
ROM_DESCRIPTION_8e2e3000 = "KTI ET32P2"
PCI_VENDOR_8e2e3000 = 0x8e2e
PCI_DEVICE_8e2e3000 = 0x3000
ROMS += 8e2e3000
ROMS_ns8390 += 8e2e3000

# NIC	nv5000sc	4a14,5000	NetVin NV5000SC
DRIVER_nv5000sc = ns8390
ROM_TYPE_nv5000sc = pci
ROM_DESCRIPTION_nv5000sc = "NetVin NV5000SC"
PCI_VENDOR_nv5000sc = 0x4a14
PCI_DEVICE_nv5000sc = 0x5000
ROMS += nv5000sc
ROMS_ns8390 += nv5000sc

# NIC	4a145000	4a14,5000	NetVin NV5000SC
DRIVER_4a145000 = ns8390
ROM_TYPE_4a145000 = pci
ROM_DESCRIPTION_4a145000 = "NetVin NV5000SC"
PCI_VENDOR_4a145000 = 0x4a14
PCI_DEVICE_4a145000 = 0x5000
ROMS += 4a145000
ROMS_ns8390 += 4a145000

# NIC	holtek80232	12c3,0058	Holtek HT80232
DRIVER_holtek80232 = ns8390
ROM_TYPE_holtek80232 = pci
ROM_DESCRIPTION_holtek80232 = "Holtek HT80232"
PCI_VENDOR_holtek80232 = 0x12c3
PCI_DEVICE_holtek80232 = 0x0058
ROMS += holtek80232
ROMS_ns8390 += holtek80232

# NIC	12c30058	12c3,0058	Holtek HT80232
DRIVER_12c30058 = ns8390
ROM_TYPE_12c30058 = pci
ROM_DESCRIPTION_12c30058 = "Holtek HT80232"
PCI_VENDOR_12c30058 = 0x12c3
PCI_DEVICE_12c30058 = 0x0058
ROMS += 12c30058
ROMS_ns8390 += 12c30058

# NIC	holtek80229	12c3,5598	Holtek HT80229
DRIVER_holtek80229 = ns8390
ROM_TYPE_holtek80229 = pci
ROM_DESCRIPTION_holtek80229 = "Holtek HT80229"
PCI_VENDOR_holtek80229 = 0x12c3
PCI_DEVICE_holtek80229 = 0x5598
ROMS += holtek80229
ROMS_ns8390 += holtek80229

# NIC	12c35598	12c3,5598	Holtek HT80229
DRIVER_12c35598 = ns8390
ROM_TYPE_12c35598 = pci
ROM_DESCRIPTION_12c35598 = "Holtek HT80229"
PCI_VENDOR_12c35598 = 0x12c3
PCI_DEVICE_12c35598 = 0x5598
ROMS += 12c35598
ROMS_ns8390 += 12c35598

# NIC	surecom-ne34	10bd,0e34	Surecom NE34
DRIVER_surecom-ne34 = ns8390
ROM_TYPE_surecom-ne34 = pci
ROM_DESCRIPTION_surecom-ne34 = "Surecom NE34"
PCI_VENDOR_surecom-ne34 = 0x10bd
PCI_DEVICE_surecom-ne34 = 0x0e34
ROMS += surecom-ne34
ROMS_ns8390 += surecom-ne34

# NIC	10bd0e34	10bd,0e34	Surecom NE34
DRIVER_10bd0e34 = ns8390
ROM_TYPE_10bd0e34 = pci
ROM_DESCRIPTION_10bd0e34 = "Surecom NE34"
PCI_VENDOR_10bd0e34 = 0x10bd
PCI_DEVICE_10bd0e34 = 0x0e34
ROMS += 10bd0e34
ROMS_ns8390 += 10bd0e34

# NIC	via86c926	1106,0926	Via 86c926
DRIVER_via86c926 = ns8390
ROM_TYPE_via86c926 = pci
ROM_DESCRIPTION_via86c926 = "Via 86c926"
PCI_VENDOR_via86c926 = 0x1106
PCI_DEVICE_via86c926 = 0x0926
ROMS += via86c926
ROMS_ns8390 += via86c926

# NIC	11060926	1106,0926	Via 86c926
DRIVER_11060926 = ns8390
ROM_TYPE_11060926 = pci
ROM_DESCRIPTION_11060926 = "Via 86c926"
PCI_VENDOR_11060926 = 0x1106
PCI_DEVICE_11060926 = 0x0926
ROMS += 11060926
ROMS_ns8390 += 11060926
