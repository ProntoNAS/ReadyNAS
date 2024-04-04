w89c840_DEPS = drivers/net/w89c840.c include/compiler.h \
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

$(BIN)/w89c840.o : drivers/net/w89c840.c $(MAKEDEPS) $(POST_O_DEPS) $(w89c840_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/w89c840.o
 
$(BIN)/w89c840.dbg%.o : drivers/net/w89c840.c $(MAKEDEPS) $(POST_O_DEPS) $(w89c840_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/w89c840.dbg%.o
 
$(BIN)/w89c840.c : drivers/net/w89c840.c $(MAKEDEPS) $(POST_O_DEPS) $(w89c840_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/w89c840.c
 
$(BIN)/w89c840.s : drivers/net/w89c840.c $(MAKEDEPS) $(POST_O_DEPS) $(w89c840_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/w89c840.s
 
bin/deps/drivers/net/w89c840.c.d : $(w89c840_DEPS)
 
TAGS : $(w89c840_DEPS)


# NIC	
# NIC	family	drivers/net/w89c840
DRIVERS += w89c840

# NIC	winbond840	1050,0840	Winbond W89C840F
DRIVER_winbond840 = w89c840
ROM_TYPE_winbond840 = pci
ROM_DESCRIPTION_winbond840 = "Winbond W89C840F"
PCI_VENDOR_winbond840 = 0x1050
PCI_DEVICE_winbond840 = 0x0840
ROMS += winbond840
ROMS_w89c840 += winbond840

# NIC	10500840	1050,0840	Winbond W89C840F
DRIVER_10500840 = w89c840
ROM_TYPE_10500840 = pci
ROM_DESCRIPTION_10500840 = "Winbond W89C840F"
PCI_VENDOR_10500840 = 0x1050
PCI_DEVICE_10500840 = 0x0840
ROMS += 10500840
ROMS_w89c840 += 10500840

# NIC	compexrl100atx	11f6,2011	Compex RL100ATX
DRIVER_compexrl100atx = w89c840
ROM_TYPE_compexrl100atx = pci
ROM_DESCRIPTION_compexrl100atx = "Compex RL100ATX"
PCI_VENDOR_compexrl100atx = 0x11f6
PCI_DEVICE_compexrl100atx = 0x2011
ROMS += compexrl100atx
ROMS_w89c840 += compexrl100atx

# NIC	11f62011	11f6,2011	Compex RL100ATX
DRIVER_11f62011 = w89c840
ROM_TYPE_11f62011 = pci
ROM_DESCRIPTION_11f62011 = "Compex RL100ATX"
PCI_VENDOR_11f62011 = 0x11f6
PCI_DEVICE_11f62011 = 0x2011
ROMS += 11f62011
ROMS_w89c840 += 11f62011
