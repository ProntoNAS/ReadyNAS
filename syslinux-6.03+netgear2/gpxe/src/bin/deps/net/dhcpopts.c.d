dhcpopts_DEPS = net/dhcpopts.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/dhcpopts.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

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

include/gpxe/dhcpopts.h:

$(BIN)/dhcpopts.o : net/dhcpopts.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpopts_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dhcpopts.o
 
$(BIN)/dhcpopts.dbg%.o : net/dhcpopts.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpopts_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dhcpopts.dbg%.o
 
$(BIN)/dhcpopts.c : net/dhcpopts.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpopts_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dhcpopts.c
 
$(BIN)/dhcpopts.s : net/dhcpopts.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcpopts_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dhcpopts.s
 
bin/deps/net/dhcpopts.c.d : $(dhcpopts_DEPS)
 
TAGS : $(dhcpopts_DEPS)

