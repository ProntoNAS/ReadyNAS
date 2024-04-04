null_nap_DEPS = core/null_nap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/nap.h include/gpxe/api.h \
 config/nap.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/null_nap.h arch/i386/include/bits/nap.h \
 arch/i386/include/gpxe/bios_nap.h arch/x86/include/gpxe/efi/efix86_nap.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/nap.h:

include/gpxe/api.h:

config/nap.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

$(BIN)/null_nap.o : core/null_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(null_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/null_nap.o
 
$(BIN)/null_nap.dbg%.o : core/null_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(null_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/null_nap.dbg%.o
 
$(BIN)/null_nap.c : core/null_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(null_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/null_nap.c
 
$(BIN)/null_nap.s : core/null_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(null_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/null_nap.s
 
bin/deps/core/null_nap.c.d : $(null_nap_DEPS)
 
TAGS : $(null_nap_DEPS)

