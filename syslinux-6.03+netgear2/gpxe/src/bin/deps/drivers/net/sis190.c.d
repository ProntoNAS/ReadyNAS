sis190_DEPS = drivers/net/sis190.c include/compiler.h \
 arch/i386/include/bits/compiler.h drivers/net/sis190.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/stdlib.h include/assert.h include/stddef.h include/string.h \
 arch/x86/include/bits/string.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h include/mii.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/ethernet.h \
 include/gpxe/if_ether.h include/gpxe/io.h config/ioapi.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/iobuf.h \
 include/gpxe/malloc.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

drivers/net/sis190.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/stddef.h:

include/string.h:

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

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

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

include/gpxe/malloc.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

$(BIN)/sis190.o : drivers/net/sis190.c $(MAKEDEPS) $(POST_O_DEPS) $(sis190_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sis190.o
 
$(BIN)/sis190.dbg%.o : drivers/net/sis190.c $(MAKEDEPS) $(POST_O_DEPS) $(sis190_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sis190.dbg%.o
 
$(BIN)/sis190.c : drivers/net/sis190.c $(MAKEDEPS) $(POST_O_DEPS) $(sis190_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sis190.c
 
$(BIN)/sis190.s : drivers/net/sis190.c $(MAKEDEPS) $(POST_O_DEPS) $(sis190_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sis190.s
 
bin/deps/drivers/net/sis190.c.d : $(sis190_DEPS)
 
TAGS : $(sis190_DEPS)


# NIC	
# NIC	family	drivers/net/sis190
DRIVERS += sis190

# NIC	sis190	1039,0190	sis190
DRIVER_sis190 = sis190
ROM_TYPE_sis190 = pci
ROM_DESCRIPTION_sis190 = "sis190"
PCI_VENDOR_sis190 = 0x1039
PCI_DEVICE_sis190 = 0x0190
ROMS += sis190
ROMS_sis190 += sis190

# NIC	10390190	1039,0190	sis190
DRIVER_10390190 = sis190
ROM_TYPE_10390190 = pci
ROM_DESCRIPTION_10390190 = "sis190"
PCI_VENDOR_10390190 = 0x1039
PCI_DEVICE_10390190 = 0x0190
ROMS += 10390190
ROMS_sis190 += 10390190

# NIC	sis191	1039,0191	sis191
DRIVER_sis191 = sis190
ROM_TYPE_sis191 = pci
ROM_DESCRIPTION_sis191 = "sis191"
PCI_VENDOR_sis191 = 0x1039
PCI_DEVICE_sis191 = 0x0191
ROMS += sis191
ROMS_sis190 += sis191

# NIC	10390191	1039,0191	sis191
DRIVER_10390191 = sis190
ROM_TYPE_10390191 = pci
ROM_DESCRIPTION_10390191 = "sis191"
PCI_VENDOR_10390191 = 0x1039
PCI_DEVICE_10390191 = 0x0191
ROMS += 10390191
ROMS_sis190 += 10390191
