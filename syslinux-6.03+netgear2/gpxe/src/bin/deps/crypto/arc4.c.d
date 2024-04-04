arc4_DEPS = crypto/arc4.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/crypto.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/gpxe/arc4.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/crypto.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/gpxe/arc4.h:

$(BIN)/arc4.o : crypto/arc4.c $(MAKEDEPS) $(POST_O_DEPS) $(arc4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/arc4.o
 
$(BIN)/arc4.dbg%.o : crypto/arc4.c $(MAKEDEPS) $(POST_O_DEPS) $(arc4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/arc4.dbg%.o
 
$(BIN)/arc4.c : crypto/arc4.c $(MAKEDEPS) $(POST_O_DEPS) $(arc4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/arc4.c
 
$(BIN)/arc4.s : crypto/arc4.c $(MAKEDEPS) $(POST_O_DEPS) $(arc4_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/arc4.s
 
bin/deps/crypto/arc4.c.d : $(arc4_DEPS)
 
TAGS : $(arc4_DEPS)

