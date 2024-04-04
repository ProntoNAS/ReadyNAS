3c503_DEPS = drivers/net/3c503.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/3c503.o : drivers/net/3c503.c $(MAKEDEPS) $(POST_O_DEPS) $(3c503_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/3c503.o
 
$(BIN)/3c503.dbg%.o : drivers/net/3c503.c $(MAKEDEPS) $(POST_O_DEPS) $(3c503_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/3c503.dbg%.o
 
$(BIN)/3c503.c : drivers/net/3c503.c $(MAKEDEPS) $(POST_O_DEPS) $(3c503_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/3c503.c
 
$(BIN)/3c503.s : drivers/net/3c503.c $(MAKEDEPS) $(POST_O_DEPS) $(3c503_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/3c503.s
 
bin/deps/drivers/net/3c503.c.d : $(3c503_DEPS)
 
TAGS : $(3c503_DEPS)

