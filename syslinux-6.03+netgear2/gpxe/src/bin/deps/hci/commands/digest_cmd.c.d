digest_cmd_DEPS = hci/commands/digest_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdio.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/unistd.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/command.h include/gpxe/tables.h include/gpxe/image.h \
 include/gpxe/list.h include/assert.h include/gpxe/uaccess.h \
 config/ioapi.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/refcnt.h include/gpxe/crypto.h include/gpxe/md5.h \
 include/gpxe/sha1.h crypto/axtls/crypto.h crypto/axtls/bigint.h \
 crypto/axtls/os_port.h include/stdlib.h include/time.h \
 include/sys/time.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h crypto/axtls/bigint_impl.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/unistd.h:

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/gpxe/command.h:

include/gpxe/tables.h:

include/gpxe/image.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/uaccess.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/refcnt.h:

include/gpxe/crypto.h:

include/gpxe/md5.h:

include/gpxe/sha1.h:

crypto/axtls/crypto.h:

crypto/axtls/bigint.h:

crypto/axtls/os_port.h:

include/stdlib.h:

include/time.h:

include/sys/time.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

crypto/axtls/bigint_impl.h:

$(BIN)/digest_cmd.o : hci/commands/digest_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(digest_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/digest_cmd.o
 
$(BIN)/digest_cmd.dbg%.o : hci/commands/digest_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(digest_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/digest_cmd.dbg%.o
 
$(BIN)/digest_cmd.c : hci/commands/digest_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(digest_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/digest_cmd.c
 
$(BIN)/digest_cmd.s : hci/commands/digest_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(digest_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/digest_cmd.s
 
bin/deps/hci/commands/digest_cmd.c.d : $(digest_cmd_DEPS)
 
TAGS : $(digest_cmd_DEPS)

