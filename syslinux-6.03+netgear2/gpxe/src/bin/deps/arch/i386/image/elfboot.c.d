elfboot_DEPS = arch/i386/image/elfboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/elf.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/refcnt.h include/gpxe/elf.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/init.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/elf.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

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

include/gpxe/refcnt.h:

include/gpxe/elf.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/init.h:

$(BIN)/elfboot.o : arch/i386/image/elfboot.c $(MAKEDEPS) $(POST_O_DEPS) $(elfboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/elfboot.o
 
$(BIN)/elfboot.dbg%.o : arch/i386/image/elfboot.c $(MAKEDEPS) $(POST_O_DEPS) $(elfboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/elfboot.dbg%.o
 
$(BIN)/elfboot.c : arch/i386/image/elfboot.c $(MAKEDEPS) $(POST_O_DEPS) $(elfboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/elfboot.c
 
$(BIN)/elfboot.s : arch/i386/image/elfboot.c $(MAKEDEPS) $(POST_O_DEPS) $(elfboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/elfboot.s
 
bin/deps/arch/i386/image/elfboot.c.d : $(elfboot_DEPS)
 
TAGS : $(elfboot_DEPS)

