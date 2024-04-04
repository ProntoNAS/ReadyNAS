kpxeprefix_DEPS = arch/i386/prefix/kpxeprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/pxeprefix.S \
 arch/i386/include/undi.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/pxeprefix.S:

arch/i386/include/undi.h:

$(BIN)/kpxeprefix.o : arch/i386/prefix/kpxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(kpxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/kpxeprefix.o
 
$(BIN)/kpxeprefix.s : arch/i386/prefix/kpxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(kpxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/kpxeprefix.s
 
bin/deps/arch/i386/prefix/kpxeprefix.S.d : $(kpxeprefix_DEPS)
 
TAGS : $(kpxeprefix_DEPS)

