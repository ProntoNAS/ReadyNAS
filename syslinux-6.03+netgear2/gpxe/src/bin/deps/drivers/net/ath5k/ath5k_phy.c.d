ath5k_phy_DEPS = drivers/net/ath5k/ath5k_phy.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/unistd.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdarg.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/stdlib.h include/assert.h drivers/net/ath5k/ath5k.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/io.h config/ioapi.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/netdevice.h \
 include/gpxe/list.h include/gpxe/tables.h include/gpxe/refcnt.h \
 include/gpxe/settings.h include/gpxe/net80211.h include/gpxe/process.h \
 include/gpxe/ieee80211.h include/gpxe/if_ether.h include/endian.h \
 include/gpxe/iobuf.h include/gpxe/rc80211.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 drivers/net/ath5k/desc.h drivers/net/ath5k/eeprom.h \
 drivers/net/ath5k/reg.h drivers/net/ath5k/base.h \
 drivers/net/ath5k/rfbuffer.h drivers/net/ath5k/rfgain.h

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

include/stdlib.h:

include/assert.h:

drivers/net/ath5k/ath5k.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/gpxe/tables.h:

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

drivers/net/ath5k/rfbuffer.h:

drivers/net/ath5k/rfgain.h:

$(BIN)/ath5k_phy.o : drivers/net/ath5k/ath5k_phy.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_phy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ath5k_phy.o
 
$(BIN)/ath5k_phy.dbg%.o : drivers/net/ath5k/ath5k_phy.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_phy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ath5k_phy.dbg%.o
 
$(BIN)/ath5k_phy.c : drivers/net/ath5k/ath5k_phy.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_phy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ath5k_phy.c
 
$(BIN)/ath5k_phy.s : drivers/net/ath5k/ath5k_phy.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_phy_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ath5k_phy.s
 
bin/deps/drivers/net/ath5k/ath5k_phy.c.d : $(ath5k_phy_DEPS)
 
TAGS : $(ath5k_phy_DEPS)

