wd_DEPS = drivers/net/wd.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/wd.o : drivers/net/wd.c $(MAKEDEPS) $(POST_O_DEPS) $(wd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wd.o
 
$(BIN)/wd.dbg%.o : drivers/net/wd.c $(MAKEDEPS) $(POST_O_DEPS) $(wd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wd.dbg%.o
 
$(BIN)/wd.c : drivers/net/wd.c $(MAKEDEPS) $(POST_O_DEPS) $(wd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wd.c
 
$(BIN)/wd.s : drivers/net/wd.c $(MAKEDEPS) $(POST_O_DEPS) $(wd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wd.s
 
bin/deps/drivers/net/wd.c.d : $(wd_DEPS)
 
TAGS : $(wd_DEPS)

