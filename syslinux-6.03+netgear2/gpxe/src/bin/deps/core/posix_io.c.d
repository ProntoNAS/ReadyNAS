posix_io_DEPS = core/posix_io.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/list.h include/gpxe/xfer.h include/stdarg.h \
 include/gpxe/interface.h include/gpxe/refcnt.h include/gpxe/iobuf.h \
 include/gpxe/open.h include/gpxe/tables.h include/gpxe/socket.h \
 include/gpxe/process.h include/gpxe/posix_io.h include/gpxe/uaccess.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h

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

include/gpxe/list.h:

include/gpxe/xfer.h:

include/stdarg.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/process.h:

include/gpxe/posix_io.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/posix_io.o : core/posix_io.c $(MAKEDEPS) $(POST_O_DEPS) $(posix_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/posix_io.o
 
$(BIN)/posix_io.dbg%.o : core/posix_io.c $(MAKEDEPS) $(POST_O_DEPS) $(posix_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/posix_io.dbg%.o
 
$(BIN)/posix_io.c : core/posix_io.c $(MAKEDEPS) $(POST_O_DEPS) $(posix_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/posix_io.c
 
$(BIN)/posix_io.s : core/posix_io.c $(MAKEDEPS) $(POST_O_DEPS) $(posix_io_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/posix_io.s
 
bin/deps/core/posix_io.c.d : $(posix_io_DEPS)
 
TAGS : $(posix_io_DEPS)

