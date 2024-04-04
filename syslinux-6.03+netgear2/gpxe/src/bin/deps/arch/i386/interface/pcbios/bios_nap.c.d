bios_nap_DEPS = arch/i386/interface/pcbios/bios_nap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/nap.h include/gpxe/api.h \
 config/nap.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/null_nap.h arch/i386/include/bits/nap.h \
 arch/i386/include/gpxe/bios_nap.h arch/x86/include/gpxe/efi/efix86_nap.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h config/ioapi.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/nap.h:

include/gpxe/api.h:

config/nap.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/bios_nap.o : arch/i386/interface/pcbios/bios_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bios_nap.o
 
$(BIN)/bios_nap.dbg%.o : arch/i386/interface/pcbios/bios_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bios_nap.dbg%.o
 
$(BIN)/bios_nap.c : arch/i386/interface/pcbios/bios_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bios_nap.c
 
$(BIN)/bios_nap.s : arch/i386/interface/pcbios/bios_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bios_nap.s
 
bin/deps/arch/i386/interface/pcbios/bios_nap.c.d : $(bios_nap_DEPS)
 
TAGS : $(bios_nap_DEPS)

