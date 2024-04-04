rtl8180_sa2400_DEPS = drivers/net/rtl818x/rtl8180_sa2400.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/unistd.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdarg.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h include/gpxe/pci_io.h \
 config/ioapi.h include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/net80211.h include/gpxe/process.h include/gpxe/refcnt.h \
 include/gpxe/ieee80211.h include/gpxe/if_ether.h include/endian.h \
 arch/i386/include/bits/endian.h include/gpxe/iobuf.h \
 include/gpxe/netdevice.h include/gpxe/settings.h include/gpxe/rc80211.h \
 drivers/net/rtl818x/rtl818x.h include/gpxe/spi_bit.h include/gpxe/spi.h \
 include/gpxe/nvs.h include/gpxe/bitbash.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/unistd.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

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

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

config/ioapi.h:

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

include/gpxe/net80211.h:

include/gpxe/process.h:

include/gpxe/refcnt.h:

include/gpxe/ieee80211.h:

include/gpxe/if_ether.h:

include/endian.h:

arch/i386/include/bits/endian.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

include/gpxe/rc80211.h:

drivers/net/rtl818x/rtl818x.h:

include/gpxe/spi_bit.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

include/gpxe/bitbash.h:

$(BIN)/rtl8180_sa2400.o : drivers/net/rtl818x/rtl8180_sa2400.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_sa2400_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/rtl8180_sa2400.o
 
$(BIN)/rtl8180_sa2400.dbg%.o : drivers/net/rtl818x/rtl8180_sa2400.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_sa2400_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/rtl8180_sa2400.dbg%.o
 
$(BIN)/rtl8180_sa2400.c : drivers/net/rtl818x/rtl8180_sa2400.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_sa2400_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/rtl8180_sa2400.c
 
$(BIN)/rtl8180_sa2400.s : drivers/net/rtl818x/rtl8180_sa2400.c $(MAKEDEPS) $(POST_O_DEPS) $(rtl8180_sa2400_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/rtl8180_sa2400.s
 
bin/deps/drivers/net/rtl818x/rtl8180_sa2400.c.d : $(rtl8180_sa2400_DEPS)
 
TAGS : $(rtl8180_sa2400_DEPS)

