mtnic_DEPS = drivers/net/mtnic.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/strings.h \
 arch/i386/include/limits.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/malloc.h include/stdlib.h \
 include/assert.h include/gpxe/umalloc.h include/gpxe/api.h \
 config/umalloc.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h config/ioapi.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_umalloc.h arch/i386/include/bits/umalloc.h \
 arch/i386/include/gpxe/memtop_umalloc.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/unistd.h include/stdarg.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/gpxe/io.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/pci_io.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/pci_ids.h include/gpxe/ethernet.h include/gpxe/netdevice.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/iobuf.h \
 drivers/net/mtnic.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/strings.h:

arch/i386/include/limits.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/malloc.h:

include/stdlib.h:

include/assert.h:

include/gpxe/umalloc.h:

include/gpxe/api.h:

config/umalloc.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_umalloc.h:

arch/i386/include/bits/umalloc.h:

arch/i386/include/gpxe/memtop_umalloc.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/unistd.h:

include/stdarg.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/io.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

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

include/gpxe/ethernet.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/iobuf.h:

drivers/net/mtnic.h:

$(BIN)/mtnic.o : drivers/net/mtnic.c $(MAKEDEPS) $(POST_O_DEPS) $(mtnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/mtnic.o
 
$(BIN)/mtnic.dbg%.o : drivers/net/mtnic.c $(MAKEDEPS) $(POST_O_DEPS) $(mtnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/mtnic.dbg%.o
 
$(BIN)/mtnic.c : drivers/net/mtnic.c $(MAKEDEPS) $(POST_O_DEPS) $(mtnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/mtnic.c
 
$(BIN)/mtnic.s : drivers/net/mtnic.c $(MAKEDEPS) $(POST_O_DEPS) $(mtnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/mtnic.s
 
bin/deps/drivers/net/mtnic.c.d : $(mtnic_DEPS)
 
TAGS : $(mtnic_DEPS)


# NIC	
# NIC	family	drivers/net/mtnic
DRIVERS += mtnic

# NIC	mt25448	15b3,6368	Mellanox ConnectX EN driver
DRIVER_mt25448 = mtnic
ROM_TYPE_mt25448 = pci
ROM_DESCRIPTION_mt25448 = "Mellanox ConnectX EN driver"
PCI_VENDOR_mt25448 = 0x15b3
PCI_DEVICE_mt25448 = 0x6368
ROMS += mt25448
ROMS_mtnic += mt25448

# NIC	15b36368	15b3,6368	Mellanox ConnectX EN driver
DRIVER_15b36368 = mtnic
ROM_TYPE_15b36368 = pci
ROM_DESCRIPTION_15b36368 = "Mellanox ConnectX EN driver"
PCI_VENDOR_15b36368 = 0x15b3
PCI_DEVICE_15b36368 = 0x6368
ROMS += 15b36368
ROMS_mtnic += 15b36368

# NIC	mt25458	15b3,6372	Mellanox ConnectX ENt driver
DRIVER_mt25458 = mtnic
ROM_TYPE_mt25458 = pci
ROM_DESCRIPTION_mt25458 = "Mellanox ConnectX ENt driver"
PCI_VENDOR_mt25458 = 0x15b3
PCI_DEVICE_mt25458 = 0x6372
ROMS += mt25458
ROMS_mtnic += mt25458

# NIC	15b36372	15b3,6372	Mellanox ConnectX ENt driver
DRIVER_15b36372 = mtnic
ROM_TYPE_15b36372 = pci
ROM_DESCRIPTION_15b36372 = "Mellanox ConnectX ENt driver"
PCI_VENDOR_15b36372 = 0x15b3
PCI_DEVICE_15b36372 = 0x6372
ROMS += 15b36372
ROMS_mtnic += 15b36372

# NIC	mt26448	15b3,6750	Mellanox ConnectX EN GEN2 driver
DRIVER_mt26448 = mtnic
ROM_TYPE_mt26448 = pci
ROM_DESCRIPTION_mt26448 = "Mellanox ConnectX EN GEN2 driver"
PCI_VENDOR_mt26448 = 0x15b3
PCI_DEVICE_mt26448 = 0x6750
ROMS += mt26448
ROMS_mtnic += mt26448

# NIC	15b36750	15b3,6750	Mellanox ConnectX EN GEN2 driver
DRIVER_15b36750 = mtnic
ROM_TYPE_15b36750 = pci
ROM_DESCRIPTION_15b36750 = "Mellanox ConnectX EN GEN2 driver"
PCI_VENDOR_15b36750 = 0x15b3
PCI_DEVICE_15b36750 = 0x6750
ROMS += 15b36750
ROMS_mtnic += 15b36750

# NIC	mt26458	15b3,675a	Mellanox ConnectX ENt GEN2 driver
DRIVER_mt26458 = mtnic
ROM_TYPE_mt26458 = pci
ROM_DESCRIPTION_mt26458 = "Mellanox ConnectX ENt GEN2 driver"
PCI_VENDOR_mt26458 = 0x15b3
PCI_DEVICE_mt26458 = 0x675a
ROMS += mt26458
ROMS_mtnic += mt26458

# NIC	15b3675a	15b3,675a	Mellanox ConnectX ENt GEN2 driver
DRIVER_15b3675a = mtnic
ROM_TYPE_15b3675a = pci
ROM_DESCRIPTION_15b3675a = "Mellanox ConnectX ENt GEN2 driver"
PCI_VENDOR_15b3675a = 0x15b3
PCI_DEVICE_15b3675a = 0x675a
ROMS += 15b3675a
ROMS_mtnic += 15b3675a
