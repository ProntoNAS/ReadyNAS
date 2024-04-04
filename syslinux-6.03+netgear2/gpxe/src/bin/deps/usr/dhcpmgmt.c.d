dhcpmgmt_DEPS = usr/dhcpmgmt.c include/compiler.h \
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
 arch/i386/include/librm.h include/gpxe/monojob.h include/gpxe/process.h \
 include/usr/ifmgmt.h include/usr/dhcpmgmt.h

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

include/gpxe/monojob.h:

include/gpxe/process.h:

include/usr/ifmgmt.h:

include/usr/dhcpmgmt.h:

$(BIN)/dhcpmgmt.o : usr/dhcpmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dhcpmgmt.o
 
$(BIN)/dhcpmgmt.dbg%.o : usr/dhcpmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dhcpmgmt.dbg%.o
 
$(BIN)/dhcpmgmt.c : usr/dhcpmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dhcpmgmt.c
 
$(BIN)/dhcpmgmt.s : usr/dhcpmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dhcpmgmt.s
 
bin/deps/usr/dhcpmgmt.c.d : $(dhcpmgmt_DEPS)
 
TAGS : $(dhcpmgmt_DEPS)

