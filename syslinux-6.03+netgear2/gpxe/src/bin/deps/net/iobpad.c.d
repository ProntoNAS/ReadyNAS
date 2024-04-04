iobpad_DEPS = net/iobpad.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/iobuf.h include/assert.h \
 include/gpxe/list.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

$(BIN)/iobpad.o : net/iobpad.c $(MAKEDEPS) $(POST_O_DEPS) $(iobpad_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iobpad.o
 
$(BIN)/iobpad.dbg%.o : net/iobpad.c $(MAKEDEPS) $(POST_O_DEPS) $(iobpad_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iobpad.dbg%.o
 
$(BIN)/iobpad.c : net/iobpad.c $(MAKEDEPS) $(POST_O_DEPS) $(iobpad_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iobpad.c
 
$(BIN)/iobpad.s : net/iobpad.c $(MAKEDEPS) $(POST_O_DEPS) $(iobpad_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iobpad.s
 
bin/deps/net/iobpad.c.d : $(iobpad_DEPS)
 
TAGS : $(iobpad_DEPS)

