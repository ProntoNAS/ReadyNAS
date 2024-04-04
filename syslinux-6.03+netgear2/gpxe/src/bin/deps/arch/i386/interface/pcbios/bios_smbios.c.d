bios_smbios_DEPS = arch/i386/interface/pcbios/bios_smbios.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/smbios.h config/general.h include/gpxe/efi/efi_smbios.h \
 arch/i386/include/bits/smbios.h arch/i386/include/gpxe/bios_smbios.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 arch/i386/include/pnpbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/smbios.h:

config/general.h:

include/gpxe/efi/efi_smbios.h:

arch/i386/include/bits/smbios.h:

arch/i386/include/gpxe/bios_smbios.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

arch/i386/include/pnpbios.h:

$(BIN)/bios_smbios.o : arch/i386/interface/pcbios/bios_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bios_smbios.o
 
$(BIN)/bios_smbios.dbg%.o : arch/i386/interface/pcbios/bios_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bios_smbios.dbg%.o
 
$(BIN)/bios_smbios.c : arch/i386/interface/pcbios/bios_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bios_smbios.c
 
$(BIN)/bios_smbios.s : arch/i386/interface/pcbios/bios_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bios_smbios.s
 
bin/deps/arch/i386/interface/pcbios/bios_smbios.c.d : $(bios_smbios_DEPS)
 
TAGS : $(bios_smbios_DEPS)

