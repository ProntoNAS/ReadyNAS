settings_DEPS = core/settings.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/strings.h \
 arch/i386/include/limits.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/vsprintf.h include/gpxe/dhcp.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/uri.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/strings.h:

arch/i386/include/limits.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/vsprintf.h:

include/gpxe/dhcp.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/uri.h:

$(BIN)/settings.o : core/settings.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/settings.o
 
$(BIN)/settings.dbg%.o : core/settings.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/settings.dbg%.o
 
$(BIN)/settings.c : core/settings.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/settings.c
 
$(BIN)/settings.s : core/settings.c $(MAKEDEPS) $(POST_O_DEPS) $(settings_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/settings.s
 
bin/deps/core/settings.c.d : $(settings_DEPS)
 
TAGS : $(settings_DEPS)

