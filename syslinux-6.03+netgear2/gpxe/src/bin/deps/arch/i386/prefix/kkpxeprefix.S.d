kkpxeprefix_DEPS = arch/i386/prefix/kkpxeprefix.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/pxeprefix.S \
 arch/i386/include/undi.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/pxeprefix.S:

arch/i386/include/undi.h:

$(BIN)/kkpxeprefix.o : arch/i386/prefix/kkpxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(kkpxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/kkpxeprefix.o
 
$(BIN)/kkpxeprefix.s : arch/i386/prefix/kkpxeprefix.S $(MAKEDEPS) $(POST_O_DEPS) $(kkpxeprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/kkpxeprefix.s
 
bin/deps/arch/i386/prefix/kkpxeprefix.S.d : $(kkpxeprefix_DEPS)
 
TAGS : $(kkpxeprefix_DEPS)

