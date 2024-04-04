ath5k_DEPS = drivers/net/ath5k/ath5k.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/gpxe/malloc.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/stddef.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/pci_io.h \
 config/ioapi.h include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 drivers/net/ath5k/base.h drivers/net/ath5k/ath5k.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/net80211.h include/gpxe/process.h include/gpxe/ieee80211.h \
 include/gpxe/if_ether.h include/endian.h include/gpxe/iobuf.h \
 include/gpxe/rc80211.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h drivers/net/ath5k/desc.h \
 drivers/net/ath5k/eeprom.h drivers/net/ath5k/reg.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/malloc.h:

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

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/stddef.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/pci.h:

include/gpxe/device.h:

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

drivers/net/ath5k/base.h:

drivers/net/ath5k/ath5k.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

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

$(BIN)/ath5k.o : drivers/net/ath5k/ath5k.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/ath5k.o
 
$(BIN)/ath5k.dbg%.o : drivers/net/ath5k/ath5k.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/ath5k.dbg%.o
 
$(BIN)/ath5k.c : drivers/net/ath5k/ath5k.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/ath5k.c
 
$(BIN)/ath5k.s : drivers/net/ath5k/ath5k.c $(MAKEDEPS) $(POST_O_DEPS) $(ath5k_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/ath5k.s
 
bin/deps/drivers/net/ath5k/ath5k.c.d : $(ath5k_DEPS)
 
TAGS : $(ath5k_DEPS)


# NIC	
# NIC	family	drivers/net/ath5k/ath5k
DRIVERS += ath5k

# NIC	ath5210e	168c,0207	Atheros 5210 early
DRIVER_ath5210e = ath5k
ROM_TYPE_ath5210e = pci
ROM_DESCRIPTION_ath5210e = "Atheros 5210 early"
PCI_VENDOR_ath5210e = 0x168c
PCI_DEVICE_ath5210e = 0x0207
ROMS += ath5210e
ROMS_ath5k += ath5210e

# NIC	168c0207	168c,0207	Atheros 5210 early
DRIVER_168c0207 = ath5k
ROM_TYPE_168c0207 = pci
ROM_DESCRIPTION_168c0207 = "Atheros 5210 early"
PCI_VENDOR_168c0207 = 0x168c
PCI_DEVICE_168c0207 = 0x0207
ROMS += 168c0207
ROMS_ath5k += 168c0207

# NIC	ath5210	168c,0007	Atheros 5210
DRIVER_ath5210 = ath5k
ROM_TYPE_ath5210 = pci
ROM_DESCRIPTION_ath5210 = "Atheros 5210"
PCI_VENDOR_ath5210 = 0x168c
PCI_DEVICE_ath5210 = 0x0007
ROMS += ath5210
ROMS_ath5k += ath5210

# NIC	168c0007	168c,0007	Atheros 5210
DRIVER_168c0007 = ath5k
ROM_TYPE_168c0007 = pci
ROM_DESCRIPTION_168c0007 = "Atheros 5210"
PCI_VENDOR_168c0007 = 0x168c
PCI_DEVICE_168c0007 = 0x0007
ROMS += 168c0007
ROMS_ath5k += 168c0007

# NIC	ath5311	168c,0011	Atheros 5311 (AHB)
DRIVER_ath5311 = ath5k
ROM_TYPE_ath5311 = pci
ROM_DESCRIPTION_ath5311 = "Atheros 5311 (AHB)"
PCI_VENDOR_ath5311 = 0x168c
PCI_DEVICE_ath5311 = 0x0011
ROMS += ath5311
ROMS_ath5k += ath5311

# NIC	168c0011	168c,0011	Atheros 5311 (AHB)
DRIVER_168c0011 = ath5k
ROM_TYPE_168c0011 = pci
ROM_DESCRIPTION_168c0011 = "Atheros 5311 (AHB)"
PCI_VENDOR_168c0011 = 0x168c
PCI_DEVICE_168c0011 = 0x0011
ROMS += 168c0011
ROMS_ath5k += 168c0011

# NIC	ath5211	168c,0012	Atheros 5211
DRIVER_ath5211 = ath5k
ROM_TYPE_ath5211 = pci
ROM_DESCRIPTION_ath5211 = "Atheros 5211"
PCI_VENDOR_ath5211 = 0x168c
PCI_DEVICE_ath5211 = 0x0012
ROMS += ath5211
ROMS_ath5k += ath5211

# NIC	168c0012	168c,0012	Atheros 5211
DRIVER_168c0012 = ath5k
ROM_TYPE_168c0012 = pci
ROM_DESCRIPTION_168c0012 = "Atheros 5211"
PCI_VENDOR_168c0012 = 0x168c
PCI_DEVICE_168c0012 = 0x0012
ROMS += 168c0012
ROMS_ath5k += 168c0012

# NIC	ath5212	168c,0013	Atheros 5212
DRIVER_ath5212 = ath5k
ROM_TYPE_ath5212 = pci
ROM_DESCRIPTION_ath5212 = "Atheros 5212"
PCI_VENDOR_ath5212 = 0x168c
PCI_DEVICE_ath5212 = 0x0013
ROMS += ath5212
ROMS_ath5k += ath5212

# NIC	168c0013	168c,0013	Atheros 5212
DRIVER_168c0013 = ath5k
ROM_TYPE_168c0013 = pci
ROM_DESCRIPTION_168c0013 = "Atheros 5212"
PCI_VENDOR_168c0013 = 0x168c
PCI_DEVICE_168c0013 = 0x0013
ROMS += 168c0013
ROMS_ath5k += 168c0013

# NIC	ath5212c	a727,0013	3com Ath 5212
DRIVER_ath5212c = ath5k
ROM_TYPE_ath5212c = pci
ROM_DESCRIPTION_ath5212c = "3com Ath 5212"
PCI_VENDOR_ath5212c = 0xa727
PCI_DEVICE_ath5212c = 0x0013
ROMS += ath5212c
ROMS_ath5k += ath5212c

# NIC	a7270013	a727,0013	3com Ath 5212
DRIVER_a7270013 = ath5k
ROM_TYPE_a7270013 = pci
ROM_DESCRIPTION_a7270013 = "3com Ath 5212"
PCI_VENDOR_a7270013 = 0xa727
PCI_DEVICE_a7270013 = 0x0013
ROMS += a7270013
ROMS_ath5k += a7270013

# NIC	rdag675	10b7,0013	3com 3CRDAG675
DRIVER_rdag675 = ath5k
ROM_TYPE_rdag675 = pci
ROM_DESCRIPTION_rdag675 = "3com 3CRDAG675"
PCI_VENDOR_rdag675 = 0x10b7
PCI_DEVICE_rdag675 = 0x0013
ROMS += rdag675
ROMS_ath5k += rdag675

# NIC	10b70013	10b7,0013	3com 3CRDAG675
DRIVER_10b70013 = ath5k
ROM_TYPE_10b70013 = pci
ROM_DESCRIPTION_10b70013 = "3com 3CRDAG675"
PCI_VENDOR_10b70013 = 0x10b7
PCI_DEVICE_10b70013 = 0x0013
ROMS += 10b70013
ROMS_ath5k += 10b70013

# NIC	ath5212m	168c,1014	Ath 5212 miniPCI
DRIVER_ath5212m = ath5k
ROM_TYPE_ath5212m = pci
ROM_DESCRIPTION_ath5212m = "Ath 5212 miniPCI"
PCI_VENDOR_ath5212m = 0x168c
PCI_DEVICE_ath5212m = 0x1014
ROMS += ath5212m
ROMS_ath5k += ath5212m

# NIC	168c1014	168c,1014	Ath 5212 miniPCI
DRIVER_168c1014 = ath5k
ROM_TYPE_168c1014 = pci
ROM_DESCRIPTION_168c1014 = "Ath 5212 miniPCI"
PCI_VENDOR_168c1014 = 0x168c
PCI_DEVICE_168c1014 = 0x1014
ROMS += 168c1014
ROMS_ath5k += 168c1014

# NIC	ath5212x14	168c,0014	Atheros 5212 x14
DRIVER_ath5212x14 = ath5k
ROM_TYPE_ath5212x14 = pci
ROM_DESCRIPTION_ath5212x14 = "Atheros 5212 x14"
PCI_VENDOR_ath5212x14 = 0x168c
PCI_DEVICE_ath5212x14 = 0x0014
ROMS += ath5212x14
ROMS_ath5k += ath5212x14

# NIC	168c0014	168c,0014	Atheros 5212 x14
DRIVER_168c0014 = ath5k
ROM_TYPE_168c0014 = pci
ROM_DESCRIPTION_168c0014 = "Atheros 5212 x14"
PCI_VENDOR_168c0014 = 0x168c
PCI_DEVICE_168c0014 = 0x0014
ROMS += 168c0014
ROMS_ath5k += 168c0014

# NIC	ath5212x15	168c,0015	Atheros 5212 x15
DRIVER_ath5212x15 = ath5k
ROM_TYPE_ath5212x15 = pci
ROM_DESCRIPTION_ath5212x15 = "Atheros 5212 x15"
PCI_VENDOR_ath5212x15 = 0x168c
PCI_DEVICE_ath5212x15 = 0x0015
ROMS += ath5212x15
ROMS_ath5k += ath5212x15

# NIC	168c0015	168c,0015	Atheros 5212 x15
DRIVER_168c0015 = ath5k
ROM_TYPE_168c0015 = pci
ROM_DESCRIPTION_168c0015 = "Atheros 5212 x15"
PCI_VENDOR_168c0015 = 0x168c
PCI_DEVICE_168c0015 = 0x0015
ROMS += 168c0015
ROMS_ath5k += 168c0015

# NIC	ath5212x16	168c,0016	Atheros 5212 x16
DRIVER_ath5212x16 = ath5k
ROM_TYPE_ath5212x16 = pci
ROM_DESCRIPTION_ath5212x16 = "Atheros 5212 x16"
PCI_VENDOR_ath5212x16 = 0x168c
PCI_DEVICE_ath5212x16 = 0x0016
ROMS += ath5212x16
ROMS_ath5k += ath5212x16

# NIC	168c0016	168c,0016	Atheros 5212 x16
DRIVER_168c0016 = ath5k
ROM_TYPE_168c0016 = pci
ROM_DESCRIPTION_168c0016 = "Atheros 5212 x16"
PCI_VENDOR_168c0016 = 0x168c
PCI_DEVICE_168c0016 = 0x0016
ROMS += 168c0016
ROMS_ath5k += 168c0016

# NIC	ath5212x17	168c,0017	Atheros 5212 x17
DRIVER_ath5212x17 = ath5k
ROM_TYPE_ath5212x17 = pci
ROM_DESCRIPTION_ath5212x17 = "Atheros 5212 x17"
PCI_VENDOR_ath5212x17 = 0x168c
PCI_DEVICE_ath5212x17 = 0x0017
ROMS += ath5212x17
ROMS_ath5k += ath5212x17

# NIC	168c0017	168c,0017	Atheros 5212 x17
DRIVER_168c0017 = ath5k
ROM_TYPE_168c0017 = pci
ROM_DESCRIPTION_168c0017 = "Atheros 5212 x17"
PCI_VENDOR_168c0017 = 0x168c
PCI_DEVICE_168c0017 = 0x0017
ROMS += 168c0017
ROMS_ath5k += 168c0017

# NIC	ath5212x18	168c,0018	Atheros 5212 x18
DRIVER_ath5212x18 = ath5k
ROM_TYPE_ath5212x18 = pci
ROM_DESCRIPTION_ath5212x18 = "Atheros 5212 x18"
PCI_VENDOR_ath5212x18 = 0x168c
PCI_DEVICE_ath5212x18 = 0x0018
ROMS += ath5212x18
ROMS_ath5k += ath5212x18

# NIC	168c0018	168c,0018	Atheros 5212 x18
DRIVER_168c0018 = ath5k
ROM_TYPE_168c0018 = pci
ROM_DESCRIPTION_168c0018 = "Atheros 5212 x18"
PCI_VENDOR_168c0018 = 0x168c
PCI_DEVICE_168c0018 = 0x0018
ROMS += 168c0018
ROMS_ath5k += 168c0018

# NIC	ath5212x19	168c,0019	Atheros 5212 x19
DRIVER_ath5212x19 = ath5k
ROM_TYPE_ath5212x19 = pci
ROM_DESCRIPTION_ath5212x19 = "Atheros 5212 x19"
PCI_VENDOR_ath5212x19 = 0x168c
PCI_DEVICE_ath5212x19 = 0x0019
ROMS += ath5212x19
ROMS_ath5k += ath5212x19

# NIC	168c0019	168c,0019	Atheros 5212 x19
DRIVER_168c0019 = ath5k
ROM_TYPE_168c0019 = pci
ROM_DESCRIPTION_168c0019 = "Atheros 5212 x19"
PCI_VENDOR_168c0019 = 0x168c
PCI_DEVICE_168c0019 = 0x0019
ROMS += 168c0019
ROMS_ath5k += 168c0019

# NIC	ath2413	168c,001a	Atheros 2413 Griffin
DRIVER_ath2413 = ath5k
ROM_TYPE_ath2413 = pci
ROM_DESCRIPTION_ath2413 = "Atheros 2413 Griffin"
PCI_VENDOR_ath2413 = 0x168c
PCI_DEVICE_ath2413 = 0x001a
ROMS += ath2413
ROMS_ath5k += ath2413

# NIC	168c001a	168c,001a	Atheros 2413 Griffin
DRIVER_168c001a = ath5k
ROM_TYPE_168c001a = pci
ROM_DESCRIPTION_168c001a = "Atheros 2413 Griffin"
PCI_VENDOR_168c001a = 0x168c
PCI_DEVICE_168c001a = 0x001a
ROMS += 168c001a
ROMS_ath5k += 168c001a

# NIC	ath5413	168c,001b	Atheros 5413 Eagle
DRIVER_ath5413 = ath5k
ROM_TYPE_ath5413 = pci
ROM_DESCRIPTION_ath5413 = "Atheros 5413 Eagle"
PCI_VENDOR_ath5413 = 0x168c
PCI_DEVICE_ath5413 = 0x001b
ROMS += ath5413
ROMS_ath5k += ath5413

# NIC	168c001b	168c,001b	Atheros 5413 Eagle
DRIVER_168c001b = ath5k
ROM_TYPE_168c001b = pci
ROM_DESCRIPTION_168c001b = "Atheros 5413 Eagle"
PCI_VENDOR_168c001b = 0x168c
PCI_DEVICE_168c001b = 0x001b
ROMS += 168c001b
ROMS_ath5k += 168c001b

# NIC	ath5212e	168c,001c	Atheros 5212 PCI-E
DRIVER_ath5212e = ath5k
ROM_TYPE_ath5212e = pci
ROM_DESCRIPTION_ath5212e = "Atheros 5212 PCI-E"
PCI_VENDOR_ath5212e = 0x168c
PCI_DEVICE_ath5212e = 0x001c
ROMS += ath5212e
ROMS_ath5k += ath5212e

# NIC	168c001c	168c,001c	Atheros 5212 PCI-E
DRIVER_168c001c = ath5k
ROM_TYPE_168c001c = pci
ROM_DESCRIPTION_168c001c = "Atheros 5212 PCI-E"
PCI_VENDOR_168c001c = 0x168c
PCI_DEVICE_168c001c = 0x001c
ROMS += 168c001c
ROMS_ath5k += 168c001c

# NIC	ath2417	168c,001d	Atheros 2417 Nala
DRIVER_ath2417 = ath5k
ROM_TYPE_ath2417 = pci
ROM_DESCRIPTION_ath2417 = "Atheros 2417 Nala"
PCI_VENDOR_ath2417 = 0x168c
PCI_DEVICE_ath2417 = 0x001d
ROMS += ath2417
ROMS_ath5k += ath2417

# NIC	168c001d	168c,001d	Atheros 2417 Nala
DRIVER_168c001d = ath5k
ROM_TYPE_168c001d = pci
ROM_DESCRIPTION_168c001d = "Atheros 2417 Nala"
PCI_VENDOR_168c001d = 0x168c
PCI_DEVICE_168c001d = 0x001d
ROMS += 168c001d
ROMS_ath5k += 168c001d
