pciextra_DEPS = drivers/bus/pciextra.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/list.h include/stddef.h include/assert.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

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

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

$(BIN)/pciextra.o : drivers/bus/pciextra.c $(MAKEDEPS) $(POST_O_DEPS) $(pciextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pciextra.o
 
$(BIN)/pciextra.dbg%.o : drivers/bus/pciextra.c $(MAKEDEPS) $(POST_O_DEPS) $(pciextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pciextra.dbg%.o
 
$(BIN)/pciextra.c : drivers/bus/pciextra.c $(MAKEDEPS) $(POST_O_DEPS) $(pciextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pciextra.c
 
$(BIN)/pciextra.s : drivers/bus/pciextra.c $(MAKEDEPS) $(POST_O_DEPS) $(pciextra_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pciextra.s
 
bin/deps/drivers/bus/pciextra.c.d : $(pciextra_DEPS)
 
TAGS : $(pciextra_DEPS)

