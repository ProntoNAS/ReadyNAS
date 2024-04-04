memcpy_DEPS = libgcc/memcpy.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

$(BIN)/memcpy.o : libgcc/memcpy.c $(MAKEDEPS) $(POST_O_DEPS) $(memcpy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/memcpy.o
 
$(BIN)/memcpy.dbg%.o : libgcc/memcpy.c $(MAKEDEPS) $(POST_O_DEPS) $(memcpy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/memcpy.dbg%.o
 
$(BIN)/memcpy.c : libgcc/memcpy.c $(MAKEDEPS) $(POST_O_DEPS) $(memcpy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/memcpy.c
 
$(BIN)/memcpy.s : libgcc/memcpy.c $(MAKEDEPS) $(POST_O_DEPS) $(memcpy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/memcpy.s
 
bin/deps/libgcc/memcpy.c.d : $(memcpy_DEPS)
 
TAGS : $(memcpy_DEPS)

