3c509-eisa_DEPS = drivers/net/3c509-eisa.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/eisa.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/isa_ids.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/device.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h include/gpxe/isa.h \
 include/console.h drivers/net/3c509.h include/nic.h include/string.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/gpxe/pci.h include/gpxe/pci_io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/isapnp.h include/gpxe/mca.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/eisa.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/isa_ids.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/isa.h:

include/console.h:

drivers/net/3c509.h:

include/nic.h:

include/string.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/pci.h:

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

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/gpxe/isapnp.h:

include/gpxe/mca.h:

$(BIN)/3c509-eisa.o : drivers/net/3c509-eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509-eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/3c509-eisa.o
 
$(BIN)/3c509-eisa.dbg%.o : drivers/net/3c509-eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509-eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/3c509-eisa.dbg%.o
 
$(BIN)/3c509-eisa.c : drivers/net/3c509-eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509-eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/3c509-eisa.c
 
$(BIN)/3c509-eisa.s : drivers/net/3c509-eisa.c $(MAKEDEPS) $(POST_O_DEPS) $(3c509-eisa_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/3c509-eisa.s
 
bin/deps/drivers/net/3c509-eisa.c.d : $(3c509-eisa_DEPS)
 
TAGS : $(3c509-eisa_DEPS)


# NIC	
# NIC	family	drivers/net/3c509-eisa
DRIVERS += 3c509-eisa

# NIC	3c509-eisa	-	3c509 (EISA)
DRIVER_3c509-eisa = 3c509-eisa
ROM_TYPE_3c509-eisa = isa
ROM_DESCRIPTION_3c509-eisa = "3c509 (EISA)"
ROMS += 3c509-eisa
ROMS_3c509-eisa += 3c509-eisa
