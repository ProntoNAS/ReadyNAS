prism2_pci_DEPS = drivers/net/prism2_pci.c include/compiler.h \
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

$(BIN)/prism2_pci.o : drivers/net/prism2_pci.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_pci_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/prism2_pci.o
 
$(BIN)/prism2_pci.dbg%.o : drivers/net/prism2_pci.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_pci_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/prism2_pci.dbg%.o
 
$(BIN)/prism2_pci.c : drivers/net/prism2_pci.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_pci_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/prism2_pci.c
 
$(BIN)/prism2_pci.s : drivers/net/prism2_pci.c $(MAKEDEPS) $(POST_O_DEPS) $(prism2_pci_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/prism2_pci.s
 
bin/deps/drivers/net/prism2_pci.c.d : $(prism2_pci_DEPS)
 
TAGS : $(prism2_pci_DEPS)


# NIC	
# NIC	family	drivers/net/prism2_pci
DRIVERS += prism2_pci

# NIC	prism2_pci	1260,3873	Harris Semiconductor Prism2.5 clone
DRIVER_prism2_pci = prism2_pci
ROM_TYPE_prism2_pci = pci
ROM_DESCRIPTION_prism2_pci = "Harris Semiconductor Prism2.5 clone"
PCI_VENDOR_prism2_pci = 0x1260
PCI_DEVICE_prism2_pci = 0x3873
ROMS += prism2_pci
ROMS_prism2_pci += prism2_pci

# NIC	12603873	1260,3873	Harris Semiconductor Prism2.5 clone
DRIVER_12603873 = prism2_pci
ROM_TYPE_12603873 = pci
ROM_DESCRIPTION_12603873 = "Harris Semiconductor Prism2.5 clone"
PCI_VENDOR_12603873 = 0x1260
PCI_DEVICE_12603873 = 0x3873
ROMS += 12603873
ROMS_prism2_pci += 12603873

# NIC	hwp01170	1260,3873	ActionTec HWP01170
DRIVER_hwp01170 = prism2_pci
ROM_TYPE_hwp01170 = pci
ROM_DESCRIPTION_hwp01170 = "ActionTec HWP01170"
PCI_VENDOR_hwp01170 = 0x1260
PCI_DEVICE_hwp01170 = 0x3873
ROMS += hwp01170
ROMS_prism2_pci += hwp01170

# NIC	12603873	1260,3873	ActionTec HWP01170
DRIVER_12603873 = prism2_pci
ROM_TYPE_12603873 = pci
ROM_DESCRIPTION_12603873 = "ActionTec HWP01170"
PCI_VENDOR_12603873 = 0x1260
PCI_DEVICE_12603873 = 0x3873
ROMS += 12603873
ROMS_prism2_pci += 12603873

# NIC	dwl520	1260,3873	DLink DWL-520
DRIVER_dwl520 = prism2_pci
ROM_TYPE_dwl520 = pci
ROM_DESCRIPTION_dwl520 = "DLink DWL-520"
PCI_VENDOR_dwl520 = 0x1260
PCI_DEVICE_dwl520 = 0x3873
ROMS += dwl520
ROMS_prism2_pci += dwl520

# NIC	12603873	1260,3873	DLink DWL-520
DRIVER_12603873 = prism2_pci
ROM_TYPE_12603873 = pci
ROM_DESCRIPTION_12603873 = "DLink DWL-520"
PCI_VENDOR_12603873 = 0x1260
PCI_DEVICE_12603873 = 0x3873
ROMS += 12603873
ROMS_prism2_pci += 12603873
