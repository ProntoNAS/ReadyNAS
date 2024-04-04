pxeprefix_DEPS = arch/i386/prefix/pxeprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/undi.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/undi.h:

$(BIN)/pxeprefix.o : arch/i386/prefix/pxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(pxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/pxeprefix.o
 
$(BIN)/pxeprefix.s : arch/i386/prefix/pxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(pxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/pxeprefix.s
 
bin/deps/arch/i386/prefix/pxeprefix.S.d : $(pxeprefix_DEPS)
 
TAGS : $(pxeprefix_DEPS)

