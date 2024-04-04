rtl8180_DEPS = drivers/net/rtl818x/rtl8180.c include/compiler.h \
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
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h

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

$(BIN)/rtl8180.o : drivers/net/rtl818x/rtl8180.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rtl8180.o
 
$(BIN)/rtl8180.dbg%.o : drivers/net/rtl818x/rtl8180.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rtl8180.dbg%.o
 
$(BIN)/rtl8180.c : drivers/net/rtl818x/rtl8180.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rtl8180.c
 
$(BIN)/rtl8180.s : drivers/net/rtl818x/rtl8180.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rtl8180.s
 
bin/deps/drivers/net/rtl818x/rtl8180.c.d : $(rtl8180_DEPS)
 
TAGS : $(rtl8180_DEPS)


# NIC	
# NIC	family	drivers/net/rtl818x/rtl8180
DRIVERS += rtl8180

# NIC	rtl8180	10ec,8180	Realtek 8180
DRIVER_rtl8180 = rtl8180
ROM_TYPE_rtl8180 = pci
ROM_DESCRIPTION_rtl8180 = "Realtek 8180"
PCI_VENDOR_rtl8180 = 0x10ec
PCI_DEVICE_rtl8180 = 0x8180
ROMS += rtl8180
ROMS_rtl8180 += rtl8180

# NIC	10ec8180	10ec,8180	Realtek 8180
DRIVER_10ec8180 = rtl8180
ROM_TYPE_10ec8180 = pci
ROM_DESCRIPTION_10ec8180 = "Realtek 8180"
PCI_VENDOR_10ec8180 = 0x10ec
PCI_DEVICE_10ec8180 = 0x8180
ROMS += 10ec8180
ROMS_rtl8180 += 10ec8180

# NIC	f5d6001	1799,6001	Belkin F5D6001
DRIVER_f5d6001 = rtl8180
ROM_TYPE_f5d6001 = pci
ROM_DESCRIPTION_f5d6001 = "Belkin F5D6001"
PCI_VENDOR_f5d6001 = 0x1799
PCI_DEVICE_f5d6001 = 0x6001
ROMS += f5d6001
ROMS_rtl8180 += f5d6001

# NIC	17996001	1799,6001	Belkin F5D6001
DRIVER_17996001 = rtl8180
ROM_TYPE_17996001 = pci
ROM_DESCRIPTION_17996001 = "Belkin F5D6001"
PCI_VENDOR_17996001 = 0x1799
PCI_DEVICE_17996001 = 0x6001
ROMS += 17996001
ROMS_rtl8180 += 17996001

# NIC	f5d6020	1799,6020	Belkin F5D6020
DRIVER_f5d6020 = rtl8180
ROM_TYPE_f5d6020 = pci
ROM_DESCRIPTION_f5d6020 = "Belkin F5D6020"
PCI_VENDOR_f5d6020 = 0x1799
PCI_DEVICE_f5d6020 = 0x6020
ROMS += f5d6020
ROMS_rtl8180 += f5d6020

# NIC	17996020	1799,6020	Belkin F5D6020
DRIVER_17996020 = rtl8180
ROM_TYPE_17996020 = pci
ROM_DESCRIPTION_17996020 = "Belkin F5D6020"
PCI_VENDOR_17996020 = 0x1799
PCI_DEVICE_17996020 = 0x6020
ROMS += 17996020
ROMS_rtl8180 += 17996020

# NIC	dwl510	1186,3300	D-Link DWL-510
DRIVER_dwl510 = rtl8180
ROM_TYPE_dwl510 = pci
ROM_DESCRIPTION_dwl510 = "D-Link DWL-510"
PCI_VENDOR_dwl510 = 0x1186
PCI_DEVICE_dwl510 = 0x3300
ROMS += dwl510
ROMS_rtl8180 += dwl510

# NIC	11863300	1186,3300	D-Link DWL-510
DRIVER_11863300 = rtl8180
ROM_TYPE_11863300 = pci
ROM_DESCRIPTION_11863300 = "D-Link DWL-510"
PCI_VENDOR_11863300 = 0x1186
PCI_DEVICE_11863300 = 0x3300
ROMS += 11863300
ROMS_rtl8180 += 11863300
