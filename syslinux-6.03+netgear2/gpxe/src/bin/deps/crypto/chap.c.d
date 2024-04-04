chap_DEPS = crypto/chap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/crypto.h include/gpxe/chap.h include/gpxe/md5.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/crypto.h:

include/gpxe/chap.h:

include/gpxe/md5.h:

$(BIN)/chap.o : crypto/chap.c $(MAKEDEPS) $(POST_O_DEPS) $(chap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/chap.o
 
$(BIN)/chap.dbg%.o : crypto/chap.c $(MAKEDEPS) $(POST_O_DEPS) $(chap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/chap.dbg%.o
 
$(BIN)/chap.c : crypto/chap.c $(MAKEDEPS) $(POST_O_DEPS) $(chap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/chap.c
 
$(BIN)/chap.s : crypto/chap.c $(MAKEDEPS) $(POST_O_DEPS) $(chap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/chap.s
 
bin/deps/crypto/chap.c.d : $(chap_DEPS)
 
TAGS : $(chap_DEPS)

