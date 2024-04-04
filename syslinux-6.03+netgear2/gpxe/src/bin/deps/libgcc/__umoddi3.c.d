__umoddi3_DEPS = libgcc/__umoddi3.c include/compiler.h \
 arch/i386/include/bits/compiler.h libgcc/libgcc.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

libgcc/libgcc.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

$(BIN)/__umoddi3.o : libgcc/__umoddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__umoddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/__umoddi3.o
 
$(BIN)/__umoddi3.dbg%.o : libgcc/__umoddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__umoddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/__umoddi3.dbg%.o
 
$(BIN)/__umoddi3.c : libgcc/__umoddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__umoddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/__umoddi3.c
 
$(BIN)/__umoddi3.s : libgcc/__umoddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__umoddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/__umoddi3.s
 
bin/deps/libgcc/__umoddi3.c.d : $(__umoddi3_DEPS)
 
TAGS : $(__umoddi3_DEPS)

