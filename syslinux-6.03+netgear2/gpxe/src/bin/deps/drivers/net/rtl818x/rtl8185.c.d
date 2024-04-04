rtl8185_DEPS = drivers/net/rtl818x/rtl8185.c include/compiler.h \
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

$(BIN)/rtl8185.o : drivers/net/rtl818x/rtl8185.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8185_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rtl8185.o
 
$(BIN)/rtl8185.dbg%.o : drivers/net/rtl818x/rtl8185.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8185_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rtl8185.dbg%.o
 
$(BIN)/rtl8185.c : drivers/net/rtl818x/rtl8185.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8185_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rtl8185.c
 
$(BIN)/rtl8185.s : drivers/net/rtl818x/rtl8185.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8185_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rtl8185.s
 
bin/deps/drivers/net/rtl818x/rtl8185.c.d : $(rtl8185_DEPS)
 
TAGS : $(rtl8185_DEPS)


# NIC	
# NIC	family	drivers/net/rtl818x/rtl8185
DRIVERS += rtl8185

# NIC	rtl8185	10ec,8185	Realtek 8185
DRIVER_rtl8185 = rtl8185
ROM_TYPE_rtl8185 = pci
ROM_DESCRIPTION_rtl8185 = "Realtek 8185"
PCI_VENDOR_rtl8185 = 0x10ec
PCI_DEVICE_rtl8185 = 0x8185
ROMS += rtl8185
ROMS_rtl8185 += rtl8185

# NIC	10ec8185	10ec,8185	Realtek 8185
DRIVER_10ec8185 = rtl8185
ROM_TYPE_10ec8185 = pci
ROM_DESCRIPTION_10ec8185 = "Realtek 8185"
PCI_VENDOR_10ec8185 = 0x10ec
PCI_DEVICE_10ec8185 = 0x8185
ROMS += 10ec8185
ROMS_rtl8185 += 10ec8185

# NIC	f5d7000	1799,700f	Belkin F5D7000
DRIVER_f5d7000 = rtl8185
ROM_TYPE_f5d7000 = pci
ROM_DESCRIPTION_f5d7000 = "Belkin F5D7000"
PCI_VENDOR_f5d7000 = 0x1799
PCI_DEVICE_f5d7000 = 0x700f
ROMS += f5d7000
ROMS_rtl8185 += f5d7000

# NIC	1799700f	1799,700f	Belkin F5D7000
DRIVER_1799700f = rtl8185
ROM_TYPE_1799700f = pci
ROM_DESCRIPTION_1799700f = "Belkin F5D7000"
PCI_VENDOR_1799700f = 0x1799
PCI_DEVICE_1799700f = 0x700f
ROMS += 1799700f
ROMS_rtl8185 += 1799700f

# NIC	f5d7010	1799,701f	Belkin F5D7010
DRIVER_f5d7010 = rtl8185
ROM_TYPE_f5d7010 = pci
ROM_DESCRIPTION_f5d7010 = "Belkin F5D7010"
PCI_VENDOR_f5d7010 = 0x1799
PCI_DEVICE_f5d7010 = 0x701f
ROMS += f5d7010
ROMS_rtl8185 += f5d7010

# NIC	1799701f	1799,701f	Belkin F5D7010
DRIVER_1799701f = rtl8185
ROM_TYPE_1799701f = pci
ROM_DESCRIPTION_1799701f = "Belkin F5D7010"
PCI_VENDOR_1799701f = 0x1799
PCI_DEVICE_1799701f = 0x701f
ROMS += 1799701f
ROMS_rtl8185 += 1799701f
