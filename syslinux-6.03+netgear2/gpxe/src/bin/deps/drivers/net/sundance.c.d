sundance_DEPS = drivers/net/sundance.c include/compiler.h \
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
 include/gpxe/eisa.h include/gpxe/mca.h include/mii.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h

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

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

$(BIN)/sundance.o : drivers/net/sundance.c $(MAKEDEPS) $(POST_O_DEPS) $(sundance_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sundance.o
 
$(BIN)/sundance.dbg%.o : drivers/net/sundance.c $(MAKEDEPS) $(POST_O_DEPS) $(sundance_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sundance.dbg%.o
 
$(BIN)/sundance.c : drivers/net/sundance.c $(MAKEDEPS) $(POST_O_DEPS) $(sundance_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sundance.c
 
$(BIN)/sundance.s : drivers/net/sundance.c $(MAKEDEPS) $(POST_O_DEPS) $(sundance_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sundance.s
 
bin/deps/drivers/net/sundance.c.d : $(sundance_DEPS)
 
TAGS : $(sundance_DEPS)


# NIC	
# NIC	family	drivers/net/sundance
DRIVERS += sundance

# NIC	sundance	13f0,0201	ST201 Sundance 'Alta' based Adaptor
DRIVER_sundance = sundance
ROM_TYPE_sundance = pci
ROM_DESCRIPTION_sundance = "ST201 Sundance 'Alta' based Adaptor"
PCI_VENDOR_sundance = 0x13f0
PCI_DEVICE_sundance = 0x0201
ROMS += sundance
ROMS_sundance += sundance

# NIC	13f00201	13f0,0201	ST201 Sundance 'Alta' based Adaptor
DRIVER_13f00201 = sundance
ROM_TYPE_13f00201 = pci
ROM_DESCRIPTION_13f00201 = "ST201 Sundance 'Alta' based Adaptor"
PCI_VENDOR_13f00201 = 0x13f0
PCI_DEVICE_13f00201 = 0x0201
ROMS += 13f00201
ROMS_sundance += 13f00201

# NIC	dfe530txs	1186,1002	D-Link DFE530TXS (Sundance ST201 Alta)
DRIVER_dfe530txs = sundance
ROM_TYPE_dfe530txs = pci
ROM_DESCRIPTION_dfe530txs = "D-Link DFE530TXS (Sundance ST201 Alta)"
PCI_VENDOR_dfe530txs = 0x1186
PCI_DEVICE_dfe530txs = 0x1002
ROMS += dfe530txs
ROMS_sundance += dfe530txs

# NIC	11861002	1186,1002	D-Link DFE530TXS (Sundance ST201 Alta)
DRIVER_11861002 = sundance
ROM_TYPE_11861002 = pci
ROM_DESCRIPTION_11861002 = "D-Link DFE530TXS (Sundance ST201 Alta)"
PCI_VENDOR_11861002 = 0x1186
PCI_DEVICE_11861002 = 0x1002
ROMS += 11861002
ROMS_sundance += 11861002

# NIC	ip100a	13f0,0200	IC+ IP100A
DRIVER_ip100a = sundance
ROM_TYPE_ip100a = pci
ROM_DESCRIPTION_ip100a = "IC+ IP100A"
PCI_VENDOR_ip100a = 0x13f0
PCI_DEVICE_ip100a = 0x0200
ROMS += ip100a
ROMS_sundance += ip100a

# NIC	13f00200	13f0,0200	IC+ IP100A
DRIVER_13f00200 = sundance
ROM_TYPE_13f00200 = pci
ROM_DESCRIPTION_13f00200 = "IC+ IP100A"
PCI_VENDOR_13f00200 = 0x13f0
PCI_DEVICE_13f00200 = 0x0200
ROMS += 13f00200
ROMS_sundance += 13f00200
