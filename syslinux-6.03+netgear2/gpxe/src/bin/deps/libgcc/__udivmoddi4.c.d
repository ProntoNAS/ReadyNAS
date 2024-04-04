__udivmoddi4_DEPS = libgcc/__udivmoddi4.c include/compiler.h \
 arch/i386/include/bits/compiler.h libgcc/libgcc.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

libgcc/libgcc.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

$(BIN)/__udivmoddi4.o : libgcc/__udivmoddi4.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivmoddi4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/__udivmoddi4.o
 
$(BIN)/__udivmoddi4.dbg%.o : libgcc/__udivmoddi4.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivmoddi4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/__udivmoddi4.dbg%.o
 
$(BIN)/__udivmoddi4.c : libgcc/__udivmoddi4.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivmoddi4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/__udivmoddi4.c
 
$(BIN)/__udivmoddi4.s : libgcc/__udivmoddi4.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivmoddi4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/__udivmoddi4.s
 
bin/deps/libgcc/__udivmoddi4.c.d : $(__udivmoddi4_DEPS)
 
TAGS : $(__udivmoddi4_DEPS)

