console_DEPS = core/console.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/console.h include/gpxe/tables.h \
 include/gpxe/process.h include/gpxe/list.h include/assert.h \
 include/gpxe/refcnt.h include/gpxe/nap.h include/gpxe/api.h config/nap.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/null_nap.h \
 arch/i386/include/bits/nap.h arch/i386/include/gpxe/bios_nap.h \
 arch/x86/include/gpxe/efi/efix86_nap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/refcnt.h:

include/gpxe/nap.h:

include/gpxe/api.h:

config/nap.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

$(BIN)/console.o : core/console.c $(MAKEDEPS) $(POST_O_DEPS) $(console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/console.o
 
$(BIN)/console.dbg%.o : core/console.c $(MAKEDEPS) $(POST_O_DEPS) $(console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/console.dbg%.o
 
$(BIN)/console.c : core/console.c $(MAKEDEPS) $(POST_O_DEPS) $(console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/console.c
 
$(BIN)/console.s : core/console.c $(MAKEDEPS) $(POST_O_DEPS) $(console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/console.s
 
bin/deps/core/console.c.d : $(console_DEPS)
 
TAGS : $(console_DEPS)

