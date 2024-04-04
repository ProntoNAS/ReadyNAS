ctype_DEPS = core/ctype.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/ctype.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/ctype.h:

$(BIN)/ctype.o : core/ctype.c $(MAKEDEPS) $(POST_O_DEPS) $(ctype_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ctype.o
 
$(BIN)/ctype.dbg%.o : core/ctype.c $(MAKEDEPS) $(POST_O_DEPS) $(ctype_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ctype.dbg%.o
 
$(BIN)/ctype.c : core/ctype.c $(MAKEDEPS) $(POST_O_DEPS) $(ctype_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ctype.c
 
$(BIN)/ctype.s : core/ctype.c $(MAKEDEPS) $(POST_O_DEPS) $(ctype_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ctype.s
 
bin/deps/core/ctype.c.d : $(ctype_DEPS)
 
TAGS : $(ctype_DEPS)

