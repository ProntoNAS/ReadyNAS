pxe_errors_DEPS = arch/i386/interface/pxe/pxe_errors.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/errortab.h \
 include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/errortab.h:

include/gpxe/tables.h:

$(BIN)/pxe_errors.o : arch/i386/interface/pxe/pxe_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_errors.o
 
$(BIN)/pxe_errors.dbg%.o : arch/i386/interface/pxe/pxe_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_errors.dbg%.o
 
$(BIN)/pxe_errors.c : arch/i386/interface/pxe/pxe_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_errors.c
 
$(BIN)/pxe_errors.s : arch/i386/interface/pxe/pxe_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_errors.s
 
bin/deps/arch/i386/interface/pxe/pxe_errors.c.d : $(pxe_errors_DEPS)
 
TAGS : $(pxe_errors_DEPS)

