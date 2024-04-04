pxe_udp_DEPS = arch/i386/interface/pxe/pxe_udp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/xfer.h include/stdarg.h \
 include/gpxe/interface.h include/gpxe/refcnt.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h include/gpxe/udp.h \
 include/gpxe/tcpip.h include/gpxe/socket.h include/gpxe/in.h \
 include/gpxe/tables.h include/gpxe/if_ether.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/process.h arch/i386/include/pxe.h \
 arch/i386/include/pxe_types.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h arch/i386/include/pxe_api.h \
 include/gpxe/device.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/udp.h:

include/gpxe/tcpip.h:

include/gpxe/socket.h:

include/gpxe/in.h:

include/gpxe/tables.h:

include/gpxe/if_ether.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/process.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

include/gpxe/device.h:

$(BIN)/pxe_udp.o : arch/i386/interface/pxe/pxe_udp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_udp.o
 
$(BIN)/pxe_udp.dbg%.o : arch/i386/interface/pxe/pxe_udp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_udp.dbg%.o
 
$(BIN)/pxe_udp.c : arch/i386/interface/pxe/pxe_udp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_udp.c
 
$(BIN)/pxe_udp.s : arch/i386/interface/pxe/pxe_udp.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_udp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_udp.s
 
bin/deps/arch/i386/interface/pxe/pxe_udp.c.d : $(pxe_udp_DEPS)
 
TAGS : $(pxe_udp_DEPS)

