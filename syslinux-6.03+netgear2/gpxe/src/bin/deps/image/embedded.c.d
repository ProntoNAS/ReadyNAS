embedded_DEPS = image/embedded.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/list.h include/assert.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/refcnt.h include/gpxe/init.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

include/gpxe/init.h:

$(BIN)/embedded.o : image/embedded.c $(MAKEDEPS) $(POST_O_DEPS) $(embedded_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/embedded.o
 
$(BIN)/embedded.dbg%.o : image/embedded.c $(MAKEDEPS) $(POST_O_DEPS) $(embedded_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/embedded.dbg%.o
 
$(BIN)/embedded.c : image/embedded.c $(MAKEDEPS) $(POST_O_DEPS) $(embedded_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/embedded.c
 
$(BIN)/embedded.s : image/embedded.c $(MAKEDEPS) $(POST_O_DEPS) $(embedded_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/embedded.s
 
bin/deps/image/embedded.c.d : $(embedded_DEPS)
 
TAGS : $(embedded_DEPS)

