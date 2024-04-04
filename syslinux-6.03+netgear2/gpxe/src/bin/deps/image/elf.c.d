elf_DEPS = image/elf.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/elf.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/segment.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/elf.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/elf.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/gpxe/segment.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/elf.h:

$(BIN)/elf.o : image/elf.c $(MAKEDEPS) $(POST_O_DEPS) $(elf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/elf.o
 
$(BIN)/elf.dbg%.o : image/elf.c $(MAKEDEPS) $(POST_O_DEPS) $(elf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/elf.dbg%.o
 
$(BIN)/elf.c : image/elf.c $(MAKEDEPS) $(POST_O_DEPS) $(elf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/elf.c
 
$(BIN)/elf.s : image/elf.c $(MAKEDEPS) $(POST_O_DEPS) $(elf_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/elf.s
 
bin/deps/image/elf.c.d : $(elf_DEPS)
 
TAGS : $(elf_DEPS)

