init_DEPS = core/init.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/gpxe/device.h include/gpxe/list.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/assert.h \
 include/gpxe/tables.h include/gpxe/init.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/init.h:

$(BIN)/init.o : core/init.c $(MAKEDEPS) $(POST_O_DEPS) $(init_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/init.o
 
$(BIN)/init.dbg%.o : core/init.c $(MAKEDEPS) $(POST_O_DEPS) $(init_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/init.dbg%.o
 
$(BIN)/init.c : core/init.c $(MAKEDEPS) $(POST_O_DEPS) $(init_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/init.c
 
$(BIN)/init.s : core/init.c $(MAKEDEPS) $(POST_O_DEPS) $(init_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/init.s
 
bin/deps/core/init.c.d : $(init_DEPS)
 
TAGS : $(init_DEPS)

