bios_timer_DEPS = arch/i386/interface/pcbios/bios_timer.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 arch/i386/include/realmode.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h config/ioapi.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h arch/i386/include/bios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

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

arch/i386/include/realmode.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

arch/i386/include/bios.h:

$(BIN)/bios_timer.o : arch/i386/interface/pcbios/bios_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bios_timer.o
 
$(BIN)/bios_timer.dbg%.o : arch/i386/interface/pcbios/bios_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bios_timer.dbg%.o
 
$(BIN)/bios_timer.c : arch/i386/interface/pcbios/bios_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bios_timer.c
 
$(BIN)/bios_timer.s : arch/i386/interface/pcbios/bios_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(bios_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bios_timer.s
 
bin/deps/arch/i386/interface/pcbios/bios_timer.c.d : $(bios_timer_DEPS)
 
TAGS : $(bios_timer_DEPS)

