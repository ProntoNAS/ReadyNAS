comboot_resolv_DEPS = arch/i386/interface/syslinux/comboot_resolv.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/comboot.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/setjmp.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/list.h include/assert.h include/gpxe/process.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/resolv.h \
 include/gpxe/interface.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/comboot.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/setjmp.h:

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

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/process.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/resolv.h:

include/gpxe/interface.h:

$(BIN)/comboot_resolv.o : arch/i386/interface/syslinux/comboot_resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/comboot_resolv.o
 
$(BIN)/comboot_resolv.dbg%.o : arch/i386/interface/syslinux/comboot_resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/comboot_resolv.dbg%.o
 
$(BIN)/comboot_resolv.c : arch/i386/interface/syslinux/comboot_resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/comboot_resolv.c
 
$(BIN)/comboot_resolv.s : arch/i386/interface/syslinux/comboot_resolv.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_resolv_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/comboot_resolv.s
 
bin/deps/arch/i386/interface/syslinux/comboot_resolv.c.d : $(comboot_resolv_DEPS)
 
TAGS : $(comboot_resolv_DEPS)

