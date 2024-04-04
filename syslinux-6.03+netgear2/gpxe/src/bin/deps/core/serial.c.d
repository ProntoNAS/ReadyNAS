serial_DEPS = core/serial.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/init.h \
 include/gpxe/tables.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/unistd.h include/stdarg.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/gpxe/serial.h \
 config/serial.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/unistd.h:

include/stdarg.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/serial.h:

config/serial.h:

$(BIN)/serial.o : core/serial.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/serial.o
 
$(BIN)/serial.dbg%.o : core/serial.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/serial.dbg%.o
 
$(BIN)/serial.c : core/serial.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/serial.c
 
$(BIN)/serial.s : core/serial.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/serial.s
 
bin/deps/core/serial.c.d : $(serial_DEPS)
 
TAGS : $(serial_DEPS)

