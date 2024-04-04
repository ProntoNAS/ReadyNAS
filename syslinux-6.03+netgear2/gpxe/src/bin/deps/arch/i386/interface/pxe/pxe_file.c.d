pxe_file_DEPS = arch/i386/interface/pxe/pxe_file.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/stdio.h \
 include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/posix_io.h \
 include/gpxe/features.h include/gpxe/tables.h include/gpxe/dhcp.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h arch/i386/include/pxe_api.h \
 include/gpxe/device.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/posix_io.h:

include/gpxe/features.h:

include/gpxe/tables.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/pxe_file.o : arch/i386/interface/pxe/pxe_file.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_file_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_file.o
 
$(BIN)/pxe_file.dbg%.o : arch/i386/interface/pxe/pxe_file.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_file_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_file.dbg%.o
 
$(BIN)/pxe_file.c : arch/i386/interface/pxe/pxe_file.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_file_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_file.c
 
$(BIN)/pxe_file.s : arch/i386/interface/pxe/pxe_file.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_file_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_file.s
 
bin/deps/arch/i386/interface/pxe/pxe_file.c.d : $(pxe_file_DEPS)
 
TAGS : $(pxe_file_DEPS)

