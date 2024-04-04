getkey_DEPS = core/getkey.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/ctype.h include/console.h \
 include/gpxe/tables.h include/gpxe/process.h include/gpxe/list.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/assert.h include/gpxe/refcnt.h include/gpxe/keys.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/ctype.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/keys.h:

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

$(BIN)/getkey.o : core/getkey.c $(MAKEDEPS) $(POST_O_DEPS) $(getkey_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/getkey.o
 
$(BIN)/getkey.dbg%.o : core/getkey.c $(MAKEDEPS) $(POST_O_DEPS) $(getkey_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/getkey.dbg%.o
 
$(BIN)/getkey.c : core/getkey.c $(MAKEDEPS) $(POST_O_DEPS) $(getkey_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/getkey.c
 
$(BIN)/getkey.s : core/getkey.c $(MAKEDEPS) $(POST_O_DEPS) $(getkey_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/getkey.s
 
bin/deps/core/getkey.c.d : $(getkey_DEPS)
 
TAGS : $(getkey_DEPS)

