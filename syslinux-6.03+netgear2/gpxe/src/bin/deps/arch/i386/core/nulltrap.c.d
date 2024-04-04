nulltrap_DEPS = arch/i386/core/nulltrap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

$(BIN)/nulltrap.o : arch/i386/core/nulltrap.c $(MAKEDEPS) $(POST_O_DEPS) $(nulltrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/nulltrap.o
 
$(BIN)/nulltrap.dbg%.o : arch/i386/core/nulltrap.c $(MAKEDEPS) $(POST_O_DEPS) $(nulltrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/nulltrap.dbg%.o
 
$(BIN)/nulltrap.c : arch/i386/core/nulltrap.c $(MAKEDEPS) $(POST_O_DEPS) $(nulltrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/nulltrap.c
 
$(BIN)/nulltrap.s : arch/i386/core/nulltrap.c $(MAKEDEPS) $(POST_O_DEPS) $(nulltrap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/nulltrap.s
 
bin/deps/arch/i386/core/nulltrap.c.d : $(nulltrap_DEPS)
 
TAGS : $(nulltrap_DEPS)

