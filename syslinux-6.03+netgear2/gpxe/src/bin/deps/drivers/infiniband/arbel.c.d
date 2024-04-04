arbel_DEPS = drivers/infiniband/arbel.c include/compiler.h \
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
 include/gpxe/malloc.h include/gpxe/umalloc.h config/umalloc.h \
 include/gpxe/efi/efi_umalloc.h arch/i386/include/bits/umalloc.h \
 arch/i386/include/gpxe/memtop_umalloc.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/infiniband.h include/gpxe/ib_packet.h include/gpxe/ib_mad.h \
 include/gpxe/ib_smc.h drivers/infiniband/arbel.h \
 drivers/infiniband/mlx_bitops.h drivers/infiniband/MT25218_PRM.h

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

drivers/infiniband/arbel.h:

drivers/infiniband/mlx_bitops.h:

drivers/infiniband/MT25218_PRM.h:

$(BIN)/arbel.o : drivers/infiniband/arbel.c $(MAKEDEPS) $(POST_O_DEPS) $(arbel_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/arbel.o
 
$(BIN)/arbel.dbg%.o : drivers/infiniband/arbel.c $(MAKEDEPS) $(POST_O_DEPS) $(arbel_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/arbel.dbg%.o
 
$(BIN)/arbel.c : drivers/infiniband/arbel.c $(MAKEDEPS) $(POST_O_DEPS) $(arbel_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/arbel.c
 
$(BIN)/arbel.s : drivers/infiniband/arbel.c $(MAKEDEPS) $(POST_O_DEPS) $(arbel_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/arbel.s
 
bin/deps/drivers/infiniband/arbel.c.d : $(arbel_DEPS)
 
TAGS : $(arbel_DEPS)


# NIC	
# NIC	family	drivers/infiniband/arbel
DRIVERS += arbel

# NIC	mt25218	15b3,6282	MT25218 HCA driver
DRIVER_mt25218 = arbel
ROM_TYPE_mt25218 = pci
ROM_DESCRIPTION_mt25218 = "MT25218 HCA driver"
PCI_VENDOR_mt25218 = 0x15b3
PCI_DEVICE_mt25218 = 0x6282
ROMS += mt25218
ROMS_arbel += mt25218

# NIC	15b36282	15b3,6282	MT25218 HCA driver
DRIVER_15b36282 = arbel
ROM_TYPE_15b36282 = pci
ROM_DESCRIPTION_15b36282 = "MT25218 HCA driver"
PCI_VENDOR_15b36282 = 0x15b3
PCI_DEVICE_15b36282 = 0x6282
ROMS += 15b36282
ROMS_arbel += 15b36282

# NIC	mt25204	15b3,6274	MT25204 HCA driver
DRIVER_mt25204 = arbel
ROM_TYPE_mt25204 = pci
ROM_DESCRIPTION_mt25204 = "MT25204 HCA driver"
PCI_VENDOR_mt25204 = 0x15b3
PCI_DEVICE_mt25204 = 0x6274
ROMS += mt25204
ROMS_arbel += mt25204

# NIC	15b36274	15b3,6274	MT25204 HCA driver
DRIVER_15b36274 = arbel
ROM_TYPE_15b36274 = pci
ROM_DESCRIPTION_15b36274 = "MT25204 HCA driver"
PCI_VENDOR_15b36274 = 0x15b3
PCI_DEVICE_15b36274 = 0x6274
ROMS += 15b36274
ROMS_arbel += 15b36274
