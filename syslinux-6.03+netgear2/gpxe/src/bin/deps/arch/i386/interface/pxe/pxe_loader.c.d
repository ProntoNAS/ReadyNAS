pxe_loader_DEPS = arch/i386/interface/pxe/pxe_loader.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/init.h \
 include/gpxe/tables.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h \
 include/gpxe/device.h include/gpxe/list.h include/stddef.h \
 include/assert.h arch/i386/include/pxe_call.h \
 arch/i386/include/pxe_api.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/init.h:

include/gpxe/tables.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/pxe_loader.o : arch/i386/interface/pxe/pxe_loader.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_loader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_loader.o
 
$(BIN)/pxe_loader.dbg%.o : arch/i386/interface/pxe/pxe_loader.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_loader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_loader.dbg%.o
 
$(BIN)/pxe_loader.c : arch/i386/interface/pxe/pxe_loader.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_loader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_loader.c
 
$(BIN)/pxe_loader.s : arch/i386/interface/pxe/pxe_loader.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_loader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_loader.s
 
bin/deps/arch/i386/interface/pxe/pxe_loader.c.d : $(pxe_loader_DEPS)
 
TAGS : $(pxe_loader_DEPS)

