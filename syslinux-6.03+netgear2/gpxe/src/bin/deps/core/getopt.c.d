getopt_DEPS = core/getopt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/getopt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/getopt.h:

$(BIN)/getopt.o : core/getopt.c $(MAKEDEPS) $(POST_O_DEPS) $(getopt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/getopt.o
 
$(BIN)/getopt.dbg%.o : core/getopt.c $(MAKEDEPS) $(POST_O_DEPS) $(getopt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/getopt.dbg%.o
 
$(BIN)/getopt.c : core/getopt.c $(MAKEDEPS) $(POST_O_DEPS) $(getopt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/getopt.c
 
$(BIN)/getopt.s : core/getopt.c $(MAKEDEPS) $(POST_O_DEPS) $(getopt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/getopt.s
 
bin/deps/core/getopt.c.d : $(getopt_DEPS)
 
TAGS : $(getopt_DEPS)

