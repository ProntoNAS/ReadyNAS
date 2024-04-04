malloc_DEPS = core/malloc.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/strings.h \
 arch/i386/include/limits.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/list.h include/assert.h \
 include/gpxe/init.h include/gpxe/tables.h include/gpxe/malloc.h \
 include/stdlib.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/strings.h:

arch/i386/include/limits.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/malloc.h:

include/stdlib.h:

$(BIN)/malloc.o : core/malloc.c $(MAKEDEPS) $(POST_O_DEPS) $(malloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/malloc.o
 
$(BIN)/malloc.dbg%.o : core/malloc.c $(MAKEDEPS) $(POST_O_DEPS) $(malloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/malloc.dbg%.o
 
$(BIN)/malloc.c : core/malloc.c $(MAKEDEPS) $(POST_O_DEPS) $(malloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/malloc.c
 
$(BIN)/malloc.s : core/malloc.c $(MAKEDEPS) $(POST_O_DEPS) $(malloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/malloc.s
 
bin/deps/core/malloc.c.d : $(malloc_DEPS)
 
TAGS : $(malloc_DEPS)

