virtio-net_DEPS = drivers/net/virtio-net.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h include/gpxe/virtio-ring.h \
 include/gpxe/virtio-pci.h drivers/net/virtio-net.h

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

include/gpxe/virtio-ring.h:

include/gpxe/virtio-pci.h:

drivers/net/virtio-net.h:

$(BIN)/virtio-net.o : drivers/net/virtio-net.c $(MAKEDEPS) $(POST_O_DEPS) $(virtio-net_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/virtio-net.o
 
$(BIN)/virtio-net.dbg%.o : drivers/net/virtio-net.c $(MAKEDEPS) $(POST_O_DEPS) $(virtio-net_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/virtio-net.dbg%.o
 
$(BIN)/virtio-net.c : drivers/net/virtio-net.c $(MAKEDEPS) $(POST_O_DEPS) $(virtio-net_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/virtio-net.c
 
$(BIN)/virtio-net.s : drivers/net/virtio-net.c $(MAKEDEPS) $(POST_O_DEPS) $(virtio-net_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/virtio-net.s
 
bin/deps/drivers/net/virtio-net.c.d : $(virtio-net_DEPS)
 
TAGS : $(virtio-net_DEPS)


# NIC	
# NIC	family	drivers/net/virtio-net
DRIVERS += virtio-net

# NIC	virtio-net	1af4,1000	Virtio Network Interface
DRIVER_virtio-net = virtio-net
ROM_TYPE_virtio-net = pci
ROM_DESCRIPTION_virtio-net = "Virtio Network Interface"
PCI_VENDOR_virtio-net = 0x1af4
PCI_DEVICE_virtio-net = 0x1000
ROMS += virtio-net
ROMS_virtio-net += virtio-net

# NIC	1af41000	1af4,1000	Virtio Network Interface
DRIVER_1af41000 = virtio-net
ROM_TYPE_1af41000 = pci
ROM_DESCRIPTION_1af41000 = "Virtio Network Interface"
PCI_VENDOR_1af41000 = 0x1af4
PCI_DEVICE_1af41000 = 0x1000
ROMS += 1af41000
ROMS_virtio-net += 1af41000
