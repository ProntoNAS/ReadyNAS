e1000_hw_DEPS = drivers/net/e1000/e1000_hw.c include/compiler.h \
 arch/i386/include/bits/compiler.h drivers/net/e1000/e1000_hw.h \
 drivers/net/e1000/e1000_osdep.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/gpxe/io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/uaccess.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/unistd.h include/gpxe/timer.h \
 config/timer.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/if_ether.h include/gpxe/ethernet.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

drivers/net/e1000/e1000_hw.h:

drivers/net/e1000/e1000_osdep.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/unistd.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

$(BIN)/e1000_hw.o : drivers/net/e1000/e1000_hw.c $(MAKEDEPS) $(POST_O_DEPS) $(e1000_hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/e1000_hw.o
 
$(BIN)/e1000_hw.dbg%.o : drivers/net/e1000/e1000_hw.c $(MAKEDEPS) $(POST_O_DEPS) $(e1000_hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/e1000_hw.dbg%.o
 
$(BIN)/e1000_hw.c : drivers/net/e1000/e1000_hw.c $(MAKEDEPS) $(POST_O_DEPS) $(e1000_hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/e1000_hw.c
 
$(BIN)/e1000_hw.s : drivers/net/e1000/e1000_hw.c $(MAKEDEPS) $(POST_O_DEPS) $(e1000_hw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/e1000_hw.s
 
bin/deps/drivers/net/e1000/e1000_hw.c.d : $(e1000_hw_DEPS)
 
TAGS : $(e1000_hw_DEPS)

