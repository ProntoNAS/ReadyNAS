aoe_DEPS = net/aoe.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/string.h arch/x86/include/bits/string.h include/stdio.h \
 include/stdarg.h include/stdlib.h include/assert.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/list.h include/gpxe/if_ether.h include/gpxe/ethernet.h \
 include/gpxe/iobuf.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/ata.h include/gpxe/blockdev.h \
 include/gpxe/refcnt.h include/gpxe/netdevice.h include/gpxe/tables.h \
 include/gpxe/settings.h include/gpxe/process.h include/gpxe/features.h \
 include/gpxe/dhcp.h include/gpxe/in.h include/gpxe/socket.h \
 include/gpxe/uuid.h include/gpxe/aoe.h include/gpxe/retry.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/list.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/ata.h:

include/gpxe/blockdev.h:

include/gpxe/refcnt.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/settings.h:

include/gpxe/process.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

include/gpxe/aoe.h:

include/gpxe/retry.h:

$(BIN)/aoe.o : net/aoe.c $(MAKEDEPS) $(POST_O_DEPS) $(aoe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/aoe.o
 
$(BIN)/aoe.dbg%.o : net/aoe.c $(MAKEDEPS) $(POST_O_DEPS) $(aoe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/aoe.dbg%.o
 
$(BIN)/aoe.c : net/aoe.c $(MAKEDEPS) $(POST_O_DEPS) $(aoe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/aoe.c
 
$(BIN)/aoe.s : net/aoe.c $(MAKEDEPS) $(POST_O_DEPS) $(aoe_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/aoe.s
 
bin/deps/net/aoe.c.d : $(aoe_DEPS)
 
TAGS : $(aoe_DEPS)

