job_DEPS = core/job.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/string.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h arch/x86/include/bits/string.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/job.h include/gpxe/interface.h include/gpxe/refcnt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/job.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

$(BIN)/job.o : core/job.c $(MAKEDEPS) $(POST_O_DEPS) $(job_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/job.o
 
$(BIN)/job.dbg%.o : core/job.c $(MAKEDEPS) $(POST_O_DEPS) $(job_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/job.dbg%.o
 
$(BIN)/job.c : core/job.c $(MAKEDEPS) $(POST_O_DEPS) $(job_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/job.c
 
$(BIN)/job.s : core/job.c $(MAKEDEPS) $(POST_O_DEPS) $(job_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/job.s
 
bin/deps/core/job.c.d : $(job_DEPS)
 
TAGS : $(job_DEPS)

