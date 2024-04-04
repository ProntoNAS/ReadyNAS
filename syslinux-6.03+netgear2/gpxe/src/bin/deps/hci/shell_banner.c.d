shell_banner_DEPS = hci/shell_banner.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/console.h \
 include/gpxe/tables.h include/unistd.h include/stddef.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 config/general.h include/gpxe/keys.h include/gpxe/shell_banner.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/console.h:

include/gpxe/tables.h:

include/unistd.h:

include/stddef.h:

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

config/general.h:

include/gpxe/keys.h:

include/gpxe/shell_banner.h:

$(BIN)/shell_banner.o : hci/shell_banner.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_banner_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/shell_banner.o
 
$(BIN)/shell_banner.dbg%.o : hci/shell_banner.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_banner_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/shell_banner.dbg%.o
 
$(BIN)/shell_banner.c : hci/shell_banner.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_banner_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/shell_banner.c
 
$(BIN)/shell_banner.s : hci/shell_banner.c $(MAKEDEPS) $(POST_O_DEPS) $(shell_banner_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/shell_banner.s
 
bin/deps/hci/shell_banner.c.d : $(shell_banner_DEPS)
 
TAGS : $(shell_banner_DEPS)

