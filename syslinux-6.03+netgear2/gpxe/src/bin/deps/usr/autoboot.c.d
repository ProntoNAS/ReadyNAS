autoboot_DEPS = usr/autoboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/assert.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/dhcp.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/uuid.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/image.h include/gpxe/sanboot.h \
 include/gpxe/uri.h include/stdlib.h include/usr/ifmgmt.h \
 include/usr/route.h include/usr/dhcpmgmt.h include/usr/imgmgmt.h \
 include/usr/autoboot.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/image.h:

include/gpxe/sanboot.h:

include/gpxe/uri.h:

include/stdlib.h:

include/usr/ifmgmt.h:

include/usr/route.h:

include/usr/dhcpmgmt.h:

include/usr/imgmgmt.h:

include/usr/autoboot.h:

$(BIN)/autoboot.o : usr/autoboot.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/autoboot.o
 
$(BIN)/autoboot.dbg%.o : usr/autoboot.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/autoboot.dbg%.o
 
$(BIN)/autoboot.c : usr/autoboot.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/autoboot.c
 
$(BIN)/autoboot.s : usr/autoboot.c $(MAKEDEPS) $(POST_O_DEPS) $(autoboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/autoboot.s
 
bin/deps/usr/autoboot.c.d : $(autoboot_DEPS)
 
TAGS : $(autoboot_DEPS)

