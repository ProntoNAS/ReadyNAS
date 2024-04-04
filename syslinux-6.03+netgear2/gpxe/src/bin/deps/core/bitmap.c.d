bitmap_DEPS = core/bitmap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/bitmap.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stddef.h include/stdlib.h \
 include/assert.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/bitmap.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stddef.h:

include/stdlib.h:

include/assert.h:

$(BIN)/bitmap.o : core/bitmap.c $(MAKEDEPS) $(POST_O_DEPS) $(bitmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bitmap.o
 
$(BIN)/bitmap.dbg%.o : core/bitmap.c $(MAKEDEPS) $(POST_O_DEPS) $(bitmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bitmap.dbg%.o
 
$(BIN)/bitmap.c : core/bitmap.c $(MAKEDEPS) $(POST_O_DEPS) $(bitmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bitmap.c
 
$(BIN)/bitmap.s : core/bitmap.c $(MAKEDEPS) $(POST_O_DEPS) $(bitmap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bitmap.s
 
bin/deps/core/bitmap.c.d : $(bitmap_DEPS)
 
TAGS : $(bitmap_DEPS)

