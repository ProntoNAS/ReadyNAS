abft_DEPS = arch/i386/interface/pcbios/abft.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/realmode.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/aoe.h include/gpxe/list.h \
 include/assert.h include/gpxe/if_ether.h include/gpxe/retry.h \
 include/gpxe/ata.h include/gpxe/blockdev.h include/gpxe/refcnt.h \
 include/gpxe/netdevice.h include/gpxe/tables.h include/gpxe/settings.h \
 arch/i386/include/gpxe/abft.h include/gpxe/acpi.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/aoe.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/if_ether.h:

include/gpxe/retry.h:

include/gpxe/ata.h:

include/gpxe/blockdev.h:

include/gpxe/refcnt.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/settings.h:

arch/i386/include/gpxe/abft.h:

include/gpxe/acpi.h:

$(BIN)/abft.o : arch/i386/interface/pcbios/abft.c $(MAKEDEPS) $(POST_O_DEPS) $(abft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/abft.o
 
$(BIN)/abft.dbg%.o : arch/i386/interface/pcbios/abft.c $(MAKEDEPS) $(POST_O_DEPS) $(abft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/abft.dbg%.o
 
$(BIN)/abft.c : arch/i386/interface/pcbios/abft.c $(MAKEDEPS) $(POST_O_DEPS) $(abft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/abft.c
 
$(BIN)/abft.s : arch/i386/interface/pcbios/abft.c $(MAKEDEPS) $(POST_O_DEPS) $(abft_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/abft.s
 
bin/deps/arch/i386/interface/pcbios/abft.c.d : $(abft_DEPS)
 
TAGS : $(abft_DEPS)

