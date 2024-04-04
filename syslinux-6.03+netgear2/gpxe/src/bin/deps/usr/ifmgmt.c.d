ifmgmt_DEPS = usr/ifmgmt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/unistd.h include/gpxe/timer.h include/gpxe/api.h config/timer.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/console.h include/gpxe/tables.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/assert.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/device.h include/gpxe/process.h \
 include/gpxe/keys.h include/usr/ifmgmt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/unistd.h:

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/device.h:

include/gpxe/process.h:

include/gpxe/keys.h:

include/usr/ifmgmt.h:

$(BIN)/ifmgmt.o : usr/ifmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ifmgmt.o
 
$(BIN)/ifmgmt.dbg%.o : usr/ifmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ifmgmt.dbg%.o
 
$(BIN)/ifmgmt.c : usr/ifmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ifmgmt.c
 
$(BIN)/ifmgmt.s : usr/ifmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(ifmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ifmgmt.s
 
bin/deps/usr/ifmgmt.c.d : $(ifmgmt_DEPS)
 
TAGS : $(ifmgmt_DEPS)

