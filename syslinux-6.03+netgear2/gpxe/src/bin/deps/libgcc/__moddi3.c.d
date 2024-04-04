__moddi3_DEPS = libgcc/__moddi3.c include/compiler.h \
 arch/i386/include/bits/compiler.h libgcc/libgcc.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

libgcc/libgcc.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

$(BIN)/__moddi3.o : libgcc/__moddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__moddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/__moddi3.o
 
$(BIN)/__moddi3.dbg%.o : libgcc/__moddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__moddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/__moddi3.dbg%.o
 
$(BIN)/__moddi3.c : libgcc/__moddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__moddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/__moddi3.c
 
$(BIN)/__moddi3.s : libgcc/__moddi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__moddi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/__moddi3.s
 
bin/deps/libgcc/__moddi3.c.d : $(__moddi3_DEPS)
 
TAGS : $(__moddi3_DEPS)

