dumpregs_DEPS = arch/i386/core/dumpregs.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

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

$(BIN)/dumpregs.o : arch/i386/core/dumpregs.c $(MAKEDEPS) $(POST_O_DEPS) $(dumpregs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dumpregs.o
 
$(BIN)/dumpregs.dbg%.o : arch/i386/core/dumpregs.c $(MAKEDEPS) $(POST_O_DEPS) $(dumpregs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dumpregs.dbg%.o
 
$(BIN)/dumpregs.c : arch/i386/core/dumpregs.c $(MAKEDEPS) $(POST_O_DEPS) $(dumpregs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dumpregs.c
 
$(BIN)/dumpregs.s : arch/i386/core/dumpregs.c $(MAKEDEPS) $(POST_O_DEPS) $(dumpregs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dumpregs.s
 
bin/deps/arch/i386/core/dumpregs.c.d : $(dumpregs_DEPS)
 
TAGS : $(dumpregs_DEPS)

