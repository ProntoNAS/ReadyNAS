filter_DEPS = core/filter.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/xfer.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdarg.h \
 include/gpxe/interface.h include/gpxe/refcnt.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h include/gpxe/filter.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/xfer.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/filter.h:

$(BIN)/filter.o : core/filter.c $(MAKEDEPS) $(POST_O_DEPS) $(filter_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/filter.o
 
$(BIN)/filter.dbg%.o : core/filter.c $(MAKEDEPS) $(POST_O_DEPS) $(filter_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/filter.dbg%.o
 
$(BIN)/filter.c : core/filter.c $(MAKEDEPS) $(POST_O_DEPS) $(filter_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/filter.c
 
$(BIN)/filter.s : core/filter.c $(MAKEDEPS) $(POST_O_DEPS) $(filter_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/filter.s
 
bin/deps/core/filter.c.d : $(filter_DEPS)
 
TAGS : $(filter_DEPS)

