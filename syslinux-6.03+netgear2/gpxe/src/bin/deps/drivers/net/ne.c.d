ne_DEPS = drivers/net/ne.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/ne.o : drivers/net/ne.c $(MAKEDEPS) $(POST_O_DEPS) $(ne_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ne.o
 
$(BIN)/ne.dbg%.o : drivers/net/ne.c $(MAKEDEPS) $(POST_O_DEPS) $(ne_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ne.dbg%.o
 
$(BIN)/ne.c : drivers/net/ne.c $(MAKEDEPS) $(POST_O_DEPS) $(ne_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ne.c
 
$(BIN)/ne.s : drivers/net/ne.c $(MAKEDEPS) $(POST_O_DEPS) $(ne_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ne.s
 
bin/deps/drivers/net/ne.c.d : $(ne_DEPS)
 
TAGS : $(ne_DEPS)

