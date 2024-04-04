bootsector_DEPS = arch/i386/image/bootsector.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/realmode.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/biosint.h \
 arch/i386/include/bootsector.h

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

arch/i386/include/bootsector.h:

$(BIN)/bootsector.o : arch/i386/image/bootsector.c $(MAKEDEPS) $(POST_O_DEPS) $(bootsector_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bootsector.o
 
$(BIN)/bootsector.dbg%.o : arch/i386/image/bootsector.c $(MAKEDEPS) $(POST_O_DEPS) $(bootsector_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bootsector.dbg%.o
 
$(BIN)/bootsector.c : arch/i386/image/bootsector.c $(MAKEDEPS) $(POST_O_DEPS) $(bootsector_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bootsector.c
 
$(BIN)/bootsector.s : arch/i386/image/bootsector.c $(MAKEDEPS) $(POST_O_DEPS) $(bootsector_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bootsector.s
 
bin/deps/arch/i386/image/bootsector.c.d : $(bootsector_DEPS)
 
TAGS : $(bootsector_DEPS)

