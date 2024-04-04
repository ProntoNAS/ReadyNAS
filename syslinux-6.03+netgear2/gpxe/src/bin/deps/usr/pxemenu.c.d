pxemenu_DEPS = usr/pxemenu.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/ctype.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/curses.h include/gpxe/keys.h include/console.h \
 include/gpxe/tables.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/uuid.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/gpxe/process.h \
 include/usr/dhcpmgmt.h include/usr/autoboot.h

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/ctype.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/curses.h:

include/gpxe/keys.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

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

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/process.h:

include/usr/dhcpmgmt.h:

include/usr/autoboot.h:

$(BIN)/pxemenu.o : usr/pxemenu.c $(MAKEDEPS) $(POST_O_DEPS) $(pxemenu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxemenu.o
 
$(BIN)/pxemenu.dbg%.o : usr/pxemenu.c $(MAKEDEPS) $(POST_O_DEPS) $(pxemenu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxemenu.dbg%.o
 
$(BIN)/pxemenu.c : usr/pxemenu.c $(MAKEDEPS) $(POST_O_DEPS) $(pxemenu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxemenu.c
 
$(BIN)/pxemenu.s : usr/pxemenu.c $(MAKEDEPS) $(POST_O_DEPS) $(pxemenu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxemenu.s
 
bin/deps/usr/pxemenu.c.d : $(pxemenu_DEPS)
 
TAGS : $(pxemenu_DEPS)

