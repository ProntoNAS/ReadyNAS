image_DEPS = core/image.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/libgen.h include/gpxe/list.h \
 include/gpxe/umalloc.h include/gpxe/api.h config/umalloc.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 config/ioapi.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_umalloc.h arch/i386/include/bits/umalloc.h \
 arch/i386/include/gpxe/memtop_umalloc.h include/gpxe/uri.h \
 include/gpxe/refcnt.h include/gpxe/image.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/libgen.h:

include/gpxe/list.h:

include/gpxe/umalloc.h:

include/gpxe/api.h:

config/umalloc.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_umalloc.h:

arch/i386/include/bits/umalloc.h:

arch/i386/include/gpxe/memtop_umalloc.h:

include/gpxe/uri.h:

include/gpxe/refcnt.h:

include/gpxe/image.h:

include/gpxe/tables.h:

$(BIN)/image.o : core/image.c $(MAKEDEPS) $(POST_O_DEPS) $(image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/image.o
 
$(BIN)/image.dbg%.o : core/image.c $(MAKEDEPS) $(POST_O_DEPS) $(image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/image.dbg%.o
 
$(BIN)/image.c : core/image.c $(MAKEDEPS) $(POST_O_DEPS) $(image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/image.c
 
$(BIN)/image.s : core/image.c $(MAKEDEPS) $(POST_O_DEPS) $(image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/image.s
 
bin/deps/core/image.c.d : $(image_DEPS)
 
TAGS : $(image_DEPS)

