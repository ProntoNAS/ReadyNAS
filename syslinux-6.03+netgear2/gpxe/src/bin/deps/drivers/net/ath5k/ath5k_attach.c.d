ath5k_attach_DEPS = drivers/net/ath5k/ath5k_attach.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/pci.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/device.h \
 include/gpxe/list.h include/stddef.h include/assert.h \
 include/gpxe/tables.h include/gpxe/pci_io.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h include/unistd.h \
 include/stdarg.h include/gpxe/timer.h config/timer.h \
 include/gpxe/efi/efi_timer.h arch/i386/include/bits/timer.h \
 arch/i386/include/gpxe/bios_timer.h arch/i386/include/gpxe/timer2.h \
 arch/i386/include/gpxe/rdtsc_timer.h include/stdlib.h \
 drivers/net/ath5k/ath5k.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/net80211.h include/gpxe/process.h \
 include/gpxe/ieee80211.h include/gpxe/if_ether.h include/endian.h \
 include/gpxe/iobuf.h include/gpxe/rc80211.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 drivers/net/ath5k/desc.h drivers/net/ath5k/eeprom.h \
 drivers/net/ath5k/reg.h drivers/net/ath5k/base.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/pci.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/unistd.h:

include/stdarg.h:

include/gpxe/timer.h:

config/timer.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

include/stdlib.h:

drivers/net/ath5k/ath5k.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

include/gpxe/iobuf.h:

include/gpxe/rc80211.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

drivers/net/ath5k/desc.h:

drivers/net/ath5k/eeprom.h:

drivers/net/ath5k/reg.h:

drivers/net/ath5k/base.h:

$(BIN)/ath5k_attach.o : drivers/net/ath5k/ath5k_attach.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_attach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ath5k_attach.o
 
$(BIN)/ath5k_attach.dbg%.o : drivers/net/ath5k/ath5k_attach.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_attach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ath5k_attach.dbg%.o
 
$(BIN)/ath5k_attach.c : drivers/net/ath5k/ath5k_attach.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_attach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ath5k_attach.c
 
$(BIN)/ath5k_attach.s : drivers/net/ath5k/ath5k_attach.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_attach_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ath5k_attach.s
 
bin/deps/drivers/net/ath5k/ath5k_attach.c.d : $(ath5k_attach_DEPS)
 
TAGS : $(ath5k_attach_DEPS)

