pxe_image_DEPS = arch/i386/image/pxe_image.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h \
 include/gpxe/device.h include/gpxe/list.h include/stddef.h \
 include/assert.h include/gpxe/tables.h arch/i386/include/pxe_call.h \
 arch/i386/include/pxe_api.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/image.h include/gpxe/refcnt.h \
 include/gpxe/segment.h include/gpxe/netdevice.h include/gpxe/settings.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/uuid.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

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

include/gpxe/image.h:

include/gpxe/refcnt.h:

include/gpxe/segment.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

$(BIN)/pxe_image.o : arch/i386/image/pxe_image.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_image.o
 
$(BIN)/pxe_image.dbg%.o : arch/i386/image/pxe_image.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_image.dbg%.o
 
$(BIN)/pxe_image.c : arch/i386/image/pxe_image.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_image.c
 
$(BIN)/pxe_image.s : arch/i386/image/pxe_image.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_image.s
 
bin/deps/arch/i386/image/pxe_image.c.d : $(pxe_image_DEPS)
 
TAGS : $(pxe_image_DEPS)

