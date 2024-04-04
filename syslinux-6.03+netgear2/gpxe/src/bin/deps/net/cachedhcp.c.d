cachedhcp_DEPS = net/cachedhcp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/stdlib.h \
 include/assert.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/dhcppkt.h include/gpxe/dhcpopts.h \
 include/gpxe/iobuf.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

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

include/gpxe/dhcppkt.h:

include/gpxe/dhcpopts.h:

include/gpxe/iobuf.h:

$(BIN)/cachedhcp.o : net/cachedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(cachedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/cachedhcp.o
 
$(BIN)/cachedhcp.dbg%.o : net/cachedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(cachedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/cachedhcp.dbg%.o
 
$(BIN)/cachedhcp.c : net/cachedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(cachedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/cachedhcp.c
 
$(BIN)/cachedhcp.s : net/cachedhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(cachedhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/cachedhcp.s
 
bin/deps/net/cachedhcp.c.d : $(cachedhcp_DEPS)
 
TAGS : $(cachedhcp_DEPS)

