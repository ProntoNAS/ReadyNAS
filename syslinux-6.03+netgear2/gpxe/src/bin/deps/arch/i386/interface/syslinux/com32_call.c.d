com32_call_DEPS = arch/i386/interface/syslinux/com32_call.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/comboot.h \
 arch/i386/include/setjmp.h include/gpxe/in.h include/gpxe/socket.h \
 include/assert.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/realmode.h:

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

arch/i386/include/comboot.h:

arch/i386/include/setjmp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/assert.h:

$(BIN)/com32_call.o : arch/i386/interface/syslinux/com32_call.c $(MAKEDEPS) $(POST_O_DEPS) $(com32_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/com32_call.o
 
$(BIN)/com32_call.dbg%.o : arch/i386/interface/syslinux/com32_call.c $(MAKEDEPS) $(POST_O_DEPS) $(com32_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/com32_call.dbg%.o
 
$(BIN)/com32_call.c : arch/i386/interface/syslinux/com32_call.c $(MAKEDEPS) $(POST_O_DEPS) $(com32_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/com32_call.c
 
$(BIN)/com32_call.s : arch/i386/interface/syslinux/com32_call.c $(MAKEDEPS) $(POST_O_DEPS) $(com32_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/com32_call.s
 
bin/deps/arch/i386/interface/syslinux/com32_call.c.d : $(com32_call_DEPS)
 
TAGS : $(com32_call_DEPS)

