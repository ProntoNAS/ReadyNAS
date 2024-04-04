biosint_DEPS = arch/i386/interface/pcbios/biosint.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/realmode.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/biosint.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

arch/i386/include/biosint.h:

$(BIN)/biosint.o : arch/i386/interface/pcbios/biosint.c $(MAKEDEPS) $(POST_O_DEPS) $(biosint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/biosint.o
 
$(BIN)/biosint.dbg%.o : arch/i386/interface/pcbios/biosint.c $(MAKEDEPS) $(POST_O_DEPS) $(biosint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/biosint.dbg%.o
 
$(BIN)/biosint.c : arch/i386/interface/pcbios/biosint.c $(MAKEDEPS) $(POST_O_DEPS) $(biosint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/biosint.c
 
$(BIN)/biosint.s : arch/i386/interface/pcbios/biosint.c $(MAKEDEPS) $(POST_O_DEPS) $(biosint_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/biosint.s
 
bin/deps/arch/i386/interface/pcbios/biosint.c.d : $(biosint_DEPS)
 
TAGS : $(biosint_DEPS)

