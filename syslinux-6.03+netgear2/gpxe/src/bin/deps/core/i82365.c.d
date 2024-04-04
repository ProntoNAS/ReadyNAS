i82365_DEPS = core/i82365.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/i82365.o : core/i82365.c $(MAKEDEPS) $(POST_O_DEPS) $(i82365_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/i82365.o
 
$(BIN)/i82365.dbg%.o : core/i82365.c $(MAKEDEPS) $(POST_O_DEPS) $(i82365_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/i82365.dbg%.o
 
$(BIN)/i82365.c : core/i82365.c $(MAKEDEPS) $(POST_O_DEPS) $(i82365_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/i82365.c
 
$(BIN)/i82365.s : core/i82365.c $(MAKEDEPS) $(POST_O_DEPS) $(i82365_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/i82365.s
 
bin/deps/core/i82365.c.d : $(i82365_DEPS)
 
TAGS : $(i82365_DEPS)

