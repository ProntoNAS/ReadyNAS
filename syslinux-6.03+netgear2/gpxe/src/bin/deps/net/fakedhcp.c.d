fakedhcp_DEPS = net/fakedhcp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/settings.h \
 include/gpxe/tables.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/netdevice.h include/gpxe/dhcppkt.h include/gpxe/dhcp.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/uuid.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/dhcpopts.h \
 include/gpxe/fakedhcp.h

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

include/gpxe/settings.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/netdevice.h:

include/gpxe/dhcppkt.h:

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

include/gpxe/dhcpopts.h:

include/gpxe/fakedhcp.h:

$(BIN)/fakedhcp.o : net/fakedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(fakedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/fakedhcp.o
 
$(BIN)/fakedhcp.dbg%.o : net/fakedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(fakedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/fakedhcp.dbg%.o
 
$(BIN)/fakedhcp.c : net/fakedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(fakedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/fakedhcp.c
 
$(BIN)/fakedhcp.s : net/fakedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(fakedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/fakedhcp.s
 
bin/deps/net/fakedhcp.c.d : $(fakedhcp_DEPS)
 
TAGS : $(fakedhcp_DEPS)

