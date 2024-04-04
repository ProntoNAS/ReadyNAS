refcnt_DEPS = core/refcnt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/refcnt.h:

$(BIN)/refcnt.o : core/refcnt.c $(MAKEDEPS) $(POST_O_DEPS) $(refcnt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/refcnt.o
 
$(BIN)/refcnt.dbg%.o : core/refcnt.c $(MAKEDEPS) $(POST_O_DEPS) $(refcnt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/refcnt.dbg%.o
 
$(BIN)/refcnt.c : core/refcnt.c $(MAKEDEPS) $(POST_O_DEPS) $(refcnt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/refcnt.c
 
$(BIN)/refcnt.s : core/refcnt.c $(MAKEDEPS) $(POST_O_DEPS) $(refcnt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/refcnt.s
 
bin/deps/core/refcnt.c.d : $(refcnt_DEPS)
 
TAGS : $(refcnt_DEPS)

