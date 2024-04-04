asn1_DEPS = crypto/asn1.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/asn1.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/asn1.h:

$(BIN)/asn1.o : crypto/asn1.c $(MAKEDEPS) $(POST_O_DEPS) $(asn1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/asn1.o
 
$(BIN)/asn1.dbg%.o : crypto/asn1.c $(MAKEDEPS) $(POST_O_DEPS) $(asn1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/asn1.dbg%.o
 
$(BIN)/asn1.c : crypto/asn1.c $(MAKEDEPS) $(POST_O_DEPS) $(asn1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/asn1.c
 
$(BIN)/asn1.s : crypto/asn1.c $(MAKEDEPS) $(POST_O_DEPS) $(asn1_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/asn1.s
 
bin/deps/crypto/asn1.c.d : $(asn1_DEPS)
 
TAGS : $(asn1_DEPS)

