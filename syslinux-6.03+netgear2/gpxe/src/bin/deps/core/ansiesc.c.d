ansiesc_DEPS = core/ansiesc.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/assert.h include/gpxe/ansiesc.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/assert.h:

include/gpxe/ansiesc.h:

$(BIN)/ansiesc.o : core/ansiesc.c $(MAKEDEPS) $(POST_O_DEPS) $(ansiesc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ansiesc.o
 
$(BIN)/ansiesc.dbg%.o : core/ansiesc.c $(MAKEDEPS) $(POST_O_DEPS) $(ansiesc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ansiesc.dbg%.o
 
$(BIN)/ansiesc.c : core/ansiesc.c $(MAKEDEPS) $(POST_O_DEPS) $(ansiesc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ansiesc.c
 
$(BIN)/ansiesc.s : core/ansiesc.c $(MAKEDEPS) $(POST_O_DEPS) $(ansiesc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ansiesc.s
 
bin/deps/core/ansiesc.c.d : $(ansiesc_DEPS)
 
TAGS : $(ansiesc_DEPS)

