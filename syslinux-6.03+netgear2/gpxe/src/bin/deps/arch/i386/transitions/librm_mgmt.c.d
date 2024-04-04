librm_mgmt_DEPS = arch/i386/transitions/librm_mgmt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/realmode.h:

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

$(BIN)/librm_mgmt.o : arch/i386/transitions/librm_mgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(librm_mgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/librm_mgmt.o
 
$(BIN)/librm_mgmt.dbg%.o : arch/i386/transitions/librm_mgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(librm_mgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/librm_mgmt.dbg%.o
 
$(BIN)/librm_mgmt.c : arch/i386/transitions/librm_mgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(librm_mgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/librm_mgmt.c
 
$(BIN)/librm_mgmt.s : arch/i386/transitions/librm_mgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(librm_mgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/librm_mgmt.s
 
bin/deps/arch/i386/transitions/librm_mgmt.c.d : $(librm_mgmt_DEPS)
 
TAGS : $(librm_mgmt_DEPS)

