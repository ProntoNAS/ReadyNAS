r8169_DEPS = drivers/net/r8169.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/stdlib.h include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/ethernet.h include/gpxe/if_ether.h include/gpxe/io.h \
 config/ioapi.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/malloc.h include/gpxe/netdevice.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h include/mii.h \
 drivers/net/r8169.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/malloc.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/mii.h:

drivers/net/r8169.h:

$(BIN)/r8169.o : drivers/net/r8169.c $(MAKEDEPS) $(POST_O_DEPS) $(r8169_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/r8169.o
 
$(BIN)/r8169.dbg%.o : drivers/net/r8169.c $(MAKEDEPS) $(POST_O_DEPS) $(r8169_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/r8169.dbg%.o
 
$(BIN)/r8169.c : drivers/net/r8169.c $(MAKEDEPS) $(POST_O_DEPS) $(r8169_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/r8169.c
 
$(BIN)/r8169.s : drivers/net/r8169.c $(MAKEDEPS) $(POST_O_DEPS) $(r8169_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/r8169.s
 
bin/deps/drivers/net/r8169.c.d : $(r8169_DEPS)
 
TAGS : $(r8169_DEPS)


# NIC	
# NIC	family	drivers/net/r8169
DRIVERS += r8169

# NIC	rtl8169-0x8129	10ec,8129	rtl8169-0x8129
DRIVER_rtl8169-0x8129 = r8169
ROM_TYPE_rtl8169-0x8129 = pci
ROM_DESCRIPTION_rtl8169-0x8129 = "rtl8169-0x8129"
PCI_VENDOR_rtl8169-0x8129 = 0x10ec
PCI_DEVICE_rtl8169-0x8129 = 0x8129
ROMS += rtl8169-0x8129
ROMS_r8169 += rtl8169-0x8129

# NIC	10ec8129	10ec,8129	rtl8169-0x8129
DRIVER_10ec8129 = r8169
ROM_TYPE_10ec8129 = pci
ROM_DESCRIPTION_10ec8129 = "rtl8169-0x8129"
PCI_VENDOR_10ec8129 = 0x10ec
PCI_DEVICE_10ec8129 = 0x8129
ROMS += 10ec8129
ROMS_r8169 += 10ec8129

# NIC	rtl8169-0x8136	10ec,8136	rtl8169-0x8136
DRIVER_rtl8169-0x8136 = r8169
ROM_TYPE_rtl8169-0x8136 = pci
ROM_DESCRIPTION_rtl8169-0x8136 = "rtl8169-0x8136"
PCI_VENDOR_rtl8169-0x8136 = 0x10ec
PCI_DEVICE_rtl8169-0x8136 = 0x8136
ROMS += rtl8169-0x8136
ROMS_r8169 += rtl8169-0x8136

# NIC	10ec8136	10ec,8136	rtl8169-0x8136
DRIVER_10ec8136 = r8169
ROM_TYPE_10ec8136 = pci
ROM_DESCRIPTION_10ec8136 = "rtl8169-0x8136"
PCI_VENDOR_10ec8136 = 0x10ec
PCI_DEVICE_10ec8136 = 0x8136
ROMS += 10ec8136
ROMS_r8169 += 10ec8136

# NIC	rtl8169-0x8167	10ec,8167	rtl8169-0x8167
DRIVER_rtl8169-0x8167 = r8169
ROM_TYPE_rtl8169-0x8167 = pci
ROM_DESCRIPTION_rtl8169-0x8167 = "rtl8169-0x8167"
PCI_VENDOR_rtl8169-0x8167 = 0x10ec
PCI_DEVICE_rtl8169-0x8167 = 0x8167
ROMS += rtl8169-0x8167
ROMS_r8169 += rtl8169-0x8167

# NIC	10ec8167	10ec,8167	rtl8169-0x8167
DRIVER_10ec8167 = r8169
ROM_TYPE_10ec8167 = pci
ROM_DESCRIPTION_10ec8167 = "rtl8169-0x8167"
PCI_VENDOR_10ec8167 = 0x10ec
PCI_DEVICE_10ec8167 = 0x8167
ROMS += 10ec8167
ROMS_r8169 += 10ec8167

# NIC	rtl8169-0x8168	10ec,8168	rtl8169-0x8168
DRIVER_rtl8169-0x8168 = r8169
ROM_TYPE_rtl8169-0x8168 = pci
ROM_DESCRIPTION_rtl8169-0x8168 = "rtl8169-0x8168"
PCI_VENDOR_rtl8169-0x8168 = 0x10ec
PCI_DEVICE_rtl8169-0x8168 = 0x8168
ROMS += rtl8169-0x8168
ROMS_r8169 += rtl8169-0x8168

# NIC	10ec8168	10ec,8168	rtl8169-0x8168
DRIVER_10ec8168 = r8169
ROM_TYPE_10ec8168 = pci
ROM_DESCRIPTION_10ec8168 = "rtl8169-0x8168"
PCI_VENDOR_10ec8168 = 0x10ec
PCI_DEVICE_10ec8168 = 0x8168
ROMS += 10ec8168
ROMS_r8169 += 10ec8168

# NIC	rtl8169-0x8169	10ec,8169	rtl8169-0x8169
DRIVER_rtl8169-0x8169 = r8169
ROM_TYPE_rtl8169-0x8169 = pci
ROM_DESCRIPTION_rtl8169-0x8169 = "rtl8169-0x8169"
PCI_VENDOR_rtl8169-0x8169 = 0x10ec
PCI_DEVICE_rtl8169-0x8169 = 0x8169
ROMS += rtl8169-0x8169
ROMS_r8169 += rtl8169-0x8169

# NIC	10ec8169	10ec,8169	rtl8169-0x8169
DRIVER_10ec8169 = r8169
ROM_TYPE_10ec8169 = pci
ROM_DESCRIPTION_10ec8169 = "rtl8169-0x8169"
PCI_VENDOR_10ec8169 = 0x10ec
PCI_DEVICE_10ec8169 = 0x8169
ROMS += 10ec8169
ROMS_r8169 += 10ec8169

# NIC	rtl8169-0x4300	1186,4300	rtl8169-0x4300
DRIVER_rtl8169-0x4300 = r8169
ROM_TYPE_rtl8169-0x4300 = pci
ROM_DESCRIPTION_rtl8169-0x4300 = "rtl8169-0x4300"
PCI_VENDOR_rtl8169-0x4300 = 0x1186
PCI_DEVICE_rtl8169-0x4300 = 0x4300
ROMS += rtl8169-0x4300
ROMS_r8169 += rtl8169-0x4300

# NIC	11864300	1186,4300	rtl8169-0x4300
DRIVER_11864300 = r8169
ROM_TYPE_11864300 = pci
ROM_DESCRIPTION_11864300 = "rtl8169-0x4300"
PCI_VENDOR_11864300 = 0x1186
PCI_DEVICE_11864300 = 0x4300
ROMS += 11864300
ROMS_r8169 += 11864300

# NIC	rtl8169-0xc107	1259,c107	rtl8169-0xc107
DRIVER_rtl8169-0xc107 = r8169
ROM_TYPE_rtl8169-0xc107 = pci
ROM_DESCRIPTION_rtl8169-0xc107 = "rtl8169-0xc107"
PCI_VENDOR_rtl8169-0xc107 = 0x1259
PCI_DEVICE_rtl8169-0xc107 = 0xc107
ROMS += rtl8169-0xc107
ROMS_r8169 += rtl8169-0xc107

# NIC	1259c107	1259,c107	rtl8169-0xc107
DRIVER_1259c107 = r8169
ROM_TYPE_1259c107 = pci
ROM_DESCRIPTION_1259c107 = "rtl8169-0xc107"
PCI_VENDOR_1259c107 = 0x1259
PCI_DEVICE_1259c107 = 0xc107
ROMS += 1259c107
ROMS_r8169 += 1259c107

# NIC	rtl8169-0x0116	16ec,0116	rtl8169-0x0116
DRIVER_rtl8169-0x0116 = r8169
ROM_TYPE_rtl8169-0x0116 = pci
ROM_DESCRIPTION_rtl8169-0x0116 = "rtl8169-0x0116"
PCI_VENDOR_rtl8169-0x0116 = 0x16ec
PCI_DEVICE_rtl8169-0x0116 = 0x0116
ROMS += rtl8169-0x0116
ROMS_r8169 += rtl8169-0x0116

# NIC	16ec0116	16ec,0116	rtl8169-0x0116
DRIVER_16ec0116 = r8169
ROM_TYPE_16ec0116 = pci
ROM_DESCRIPTION_16ec0116 = "rtl8169-0x0116"
PCI_VENDOR_16ec0116 = 0x16ec
PCI_DEVICE_16ec0116 = 0x0116
ROMS += 16ec0116
ROMS_r8169 += 16ec0116

# NIC	rtl8169-0x1032	1737,1032	rtl8169-0x1032
DRIVER_rtl8169-0x1032 = r8169
ROM_TYPE_rtl8169-0x1032 = pci
ROM_DESCRIPTION_rtl8169-0x1032 = "rtl8169-0x1032"
PCI_VENDOR_rtl8169-0x1032 = 0x1737
PCI_DEVICE_rtl8169-0x1032 = 0x1032
ROMS += rtl8169-0x1032
ROMS_r8169 += rtl8169-0x1032

# NIC	17371032	1737,1032	rtl8169-0x1032
DRIVER_17371032 = r8169
ROM_TYPE_17371032 = pci
ROM_DESCRIPTION_17371032 = "rtl8169-0x1032"
PCI_VENDOR_17371032 = 0x1737
PCI_DEVICE_17371032 = 0x1032
ROMS += 17371032
ROMS_r8169 += 17371032

# NIC	rtl8169-0x8168	0001,8168	rtl8169-0x8168
DRIVER_rtl8169-0x8168 = r8169
ROM_TYPE_rtl8169-0x8168 = pci
ROM_DESCRIPTION_rtl8169-0x8168 = "rtl8169-0x8168"
PCI_VENDOR_rtl8169-0x8168 = 0x0001
PCI_DEVICE_rtl8169-0x8168 = 0x8168
ROMS += rtl8169-0x8168
ROMS_r8169 += rtl8169-0x8168

# NIC	00018168	0001,8168	rtl8169-0x8168
DRIVER_00018168 = r8169
ROM_TYPE_00018168 = pci
ROM_DESCRIPTION_00018168 = "rtl8169-0x8168"
PCI_VENDOR_00018168 = 0x0001
PCI_DEVICE_00018168 = 0x8168
ROMS += 00018168
ROMS_r8169 += 00018168
