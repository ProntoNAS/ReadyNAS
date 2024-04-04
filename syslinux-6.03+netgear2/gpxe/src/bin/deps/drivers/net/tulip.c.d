tulip_DEPS = drivers/net/tulip.c include/compiler.h \
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

$(BIN)/tulip.o : drivers/net/tulip.c $(MAKEDEPS) $(POST_O_DEPS) $(tulip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tulip.o
 
$(BIN)/tulip.dbg%.o : drivers/net/tulip.c $(MAKEDEPS) $(POST_O_DEPS) $(tulip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tulip.dbg%.o
 
$(BIN)/tulip.c : drivers/net/tulip.c $(MAKEDEPS) $(POST_O_DEPS) $(tulip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tulip.c
 
$(BIN)/tulip.s : drivers/net/tulip.c $(MAKEDEPS) $(POST_O_DEPS) $(tulip_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tulip.s
 
bin/deps/drivers/net/tulip.c.d : $(tulip_DEPS)
 
TAGS : $(tulip_DEPS)


# NIC	
# NIC	family	drivers/net/tulip
DRIVERS += tulip

# NIC	dc21040	1011,0002	Digital Tulip
DRIVER_dc21040 = tulip
ROM_TYPE_dc21040 = pci
ROM_DESCRIPTION_dc21040 = "Digital Tulip"
PCI_VENDOR_dc21040 = 0x1011
PCI_DEVICE_dc21040 = 0x0002
ROMS += dc21040
ROMS_tulip += dc21040

# NIC	10110002	1011,0002	Digital Tulip
DRIVER_10110002 = tulip
ROM_TYPE_10110002 = pci
ROM_DESCRIPTION_10110002 = "Digital Tulip"
PCI_VENDOR_10110002 = 0x1011
PCI_DEVICE_10110002 = 0x0002
ROMS += 10110002
ROMS_tulip += 10110002

# NIC	ds21140	1011,0009	Digital Tulip Fast
DRIVER_ds21140 = tulip
ROM_TYPE_ds21140 = pci
ROM_DESCRIPTION_ds21140 = "Digital Tulip Fast"
PCI_VENDOR_ds21140 = 0x1011
PCI_DEVICE_ds21140 = 0x0009
ROMS += ds21140
ROMS_tulip += ds21140

# NIC	10110009	1011,0009	Digital Tulip Fast
DRIVER_10110009 = tulip
ROM_TYPE_10110009 = pci
ROM_DESCRIPTION_10110009 = "Digital Tulip Fast"
PCI_VENDOR_10110009 = 0x1011
PCI_DEVICE_10110009 = 0x0009
ROMS += 10110009
ROMS_tulip += 10110009

# NIC	dc21041	1011,0014	Digital Tulip+
DRIVER_dc21041 = tulip
ROM_TYPE_dc21041 = pci
ROM_DESCRIPTION_dc21041 = "Digital Tulip+"
PCI_VENDOR_dc21041 = 0x1011
PCI_DEVICE_dc21041 = 0x0014
ROMS += dc21041
ROMS_tulip += dc21041

# NIC	10110014	1011,0014	Digital Tulip+
DRIVER_10110014 = tulip
ROM_TYPE_10110014 = pci
ROM_DESCRIPTION_10110014 = "Digital Tulip+"
PCI_VENDOR_10110014 = 0x1011
PCI_DEVICE_10110014 = 0x0014
ROMS += 10110014
ROMS_tulip += 10110014

# NIC	ds21142	1011,0019	Digital Tulip 21142
DRIVER_ds21142 = tulip
ROM_TYPE_ds21142 = pci
ROM_DESCRIPTION_ds21142 = "Digital Tulip 21142"
PCI_VENDOR_ds21142 = 0x1011
PCI_DEVICE_ds21142 = 0x0019
ROMS += ds21142
ROMS_tulip += ds21142

# NIC	10110019	1011,0019	Digital Tulip 21142
DRIVER_10110019 = tulip
ROM_TYPE_10110019 = pci
ROM_DESCRIPTION_10110019 = "Digital Tulip 21142"
PCI_VENDOR_10110019 = 0x1011
PCI_DEVICE_10110019 = 0x0019
ROMS += 10110019
ROMS_tulip += 10110019

# NIC	3csoho100b-tx	10b7,9300	3ComSOHO100B-TX
DRIVER_3csoho100b-tx = tulip
ROM_TYPE_3csoho100b-tx = pci
ROM_DESCRIPTION_3csoho100b-tx = "3ComSOHO100B-TX"
PCI_VENDOR_3csoho100b-tx = 0x10b7
PCI_DEVICE_3csoho100b-tx = 0x9300
ROMS += 3csoho100b-tx
ROMS_tulip += 3csoho100b-tx

# NIC	10b79300	10b7,9300	3ComSOHO100B-TX
DRIVER_10b79300 = tulip
ROM_TYPE_10b79300 = pci
ROM_DESCRIPTION_10b79300 = "3ComSOHO100B-TX"
PCI_VENDOR_10b79300 = 0x10b7
PCI_DEVICE_10b79300 = 0x9300
ROMS += 10b79300
ROMS_tulip += 10b79300

# NIC	ali1563	10b9,5261	ALi 1563 integrated ethernet
DRIVER_ali1563 = tulip
ROM_TYPE_ali1563 = pci
ROM_DESCRIPTION_ali1563 = "ALi 1563 integrated ethernet"
PCI_VENDOR_ali1563 = 0x10b9
PCI_DEVICE_ali1563 = 0x5261
ROMS += ali1563
ROMS_tulip += ali1563

# NIC	10b95261	10b9,5261	ALi 1563 integrated ethernet
DRIVER_10b95261 = tulip
ROM_TYPE_10b95261 = pci
ROM_DESCRIPTION_10b95261 = "ALi 1563 integrated ethernet"
PCI_VENDOR_10b95261 = 0x10b9
PCI_DEVICE_10b95261 = 0x5261
ROMS += 10b95261
ROMS_tulip += 10b95261

# NIC	mx98713	10d9,0512	Macronix MX987x3
DRIVER_mx98713 = tulip
ROM_TYPE_mx98713 = pci
ROM_DESCRIPTION_mx98713 = "Macronix MX987x3"
PCI_VENDOR_mx98713 = 0x10d9
PCI_DEVICE_mx98713 = 0x0512
ROMS += mx98713
ROMS_tulip += mx98713

# NIC	10d90512	10d9,0512	Macronix MX987x3
DRIVER_10d90512 = tulip
ROM_TYPE_10d90512 = pci
ROM_DESCRIPTION_10d90512 = "Macronix MX987x3"
PCI_VENDOR_10d90512 = 0x10d9
PCI_DEVICE_10d90512 = 0x0512
ROMS += 10d90512
ROMS_tulip += 10d90512

# NIC	mx98715	10d9,0531	Macronix MX987x5
DRIVER_mx98715 = tulip
ROM_TYPE_mx98715 = pci
ROM_DESCRIPTION_mx98715 = "Macronix MX987x5"
PCI_VENDOR_mx98715 = 0x10d9
PCI_DEVICE_mx98715 = 0x0531
ROMS += mx98715
ROMS_tulip += mx98715

# NIC	10d90531	10d9,0531	Macronix MX987x5
DRIVER_10d90531 = tulip
ROM_TYPE_10d90531 = pci
ROM_DESCRIPTION_10d90531 = "Macronix MX987x5"
PCI_VENDOR_10d90531 = 0x10d9
PCI_DEVICE_10d90531 = 0x0531
ROMS += 10d90531
ROMS_tulip += 10d90531

# NIC	mxic-98715	1113,1217	Macronix MX987x5
DRIVER_mxic-98715 = tulip
ROM_TYPE_mxic-98715 = pci
ROM_DESCRIPTION_mxic-98715 = "Macronix MX987x5"
PCI_VENDOR_mxic-98715 = 0x1113
PCI_DEVICE_mxic-98715 = 0x1217
ROMS += mxic-98715
ROMS_tulip += mxic-98715

# NIC	11131217	1113,1217	Macronix MX987x5
DRIVER_11131217 = tulip
ROM_TYPE_11131217 = pci
ROM_DESCRIPTION_11131217 = "Macronix MX987x5"
PCI_VENDOR_11131217 = 0x1113
PCI_DEVICE_11131217 = 0x1217
ROMS += 11131217
ROMS_tulip += 11131217

# NIC	lc82c115	11ad,c115	LinkSys LNE100TX
DRIVER_lc82c115 = tulip
ROM_TYPE_lc82c115 = pci
ROM_DESCRIPTION_lc82c115 = "LinkSys LNE100TX"
PCI_VENDOR_lc82c115 = 0x11ad
PCI_DEVICE_lc82c115 = 0xc115
ROMS += lc82c115
ROMS_tulip += lc82c115

# NIC	11adc115	11ad,c115	LinkSys LNE100TX
DRIVER_11adc115 = tulip
ROM_TYPE_11adc115 = pci
ROM_DESCRIPTION_11adc115 = "LinkSys LNE100TX"
PCI_VENDOR_11adc115 = 0x11ad
PCI_DEVICE_11adc115 = 0xc115
ROMS += 11adc115
ROMS_tulip += 11adc115

# NIC	82c168	11ad,0002	Netgear FA310TX
DRIVER_82c168 = tulip
ROM_TYPE_82c168 = pci
ROM_DESCRIPTION_82c168 = "Netgear FA310TX"
PCI_VENDOR_82c168 = 0x11ad
PCI_DEVICE_82c168 = 0x0002
ROMS += 82c168
ROMS_tulip += 82c168

# NIC	11ad0002	11ad,0002	Netgear FA310TX
DRIVER_11ad0002 = tulip
ROM_TYPE_11ad0002 = pci
ROM_DESCRIPTION_11ad0002 = "Netgear FA310TX"
PCI_VENDOR_11ad0002 = 0x11ad
PCI_DEVICE_11ad0002 = 0x0002
ROMS += 11ad0002
ROMS_tulip += 11ad0002

# NIC	dm9100	1282,9100	Davicom 9100
DRIVER_dm9100 = tulip
ROM_TYPE_dm9100 = pci
ROM_DESCRIPTION_dm9100 = "Davicom 9100"
PCI_VENDOR_dm9100 = 0x1282
PCI_DEVICE_dm9100 = 0x9100
ROMS += dm9100
ROMS_tulip += dm9100

# NIC	12829100	1282,9100	Davicom 9100
DRIVER_12829100 = tulip
ROM_TYPE_12829100 = pci
ROM_DESCRIPTION_12829100 = "Davicom 9100"
PCI_VENDOR_12829100 = 0x1282
PCI_DEVICE_12829100 = 0x9100
ROMS += 12829100
ROMS_tulip += 12829100

# NIC	dm9102	1282,9102	Davicom 9102
DRIVER_dm9102 = tulip
ROM_TYPE_dm9102 = pci
ROM_DESCRIPTION_dm9102 = "Davicom 9102"
PCI_VENDOR_dm9102 = 0x1282
PCI_DEVICE_dm9102 = 0x9102
ROMS += dm9102
ROMS_tulip += dm9102

# NIC	12829102	1282,9102	Davicom 9102
DRIVER_12829102 = tulip
ROM_TYPE_12829102 = pci
ROM_DESCRIPTION_12829102 = "Davicom 9102"
PCI_VENDOR_12829102 = 0x1282
PCI_DEVICE_12829102 = 0x9102
ROMS += 12829102
ROMS_tulip += 12829102

# NIC	dm9009	1282,9009	Davicom 9009
DRIVER_dm9009 = tulip
ROM_TYPE_dm9009 = pci
ROM_DESCRIPTION_dm9009 = "Davicom 9009"
PCI_VENDOR_dm9009 = 0x1282
PCI_DEVICE_dm9009 = 0x9009
ROMS += dm9009
ROMS_tulip += dm9009

# NIC	12829009	1282,9009	Davicom 9009
DRIVER_12829009 = tulip
ROM_TYPE_12829009 = pci
ROM_DESCRIPTION_12829009 = "Davicom 9009"
PCI_VENDOR_12829009 = 0x1282
PCI_DEVICE_12829009 = 0x9009
ROMS += 12829009
ROMS_tulip += 12829009

# NIC	dm9132	1282,9132	Davicom 9132
DRIVER_dm9132 = tulip
ROM_TYPE_dm9132 = pci
ROM_DESCRIPTION_dm9132 = "Davicom 9132"
PCI_VENDOR_dm9132 = 0x1282
PCI_DEVICE_dm9132 = 0x9132
ROMS += dm9132
ROMS_tulip += dm9132

# NIC	12829132	1282,9132	Davicom 9132
DRIVER_12829132 = tulip
ROM_TYPE_12829132 = pci
ROM_DESCRIPTION_12829132 = "Davicom 9132"
PCI_VENDOR_12829132 = 0x1282
PCI_DEVICE_12829132 = 0x9132
ROMS += 12829132
ROMS_tulip += 12829132

# NIC	centaur-p	1317,0985	ADMtek Centaur-P
DRIVER_centaur-p = tulip
ROM_TYPE_centaur-p = pci
ROM_DESCRIPTION_centaur-p = "ADMtek Centaur-P"
PCI_VENDOR_centaur-p = 0x1317
PCI_DEVICE_centaur-p = 0x0985
ROMS += centaur-p
ROMS_tulip += centaur-p

# NIC	13170985	1317,0985	ADMtek Centaur-P
DRIVER_13170985 = tulip
ROM_TYPE_13170985 = pci
ROM_DESCRIPTION_13170985 = "ADMtek Centaur-P"
PCI_VENDOR_13170985 = 0x1317
PCI_DEVICE_13170985 = 0x0985
ROMS += 13170985
ROMS_tulip += 13170985

# NIC	an981	1317,0981	ADMtek AN981 Comet
DRIVER_an981 = tulip
ROM_TYPE_an981 = pci
ROM_DESCRIPTION_an981 = "ADMtek AN981 Comet"
PCI_VENDOR_an981 = 0x1317
PCI_DEVICE_an981 = 0x0981
ROMS += an981
ROMS_tulip += an981

# NIC	13170981	1317,0981	ADMtek AN981 Comet
DRIVER_13170981 = tulip
ROM_TYPE_13170981 = pci
ROM_DESCRIPTION_13170981 = "ADMtek AN981 Comet"
PCI_VENDOR_13170981 = 0x1317
PCI_DEVICE_13170981 = 0x0981
ROMS += 13170981
ROMS_tulip += 13170981

# NIC	an983	1113,1216	ADMTek AN983 Comet
DRIVER_an983 = tulip
ROM_TYPE_an983 = pci
ROM_DESCRIPTION_an983 = "ADMTek AN983 Comet"
PCI_VENDOR_an983 = 0x1113
PCI_DEVICE_an983 = 0x1216
ROMS += an983
ROMS_tulip += an983

# NIC	11131216	1113,1216	ADMTek AN983 Comet
DRIVER_11131216 = tulip
ROM_TYPE_11131216 = pci
ROM_DESCRIPTION_11131216 = "ADMTek AN983 Comet"
PCI_VENDOR_11131216 = 0x1113
PCI_DEVICE_11131216 = 0x1216
ROMS += 11131216
ROMS_tulip += 11131216

# NIC	an983b	1317,9511	ADMTek Comet 983b
DRIVER_an983b = tulip
ROM_TYPE_an983b = pci
ROM_DESCRIPTION_an983b = "ADMTek Comet 983b"
PCI_VENDOR_an983b = 0x1317
PCI_DEVICE_an983b = 0x9511
ROMS += an983b
ROMS_tulip += an983b

# NIC	13179511	1317,9511	ADMTek Comet 983b
DRIVER_13179511 = tulip
ROM_TYPE_13179511 = pci
ROM_DESCRIPTION_13179511 = "ADMTek Comet 983b"
PCI_VENDOR_13179511 = 0x1317
PCI_DEVICE_13179511 = 0x9511
ROMS += 13179511
ROMS_tulip += 13179511

# NIC	centaur-c	1317,1985	ADMTek Centaur-C
DRIVER_centaur-c = tulip
ROM_TYPE_centaur-c = pci
ROM_DESCRIPTION_centaur-c = "ADMTek Centaur-C"
PCI_VENDOR_centaur-c = 0x1317
PCI_DEVICE_centaur-c = 0x1985
ROMS += centaur-c
ROMS_tulip += centaur-c

# NIC	13171985	1317,1985	ADMTek Centaur-C
DRIVER_13171985 = tulip
ROM_TYPE_13171985 = pci
ROM_DESCRIPTION_13171985 = "ADMTek Centaur-C"
PCI_VENDOR_13171985 = 0x1317
PCI_DEVICE_13171985 = 0x1985
ROMS += 13171985
ROMS_tulip += 13171985

# NIC	intel21145	8086,0039	Intel Tulip
DRIVER_intel21145 = tulip
ROM_TYPE_intel21145 = pci
ROM_DESCRIPTION_intel21145 = "Intel Tulip"
PCI_VENDOR_intel21145 = 0x8086
PCI_DEVICE_intel21145 = 0x0039
ROMS += intel21145
ROMS_tulip += intel21145

# NIC	80860039	8086,0039	Intel Tulip
DRIVER_80860039 = tulip
ROM_TYPE_80860039 = pci
ROM_DESCRIPTION_80860039 = "Intel Tulip"
PCI_VENDOR_80860039 = 0x8086
PCI_DEVICE_80860039 = 0x0039
ROMS += 80860039
ROMS_tulip += 80860039

# NIC	ax88140	125b,1400	ASIX AX88140
DRIVER_ax88140 = tulip
ROM_TYPE_ax88140 = pci
ROM_DESCRIPTION_ax88140 = "ASIX AX88140"
PCI_VENDOR_ax88140 = 0x125b
PCI_DEVICE_ax88140 = 0x1400
ROMS += ax88140
ROMS_tulip += ax88140

# NIC	125b1400	125b,1400	ASIX AX88140
DRIVER_125b1400 = tulip
ROM_TYPE_125b1400 = pci
ROM_DESCRIPTION_125b1400 = "ASIX AX88140"
PCI_VENDOR_125b1400 = 0x125b
PCI_DEVICE_125b1400 = 0x1400
ROMS += 125b1400
ROMS_tulip += 125b1400

# NIC	rl100tx	11f6,9881	Compex RL100-TX
DRIVER_rl100tx = tulip
ROM_TYPE_rl100tx = pci
ROM_DESCRIPTION_rl100tx = "Compex RL100-TX"
PCI_VENDOR_rl100tx = 0x11f6
PCI_DEVICE_rl100tx = 0x9881
ROMS += rl100tx
ROMS_tulip += rl100tx

# NIC	11f69881	11f6,9881	Compex RL100-TX
DRIVER_11f69881 = tulip
ROM_TYPE_11f69881 = pci
ROM_DESCRIPTION_11f69881 = "Compex RL100-TX"
PCI_VENDOR_11f69881 = 0x11f6
PCI_DEVICE_11f69881 = 0x9881
ROMS += 11f69881
ROMS_tulip += 11f69881

# NIC	xircomtulip	115d,0003	Xircom Tulip
DRIVER_xircomtulip = tulip
ROM_TYPE_xircomtulip = pci
ROM_DESCRIPTION_xircomtulip = "Xircom Tulip"
PCI_VENDOR_xircomtulip = 0x115d
PCI_DEVICE_xircomtulip = 0x0003
ROMS += xircomtulip
ROMS_tulip += xircomtulip

# NIC	115d0003	115d,0003	Xircom Tulip
DRIVER_115d0003 = tulip
ROM_TYPE_115d0003 = pci
ROM_DESCRIPTION_115d0003 = "Xircom Tulip"
PCI_VENDOR_115d0003 = 0x115d
PCI_DEVICE_115d0003 = 0x0003
ROMS += 115d0003
ROMS_tulip += 115d0003

# NIC	tulip-0981	104a,0981	Tulip 0x104a 0x0981
DRIVER_tulip-0981 = tulip
ROM_TYPE_tulip-0981 = pci
ROM_DESCRIPTION_tulip-0981 = "Tulip 0x104a 0x0981"
PCI_VENDOR_tulip-0981 = 0x104a
PCI_DEVICE_tulip-0981 = 0x0981
ROMS += tulip-0981
ROMS_tulip += tulip-0981

# NIC	104a0981	104a,0981	Tulip 0x104a 0x0981
DRIVER_104a0981 = tulip
ROM_TYPE_104a0981 = pci
ROM_DESCRIPTION_104a0981 = "Tulip 0x104a 0x0981"
PCI_VENDOR_104a0981 = 0x104a
PCI_DEVICE_104a0981 = 0x0981
ROMS += 104a0981
ROMS_tulip += 104a0981

# NIC	SGThomson-STE10100A	104a,2774	Tulip 0x104a 0x2774
DRIVER_SGThomson-STE10100A = tulip
ROM_TYPE_SGThomson-STE10100A = pci
ROM_DESCRIPTION_SGThomson-STE10100A = "Tulip 0x104a 0x2774"
PCI_VENDOR_SGThomson-STE10100A = 0x104a
PCI_DEVICE_SGThomson-STE10100A = 0x2774
ROMS += SGThomson-STE10100A
ROMS_tulip += SGThomson-STE10100A

# NIC	104a2774	104a,2774	Tulip 0x104a 0x2774
DRIVER_104a2774 = tulip
ROM_TYPE_104a2774 = pci
ROM_DESCRIPTION_104a2774 = "Tulip 0x104a 0x2774"
PCI_VENDOR_104a2774 = 0x104a
PCI_DEVICE_104a2774 = 0x2774
ROMS += 104a2774
ROMS_tulip += 104a2774

# NIC	tulip-9511	1113,9511	Tulip 0x1113 0x9511
DRIVER_tulip-9511 = tulip
ROM_TYPE_tulip-9511 = pci
ROM_DESCRIPTION_tulip-9511 = "Tulip 0x1113 0x9511"
PCI_VENDOR_tulip-9511 = 0x1113
PCI_DEVICE_tulip-9511 = 0x9511
ROMS += tulip-9511
ROMS_tulip += tulip-9511

# NIC	11139511	1113,9511	Tulip 0x1113 0x9511
DRIVER_11139511 = tulip
ROM_TYPE_11139511 = pci
ROM_DESCRIPTION_11139511 = "Tulip 0x1113 0x9511"
PCI_VENDOR_11139511 = 0x1113
PCI_DEVICE_11139511 = 0x9511
ROMS += 11139511
ROMS_tulip += 11139511

# NIC	tulip-1561	1186,1561	Tulip 0x1186 0x1561
DRIVER_tulip-1561 = tulip
ROM_TYPE_tulip-1561 = pci
ROM_DESCRIPTION_tulip-1561 = "Tulip 0x1186 0x1561"
PCI_VENDOR_tulip-1561 = 0x1186
PCI_DEVICE_tulip-1561 = 0x1561
ROMS += tulip-1561
ROMS_tulip += tulip-1561

# NIC	11861561	1186,1561	Tulip 0x1186 0x1561
DRIVER_11861561 = tulip
ROM_TYPE_11861561 = pci
ROM_DESCRIPTION_11861561 = "Tulip 0x1186 0x1561"
PCI_VENDOR_11861561 = 0x1186
PCI_DEVICE_11861561 = 0x1561
ROMS += 11861561
ROMS_tulip += 11861561

# NIC	tulip-a120	1259,a120	Tulip 0x1259 0xa120
DRIVER_tulip-a120 = tulip
ROM_TYPE_tulip-a120 = pci
ROM_DESCRIPTION_tulip-a120 = "Tulip 0x1259 0xa120"
PCI_VENDOR_tulip-a120 = 0x1259
PCI_DEVICE_tulip-a120 = 0xa120
ROMS += tulip-a120
ROMS_tulip += tulip-a120

# NIC	1259a120	1259,a120	Tulip 0x1259 0xa120
DRIVER_1259a120 = tulip
ROM_TYPE_1259a120 = pci
ROM_DESCRIPTION_1259a120 = "Tulip 0x1259 0xa120"
PCI_VENDOR_1259a120 = 0x1259
PCI_DEVICE_1259a120 = 0xa120
ROMS += 1259a120
ROMS_tulip += 1259a120

# NIC	tulip-ab02	13d1,ab02	Tulip 0x13d1 0xab02
DRIVER_tulip-ab02 = tulip
ROM_TYPE_tulip-ab02 = pci
ROM_DESCRIPTION_tulip-ab02 = "Tulip 0x13d1 0xab02"
PCI_VENDOR_tulip-ab02 = 0x13d1
PCI_DEVICE_tulip-ab02 = 0xab02
ROMS += tulip-ab02
ROMS_tulip += tulip-ab02

# NIC	13d1ab02	13d1,ab02	Tulip 0x13d1 0xab02
DRIVER_13d1ab02 = tulip
ROM_TYPE_13d1ab02 = pci
ROM_DESCRIPTION_13d1ab02 = "Tulip 0x13d1 0xab02"
PCI_VENDOR_13d1ab02 = 0x13d1
PCI_DEVICE_13d1ab02 = 0xab02
ROMS += 13d1ab02
ROMS_tulip += 13d1ab02

# NIC	tulip-ab03	13d1,ab03	Tulip 0x13d1 0xab03
DRIVER_tulip-ab03 = tulip
ROM_TYPE_tulip-ab03 = pci
ROM_DESCRIPTION_tulip-ab03 = "Tulip 0x13d1 0xab03"
PCI_VENDOR_tulip-ab03 = 0x13d1
PCI_DEVICE_tulip-ab03 = 0xab03
ROMS += tulip-ab03
ROMS_tulip += tulip-ab03

# NIC	13d1ab03	13d1,ab03	Tulip 0x13d1 0xab03
DRIVER_13d1ab03 = tulip
ROM_TYPE_13d1ab03 = pci
ROM_DESCRIPTION_13d1ab03 = "Tulip 0x13d1 0xab03"
PCI_VENDOR_13d1ab03 = 0x13d1
PCI_DEVICE_13d1ab03 = 0xab03
ROMS += 13d1ab03
ROMS_tulip += 13d1ab03

# NIC	tulip-ab08	13d1,ab08	Tulip 0x13d1 0xab08
DRIVER_tulip-ab08 = tulip
ROM_TYPE_tulip-ab08 = pci
ROM_DESCRIPTION_tulip-ab08 = "Tulip 0x13d1 0xab08"
PCI_VENDOR_tulip-ab08 = 0x13d1
PCI_DEVICE_tulip-ab08 = 0xab08
ROMS += tulip-ab08
ROMS_tulip += tulip-ab08

# NIC	13d1ab08	13d1,ab08	Tulip 0x13d1 0xab08
DRIVER_13d1ab08 = tulip
ROM_TYPE_13d1ab08 = pci
ROM_DESCRIPTION_13d1ab08 = "Tulip 0x13d1 0xab08"
PCI_VENDOR_13d1ab08 = 0x13d1
PCI_DEVICE_13d1ab08 = 0xab08
ROMS += 13d1ab08
ROMS_tulip += 13d1ab08

# NIC	lanfinity	14f1,1803	Conexant LANfinity
DRIVER_lanfinity = tulip
ROM_TYPE_lanfinity = pci
ROM_DESCRIPTION_lanfinity = "Conexant LANfinity"
PCI_VENDOR_lanfinity = 0x14f1
PCI_DEVICE_lanfinity = 0x1803
ROMS += lanfinity
ROMS_tulip += lanfinity

# NIC	14f11803	14f1,1803	Conexant LANfinity
DRIVER_14f11803 = tulip
ROM_TYPE_14f11803 = pci
ROM_DESCRIPTION_14f11803 = "Conexant LANfinity"
PCI_VENDOR_14f11803 = 0x14f1
PCI_DEVICE_14f11803 = 0x1803
ROMS += 14f11803
ROMS_tulip += 14f11803

# NIC	tulip-8410	1626,8410	Tulip 0x1626 0x8410
DRIVER_tulip-8410 = tulip
ROM_TYPE_tulip-8410 = pci
ROM_DESCRIPTION_tulip-8410 = "Tulip 0x1626 0x8410"
PCI_VENDOR_tulip-8410 = 0x1626
PCI_DEVICE_tulip-8410 = 0x8410
ROMS += tulip-8410
ROMS_tulip += tulip-8410

# NIC	16268410	1626,8410	Tulip 0x1626 0x8410
DRIVER_16268410 = tulip
ROM_TYPE_16268410 = pci
ROM_DESCRIPTION_16268410 = "Tulip 0x1626 0x8410"
PCI_VENDOR_16268410 = 0x1626
PCI_DEVICE_16268410 = 0x8410
ROMS += 16268410
ROMS_tulip += 16268410

# NIC	tulip-1737-ab08	1737,ab08	Tulip 0x1737 0xab08
DRIVER_tulip-1737-ab08 = tulip
ROM_TYPE_tulip-1737-ab08 = pci
ROM_DESCRIPTION_tulip-1737-ab08 = "Tulip 0x1737 0xab08"
PCI_VENDOR_tulip-1737-ab08 = 0x1737
PCI_DEVICE_tulip-1737-ab08 = 0xab08
ROMS += tulip-1737-ab08
ROMS_tulip += tulip-1737-ab08

# NIC	1737ab08	1737,ab08	Tulip 0x1737 0xab08
DRIVER_1737ab08 = tulip
ROM_TYPE_1737ab08 = pci
ROM_DESCRIPTION_1737ab08 = "Tulip 0x1737 0xab08"
PCI_VENDOR_1737ab08 = 0x1737
PCI_DEVICE_1737ab08 = 0xab08
ROMS += 1737ab08
ROMS_tulip += 1737ab08

# NIC	tulip-ab09	1737,ab09	Tulip 0x1737 0xab09
DRIVER_tulip-ab09 = tulip
ROM_TYPE_tulip-ab09 = pci
ROM_DESCRIPTION_tulip-ab09 = "Tulip 0x1737 0xab09"
PCI_VENDOR_tulip-ab09 = 0x1737
PCI_DEVICE_tulip-ab09 = 0xab09
ROMS += tulip-ab09
ROMS_tulip += tulip-ab09

# NIC	1737ab09	1737,ab09	Tulip 0x1737 0xab09
DRIVER_1737ab09 = tulip
ROM_TYPE_1737ab09 = pci
ROM_DESCRIPTION_1737ab09 = "Tulip 0x1737 0xab09"
PCI_VENDOR_1737ab09 = 0x1737
PCI_DEVICE_1737ab09 = 0xab09
ROMS += 1737ab09
ROMS_tulip += 1737ab09
