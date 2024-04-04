rtl8139_DEPS = drivers/net/rtl8139.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/unistd.h include/gpxe/timer.h \
 config/timer.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/if_ether.h include/gpxe/ethernet.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/spi_bit.h include/gpxe/spi.h include/gpxe/nvs.h \
 include/gpxe/bitbash.h include/gpxe/threewire.h \
 arch/i386/include/limits.h include/gpxe/nvo.h include/gpxe/dhcpopts.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/unistd.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/spi_bit.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

include/gpxe/bitbash.h:

include/gpxe/threewire.h:

arch/i386/include/limits.h:

include/gpxe/nvo.h:

include/gpxe/dhcpopts.h:

$(BIN)/rtl8139.o : drivers/net/rtl8139.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8139_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rtl8139.o
 
$(BIN)/rtl8139.dbg%.o : drivers/net/rtl8139.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8139_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rtl8139.dbg%.o
 
$(BIN)/rtl8139.c : drivers/net/rtl8139.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8139_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rtl8139.c
 
$(BIN)/rtl8139.s : drivers/net/rtl8139.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8139_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rtl8139.s
 
bin/deps/drivers/net/rtl8139.c.d : $(rtl8139_DEPS)
 
TAGS : $(rtl8139_DEPS)


# NIC	
# NIC	family	drivers/net/rtl8139
DRIVERS += rtl8139

# NIC	rtl8129	10ec,8129	Realtek 8129
DRIVER_rtl8129 = rtl8139
ROM_TYPE_rtl8129 = pci
ROM_DESCRIPTION_rtl8129 = "Realtek 8129"
PCI_VENDOR_rtl8129 = 0x10ec
PCI_DEVICE_rtl8129 = 0x8129
ROMS += rtl8129
ROMS_rtl8139 += rtl8129

# NIC	10ec8129	10ec,8129	Realtek 8129
DRIVER_10ec8129 = rtl8139
ROM_TYPE_10ec8129 = pci
ROM_DESCRIPTION_10ec8129 = "Realtek 8129"
PCI_VENDOR_10ec8129 = 0x10ec
PCI_DEVICE_10ec8129 = 0x8129
ROMS += 10ec8129
ROMS_rtl8139 += 10ec8129

# NIC	rtl8139	10ec,8139	Realtek 8139
DRIVER_rtl8139 = rtl8139
ROM_TYPE_rtl8139 = pci
ROM_DESCRIPTION_rtl8139 = "Realtek 8139"
PCI_VENDOR_rtl8139 = 0x10ec
PCI_DEVICE_rtl8139 = 0x8139
ROMS += rtl8139
ROMS_rtl8139 += rtl8139

# NIC	10ec8139	10ec,8139	Realtek 8139
DRIVER_10ec8139 = rtl8139
ROM_TYPE_10ec8139 = pci
ROM_DESCRIPTION_10ec8139 = "Realtek 8139"
PCI_VENDOR_10ec8139 = 0x10ec
PCI_DEVICE_10ec8139 = 0x8139
ROMS += 10ec8139
ROMS_rtl8139 += 10ec8139

# NIC	rtl8139b	10ec,8138	Realtek 8139B
DRIVER_rtl8139b = rtl8139
ROM_TYPE_rtl8139b = pci
ROM_DESCRIPTION_rtl8139b = "Realtek 8139B"
PCI_VENDOR_rtl8139b = 0x10ec
PCI_DEVICE_rtl8139b = 0x8138
ROMS += rtl8139b
ROMS_rtl8139 += rtl8139b

# NIC	10ec8138	10ec,8138	Realtek 8139B
DRIVER_10ec8138 = rtl8139
ROM_TYPE_10ec8138 = pci
ROM_DESCRIPTION_10ec8138 = "Realtek 8139B"
PCI_VENDOR_10ec8138 = 0x10ec
PCI_DEVICE_10ec8138 = 0x8138
ROMS += 10ec8138
ROMS_rtl8139 += 10ec8138

# NIC	dfe538	1186,1300	DFE530TX+/DFE538TX
DRIVER_dfe538 = rtl8139
ROM_TYPE_dfe538 = pci
ROM_DESCRIPTION_dfe538 = "DFE530TX+/DFE538TX"
PCI_VENDOR_dfe538 = 0x1186
PCI_DEVICE_dfe538 = 0x1300
ROMS += dfe538
ROMS_rtl8139 += dfe538

# NIC	11861300	1186,1300	DFE530TX+/DFE538TX
DRIVER_11861300 = rtl8139
ROM_TYPE_11861300 = pci
ROM_DESCRIPTION_11861300 = "DFE530TX+/DFE538TX"
PCI_VENDOR_11861300 = 0x1186
PCI_DEVICE_11861300 = 0x1300
ROMS += 11861300
ROMS_rtl8139 += 11861300

# NIC	smc1211-1	1113,1211	SMC EZ10/100
DRIVER_smc1211-1 = rtl8139
ROM_TYPE_smc1211-1 = pci
ROM_DESCRIPTION_smc1211-1 = "SMC EZ10/100"
PCI_VENDOR_smc1211-1 = 0x1113
PCI_DEVICE_smc1211-1 = 0x1211
ROMS += smc1211-1
ROMS_rtl8139 += smc1211-1

# NIC	11131211	1113,1211	SMC EZ10/100
DRIVER_11131211 = rtl8139
ROM_TYPE_11131211 = pci
ROM_DESCRIPTION_11131211 = "SMC EZ10/100"
PCI_VENDOR_11131211 = 0x1113
PCI_DEVICE_11131211 = 0x1211
ROMS += 11131211
ROMS_rtl8139 += 11131211

# NIC	smc1211	1112,1211	SMC EZ10/100
DRIVER_smc1211 = rtl8139
ROM_TYPE_smc1211 = pci
ROM_DESCRIPTION_smc1211 = "SMC EZ10/100"
PCI_VENDOR_smc1211 = 0x1112
PCI_DEVICE_smc1211 = 0x1211
ROMS += smc1211
ROMS_rtl8139 += smc1211

# NIC	11121211	1112,1211	SMC EZ10/100
DRIVER_11121211 = rtl8139
ROM_TYPE_11121211 = pci
ROM_DESCRIPTION_11121211 = "SMC EZ10/100"
PCI_VENDOR_11121211 = 0x1112
PCI_DEVICE_11121211 = 0x1211
ROMS += 11121211
ROMS_rtl8139 += 11121211

# NIC	delta8139	1500,1360	Delta Electronics 8139
DRIVER_delta8139 = rtl8139
ROM_TYPE_delta8139 = pci
ROM_DESCRIPTION_delta8139 = "Delta Electronics 8139"
PCI_VENDOR_delta8139 = 0x1500
PCI_DEVICE_delta8139 = 0x1360
ROMS += delta8139
ROMS_rtl8139 += delta8139

# NIC	15001360	1500,1360	Delta Electronics 8139
DRIVER_15001360 = rtl8139
ROM_TYPE_15001360 = pci
ROM_DESCRIPTION_15001360 = "Delta Electronics 8139"
PCI_VENDOR_15001360 = 0x1500
PCI_DEVICE_15001360 = 0x1360
ROMS += 15001360
ROMS_rtl8139 += 15001360

# NIC	addtron8139	4033,1360	Addtron Technology 8139
DRIVER_addtron8139 = rtl8139
ROM_TYPE_addtron8139 = pci
ROM_DESCRIPTION_addtron8139 = "Addtron Technology 8139"
PCI_VENDOR_addtron8139 = 0x4033
PCI_DEVICE_addtron8139 = 0x1360
ROMS += addtron8139
ROMS_rtl8139 += addtron8139

# NIC	40331360	4033,1360	Addtron Technology 8139
DRIVER_40331360 = rtl8139
ROM_TYPE_40331360 = pci
ROM_DESCRIPTION_40331360 = "Addtron Technology 8139"
PCI_VENDOR_40331360 = 0x4033
PCI_DEVICE_40331360 = 0x1360
ROMS += 40331360
ROMS_rtl8139 += 40331360

# NIC	dfe690txd	1186,1340	D-Link DFE690TXD
DRIVER_dfe690txd = rtl8139
ROM_TYPE_dfe690txd = pci
ROM_DESCRIPTION_dfe690txd = "D-Link DFE690TXD"
PCI_VENDOR_dfe690txd = 0x1186
PCI_DEVICE_dfe690txd = 0x1340
ROMS += dfe690txd
ROMS_rtl8139 += dfe690txd

# NIC	11861340	1186,1340	D-Link DFE690TXD
DRIVER_11861340 = rtl8139
ROM_TYPE_11861340 = pci
ROM_DESCRIPTION_11861340 = "D-Link DFE690TXD"
PCI_VENDOR_11861340 = 0x1186
PCI_DEVICE_11861340 = 0x1340
ROMS += 11861340
ROMS_rtl8139 += 11861340

# NIC	fe2000vx	13d1,ab06	AboCom FE2000VX
DRIVER_fe2000vx = rtl8139
ROM_TYPE_fe2000vx = pci
ROM_DESCRIPTION_fe2000vx = "AboCom FE2000VX"
PCI_VENDOR_fe2000vx = 0x13d1
PCI_DEVICE_fe2000vx = 0xab06
ROMS += fe2000vx
ROMS_rtl8139 += fe2000vx

# NIC	13d1ab06	13d1,ab06	AboCom FE2000VX
DRIVER_13d1ab06 = rtl8139
ROM_TYPE_13d1ab06 = pci
ROM_DESCRIPTION_13d1ab06 = "AboCom FE2000VX"
PCI_VENDOR_13d1ab06 = 0x13d1
PCI_DEVICE_13d1ab06 = 0xab06
ROMS += 13d1ab06
ROMS_rtl8139 += 13d1ab06

# NIC	allied8139	1259,a117	Allied Telesyn 8139
DRIVER_allied8139 = rtl8139
ROM_TYPE_allied8139 = pci
ROM_DESCRIPTION_allied8139 = "Allied Telesyn 8139"
PCI_VENDOR_allied8139 = 0x1259
PCI_DEVICE_allied8139 = 0xa117
ROMS += allied8139
ROMS_rtl8139 += allied8139

# NIC	1259a117	1259,a117	Allied Telesyn 8139
DRIVER_1259a117 = rtl8139
ROM_TYPE_1259a117 = pci
ROM_DESCRIPTION_1259a117 = "Allied Telesyn 8139"
PCI_VENDOR_1259a117 = 0x1259
PCI_DEVICE_1259a117 = 0xa117
ROMS += 1259a117
ROMS_rtl8139 += 1259a117

# NIC	fnw3603tx	14ea,ab06	Planex FNW-3603-TX
DRIVER_fnw3603tx = rtl8139
ROM_TYPE_fnw3603tx = pci
ROM_DESCRIPTION_fnw3603tx = "Planex FNW-3603-TX"
PCI_VENDOR_fnw3603tx = 0x14ea
PCI_DEVICE_fnw3603tx = 0xab06
ROMS += fnw3603tx
ROMS_rtl8139 += fnw3603tx

# NIC	14eaab06	14ea,ab06	Planex FNW-3603-TX
DRIVER_14eaab06 = rtl8139
ROM_TYPE_14eaab06 = pci
ROM_DESCRIPTION_14eaab06 = "Planex FNW-3603-TX"
PCI_VENDOR_14eaab06 = 0x14ea
PCI_DEVICE_14eaab06 = 0xab06
ROMS += 14eaab06
ROMS_rtl8139 += 14eaab06

# NIC	fnw3800tx	14ea,ab07	Planex FNW-3800-TX
DRIVER_fnw3800tx = rtl8139
ROM_TYPE_fnw3800tx = pci
ROM_DESCRIPTION_fnw3800tx = "Planex FNW-3800-TX"
PCI_VENDOR_fnw3800tx = 0x14ea
PCI_DEVICE_fnw3800tx = 0xab07
ROMS += fnw3800tx
ROMS_rtl8139 += fnw3800tx

# NIC	14eaab07	14ea,ab07	Planex FNW-3800-TX
DRIVER_14eaab07 = rtl8139
ROM_TYPE_14eaab07 = pci
ROM_DESCRIPTION_14eaab07 = "Planex FNW-3800-TX"
PCI_VENDOR_14eaab07 = 0x14ea
PCI_DEVICE_14eaab07 = 0xab07
ROMS += 14eaab07
ROMS_rtl8139 += 14eaab07

# NIC	clone-rtl8139	ffff,8139	Cloned 8139
DRIVER_clone-rtl8139 = rtl8139
ROM_TYPE_clone-rtl8139 = pci
ROM_DESCRIPTION_clone-rtl8139 = "Cloned 8139"
PCI_VENDOR_clone-rtl8139 = 0xffff
PCI_DEVICE_clone-rtl8139 = 0x8139
ROMS += clone-rtl8139
ROMS_rtl8139 += clone-rtl8139

# NIC	ffff8139	ffff,8139	Cloned 8139
DRIVER_ffff8139 = rtl8139
ROM_TYPE_ffff8139 = pci
ROM_DESCRIPTION_ffff8139 = "Cloned 8139"
PCI_VENDOR_ffff8139 = 0xffff
PCI_DEVICE_ffff8139 = 0x8139
ROMS += ffff8139
ROMS_rtl8139 += ffff8139
