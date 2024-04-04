hermon_DEPS = drivers/infiniband/hermon.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/strings.h \
 arch/i386/include/limits.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/io.h config/ioapi.h include/gpxe/uaccess.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/pcibackup.h include/gpxe/malloc.h include/gpxe/umalloc.h \
 config/umalloc.h include/gpxe/efi/efi_umalloc.h \
 arch/i386/include/bits/umalloc.h arch/i386/include/gpxe/memtop_umalloc.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/infiniband.h \
 include/gpxe/ib_packet.h include/gpxe/ib_mad.h include/gpxe/ib_smc.h \
 drivers/infiniband/hermon.h drivers/infiniband/mlx_bitops.h \
 drivers/infiniband/MT25408_PRM.h

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

include/strings.h:

arch/i386/include/limits.h:

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

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

include/gpxe/pcibackup.h:

include/gpxe/malloc.h:

include/gpxe/umalloc.h:

config/umalloc.h:

include/gpxe/efi/efi_umalloc.h:

arch/i386/include/bits/umalloc.h:

arch/i386/include/gpxe/memtop_umalloc.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/infiniband.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_smc.h:

drivers/infiniband/hermon.h:

drivers/infiniband/mlx_bitops.h:

drivers/infiniband/MT25408_PRM.h:

$(BIN)/hermon.o : drivers/infiniband/hermon.c $(MAKEDEPS) $(POST_O_DEPS) $(hermon_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/hermon.o
 
$(BIN)/hermon.dbg%.o : drivers/infiniband/hermon.c $(MAKEDEPS) $(POST_O_DEPS) $(hermon_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/hermon.dbg%.o
 
$(BIN)/hermon.c : drivers/infiniband/hermon.c $(MAKEDEPS) $(POST_O_DEPS) $(hermon_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/hermon.c
 
$(BIN)/hermon.s : drivers/infiniband/hermon.c $(MAKEDEPS) $(POST_O_DEPS) $(hermon_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/hermon.s
 
bin/deps/drivers/infiniband/hermon.c.d : $(hermon_DEPS)
 
TAGS : $(hermon_DEPS)


# NIC	
# NIC	family	drivers/infiniband/hermon
DRIVERS += hermon

# NIC	mt25408	15b3,6340	MT25408 HCA driver
DRIVER_mt25408 = hermon
ROM_TYPE_mt25408 = pci
ROM_DESCRIPTION_mt25408 = "MT25408 HCA driver"
PCI_VENDOR_mt25408 = 0x15b3
PCI_DEVICE_mt25408 = 0x6340
ROMS += mt25408
ROMS_hermon += mt25408

# NIC	15b36340	15b3,6340	MT25408 HCA driver
DRIVER_15b36340 = hermon
ROM_TYPE_15b36340 = pci
ROM_DESCRIPTION_15b36340 = "MT25408 HCA driver"
PCI_VENDOR_15b36340 = 0x15b3
PCI_DEVICE_15b36340 = 0x6340
ROMS += 15b36340
ROMS_hermon += 15b36340

# NIC	mt25418	15b3,634a	MT25418 HCA driver
DRIVER_mt25418 = hermon
ROM_TYPE_mt25418 = pci
ROM_DESCRIPTION_mt25418 = "MT25418 HCA driver"
PCI_VENDOR_mt25418 = 0x15b3
PCI_DEVICE_mt25418 = 0x634a
ROMS += mt25418
ROMS_hermon += mt25418

# NIC	15b3634a	15b3,634a	MT25418 HCA driver
DRIVER_15b3634a = hermon
ROM_TYPE_15b3634a = pci
ROM_DESCRIPTION_15b3634a = "MT25418 HCA driver"
PCI_VENDOR_15b3634a = 0x15b3
PCI_DEVICE_15b3634a = 0x634a
ROMS += 15b3634a
ROMS_hermon += 15b3634a

# NIC	mt26418	15b3,6732	MT26418 HCA driver
DRIVER_mt26418 = hermon
ROM_TYPE_mt26418 = pci
ROM_DESCRIPTION_mt26418 = "MT26418 HCA driver"
PCI_VENDOR_mt26418 = 0x15b3
PCI_DEVICE_mt26418 = 0x6732
ROMS += mt26418
ROMS_hermon += mt26418

# NIC	15b36732	15b3,6732	MT26418 HCA driver
DRIVER_15b36732 = hermon
ROM_TYPE_15b36732 = pci
ROM_DESCRIPTION_15b36732 = "MT26418 HCA driver"
PCI_VENDOR_15b36732 = 0x15b3
PCI_DEVICE_15b36732 = 0x6732
ROMS += 15b36732
ROMS_hermon += 15b36732

# NIC	mt26428	15b3,673c	MT26428 HCA driver
DRIVER_mt26428 = hermon
ROM_TYPE_mt26428 = pci
ROM_DESCRIPTION_mt26428 = "MT26428 HCA driver"
PCI_VENDOR_mt26428 = 0x15b3
PCI_DEVICE_mt26428 = 0x673c
ROMS += mt26428
ROMS_hermon += mt26428

# NIC	15b3673c	15b3,673c	MT26428 HCA driver
DRIVER_15b3673c = hermon
ROM_TYPE_15b3673c = pci
ROM_DESCRIPTION_15b3673c = "MT26428 HCA driver"
PCI_VENDOR_15b3673c = 0x15b3
PCI_DEVICE_15b3673c = 0x673c
ROMS += 15b3673c
ROMS_hermon += 15b3673c
