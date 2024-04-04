__divdi3_DEPS = libgcc/__divdi3.c include/compiler.h \
 arch/i386/include/bits/compiler.h libgcc/libgcc.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

libgcc/libgcc.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

$(BIN)/__divdi3.o : libgcc/__divdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__divdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/__divdi3.o
 
$(BIN)/__divdi3.dbg%.o : libgcc/__divdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__divdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/__divdi3.dbg%.o
 
$(BIN)/__divdi3.c : libgcc/__divdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__divdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/__divdi3.c
 
$(BIN)/__divdi3.s : libgcc/__divdi3.c $(MAKEDEPS) $(POST_O_DEPS) $(__divdi3_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/__divdi3.s
 
bin/deps/libgcc/__divdi3.c.d : $(__divdi3_DEPS)
 
TAGS : $(__divdi3_DEPS)

