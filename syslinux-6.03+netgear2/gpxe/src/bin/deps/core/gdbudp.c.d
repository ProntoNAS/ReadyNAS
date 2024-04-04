gdbudp_DEPS = core/gdbudp.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/iobuf.h include/assert.h include/gpxe/list.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/if_arp.h \
 include/gpxe/if_ether.h include/gpxe/ip.h include/gpxe/retry.h \
 include/gpxe/udp.h include/gpxe/tcpip.h include/gpxe/tables.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/nap.h include/gpxe/api.h config/nap.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/null_nap.h \
 arch/i386/include/bits/nap.h arch/i386/include/gpxe/bios_nap.h \
 arch/x86/include/gpxe/efi/efix86_nap.h include/gpxe/gdbstub.h \
 arch/i386/include/gdbmach.h include/gpxe/gdbudp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/if_arp.h:

include/gpxe/if_ether.h:

include/gpxe/ip.h:

include/gpxe/retry.h:

include/gpxe/udp.h:

include/gpxe/tcpip.h:

include/gpxe/tables.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/nap.h:

include/gpxe/api.h:

config/nap.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

include/gpxe/gdbstub.h:

arch/i386/include/gdbmach.h:

include/gpxe/gdbudp.h:

$(BIN)/gdbudp.o : core/gdbudp.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbudp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/gdbudp.o
 
$(BIN)/gdbudp.dbg%.o : core/gdbudp.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbudp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/gdbudp.dbg%.o
 
$(BIN)/gdbudp.c : core/gdbudp.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbudp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/gdbudp.c
 
$(BIN)/gdbudp.s : core/gdbudp.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbudp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/gdbudp.s
 
bin/deps/core/gdbudp.c.d : $(gdbudp_DEPS)
 
TAGS : $(gdbudp_DEPS)

