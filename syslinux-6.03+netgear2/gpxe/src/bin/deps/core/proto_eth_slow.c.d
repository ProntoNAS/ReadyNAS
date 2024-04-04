proto_eth_slow_DEPS = core/proto_eth_slow.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/proto_eth_slow.o : core/proto_eth_slow.c $(MAKEDEPS) $(POST_O_DEPS) $(proto_eth_slow_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/proto_eth_slow.o
 
$(BIN)/proto_eth_slow.dbg%.o : core/proto_eth_slow.c $(MAKEDEPS) $(POST_O_DEPS) $(proto_eth_slow_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/proto_eth_slow.dbg%.o
 
$(BIN)/proto_eth_slow.c : core/proto_eth_slow.c $(MAKEDEPS) $(POST_O_DEPS) $(proto_eth_slow_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/proto_eth_slow.c
 
$(BIN)/proto_eth_slow.s : core/proto_eth_slow.c $(MAKEDEPS) $(POST_O_DEPS) $(proto_eth_slow_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/proto_eth_slow.s
 
bin/deps/core/proto_eth_slow.c.d : $(proto_eth_slow_DEPS)
 
TAGS : $(proto_eth_slow_DEPS)

