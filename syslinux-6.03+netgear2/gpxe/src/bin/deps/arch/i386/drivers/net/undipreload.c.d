undipreload_DEPS = arch/i386/drivers/net/undipreload.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/realmode.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/undipreload.h \
 arch/i386/include/undi.h include/gpxe/device.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h arch/i386/include/pxe_types.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

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

arch/i386/include/undipreload.h:

arch/i386/include/undi.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

$(BIN)/undipreload.o : arch/i386/drivers/net/undipreload.c $(MAKEDEPS) $(POST_O_DEPS) $(undipreload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/undipreload.o
 
$(BIN)/undipreload.dbg%.o : arch/i386/drivers/net/undipreload.c $(MAKEDEPS) $(POST_O_DEPS) $(undipreload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/undipreload.dbg%.o
 
$(BIN)/undipreload.c : arch/i386/drivers/net/undipreload.c $(MAKEDEPS) $(POST_O_DEPS) $(undipreload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/undipreload.c
 
$(BIN)/undipreload.s : arch/i386/drivers/net/undipreload.c $(MAKEDEPS) $(POST_O_DEPS) $(undipreload_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/undipreload.s
 
bin/deps/arch/i386/drivers/net/undipreload.c.d : $(undipreload_DEPS)
 
TAGS : $(undipreload_DEPS)

