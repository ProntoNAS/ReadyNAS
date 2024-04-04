iscsiboot_DEPS = arch/i386/interface/pcbios/iscsiboot.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/iscsi.h \
 include/gpxe/socket.h include/gpxe/scsi.h include/gpxe/blockdev.h \
 include/gpxe/uaccess.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/refcnt.h include/gpxe/chap.h \
 include/gpxe/md5.h include/gpxe/xfer.h include/gpxe/interface.h \
 include/gpxe/iobuf.h include/gpxe/list.h include/gpxe/process.h \
 include/gpxe/tables.h include/gpxe/netdevice.h include/gpxe/settings.h \
 arch/i386/include/gpxe/ibft.h include/gpxe/acpi.h include/gpxe/in.h \
 include/gpxe/sanboot.h arch/i386/include/int13.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/iscsi.h:

include/gpxe/socket.h:

include/gpxe/scsi.h:

include/gpxe/blockdev.h:

include/gpxe/uaccess.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

include/gpxe/chap.h:

include/gpxe/md5.h:

include/gpxe/xfer.h:

include/gpxe/interface.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/process.h:

include/gpxe/tables.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

arch/i386/include/gpxe/ibft.h:

include/gpxe/acpi.h:

include/gpxe/in.h:

include/gpxe/sanboot.h:

arch/i386/include/int13.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

$(BIN)/iscsiboot.o : arch/i386/interface/pcbios/iscsiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/iscsiboot.o
 
$(BIN)/iscsiboot.dbg%.o : arch/i386/interface/pcbios/iscsiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/iscsiboot.dbg%.o
 
$(BIN)/iscsiboot.c : arch/i386/interface/pcbios/iscsiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/iscsiboot.c
 
$(BIN)/iscsiboot.s : arch/i386/interface/pcbios/iscsiboot.c $(MAKEDEPS) $(POST_O_DEPS) $(iscsiboot_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/iscsiboot.s
 
bin/deps/arch/i386/interface/pcbios/iscsiboot.c.d : $(iscsiboot_DEPS)
 
TAGS : $(iscsiboot_DEPS)

