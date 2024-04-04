pcidirect_DEPS = arch/x86/core/pcidirect.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/io.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h include/gpxe/pci_io.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/pci_ids.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/io.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

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

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

$(BIN)/pcidirect.o : arch/x86/core/pcidirect.c $(MAKEDEPS) $(POST_O_DEPS) $(pcidirect_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pcidirect.o
 
$(BIN)/pcidirect.dbg%.o : arch/x86/core/pcidirect.c $(MAKEDEPS) $(POST_O_DEPS) $(pcidirect_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pcidirect.dbg%.o
 
$(BIN)/pcidirect.c : arch/x86/core/pcidirect.c $(MAKEDEPS) $(POST_O_DEPS) $(pcidirect_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pcidirect.c
 
$(BIN)/pcidirect.s : arch/x86/core/pcidirect.c $(MAKEDEPS) $(POST_O_DEPS) $(pcidirect_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pcidirect.s
 
bin/deps/arch/x86/core/pcidirect.c.d : $(pcidirect_DEPS)
 
TAGS : $(pcidirect_DEPS)

