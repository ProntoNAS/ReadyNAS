__udivdi3_DEPS = libgcc/__udivdi3.c include/compiler.h \
 arch/i386/include/bits/compiler.h libgcc/libgcc.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

libgcc/libgcc.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

$(BIN)/__udivdi3.o : libgcc/__udivdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/__udivdi3.o
 
$(BIN)/__udivdi3.dbg%.o : libgcc/__udivdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/__udivdi3.dbg%.o
 
$(BIN)/__udivdi3.c : libgcc/__udivdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/__udivdi3.c
 
$(BIN)/__udivdi3.s : libgcc/__udivdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__udivdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/__udivdi3.s
 
bin/deps/libgcc/__udivdi3.c.d : $(__udivdi3_DEPS)
 
TAGS : $(__udivdi3_DEPS)

