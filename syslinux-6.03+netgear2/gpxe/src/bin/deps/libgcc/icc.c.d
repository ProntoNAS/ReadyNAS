icc_DEPS = libgcc/icc.c include/compiler.h arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/icc.o : libgcc/icc.c $(MAKEDEPS) $(POST_O_DEPS) $(icc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/icc.o
 
$(BIN)/icc.dbg%.o : libgcc/icc.c $(MAKEDEPS) $(POST_O_DEPS) $(icc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/icc.dbg%.o
 
$(BIN)/icc.c : libgcc/icc.c $(MAKEDEPS) $(POST_O_DEPS) $(icc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/icc.c
 
$(BIN)/icc.s : libgcc/icc.c $(MAKEDEPS) $(POST_O_DEPS) $(icc_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/icc.s
 
bin/deps/libgcc/icc.c.d : $(icc_DEPS)
 
TAGS : $(icc_DEPS)

