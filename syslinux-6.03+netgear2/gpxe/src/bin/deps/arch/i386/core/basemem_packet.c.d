basemem_packet_DEPS = arch/i386/core/basemem_packet.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/basemem_packet.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/basemem_packet.h:

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

$(BIN)/basemem_packet.o : arch/i386/core/basemem_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/basemem_packet.o
 
$(BIN)/basemem_packet.dbg%.o : arch/i386/core/basemem_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/basemem_packet.dbg%.o
 
$(BIN)/basemem_packet.c : arch/i386/core/basemem_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/basemem_packet.c
 
$(BIN)/basemem_packet.s : arch/i386/core/basemem_packet.c $(MAKEDEPS) $(POST_O_DEPS) $(basemem_packet_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/basemem_packet.s
 
bin/deps/arch/i386/core/basemem_packet.c.d : $(basemem_packet_DEPS)
 
TAGS : $(basemem_packet_DEPS)

