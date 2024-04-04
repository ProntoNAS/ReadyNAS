undionly_DEPS = arch/i386/drivers/net/undionly.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/device.h include/gpxe/list.h include/gpxe/tables.h \
 include/gpxe/init.h arch/i386/include/undi.h \
 arch/i386/include/pxe_types.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/undinet.h \
 arch/i386/include/undipreload.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/init.h:

arch/i386/include/undi.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/undinet.h:

arch/i386/include/undipreload.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/undionly.o : arch/i386/drivers/net/undionly.c $(MAKEDEPS) $(POST_O_DEPS) $(undionly_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/undionly.o
 
$(BIN)/undionly.dbg%.o : arch/i386/drivers/net/undionly.c $(MAKEDEPS) $(POST_O_DEPS) $(undionly_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/undionly.dbg%.o
 
$(BIN)/undionly.c : arch/i386/drivers/net/undionly.c $(MAKEDEPS) $(POST_O_DEPS) $(undionly_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/undionly.c
 
$(BIN)/undionly.s : arch/i386/drivers/net/undionly.c $(MAKEDEPS) $(POST_O_DEPS) $(undionly_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/undionly.s
 
bin/deps/arch/i386/drivers/net/undionly.c.d : $(undionly_DEPS)
 
TAGS : $(undionly_DEPS)

