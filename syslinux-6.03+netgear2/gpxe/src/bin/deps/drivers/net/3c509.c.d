3c509_DEPS = drivers/net/3c509.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/io.h include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/unistd.h include/stdarg.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/isa.h \
 include/gpxe/isa_ids.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h drivers/net/3c509.h include/nic.h include/stdio.h \
 include/gpxe/pci.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/isapnp.h include/gpxe/eisa.h include/gpxe/mca.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/unistd.h:

include/stdarg.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/isa.h:

include/gpxe/isa_ids.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

drivers/net/3c509.h:

include/nic.h:

include/stdio.h:

include/gpxe/pci.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/isapnp.h:

include/gpxe/eisa.h:

include/gpxe/mca.h:

$(BIN)/3c509.o : drivers/net/3c509.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/3c509.o
 
$(BIN)/3c509.dbg%.o : drivers/net/3c509.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/3c509.dbg%.o
 
$(BIN)/3c509.c : drivers/net/3c509.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/3c509.c
 
$(BIN)/3c509.s : drivers/net/3c509.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/3c509.s
 
bin/deps/drivers/net/3c509.c.d : $(3c509_DEPS)
 
TAGS : $(3c509_DEPS)


# NIC	
# NIC	family	drivers/net/3c509
DRIVERS += 3c509

# NIC	3c509	-	3c509
DRIVER_3c509 = 3c509
ROM_TYPE_3c509 = isa
ROM_DESCRIPTION_3c509 = "3c509"
ROMS += 3c509
ROMS_3c509 += 3c509
