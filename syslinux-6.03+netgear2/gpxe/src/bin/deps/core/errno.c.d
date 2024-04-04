errno_DEPS = core/errno.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

$(BIN)/errno.o : core/errno.c $(MAKEDEPS) $(POST_O_DEPS) $(errno_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/errno.o
 
$(BIN)/errno.dbg%.o : core/errno.c $(MAKEDEPS) $(POST_O_DEPS) $(errno_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/errno.dbg%.o
 
$(BIN)/errno.c : core/errno.c $(MAKEDEPS) $(POST_O_DEPS) $(errno_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/errno.c
 
$(BIN)/errno.s : core/errno.c $(MAKEDEPS) $(POST_O_DEPS) $(errno_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/errno.s
 
bin/deps/core/errno.c.d : $(errno_DEPS)
 
TAGS : $(errno_DEPS)

