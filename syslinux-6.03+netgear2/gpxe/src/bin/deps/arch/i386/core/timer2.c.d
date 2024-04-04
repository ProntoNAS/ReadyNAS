timer2_DEPS = arch/i386/core/timer2.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/gpxe/timer2.h \
 include/gpxe/io.h include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/gpxe/timer2.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

$(BIN)/timer2.o : arch/i386/core/timer2.c $(MAKEDEPS) $(POST_O_DEPS) $(timer2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/timer2.o
 
$(BIN)/timer2.dbg%.o : arch/i386/core/timer2.c $(MAKEDEPS) $(POST_O_DEPS) $(timer2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/timer2.dbg%.o
 
$(BIN)/timer2.c : arch/i386/core/timer2.c $(MAKEDEPS) $(POST_O_DEPS) $(timer2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/timer2.c
 
$(BIN)/timer2.s : arch/i386/core/timer2.c $(MAKEDEPS) $(POST_O_DEPS) $(timer2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/timer2.s
 
bin/deps/arch/i386/core/timer2.c.d : $(timer2_DEPS)
 
TAGS : $(timer2_DEPS)

