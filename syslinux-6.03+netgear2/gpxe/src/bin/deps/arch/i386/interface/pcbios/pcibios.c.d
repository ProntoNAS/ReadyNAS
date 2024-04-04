pcibios_DEPS = arch/i386/interface/pcbios/pcibios.c include/compiler.h \
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
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h

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

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/pcibios.o : arch/i386/interface/pcbios/pcibios.c $(MAKEDEPS) $(POST_O_DEPS) $(pcibios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pcibios.o
 
$(BIN)/pcibios.dbg%.o : arch/i386/interface/pcbios/pcibios.c $(MAKEDEPS) $(POST_O_DEPS) $(pcibios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pcibios.dbg%.o
 
$(BIN)/pcibios.c : arch/i386/interface/pcbios/pcibios.c $(MAKEDEPS) $(POST_O_DEPS) $(pcibios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pcibios.c
 
$(BIN)/pcibios.s : arch/i386/interface/pcbios/pcibios.c $(MAKEDEPS) $(POST_O_DEPS) $(pcibios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pcibios.s
 
bin/deps/arch/i386/interface/pcbios/pcibios.c.d : $(pcibios_DEPS)
 
TAGS : $(pcibios_DEPS)

