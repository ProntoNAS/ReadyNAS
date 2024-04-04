pxe_call_DEPS = arch/i386/interface/pxe/pxe_call.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/uaccess.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/init.h include/gpxe/tables.h \
 arch/i386/include/registers.h arch/i386/include/biosint.h \
 arch/i386/include/realmode.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h \
 include/gpxe/device.h include/gpxe/list.h include/assert.h \
 arch/i386/include/pxe_call.h arch/i386/include/pxe_api.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

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

include/gpxe/init.h:

include/gpxe/tables.h:

arch/i386/include/registers.h:

arch/i386/include/biosint.h:

arch/i386/include/realmode.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/assert.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

$(BIN)/pxe_call.o : arch/i386/interface/pxe/pxe_call.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_call.o
 
$(BIN)/pxe_call.dbg%.o : arch/i386/interface/pxe/pxe_call.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_call.dbg%.o
 
$(BIN)/pxe_call.c : arch/i386/interface/pxe/pxe_call.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_call.c
 
$(BIN)/pxe_call.s : arch/i386/interface/pxe/pxe_call.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_call.s
 
bin/deps/arch/i386/interface/pxe/pxe_call.c.d : $(pxe_call_DEPS)
 
TAGS : $(pxe_call_DEPS)

