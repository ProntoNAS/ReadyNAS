strerror_DEPS = hci/strerror.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/stdio.h include/stdarg.h \
 include/gpxe/errortab.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/errortab.h:

include/gpxe/tables.h:

$(BIN)/strerror.o : hci/strerror.c $(MAKEDEPS) $(POST_O_DEPS) $(strerror_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/strerror.o
 
$(BIN)/strerror.dbg%.o : hci/strerror.c $(MAKEDEPS) $(POST_O_DEPS) $(strerror_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/strerror.dbg%.o
 
$(BIN)/strerror.c : hci/strerror.c $(MAKEDEPS) $(POST_O_DEPS) $(strerror_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/strerror.c
 
$(BIN)/strerror.s : hci/strerror.c $(MAKEDEPS) $(POST_O_DEPS) $(strerror_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/strerror.s
 
bin/deps/hci/strerror.c.d : $(strerror_DEPS)
 
TAGS : $(strerror_DEPS)

