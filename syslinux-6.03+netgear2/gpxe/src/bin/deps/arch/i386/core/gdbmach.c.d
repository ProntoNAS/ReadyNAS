gdbmach_DEPS = arch/i386/core/gdbmach.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/assert.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/gdbstub.h include/gpxe/tables.h \
 arch/i386/include/gdbmach.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/assert.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/gdbstub.h:

include/gpxe/tables.h:

arch/i386/include/gdbmach.h:

$(BIN)/gdbmach.o : arch/i386/core/gdbmach.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbmach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/gdbmach.o
 
$(BIN)/gdbmach.dbg%.o : arch/i386/core/gdbmach.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbmach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/gdbmach.dbg%.o
 
$(BIN)/gdbmach.c : arch/i386/core/gdbmach.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbmach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/gdbmach.c
 
$(BIN)/gdbmach.s : arch/i386/core/gdbmach.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbmach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/gdbmach.s
 
bin/deps/arch/i386/core/gdbmach.c.d : $(gdbmach_DEPS)
 
TAGS : $(gdbmach_DEPS)

