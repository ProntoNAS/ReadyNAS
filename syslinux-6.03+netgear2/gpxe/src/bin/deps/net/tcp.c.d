tcp_DEPS = net/tcp.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/stdlib.h include/assert.h include/stdio.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/malloc.h \
 include/gpxe/retry.h include/gpxe/refcnt.h include/gpxe/xfer.h \
 include/gpxe/interface.h include/gpxe/open.h include/gpxe/tables.h \
 include/gpxe/socket.h include/gpxe/uri.h include/gpxe/tcpip.h \
 include/gpxe/in.h include/gpxe/tcp.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

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

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/malloc.h:

include/gpxe/retry.h:

include/gpxe/refcnt.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/uri.h:

include/gpxe/tcpip.h:

include/gpxe/in.h:

include/gpxe/tcp.h:

$(BIN)/tcp.o : net/tcp.c $(MAKEDEPS) $(POST_O_DEPS) $(tcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/tcp.o
 
$(BIN)/tcp.dbg%.o : net/tcp.c $(MAKEDEPS) $(POST_O_DEPS) $(tcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/tcp.dbg%.o
 
$(BIN)/tcp.c : net/tcp.c $(MAKEDEPS) $(POST_O_DEPS) $(tcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/tcp.c
 
$(BIN)/tcp.s : net/tcp.c $(MAKEDEPS) $(POST_O_DEPS) $(tcp_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/tcp.s
 
bin/deps/net/tcp.c.d : $(tcp_DEPS)
 
TAGS : $(tcp_DEPS)

