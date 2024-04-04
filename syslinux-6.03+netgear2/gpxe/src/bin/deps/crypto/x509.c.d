x509_DEPS = crypto/x509.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/asn1.h include/gpxe/x509.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/asn1.h:

include/gpxe/x509.h:

$(BIN)/x509.o : crypto/x509.c $(MAKEDEPS) $(POST_O_DEPS) $(x509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/x509.o
 
$(BIN)/x509.dbg%.o : crypto/x509.c $(MAKEDEPS) $(POST_O_DEPS) $(x509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/x509.dbg%.o
 
$(BIN)/x509.c : crypto/x509.c $(MAKEDEPS) $(POST_O_DEPS) $(x509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/x509.c
 
$(BIN)/x509.s : crypto/x509.c $(MAKEDEPS) $(POST_O_DEPS) $(x509_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/x509.s
 
bin/deps/crypto/x509.c.d : $(x509_DEPS)
 
TAGS : $(x509_DEPS)

