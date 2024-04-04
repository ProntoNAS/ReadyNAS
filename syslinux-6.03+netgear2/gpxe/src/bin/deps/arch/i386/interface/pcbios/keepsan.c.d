keepsan_DEPS = arch/i386/interface/pcbios/keepsan.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/gpxe/settings.h include/gpxe/tables.h include/gpxe/list.h \
 include/stddef.h include/assert.h include/gpxe/refcnt.h \
 include/gpxe/dhcp.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/uuid.h include/gpxe/netdevice.h include/gpxe/uaccess.h \
 include/string.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/init.h include/gpxe/sanboot.h \
 include/usr/autoboot.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/settings.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

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

include/gpxe/init.h:

include/gpxe/sanboot.h:

include/usr/autoboot.h:

$(BIN)/keepsan.o : arch/i386/interface/pcbios/keepsan.c $(MAKEDEPS) $(POST_O_DEPS) $(keepsan_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/keepsan.o
 
$(BIN)/keepsan.dbg%.o : arch/i386/interface/pcbios/keepsan.c $(MAKEDEPS) $(POST_O_DEPS) $(keepsan_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/keepsan.dbg%.o
 
$(BIN)/keepsan.c : arch/i386/interface/pcbios/keepsan.c $(MAKEDEPS) $(POST_O_DEPS) $(keepsan_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/keepsan.c
 
$(BIN)/keepsan.s : arch/i386/interface/pcbios/keepsan.c $(MAKEDEPS) $(POST_O_DEPS) $(keepsan_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/keepsan.s
 
bin/deps/arch/i386/interface/pcbios/keepsan.c.d : $(keepsan_DEPS)
 
TAGS : $(keepsan_DEPS)

