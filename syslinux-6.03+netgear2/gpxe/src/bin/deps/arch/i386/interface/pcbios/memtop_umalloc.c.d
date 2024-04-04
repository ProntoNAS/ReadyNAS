memtop_umalloc_DEPS = arch/i386/interface/pcbios/memtop_umalloc.c \
 include/compiler.h arch/i386/include/bits/compiler.h \
 arch/i386/include/limits.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/uaccess.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/hidemem.h include/gpxe/memmap.h \
 include/gpxe/umalloc.h config/umalloc.h include/gpxe/efi/efi_umalloc.h \
 arch/i386/include/bits/umalloc.h arch/i386/include/gpxe/memtop_umalloc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/limits.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/uaccess.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

include/gpxe/hidemem.h:

include/gpxe/memmap.h:

include/gpxe/umalloc.h:

config/umalloc.h:

include/gpxe/efi/efi_umalloc.h:

arch/i386/include/bits/umalloc.h:

arch/i386/include/gpxe/memtop_umalloc.h:

$(BIN)/memtop_umalloc.o : arch/i386/interface/pcbios/memtop_umalloc.c $(MAKEDEPS) $(POST_O_DEPS) $(memtop_umalloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/memtop_umalloc.o
 
$(BIN)/memtop_umalloc.dbg%.o : arch/i386/interface/pcbios/memtop_umalloc.c $(MAKEDEPS) $(POST_O_DEPS) $(memtop_umalloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/memtop_umalloc.dbg%.o
 
$(BIN)/memtop_umalloc.c : arch/i386/interface/pcbios/memtop_umalloc.c $(MAKEDEPS) $(POST_O_DEPS) $(memtop_umalloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/memtop_umalloc.c
 
$(BIN)/memtop_umalloc.s : arch/i386/interface/pcbios/memtop_umalloc.c $(MAKEDEPS) $(POST_O_DEPS) $(memtop_umalloc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/memtop_umalloc.s
 
bin/deps/arch/i386/interface/pcbios/memtop_umalloc.c.d : $(memtop_umalloc_DEPS)
 
TAGS : $(memtop_umalloc_DEPS)

