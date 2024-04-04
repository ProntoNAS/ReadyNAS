natsemi_DEPS = drivers/net/natsemi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/unistd.h include/gpxe/timer.h \
 config/timer.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/if_ether.h include/gpxe/ethernet.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/spi_bit.h include/gpxe/spi.h include/gpxe/nvs.h \
 include/gpxe/bitbash.h include/gpxe/threewire.h \
 arch/i386/include/limits.h include/gpxe/nvo.h include/gpxe/dhcpopts.h \
 drivers/net/natsemi.h

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

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/unistd.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

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

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/spi_bit.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

include/gpxe/bitbash.h:

include/gpxe/threewire.h:

arch/i386/include/limits.h:

include/gpxe/nvo.h:

include/gpxe/dhcpopts.h:

drivers/net/natsemi.h:

$(BIN)/natsemi.o : drivers/net/natsemi.c $(MAKEDEPS) $(POST_O_DEPS) $(natsemi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/natsemi.o
 
$(BIN)/natsemi.dbg%.o : drivers/net/natsemi.c $(MAKEDEPS) $(POST_O_DEPS) $(natsemi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/natsemi.dbg%.o
 
$(BIN)/natsemi.c : drivers/net/natsemi.c $(MAKEDEPS) $(POST_O_DEPS) $(natsemi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/natsemi.c
 
$(BIN)/natsemi.s : drivers/net/natsemi.c $(MAKEDEPS) $(POST_O_DEPS) $(natsemi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/natsemi.s
 
bin/deps/drivers/net/natsemi.c.d : $(natsemi_DEPS)
 
TAGS : $(natsemi_DEPS)


# NIC	
# NIC	family	drivers/net/natsemi
DRIVERS += natsemi

# NIC	dp83815	100b,0020	DP83815
DRIVER_dp83815 = natsemi
ROM_TYPE_dp83815 = pci
ROM_DESCRIPTION_dp83815 = "DP83815"
PCI_VENDOR_dp83815 = 0x100b
PCI_DEVICE_dp83815 = 0x0020
ROMS += dp83815
ROMS_natsemi += dp83815

# NIC	100b0020	100b,0020	DP83815
DRIVER_100b0020 = natsemi
ROM_TYPE_100b0020 = pci
ROM_DESCRIPTION_100b0020 = "DP83815"
PCI_VENDOR_100b0020 = 0x100b
PCI_DEVICE_100b0020 = 0x0020
ROMS += 100b0020
ROMS_natsemi += 100b0020
