linda_DEPS = drivers/infiniband/linda.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/unistd.h include/stddef.h include/stdarg.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/io.h config/ioapi.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/pci_io.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/pci_ids.h include/gpxe/infiniband.h include/gpxe/refcnt.h \
 include/gpxe/ib_packet.h include/gpxe/ib_mad.h include/gpxe/i2c.h \
 include/gpxe/bitbash.h include/gpxe/malloc.h include/gpxe/iobuf.h \
 drivers/infiniband/linda.h include/gpxe/bitops.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 drivers/infiniband/qib_7220_regs.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/unistd.h:

include/stddef.h:

include/stdarg.h:

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

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

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

include/gpxe/infiniband.h:

include/gpxe/refcnt.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/i2c.h:

include/gpxe/bitbash.h:

include/gpxe/malloc.h:

include/gpxe/iobuf.h:

drivers/infiniband/linda.h:

include/gpxe/bitops.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

drivers/infiniband/qib_7220_regs.h:

$(BIN)/linda.o : drivers/infiniband/linda.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/linda.o
 
$(BIN)/linda.dbg%.o : drivers/infiniband/linda.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/linda.dbg%.o
 
$(BIN)/linda.c : drivers/infiniband/linda.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/linda.c
 
$(BIN)/linda.s : drivers/infiniband/linda.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/linda.s
 
bin/deps/drivers/infiniband/linda.c.d : $(linda_DEPS)
 
TAGS : $(linda_DEPS)


# NIC	
# NIC	family	drivers/infiniband/linda
DRIVERS += linda

# NIC	iba7220	1077,7220	QLE7240/7280 HCA driver
DRIVER_iba7220 = linda
ROM_TYPE_iba7220 = pci
ROM_DESCRIPTION_iba7220 = "QLE7240/7280 HCA driver"
PCI_VENDOR_iba7220 = 0x1077
PCI_DEVICE_iba7220 = 0x7220
ROMS += iba7220
ROMS_linda += iba7220

# NIC	10777220	1077,7220	QLE7240/7280 HCA driver
DRIVER_10777220 = linda
ROM_TYPE_10777220 = pci
ROM_DESCRIPTION_10777220 = "QLE7240/7280 HCA driver"
PCI_VENDOR_10777220 = 0x1077
PCI_DEVICE_10777220 = 0x7220
ROMS += 10777220
ROMS_linda += 10777220
