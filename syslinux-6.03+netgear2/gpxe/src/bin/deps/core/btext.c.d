btext_DEPS = core/btext.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/btext.o : core/btext.c $(MAKEDEPS) $(POST_O_DEPS) $(btext_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/btext.o
 
$(BIN)/btext.dbg%.o : core/btext.c $(MAKEDEPS) $(POST_O_DEPS) $(btext_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/btext.dbg%.o
 
$(BIN)/btext.c : core/btext.c $(MAKEDEPS) $(POST_O_DEPS) $(btext_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/btext.c
 
$(BIN)/btext.s : core/btext.c $(MAKEDEPS) $(POST_O_DEPS) $(btext_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/btext.s
 
bin/deps/core/btext.c.d : $(btext_DEPS)
 
TAGS : $(btext_DEPS)

