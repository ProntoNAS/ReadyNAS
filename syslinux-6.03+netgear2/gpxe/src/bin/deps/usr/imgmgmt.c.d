imgmgmt_DEPS = usr/imgmgmt.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/image.h \
 include/gpxe/tables.h include/gpxe/list.h include/stddef.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/refcnt.h include/gpxe/downloader.h include/gpxe/monojob.h \
 include/gpxe/open.h include/gpxe/socket.h include/gpxe/uri.h \
 include/usr/imgmgmt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

include/gpxe/downloader.h:

include/gpxe/monojob.h:

include/gpxe/open.h:

include/gpxe/socket.h:

include/gpxe/uri.h:

include/usr/imgmgmt.h:

$(BIN)/imgmgmt.o : usr/imgmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(imgmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/imgmgmt.o
 
$(BIN)/imgmgmt.dbg%.o : usr/imgmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(imgmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/imgmgmt.dbg%.o
 
$(BIN)/imgmgmt.c : usr/imgmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(imgmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/imgmgmt.c
 
$(BIN)/imgmgmt.s : usr/imgmgmt.c $(MAKEDEPS) $(POST_O_DEPS) $(imgmgmt_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/imgmgmt.s
 
bin/deps/usr/imgmgmt.c.d : $(imgmgmt_DEPS)
 
TAGS : $(imgmgmt_DEPS)

