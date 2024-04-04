aoeboot_DEPS = arch/i386/interface/pcbios/aoeboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/aoe.h include/gpxe/list.h \
 include/gpxe/if_ether.h include/gpxe/retry.h include/gpxe/ata.h \
 include/gpxe/blockdev.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/refcnt.h include/gpxe/netdevice.h \
 include/gpxe/tables.h include/gpxe/settings.h include/gpxe/sanboot.h \
 arch/i386/include/gpxe/abft.h include/gpxe/acpi.h \
 arch/i386/include/int13.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/aoe.h:

include/gpxe/list.h:

include/gpxe/if_ether.h:

include/gpxe/retry.h:

include/gpxe/ata.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/settings.h:

include/gpxe/sanboot.h:

arch/i386/include/gpxe/abft.h:

include/gpxe/acpi.h:

arch/i386/include/int13.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/aoeboot.o : arch/i386/interface/pcbios/aoeboot.c $(MAKEDEPS) $(POST_O_DEPS) $(aoeboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/aoeboot.o
 
$(BIN)/aoeboot.dbg%.o : arch/i386/interface/pcbios/aoeboot.c $(MAKEDEPS) $(POST_O_DEPS) $(aoeboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/aoeboot.dbg%.o
 
$(BIN)/aoeboot.c : arch/i386/interface/pcbios/aoeboot.c $(MAKEDEPS) $(POST_O_DEPS) $(aoeboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/aoeboot.c
 
$(BIN)/aoeboot.s : arch/i386/interface/pcbios/aoeboot.c $(MAKEDEPS) $(POST_O_DEPS) $(aoeboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/aoeboot.s
 
bin/deps/arch/i386/interface/pcbios/aoeboot.c.d : $(aoeboot_DEPS)
 
TAGS : $(aoeboot_DEPS)

