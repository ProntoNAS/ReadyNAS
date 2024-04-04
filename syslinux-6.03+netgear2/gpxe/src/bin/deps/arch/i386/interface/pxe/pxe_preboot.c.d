pxe_preboot_DEPS = arch/i386/interface/pxe/pxe_preboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/netdevice.h \
 include/gpxe/settings.h include/gpxe/fakedhcp.h include/gpxe/device.h \
 include/gpxe/isapnp.h include/gpxe/isa_ids.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/init.h include/gpxe/if_ether.h \
 arch/i386/include/basemem_packet.h arch/i386/include/realmode.h \
 arch/i386/include/registers.h arch/i386/include/biosint.h \
 arch/i386/include/pxe.h arch/i386/include/pxe_types.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/pxe_api.h arch/i386/include/pxe_call.h \
 arch/i386/include/pxe_api.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/uuid.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/fakedhcp.h:

include/gpxe/device.h:

include/gpxe/isapnp.h:

include/gpxe/isa_ids.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/init.h:

include/gpxe/if_ether.h:

arch/i386/include/basemem_packet.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

arch/i386/include/biosint.h:

arch/i386/include/pxe.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/pxe_api.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

$(BIN)/pxe_preboot.o : arch/i386/interface/pxe/pxe_preboot.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_preboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_preboot.o
 
$(BIN)/pxe_preboot.dbg%.o : arch/i386/interface/pxe/pxe_preboot.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_preboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_preboot.dbg%.o
 
$(BIN)/pxe_preboot.c : arch/i386/interface/pxe/pxe_preboot.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_preboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_preboot.c
 
$(BIN)/pxe_preboot.s : arch/i386/interface/pxe/pxe_preboot.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_preboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_preboot.s
 
bin/deps/arch/i386/interface/pxe/pxe_preboot.c.d : $(pxe_preboot_DEPS)
 
TAGS : $(pxe_preboot_DEPS)

