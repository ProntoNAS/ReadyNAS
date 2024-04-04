pxeparent_dhcp_DEPS = arch/i386/interface/pxeparent/pxeparent_dhcp.c \
 include/compiler.h arch/i386/include/bits/compiler.h include/string.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 arch/i386/include/undipreload.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h arch/i386/include/undi.h \
 include/gpxe/device.h arch/i386/include/pxe_types.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/pxeparent.h arch/i386/include/pxe_api.h \
 arch/i386/include/pxe_types.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/assert.h:

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

arch/i386/include/undipreload.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

arch/i386/include/undi.h:

include/gpxe/device.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxeparent.h:

arch/i386/include/pxe_api.h:

arch/i386/include/pxe_types.h:

$(BIN)/pxeparent_dhcp.o : arch/i386/interface/pxeparent/pxeparent_dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxeparent_dhcp.o
 
$(BIN)/pxeparent_dhcp.dbg%.o : arch/i386/interface/pxeparent/pxeparent_dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxeparent_dhcp.dbg%.o
 
$(BIN)/pxeparent_dhcp.c : arch/i386/interface/pxeparent/pxeparent_dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxeparent_dhcp.c
 
$(BIN)/pxeparent_dhcp.s : arch/i386/interface/pxeparent/pxeparent_dhcp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_dhcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxeparent_dhcp.s
 
bin/deps/arch/i386/interface/pxeparent/pxeparent_dhcp.c.d : $(pxeparent_dhcp_DEPS)
 
TAGS : $(pxeparent_dhcp_DEPS)

