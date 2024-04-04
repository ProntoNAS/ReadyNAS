epic100_DEPS = drivers/net/epic100.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/etherboot.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/unistd.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/strings.h arch/i386/include/limits.h include/string.h \
 arch/x86/include/bits/string.h include/console.h include/gpxe/tables.h \
 include/gpxe/if_arp.h include/gpxe/if_ether.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/list.h include/gpxe/pci_io.h \
 config/ioapi.h include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/ethernet.h include/nic.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/isapnp.h include/gpxe/isa_ids.h include/gpxe/isa.h \
 include/gpxe/eisa.h include/gpxe/mca.h drivers/net/epic100.h

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

include/gpxe/ethernet.h:

include/nic.h:

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

drivers/net/epic100.h:

$(BIN)/epic100.o : drivers/net/epic100.c $(MAKEDEPS) $(POST_O_DEPS) $(epic100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/epic100.o
 
$(BIN)/epic100.dbg%.o : drivers/net/epic100.c $(MAKEDEPS) $(POST_O_DEPS) $(epic100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/epic100.dbg%.o
 
$(BIN)/epic100.c : drivers/net/epic100.c $(MAKEDEPS) $(POST_O_DEPS) $(epic100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/epic100.c
 
$(BIN)/epic100.s : drivers/net/epic100.c $(MAKEDEPS) $(POST_O_DEPS) $(epic100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/epic100.s
 
bin/deps/drivers/net/epic100.c.d : $(epic100_DEPS)
 
TAGS : $(epic100_DEPS)


# NIC	
# NIC	family	drivers/net/epic100
DRIVERS += epic100

# NIC	epic100	10b8,0005	SMC EtherPowerII
DRIVER_epic100 = epic100
ROM_TYPE_epic100 = pci
ROM_DESCRIPTION_epic100 = "SMC EtherPowerII"
PCI_VENDOR_epic100 = 0x10b8
PCI_DEVICE_epic100 = 0x0005
ROMS += epic100
ROMS_epic100 += epic100

# NIC	10b80005	10b8,0005	SMC EtherPowerII
DRIVER_10b80005 = epic100
ROM_TYPE_10b80005 = pci
ROM_DESCRIPTION_10b80005 = "SMC EtherPowerII"
PCI_VENDOR_10b80005 = 0x10b8
PCI_DEVICE_10b80005 = 0x0005
ROMS += 10b80005
ROMS_epic100 += 10b80005

# NIC	smc-83c175	10b8,0006	SMC EPIC/C 83c175
DRIVER_smc-83c175 = epic100
ROM_TYPE_smc-83c175 = pci
ROM_DESCRIPTION_smc-83c175 = "SMC EPIC/C 83c175"
PCI_VENDOR_smc-83c175 = 0x10b8
PCI_DEVICE_smc-83c175 = 0x0006
ROMS += smc-83c175
ROMS_epic100 += smc-83c175

# NIC	10b80006	10b8,0006	SMC EPIC/C 83c175
DRIVER_10b80006 = epic100
ROM_TYPE_10b80006 = pci
ROM_DESCRIPTION_10b80006 = "SMC EPIC/C 83c175"
PCI_VENDOR_10b80006 = 0x10b8
PCI_DEVICE_10b80006 = 0x0006
ROMS += 10b80006
ROMS_epic100 += 10b80006
