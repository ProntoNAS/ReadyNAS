nbi_DEPS = arch/i386/image/nbi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/gateA20.h \
 arch/i386/include/memsizes.h arch/i386/include/basemem.h \
 arch/i386/include/bios.h arch/i386/include/basemem_packet.h \
 include/gpxe/segment.h include/gpxe/init.h include/gpxe/tables.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/fakedhcp.h include/gpxe/image.h \
 include/gpxe/features.h include/gpxe/dhcp.h include/gpxe/in.h \
 include/gpxe/socket.h include/gpxe/uuid.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/registers.h:

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

arch/i386/include/gateA20.h:

arch/i386/include/memsizes.h:

arch/i386/include/basemem.h:

arch/i386/include/bios.h:

arch/i386/include/basemem_packet.h:

include/gpxe/segment.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/fakedhcp.h:

include/gpxe/image.h:

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/uuid.h:

$(BIN)/nbi.o : arch/i386/image/nbi.c $(MAKEDEPS) $(POST_O_DEPS) $(nbi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/nbi.o
 
$(BIN)/nbi.dbg%.o : arch/i386/image/nbi.c $(MAKEDEPS) $(POST_O_DEPS) $(nbi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/nbi.dbg%.o
 
$(BIN)/nbi.c : arch/i386/image/nbi.c $(MAKEDEPS) $(POST_O_DEPS) $(nbi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/nbi.c
 
$(BIN)/nbi.s : arch/i386/image/nbi.c $(MAKEDEPS) $(POST_O_DEPS) $(nbi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/nbi.s
 
bin/deps/arch/i386/image/nbi.c.d : $(nbi_DEPS)
 
TAGS : $(nbi_DEPS)

