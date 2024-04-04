scsi_DEPS = drivers/block/scsi.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/string.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/blockdev.h include/gpxe/uaccess.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/process.h include/gpxe/list.h \
 include/gpxe/refcnt.h include/gpxe/tables.h include/gpxe/scsi.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/string.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/process.h:

include/gpxe/list.h:

include/gpxe/refcnt.h:

include/gpxe/tables.h:

include/gpxe/scsi.h:

$(BIN)/scsi.o : drivers/block/scsi.c $(MAKEDEPS) $(POST_O_DEPS) $(scsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/scsi.o
 
$(BIN)/scsi.dbg%.o : drivers/block/scsi.c $(MAKEDEPS) $(POST_O_DEPS) $(scsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/scsi.dbg%.o
 
$(BIN)/scsi.c : drivers/block/scsi.c $(MAKEDEPS) $(POST_O_DEPS) $(scsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/scsi.c
 
$(BIN)/scsi.s : drivers/block/scsi.c $(MAKEDEPS) $(POST_O_DEPS) $(scsi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/scsi.s
 
bin/deps/drivers/block/scsi.c.d : $(scsi_DEPS)
 
TAGS : $(scsi_DEPS)

