main_DEPS = core/main.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdio.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/stdarg.h include/gpxe/init.h include/gpxe/tables.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/refcnt.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/uaccess.h \
 include/string.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/shell.h \
 include/gpxe/shell_banner.h include/gpxe/image.h include/usr/autoboot.h \
 config/general.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

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

include/gpxe/shell.h:

include/gpxe/shell_banner.h:

include/gpxe/image.h:

include/usr/autoboot.h:

config/general.h:

$(BIN)/main.o : core/main.c $(MAKEDEPS) $(POST_O_DEPS) $(main_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/main.o
 
$(BIN)/main.dbg%.o : core/main.c $(MAKEDEPS) $(POST_O_DEPS) $(main_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/main.dbg%.o
 
$(BIN)/main.c : core/main.c $(MAKEDEPS) $(POST_O_DEPS) $(main_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/main.c
 
$(BIN)/main.s : core/main.c $(MAKEDEPS) $(POST_O_DEPS) $(main_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/main.s
 
bin/deps/core/main.c.d : $(main_DEPS)
 
TAGS : $(main_DEPS)

