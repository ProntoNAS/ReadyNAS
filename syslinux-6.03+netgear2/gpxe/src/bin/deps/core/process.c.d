process_DEPS = core/process.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/list.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/assert.h \
 include/gpxe/init.h include/gpxe/tables.h include/gpxe/process.h \
 include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/list.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/process.h:

include/gpxe/refcnt.h:

$(BIN)/process.o : core/process.c $(MAKEDEPS) $(POST_O_DEPS) $(process_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/process.o
 
$(BIN)/process.dbg%.o : core/process.c $(MAKEDEPS) $(POST_O_DEPS) $(process_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/process.dbg%.o
 
$(BIN)/process.c : core/process.c $(MAKEDEPS) $(POST_O_DEPS) $(process_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/process.c
 
$(BIN)/process.s : core/process.c $(MAKEDEPS) $(POST_O_DEPS) $(process_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/process.s
 
bin/deps/core/process.c.d : $(process_DEPS)
 
TAGS : $(process_DEPS)

