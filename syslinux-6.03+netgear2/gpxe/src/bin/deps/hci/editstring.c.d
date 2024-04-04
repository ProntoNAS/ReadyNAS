editstring_DEPS = hci/editstring.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/assert.h include/string.h \
 include/stddef.h include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/keys.h \
 include/gpxe/editstring.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/assert.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/keys.h:

include/gpxe/editstring.h:

$(BIN)/editstring.o : hci/editstring.c $(MAKEDEPS) $(POST_O_DEPS) $(editstring_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/editstring.o
 
$(BIN)/editstring.dbg%.o : hci/editstring.c $(MAKEDEPS) $(POST_O_DEPS) $(editstring_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/editstring.dbg%.o
 
$(BIN)/editstring.c : hci/editstring.c $(MAKEDEPS) $(POST_O_DEPS) $(editstring_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/editstring.c
 
$(BIN)/editstring.s : hci/editstring.c $(MAKEDEPS) $(POST_O_DEPS) $(editstring_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/editstring.s
 
bin/deps/hci/editstring.c.d : $(editstring_DEPS)
 
TAGS : $(editstring_DEPS)

