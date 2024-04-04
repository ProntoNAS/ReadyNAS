cwuri_DEPS = core/cwuri.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/uri.h include/stdlib.h \
 include/assert.h include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/uri.h:

include/stdlib.h:

include/assert.h:

include/gpxe/refcnt.h:

$(BIN)/cwuri.o : core/cwuri.c $(MAKEDEPS) $(POST_O_DEPS) $(cwuri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/cwuri.o
 
$(BIN)/cwuri.dbg%.o : core/cwuri.c $(MAKEDEPS) $(POST_O_DEPS) $(cwuri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/cwuri.dbg%.o
 
$(BIN)/cwuri.c : core/cwuri.c $(MAKEDEPS) $(POST_O_DEPS) $(cwuri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/cwuri.c
 
$(BIN)/cwuri.s : core/cwuri.c $(MAKEDEPS) $(POST_O_DEPS) $(cwuri_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/cwuri.s
 
bin/deps/core/cwuri.c.d : $(cwuri_DEPS)
 
TAGS : $(cwuri_DEPS)

