prism2_plx_DEPS = drivers/net/prism2_plx.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/pci.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/device.h \
 include/gpxe/list.h include/stddef.h include/assert.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h include/nic.h \
 include/stdio.h include/stdarg.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/isapnp.h include/gpxe/isa_ids.h \
 include/gpxe/isa.h include/gpxe/eisa.h include/gpxe/mca.h \
 drivers/net/prism2.c include/etherboot.h include/stdlib.h \
 include/unistd.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/strings.h \
 arch/i386/include/limits.h include/console.h include/gpxe/if_arp.h \
 include/gpxe/if_ether.h include/gpxe/ethernet.h \
 drivers/net/wlan_compat.h drivers/net/p80211hdr.h drivers/net/hfa384x.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/pci.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/nic.h:

include/stdio.h:

include/stdarg.h:

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

drivers/net/prism2.c:

include/etherboot.h:

include/stdlib.h:

include/unistd.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/strings.h:

arch/i386/include/limits.h:

include/console.h:

include/gpxe/if_arp.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

drivers/net/wlan_compat.h:

drivers/net/p80211hdr.h:

drivers/net/hfa384x.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

$(BIN)/prism2_plx.o : drivers/net/prism2_plx.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_plx_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/prism2_plx.o
 
$(BIN)/prism2_plx.dbg%.o : drivers/net/prism2_plx.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_plx_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/prism2_plx.dbg%.o
 
$(BIN)/prism2_plx.c : drivers/net/prism2_plx.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_plx_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/prism2_plx.c
 
$(BIN)/prism2_plx.s : drivers/net/prism2_plx.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_plx_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/prism2_plx.s
 
bin/deps/drivers/net/prism2_plx.c.d : $(prism2_plx_DEPS)
 
TAGS : $(prism2_plx_DEPS)


# NIC	
# NIC	family	drivers/net/prism2_plx
DRIVERS += prism2_plx

# NIC	ma301	1385,4100	Netgear MA301
DRIVER_ma301 = prism2_plx
ROM_TYPE_ma301 = pci
ROM_DESCRIPTION_ma301 = "Netgear MA301"
PCI_VENDOR_ma301 = 0x1385
PCI_DEVICE_ma301 = 0x4100
ROMS += ma301
ROMS_prism2_plx += ma301

# NIC	13854100	1385,4100	Netgear MA301
DRIVER_13854100 = prism2_plx
ROM_TYPE_13854100 = pci
ROM_DESCRIPTION_13854100 = "Netgear MA301"
PCI_VENDOR_13854100 = 0x1385
PCI_DEVICE_13854100 = 0x4100
ROMS += 13854100
ROMS_prism2_plx += 13854100

# NIC	3c-airconnect	10b7,7770	3Com AirConnect
DRIVER_3c-airconnect = prism2_plx
ROM_TYPE_3c-airconnect = pci
ROM_DESCRIPTION_3c-airconnect = "3Com AirConnect"
PCI_VENDOR_3c-airconnect = 0x10b7
PCI_DEVICE_3c-airconnect = 0x7770
ROMS += 3c-airconnect
ROMS_prism2_plx += 3c-airconnect

# NIC	10b77770	10b7,7770	3Com AirConnect
DRIVER_10b77770 = prism2_plx
ROM_TYPE_10b77770 = pci
ROM_DESCRIPTION_10b77770 = "3Com AirConnect"
PCI_VENDOR_10b77770 = 0x10b7
PCI_DEVICE_10b77770 = 0x7770
ROMS += 10b77770
ROMS_prism2_plx += 10b77770

# NIC	ss1023	111a,1023	Siemens SpeedStream SS1023
DRIVER_ss1023 = prism2_plx
ROM_TYPE_ss1023 = pci
ROM_DESCRIPTION_ss1023 = "Siemens SpeedStream SS1023"
PCI_VENDOR_ss1023 = 0x111a
PCI_DEVICE_ss1023 = 0x1023
ROMS += ss1023
ROMS_prism2_plx += ss1023

# NIC	111a1023	111a,1023	Siemens SpeedStream SS1023
DRIVER_111a1023 = prism2_plx
ROM_TYPE_111a1023 = pci
ROM_DESCRIPTION_111a1023 = "Siemens SpeedStream SS1023"
PCI_VENDOR_111a1023 = 0x111a
PCI_DEVICE_111a1023 = 0x1023
ROMS += 111a1023
ROMS_prism2_plx += 111a1023

# NIC	correga	15e8,0130	Correga
DRIVER_correga = prism2_plx
ROM_TYPE_correga = pci
ROM_DESCRIPTION_correga = "Correga"
PCI_VENDOR_correga = 0x15e8
PCI_DEVICE_correga = 0x0130
ROMS += correga
ROMS_prism2_plx += correga

# NIC	15e80130	15e8,0130	Correga
DRIVER_15e80130 = prism2_plx
ROM_TYPE_15e80130 = pci
ROM_DESCRIPTION_15e80130 = "Correga"
PCI_VENDOR_15e80130 = 0x15e8
PCI_DEVICE_15e80130 = 0x0130
ROMS += 15e80130
ROMS_prism2_plx += 15e80130

# NIC	smc2602w	1638,1100	SMC EZConnect SMC2602W
DRIVER_smc2602w = prism2_plx
ROM_TYPE_smc2602w = pci
ROM_DESCRIPTION_smc2602w = "SMC EZConnect SMC2602W"
PCI_VENDOR_smc2602w = 0x1638
PCI_DEVICE_smc2602w = 0x1100
ROMS += smc2602w
ROMS_prism2_plx += smc2602w

# NIC	16381100	1638,1100	SMC EZConnect SMC2602W
DRIVER_16381100 = prism2_plx
ROM_TYPE_16381100 = pci
ROM_DESCRIPTION_16381100 = "SMC EZConnect SMC2602W"
PCI_VENDOR_16381100 = 0x1638
PCI_DEVICE_16381100 = 0x1100
ROMS += 16381100
ROMS_prism2_plx += 16381100

# NIC	gl24110p	16ab,1100	Global Sun Tech GL24110P
DRIVER_gl24110p = prism2_plx
ROM_TYPE_gl24110p = pci
ROM_DESCRIPTION_gl24110p = "Global Sun Tech GL24110P"
PCI_VENDOR_gl24110p = 0x16ab
PCI_DEVICE_gl24110p = 0x1100
ROMS += gl24110p
ROMS_prism2_plx += gl24110p

# NIC	16ab1100	16ab,1100	Global Sun Tech GL24110P
DRIVER_16ab1100 = prism2_plx
ROM_TYPE_16ab1100 = pci
ROM_DESCRIPTION_16ab1100 = "Global Sun Tech GL24110P"
PCI_VENDOR_16ab1100 = 0x16ab
PCI_DEVICE_16ab1100 = 0x1100
ROMS += 16ab1100
ROMS_prism2_plx += 16ab1100

# NIC	16ab-1101	16ab,1101	Unknown
DRIVER_16ab-1101 = prism2_plx
ROM_TYPE_16ab-1101 = pci
ROM_DESCRIPTION_16ab-1101 = "Unknown"
PCI_VENDOR_16ab-1101 = 0x16ab
PCI_DEVICE_16ab-1101 = 0x1101
ROMS += 16ab-1101
ROMS_prism2_plx += 16ab-1101

# NIC	16ab1101	16ab,1101	Unknown
DRIVER_16ab1101 = prism2_plx
ROM_TYPE_16ab1101 = pci
ROM_DESCRIPTION_16ab1101 = "Unknown"
PCI_VENDOR_16ab1101 = 0x16ab
PCI_DEVICE_16ab1101 = 0x1101
ROMS += 16ab1101
ROMS_prism2_plx += 16ab1101

# NIC	wdt11	16ab,1102	Linksys WDT11
DRIVER_wdt11 = prism2_plx
ROM_TYPE_wdt11 = pci
ROM_DESCRIPTION_wdt11 = "Linksys WDT11"
PCI_VENDOR_wdt11 = 0x16ab
PCI_DEVICE_wdt11 = 0x1102
ROMS += wdt11
ROMS_prism2_plx += wdt11

# NIC	16ab1102	16ab,1102	Linksys WDT11
DRIVER_16ab1102 = prism2_plx
ROM_TYPE_16ab1102 = pci
ROM_DESCRIPTION_16ab1102 = "Linksys WDT11"
PCI_VENDOR_16ab1102 = 0x16ab
PCI_DEVICE_16ab1102 = 0x1102
ROMS += 16ab1102
ROMS_prism2_plx += 16ab1102

# NIC	usr2415	16ec,3685	USR 2415
DRIVER_usr2415 = prism2_plx
ROM_TYPE_usr2415 = pci
ROM_DESCRIPTION_usr2415 = "USR 2415"
PCI_VENDOR_usr2415 = 0x16ec
PCI_DEVICE_usr2415 = 0x3685
ROMS += usr2415
ROMS_prism2_plx += usr2415

# NIC	16ec3685	16ec,3685	USR 2415
DRIVER_16ec3685 = prism2_plx
ROM_TYPE_16ec3685 = pci
ROM_DESCRIPTION_16ec3685 = "USR 2415"
PCI_VENDOR_16ec3685 = 0x16ec
PCI_DEVICE_16ec3685 = 0x3685
ROMS += 16ec3685
ROMS_prism2_plx += 16ec3685

# NIC	f5d6000	ec80,ec00	Belkin F5D6000
DRIVER_f5d6000 = prism2_plx
ROM_TYPE_f5d6000 = pci
ROM_DESCRIPTION_f5d6000 = "Belkin F5D6000"
PCI_VENDOR_f5d6000 = 0xec80
PCI_DEVICE_f5d6000 = 0xec00
ROMS += f5d6000
ROMS_prism2_plx += f5d6000

# NIC	ec80ec00	ec80,ec00	Belkin F5D6000
DRIVER_ec80ec00 = prism2_plx
ROM_TYPE_ec80ec00 = pci
ROM_DESCRIPTION_ec80ec00 = "Belkin F5D6000"
PCI_VENDOR_ec80ec00 = 0xec80
PCI_DEVICE_ec80ec00 = 0xec00
ROMS += ec80ec00
ROMS_prism2_plx += ec80ec00

# NIC	emobility	126c,8030	Nortel emobility
DRIVER_emobility = prism2_plx
ROM_TYPE_emobility = pci
ROM_DESCRIPTION_emobility = "Nortel emobility"
PCI_VENDOR_emobility = 0x126c
PCI_DEVICE_emobility = 0x8030
ROMS += emobility
ROMS_prism2_plx += emobility

# NIC	126c8030	126c,8030	Nortel emobility
DRIVER_126c8030 = prism2_plx
ROM_TYPE_126c8030 = pci
ROM_DESCRIPTION_126c8030 = "Nortel emobility"
PCI_VENDOR_126c8030 = 0x126c
PCI_DEVICE_126c8030 = 0x8030
ROMS += 126c8030
ROMS_prism2_plx += 126c8030
