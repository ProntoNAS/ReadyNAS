script_DEPS = image/script.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/ctype.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/list.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/ctype.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

$(BIN)/script.o : image/script.c $(MAKEDEPS) $(POST_O_DEPS) $(script_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/script.o
 
$(BIN)/script.dbg%.o : image/script.c $(MAKEDEPS) $(POST_O_DEPS) $(script_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/script.dbg%.o
 
$(BIN)/script.c : image/script.c $(MAKEDEPS) $(POST_O_DEPS) $(script_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/script.c
 
$(BIN)/script.s : image/script.c $(MAKEDEPS) $(POST_O_DEPS) $(script_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/script.s
 
bin/deps/image/script.c.d : $(script_DEPS)
 
TAGS : $(script_DEPS)

