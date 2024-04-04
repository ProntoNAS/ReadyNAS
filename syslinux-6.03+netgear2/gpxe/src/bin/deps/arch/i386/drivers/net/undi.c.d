undi_DEPS = arch/i386/drivers/net/undi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/pci_io.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 arch/i386/include/undi.h arch/i386/include/pxe_types.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/undirom.h arch/i386/include/undiload.h \
 arch/i386/include/undinet.h arch/i386/include/undipreload.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h

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

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

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

arch/i386/include/undi.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/undirom.h:

arch/i386/include/undiload.h:

arch/i386/include/undinet.h:

arch/i386/include/undipreload.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/undi.o : arch/i386/drivers/net/undi.c $(MAKEDEPS) $(POST_O_DEPS) $(undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/undi.o
 
$(BIN)/undi.dbg%.o : arch/i386/drivers/net/undi.c $(MAKEDEPS) $(POST_O_DEPS) $(undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/undi.dbg%.o
 
$(BIN)/undi.c : arch/i386/drivers/net/undi.c $(MAKEDEPS) $(POST_O_DEPS) $(undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/undi.c
 
$(BIN)/undi.s : arch/i386/drivers/net/undi.c $(MAKEDEPS) $(POST_O_DEPS) $(undi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/undi.s
 
bin/deps/arch/i386/drivers/net/undi.c.d : $(undi_DEPS)
 
TAGS : $(undi_DEPS)


# NIC	
# NIC	family	arch/i386/drivers/net/undi
DRIVERS += undi

# NIC	undipci	ffff,ffff	UNDI (PCI)
DRIVER_undipci = undi
ROM_TYPE_undipci = pci
ROM_DESCRIPTION_undipci = "UNDI (PCI)"
PCI_VENDOR_undipci = 0xffff
PCI_DEVICE_undipci = 0xffff
ROMS += undipci
ROMS_undi += undipci

# NIC	ffffffff	ffff,ffff	UNDI (PCI)
DRIVER_ffffffff = undi
ROM_TYPE_ffffffff = pci
ROM_DESCRIPTION_ffffffff = "UNDI (PCI)"
PCI_VENDOR_ffffffff = 0xffff
PCI_DEVICE_ffffffff = 0xffff
ROMS += ffffffff
ROMS_undi += ffffffff
