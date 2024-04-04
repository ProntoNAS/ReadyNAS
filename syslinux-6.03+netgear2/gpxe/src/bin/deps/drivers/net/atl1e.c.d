atl1e_DEPS = drivers/net/atl1e.c include/compiler.h \
 arch/i386/include/bits/compiler.h drivers/net/atl1e.h include/mii.h \
 include/gpxe/netdevice.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/stdlib.h include/string.h \
 arch/x86/include/bits/string.h include/unistd.h include/stdarg.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/malloc.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/pci_io.h config/ioapi.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/iobuf.h include/gpxe/ethernet.h include/gpxe/if_ether.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

drivers/net/atl1e.h:

include/mii.h:

include/gpxe/netdevice.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/stdlib.h:

include/string.h:

arch/x86/include/bits/string.h:

include/unistd.h:

include/stdarg.h:

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

include/gpxe/malloc.h:

include/gpxe/pci.h:

include/gpxe/device.h:

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

include/gpxe/iobuf.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

$(BIN)/atl1e.o : drivers/net/atl1e.c $(MAKEDEPS) $(POST_O_DEPS) $(atl1e_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/atl1e.o
 
$(BIN)/atl1e.dbg%.o : drivers/net/atl1e.c $(MAKEDEPS) $(POST_O_DEPS) $(atl1e_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/atl1e.dbg%.o
 
$(BIN)/atl1e.c : drivers/net/atl1e.c $(MAKEDEPS) $(POST_O_DEPS) $(atl1e_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/atl1e.c
 
$(BIN)/atl1e.s : drivers/net/atl1e.c $(MAKEDEPS) $(POST_O_DEPS) $(atl1e_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/atl1e.s
 
bin/deps/drivers/net/atl1e.c.d : $(atl1e_DEPS)
 
TAGS : $(atl1e_DEPS)


# NIC	
# NIC	family	drivers/net/atl1e
DRIVERS += atl1e

# NIC	atl1e_26	1969,1026	Attansic L1E 0x1026
DRIVER_atl1e_26 = atl1e
ROM_TYPE_atl1e_26 = pci
ROM_DESCRIPTION_atl1e_26 = "Attansic L1E 0x1026"
PCI_VENDOR_atl1e_26 = 0x1969
PCI_DEVICE_atl1e_26 = 0x1026
ROMS += atl1e_26
ROMS_atl1e += atl1e_26

# NIC	19691026	1969,1026	Attansic L1E 0x1026
DRIVER_19691026 = atl1e
ROM_TYPE_19691026 = pci
ROM_DESCRIPTION_19691026 = "Attansic L1E 0x1026"
PCI_VENDOR_19691026 = 0x1969
PCI_DEVICE_19691026 = 0x1026
ROMS += 19691026
ROMS_atl1e += 19691026

# NIC	atl1e_66	1969,1066	Attansic L1E 0x1066
DRIVER_atl1e_66 = atl1e
ROM_TYPE_atl1e_66 = pci
ROM_DESCRIPTION_atl1e_66 = "Attansic L1E 0x1066"
PCI_VENDOR_atl1e_66 = 0x1969
PCI_DEVICE_atl1e_66 = 0x1066
ROMS += atl1e_66
ROMS_atl1e += atl1e_66

# NIC	19691066	1969,1066	Attansic L1E 0x1066
DRIVER_19691066 = atl1e
ROM_TYPE_19691066 = pci
ROM_DESCRIPTION_19691066 = "Attansic L1E 0x1066"
PCI_VENDOR_19691066 = 0x1969
PCI_DEVICE_19691066 = 0x1066
ROMS += 19691066
ROMS_atl1e += 19691066
