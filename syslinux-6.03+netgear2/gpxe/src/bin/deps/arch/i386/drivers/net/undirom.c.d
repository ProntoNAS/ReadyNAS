undirom_DEPS = arch/i386/drivers/net/undirom.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 arch/i386/include/pxe.h arch/i386/include/pxe_types.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/pxe_api.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/undirom.h \
 arch/i386/include/pxe_types.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

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

arch/i386/include/undirom.h:

arch/i386/include/pxe_types.h:

$(BIN)/undirom.o : arch/i386/drivers/net/undirom.c $(MAKEDEPS) $(POST_O_DEPS) $(undirom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/undirom.o
 
$(BIN)/undirom.dbg%.o : arch/i386/drivers/net/undirom.c $(MAKEDEPS) $(POST_O_DEPS) $(undirom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/undirom.dbg%.o
 
$(BIN)/undirom.c : arch/i386/drivers/net/undirom.c $(MAKEDEPS) $(POST_O_DEPS) $(undirom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/undirom.c
 
$(BIN)/undirom.s : arch/i386/drivers/net/undirom.c $(MAKEDEPS) $(POST_O_DEPS) $(undirom_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/undirom.s
 
bin/deps/arch/i386/drivers/net/undirom.c.d : $(undirom_DEPS)
 
TAGS : $(undirom_DEPS)

