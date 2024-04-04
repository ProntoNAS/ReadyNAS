comboot_DEPS = arch/i386/image/comboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/strings.h arch/i386/include/limits.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/basemem.h \
 arch/i386/include/bios.h arch/i386/include/comboot.h \
 arch/i386/include/setjmp.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/image.h include/gpxe/tables.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/segment.h include/gpxe/init.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/strings.h:

arch/i386/include/limits.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

arch/i386/include/basemem.h:

arch/i386/include/bios.h:

arch/i386/include/comboot.h:

arch/i386/include/setjmp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/segment.h:

include/gpxe/init.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

$(BIN)/comboot.o : arch/i386/image/comboot.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/comboot.o
 
$(BIN)/comboot.dbg%.o : arch/i386/image/comboot.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/comboot.dbg%.o
 
$(BIN)/comboot.c : arch/i386/image/comboot.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/comboot.c
 
$(BIN)/comboot.s : arch/i386/image/comboot.c $(MAKEDEPS) $(POST_O_DEPS) $(comboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/comboot.s
 
bin/deps/arch/i386/image/comboot.c.d : $(comboot_DEPS)
 
TAGS : $(comboot_DEPS)

