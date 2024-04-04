comboot_call_DEPS = arch/i386/interface/syslinux/comboot_call.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/biosint.h include/console.h \
 include/gpxe/tables.h include/stdlib.h include/assert.h \
 arch/i386/include/comboot.h arch/i386/include/setjmp.h include/gpxe/in.h \
 include/gpxe/socket.h arch/i386/include/bzimage.h \
 arch/i386/include/pxe_call.h arch/i386/include/pxe_api.h \
 arch/i386/include/pxe_types.h include/gpxe/posix_io.h \
 include/gpxe/process.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/serial.h include/gpxe/init.h include/gpxe/image.h \
 include/usr/imgmgmt.h config/console.h config/serial.h

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

include/console.h:

include/gpxe/tables.h:

include/stdlib.h:

include/assert.h:

arch/i386/include/comboot.h:

arch/i386/include/setjmp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

arch/i386/include/bzimage.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

arch/i386/include/pxe_types.h:

include/gpxe/posix_io.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/serial.h:

include/gpxe/init.h:

include/gpxe/image.h:

include/usr/imgmgmt.h:

config/console.h:

config/serial.h:

$(BIN)/comboot_call.o : arch/i386/interface/syslinux/comboot_call.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/comboot_call.o
 
$(BIN)/comboot_call.dbg%.o : arch/i386/interface/syslinux/comboot_call.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/comboot_call.dbg%.o
 
$(BIN)/comboot_call.c : arch/i386/interface/syslinux/comboot_call.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/comboot_call.c
 
$(BIN)/comboot_call.s : arch/i386/interface/syslinux/comboot_call.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_call_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/comboot_call.s
 
bin/deps/arch/i386/interface/syslinux/comboot_call.c.d : $(comboot_call_DEPS)
 
TAGS : $(comboot_call_DEPS)

