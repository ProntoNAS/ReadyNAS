wireless_errors_DEPS = hci/wireless_errors.c include/compiler.h \
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

$(BIN)/wireless_errors.o : hci/wireless_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(wireless_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/wireless_errors.o
 
$(BIN)/wireless_errors.dbg%.o : hci/wireless_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(wireless_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/wireless_errors.dbg%.o
 
$(BIN)/wireless_errors.c : hci/wireless_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(wireless_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/wireless_errors.c
 
$(BIN)/wireless_errors.s : hci/wireless_errors.c $(MAKEDEPS) $(POST_O_DEPS) $(wireless_errors_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/wireless_errors.s
 
bin/deps/hci/wireless_errors.c.d : $(wireless_errors_DEPS)
 
TAGS : $(wireless_errors_DEPS)

