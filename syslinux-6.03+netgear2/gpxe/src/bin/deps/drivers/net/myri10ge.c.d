myri10ge_DEPS = drivers/net/myri10ge.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/ethernet.h \
 include/gpxe/if_ether.h include/gpxe/iobuf.h include/assert.h \
 include/gpxe/list.h include/stddef.h include/gpxe/malloc.h \
 include/stdlib.h include/gpxe/netdevice.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/pci_io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/timer.h config/timer.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 drivers/net/myri10ge_mcp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/malloc.h:

include/stdlib.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/pci_io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

drivers/net/myri10ge_mcp.h:

$(BIN)/myri10ge.o : drivers/net/myri10ge.c $(MAKEDEPS) $(POST_O_DEPS) $(myri10ge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/myri10ge.o
 
$(BIN)/myri10ge.dbg%.o : drivers/net/myri10ge.c $(MAKEDEPS) $(POST_O_DEPS) $(myri10ge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/myri10ge.dbg%.o
 
$(BIN)/myri10ge.c : drivers/net/myri10ge.c $(MAKEDEPS) $(POST_O_DEPS) $(myri10ge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/myri10ge.c
 
$(BIN)/myri10ge.s : drivers/net/myri10ge.c $(MAKEDEPS) $(POST_O_DEPS) $(myri10ge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/myri10ge.s
 
bin/deps/drivers/net/myri10ge.c.d : $(myri10ge_DEPS)
 
TAGS : $(myri10ge_DEPS)


# NIC	
# NIC	family	drivers/net/myri10ge
DRIVERS += myri10ge

# NIC	myri10ge	14c1,0008	Myricom 10Gb Ethernet Adapter
DRIVER_myri10ge = myri10ge
ROM_TYPE_myri10ge = pci
ROM_DESCRIPTION_myri10ge = "Myricom 10Gb Ethernet Adapter"
PCI_VENDOR_myri10ge = 0x14c1
PCI_DEVICE_myri10ge = 0x0008
ROMS += myri10ge
ROMS_myri10ge += myri10ge

# NIC	14c10008	14c1,0008	Myricom 10Gb Ethernet Adapter
DRIVER_14c10008 = myri10ge
ROM_TYPE_14c10008 = pci
ROM_DESCRIPTION_14c10008 = "Myricom 10Gb Ethernet Adapter"
PCI_VENDOR_14c10008 = 0x14c1
PCI_DEVICE_14c10008 = 0x0008
ROMS += 14c10008
ROMS_myri10ge += 14c10008
