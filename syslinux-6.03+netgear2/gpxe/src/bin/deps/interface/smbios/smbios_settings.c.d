smbios_settings_DEPS = interface/smbios/smbios_settings.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/settings.h \
 include/gpxe/tables.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/init.h include/gpxe/uuid.h \
 include/gpxe/smbios.h include/gpxe/api.h config/general.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 config/ioapi.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_smbios.h arch/i386/include/bits/smbios.h \
 arch/i386/include/gpxe/bios_smbios.h

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

include/gpxe/settings.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/init.h:

include/gpxe/uuid.h:

include/gpxe/smbios.h:

include/gpxe/api.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_smbios.h:

arch/i386/include/bits/smbios.h:

arch/i386/include/gpxe/bios_smbios.h:

$(BIN)/smbios_settings.o : interface/smbios/smbios_settings.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/smbios_settings.o
 
$(BIN)/smbios_settings.dbg%.o : interface/smbios/smbios_settings.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/smbios_settings.dbg%.o
 
$(BIN)/smbios_settings.c : interface/smbios/smbios_settings.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/smbios_settings.c
 
$(BIN)/smbios_settings.s : interface/smbios/smbios_settings.c $(MAKEDEPS) $(POST_O_DEPS) $(smbios_settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/smbios_settings.s
 
bin/deps/interface/smbios/smbios_settings.c.d : $(smbios_settings_DEPS)
 
TAGS : $(smbios_settings_DEPS)

