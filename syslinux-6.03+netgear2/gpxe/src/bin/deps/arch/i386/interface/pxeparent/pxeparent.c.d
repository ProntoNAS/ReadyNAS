pxeparent_DEPS = arch/i386/interface/pxeparent/pxeparent.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/dhcp.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/list.h include/stddef.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/uuid.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/uaccess.h \
 include/string.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/pxeparent.h \
 arch/i386/include/pxe_types.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h \
 arch/i386/include/pxe_types.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_api.h include/gpxe/device.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/dhcp.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

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

arch/i386/include/pxeparent.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

arch/i386/include/pxe_types.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

$(BIN)/pxeparent.o : arch/i386/interface/pxeparent/pxeparent.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxeparent.o
 
$(BIN)/pxeparent.dbg%.o : arch/i386/interface/pxeparent/pxeparent.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxeparent.dbg%.o
 
$(BIN)/pxeparent.c : arch/i386/interface/pxeparent/pxeparent.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxeparent.c
 
$(BIN)/pxeparent.s : arch/i386/interface/pxeparent/pxeparent.c $(MAKEDEPS) $(POST_O_DEPS) $(pxeparent_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxeparent.s
 
bin/deps/arch/i386/interface/pxeparent/pxeparent.c.d : $(pxeparent_DEPS)
 
TAGS : $(pxeparent_DEPS)

