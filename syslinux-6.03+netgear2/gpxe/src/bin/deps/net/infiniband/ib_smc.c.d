ib_smc_DEPS = net/infiniband/ib_smc.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/unistd.h include/stdarg.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/infiniband.h include/gpxe/refcnt.h include/gpxe/device.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/ib_packet.h \
 include/gpxe/ib_mad.h include/gpxe/ib_smc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/unistd.h:

include/stdarg.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/infiniband.h:

include/gpxe/refcnt.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/ib_packet.h:

include/gpxe/ib_mad.h:

include/gpxe/ib_smc.h:

$(BIN)/ib_smc.o : net/infiniband/ib_smc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_smc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ib_smc.o
 
$(BIN)/ib_smc.dbg%.o : net/infiniband/ib_smc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_smc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ib_smc.dbg%.o
 
$(BIN)/ib_smc.c : net/infiniband/ib_smc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_smc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ib_smc.c
 
$(BIN)/ib_smc.s : net/infiniband/ib_smc.c $(MAKEDEPS) $(POST_O_DEPS) $(ib_smc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ib_smc.s
 
bin/deps/net/infiniband/ib_smc.c.d : $(ib_smc_DEPS)
 
TAGS : $(ib_smc_DEPS)

