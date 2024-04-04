interface_DEPS = core/interface.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/interface.h \
 include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

$(BIN)/interface.o : core/interface.c $(MAKEDEPS) $(POST_O_DEPS) $(interface_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/interface.o
 
$(BIN)/interface.dbg%.o : core/interface.c $(MAKEDEPS) $(POST_O_DEPS) $(interface_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/interface.dbg%.o
 
$(BIN)/interface.c : core/interface.c $(MAKEDEPS) $(POST_O_DEPS) $(interface_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/interface.c
 
$(BIN)/interface.s : core/interface.c $(MAKEDEPS) $(POST_O_DEPS) $(interface_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/interface.s
 
bin/deps/core/interface.c.d : $(interface_DEPS)
 
TAGS : $(interface_DEPS)

