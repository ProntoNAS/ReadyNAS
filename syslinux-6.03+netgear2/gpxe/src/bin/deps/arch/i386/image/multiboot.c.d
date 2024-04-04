multiboot_DEPS = arch/i386/image/multiboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h include/assert.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/multiboot.h \
 include/gpxe/image.h include/gpxe/tables.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/segment.h include/gpxe/memmap.h \
 include/gpxe/elf.h include/elf.h include/gpxe/init.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

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

arch/i386/include/multiboot.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/segment.h:

include/gpxe/memmap.h:

include/gpxe/elf.h:

include/elf.h:

include/gpxe/init.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

$(BIN)/multiboot.o : arch/i386/image/multiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(multiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/multiboot.o
 
$(BIN)/multiboot.dbg%.o : arch/i386/image/multiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(multiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/multiboot.dbg%.o
 
$(BIN)/multiboot.c : arch/i386/image/multiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(multiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/multiboot.c
 
$(BIN)/multiboot.s : arch/i386/image/multiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(multiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/multiboot.s
 
bin/deps/arch/i386/image/multiboot.c.d : $(multiboot_DEPS)
 
TAGS : $(multiboot_DEPS)

