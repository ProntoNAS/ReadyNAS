dhcppkt_DEPS = net/dhcppkt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/uuid.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/dhcpopts.h include/gpxe/dhcppkt.h

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

include/gpxe/netdevice.h:

include/gpxe/list.h:

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

include/gpxe/dhcpopts.h:

include/gpxe/dhcppkt.h:

$(BIN)/dhcppkt.o : net/dhcppkt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcppkt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/dhcppkt.o
 
$(BIN)/dhcppkt.dbg%.o : net/dhcppkt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcppkt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/dhcppkt.dbg%.o
 
$(BIN)/dhcppkt.c : net/dhcppkt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcppkt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/dhcppkt.c
 
$(BIN)/dhcppkt.s : net/dhcppkt.c $(MAKEDEPS) $(POST_O_DEPS) $(dhcppkt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/dhcppkt.s
 
bin/deps/net/dhcppkt.c.d : $(dhcppkt_DEPS)
 
TAGS : $(dhcppkt_DEPS)

