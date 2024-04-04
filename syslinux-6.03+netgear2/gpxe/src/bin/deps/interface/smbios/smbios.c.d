smbios_DEPS = interface/smbios/smbios.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/smbios.h config/general.h include/gpxe/efi/efi_smbios.h \
 arch/i386/include/bits/smbios.h arch/i386/include/gpxe/bios_smbios.h

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

$(BIN)/smbios.o : interface/smbios/smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/smbios.o
 
$(BIN)/smbios.dbg%.o : interface/smbios/smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/smbios.dbg%.o
 
$(BIN)/smbios.c : interface/smbios/smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/smbios.c
 
$(BIN)/smbios.s : interface/smbios/smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/smbios.s
 
bin/deps/interface/smbios/smbios.c.d : $(smbios_DEPS)
 
TAGS : $(smbios_DEPS)

