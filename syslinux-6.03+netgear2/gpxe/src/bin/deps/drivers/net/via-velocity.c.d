via-velocity_DEPS = drivers/net/via-velocity.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h include/gpxe/ethernet.h \
 drivers/net/via-velocity.h

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

drivers/net/via-velocity.h:

$(BIN)/via-velocity.o : drivers/net/via-velocity.c $(MAKEDEPS) $(POST_O_DEPS) $(via-velocity_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/via-velocity.o
 
$(BIN)/via-velocity.dbg%.o : drivers/net/via-velocity.c $(MAKEDEPS) $(POST_O_DEPS) $(via-velocity_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/via-velocity.dbg%.o
 
$(BIN)/via-velocity.c : drivers/net/via-velocity.c $(MAKEDEPS) $(POST_O_DEPS) $(via-velocity_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/via-velocity.c
 
$(BIN)/via-velocity.s : drivers/net/via-velocity.c $(MAKEDEPS) $(POST_O_DEPS) $(via-velocity_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/via-velocity.s
 
bin/deps/drivers/net/via-velocity.c.d : $(via-velocity_DEPS)
 
TAGS : $(via-velocity_DEPS)


# NIC	
# NIC	family	drivers/net/via-velocity
DRIVERS += via-velocity

# NIC	via-velocity	1106,3119	VIA Networking Velocity Family Gigabit Ethernet Adapter
DRIVER_via-velocity = via-velocity
ROM_TYPE_via-velocity = pci
ROM_DESCRIPTION_via-velocity = "VIA Networking Velocity Family Gigabit Ethernet Adapter"
PCI_VENDOR_via-velocity = 0x1106
PCI_DEVICE_via-velocity = 0x3119
ROMS += via-velocity
ROMS_via-velocity += via-velocity

# NIC	11063119	1106,3119	VIA Networking Velocity Family Gigabit Ethernet Adapter
DRIVER_11063119 = via-velocity
ROM_TYPE_11063119 = pci
ROM_DESCRIPTION_11063119 = "VIA Networking Velocity Family Gigabit Ethernet Adapter"
PCI_VENDOR_11063119 = 0x1106
PCI_DEVICE_11063119 = 0x3119
ROMS += 11063119
ROMS_via-velocity += 11063119
