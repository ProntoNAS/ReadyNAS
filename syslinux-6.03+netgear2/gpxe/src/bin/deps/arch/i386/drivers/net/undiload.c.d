undiload_DEPS = arch/i386/drivers/net/undiload.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 arch/i386/include/pxe.h arch/i386/include/pxe_types.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/pxe_api.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/bios.h \
 arch/i386/include/pnpbios.h arch/i386/include/basemem.h \
 include/gpxe/pci.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 arch/i386/include/undi.h arch/i386/include/pxe_types.h \
 arch/i386/include/undirom.h arch/i386/include/undiload.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

arch/i386/include/bios.h:

arch/i386/include/pnpbios.h:

arch/i386/include/basemem.h:

include/gpxe/pci.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

arch/i386/include/undi.h:

arch/i386/include/pxe_types.h:

arch/i386/include/undirom.h:

arch/i386/include/undiload.h:

$(BIN)/undiload.o : arch/i386/drivers/net/undiload.c $(MAKEDEPS) $(POST_O_DEPS) $(undiload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/undiload.o
 
$(BIN)/undiload.dbg%.o : arch/i386/drivers/net/undiload.c $(MAKEDEPS) $(POST_O_DEPS) $(undiload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/undiload.dbg%.o
 
$(BIN)/undiload.c : arch/i386/drivers/net/undiload.c $(MAKEDEPS) $(POST_O_DEPS) $(undiload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/undiload.c
 
$(BIN)/undiload.s : arch/i386/drivers/net/undiload.c $(MAKEDEPS) $(POST_O_DEPS) $(undiload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/undiload.s
 
bin/deps/arch/i386/drivers/net/undiload.c.d : $(undiload_DEPS)
 
TAGS : $(undiload_DEPS)

