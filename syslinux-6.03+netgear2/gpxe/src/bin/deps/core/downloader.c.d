downloader_DEPS = core/downloader.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/stdarg.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/xfer.h include/stddef.h include/gpxe/interface.h \
 include/gpxe/refcnt.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/open.h include/gpxe/tables.h include/gpxe/socket.h \
 include/gpxe/job.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/umalloc.h config/umalloc.h \
 include/gpxe/efi/efi_umalloc.h arch/i386/include/bits/umalloc.h \
 arch/i386/include/gpxe/memtop_umalloc.h include/gpxe/image.h \
 include/gpxe/downloader.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/xfer.h:

include/stddef.h:

include/gpxe/interface.h:

include/gpxe/refcnt.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/open.h:

include/gpxe/tables.h:

include/gpxe/socket.h:

include/gpxe/job.h:

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

include/gpxe/umalloc.h:

config/umalloc.h:

include/gpxe/efi/efi_umalloc.h:

arch/i386/include/bits/umalloc.h:

arch/i386/include/gpxe/memtop_umalloc.h:

include/gpxe/image.h:

include/gpxe/downloader.h:

$(BIN)/downloader.o : core/downloader.c $(MAKEDEPS) $(POST_O_DEPS) $(downloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/downloader.o
 
$(BIN)/downloader.dbg%.o : core/downloader.c $(MAKEDEPS) $(POST_O_DEPS) $(downloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/downloader.dbg%.o
 
$(BIN)/downloader.c : core/downloader.c $(MAKEDEPS) $(POST_O_DEPS) $(downloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/downloader.c
 
$(BIN)/downloader.s : core/downloader.c $(MAKEDEPS) $(POST_O_DEPS) $(downloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/downloader.s
 
bin/deps/core/downloader.c.d : $(downloader_DEPS)
 
TAGS : $(downloader_DEPS)

