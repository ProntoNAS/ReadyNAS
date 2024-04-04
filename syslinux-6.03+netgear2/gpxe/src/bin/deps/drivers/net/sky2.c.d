sky2_DEPS = drivers/net/sky2.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/stdio.h include/stdarg.h \
 include/unistd.h include/stddef.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/ethernet.h include/gpxe/if_ether.h include/gpxe/iobuf.h \
 include/assert.h include/gpxe/list.h include/gpxe/malloc.h \
 include/stdlib.h include/gpxe/pci.h include/gpxe/device.h \
 include/gpxe/tables.h include/gpxe/pci_io.h config/ioapi.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/mii.h \
 include/gpxe/netdevice.h include/gpxe/refcnt.h include/gpxe/settings.h \
 drivers/net/sky2.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/stdio.h:

include/stdarg.h:

include/unistd.h:

include/stddef.h:

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

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/assert.h:

include/gpxe/list.h:

include/gpxe/malloc.h:

include/stdlib.h:

include/gpxe/pci.h:

include/gpxe/device.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

drivers/net/sky2.h:

$(BIN)/sky2.o : drivers/net/sky2.c $(MAKEDEPS) $(POST_O_DEPS) $(sky2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/sky2.o
 
$(BIN)/sky2.dbg%.o : drivers/net/sky2.c $(MAKEDEPS) $(POST_O_DEPS) $(sky2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/sky2.dbg%.o
 
$(BIN)/sky2.c : drivers/net/sky2.c $(MAKEDEPS) $(POST_O_DEPS) $(sky2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/sky2.c
 
$(BIN)/sky2.s : drivers/net/sky2.c $(MAKEDEPS) $(POST_O_DEPS) $(sky2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/sky2.s
 
bin/deps/drivers/net/sky2.c.d : $(sky2_DEPS)
 
TAGS : $(sky2_DEPS)


# NIC	
# NIC	family	drivers/net/sky2
DRIVERS += sky2

# NIC	sk9sxx	1148,9000	Syskonnect SK-9Sxx
DRIVER_sk9sxx = sky2
ROM_TYPE_sk9sxx = pci
ROM_DESCRIPTION_sk9sxx = "Syskonnect SK-9Sxx"
PCI_VENDOR_sk9sxx = 0x1148
PCI_DEVICE_sk9sxx = 0x9000
ROMS += sk9sxx
ROMS_sky2 += sk9sxx

# NIC	11489000	1148,9000	Syskonnect SK-9Sxx
DRIVER_11489000 = sky2
ROM_TYPE_11489000 = pci
ROM_DESCRIPTION_11489000 = "Syskonnect SK-9Sxx"
PCI_VENDOR_11489000 = 0x1148
PCI_DEVICE_11489000 = 0x9000
ROMS += 11489000
ROMS_sky2 += 11489000

# NIC	sk9exx	1148,9e00	Syskonnect SK-9Exx
DRIVER_sk9exx = sky2
ROM_TYPE_sk9exx = pci
ROM_DESCRIPTION_sk9exx = "Syskonnect SK-9Exx"
PCI_VENDOR_sk9exx = 0x1148
PCI_DEVICE_sk9exx = 0x9e00
ROMS += sk9exx
ROMS_sky2 += sk9exx

# NIC	11489e00	1148,9e00	Syskonnect SK-9Exx
DRIVER_11489e00 = sky2
ROM_TYPE_11489e00 = pci
ROM_DESCRIPTION_11489e00 = "Syskonnect SK-9Exx"
PCI_VENDOR_11489e00 = 0x1148
PCI_DEVICE_11489e00 = 0x9e00
ROMS += 11489e00
ROMS_sky2 += 11489e00

# NIC	dge560t	1186,4b00	D-Link DGE-560T
DRIVER_dge560t = sky2
ROM_TYPE_dge560t = pci
ROM_DESCRIPTION_dge560t = "D-Link DGE-560T"
PCI_VENDOR_dge560t = 0x1186
PCI_DEVICE_dge560t = 0x4b00
ROMS += dge560t
ROMS_sky2 += dge560t

# NIC	11864b00	1186,4b00	D-Link DGE-560T
DRIVER_11864b00 = sky2
ROM_TYPE_11864b00 = pci
ROM_DESCRIPTION_11864b00 = "D-Link DGE-560T"
PCI_VENDOR_11864b00 = 0x1186
PCI_DEVICE_11864b00 = 0x4b00
ROMS += 11864b00
ROMS_sky2 += 11864b00

# NIC	dge550sx	1186,4001	D-Link DGE-550SX
DRIVER_dge550sx = sky2
ROM_TYPE_dge550sx = pci
ROM_DESCRIPTION_dge550sx = "D-Link DGE-550SX"
PCI_VENDOR_dge550sx = 0x1186
PCI_DEVICE_dge550sx = 0x4001
ROMS += dge550sx
ROMS_sky2 += dge550sx

# NIC	11864001	1186,4001	D-Link DGE-550SX
DRIVER_11864001 = sky2
ROM_TYPE_11864001 = pci
ROM_DESCRIPTION_11864001 = "D-Link DGE-550SX"
PCI_VENDOR_11864001 = 0x1186
PCI_DEVICE_11864001 = 0x4001
ROMS += 11864001
ROMS_sky2 += 11864001

# NIC	dge560sx	1186,4b02	D-Link DGE-560SX
DRIVER_dge560sx = sky2
ROM_TYPE_dge560sx = pci
ROM_DESCRIPTION_dge560sx = "D-Link DGE-560SX"
PCI_VENDOR_dge560sx = 0x1186
PCI_DEVICE_dge560sx = 0x4b02
ROMS += dge560sx
ROMS_sky2 += dge560sx

# NIC	11864b02	1186,4b02	D-Link DGE-560SX
DRIVER_11864b02 = sky2
ROM_TYPE_11864b02 = pci
ROM_DESCRIPTION_11864b02 = "D-Link DGE-560SX"
PCI_VENDOR_11864b02 = 0x1186
PCI_DEVICE_11864b02 = 0x4b02
ROMS += 11864b02
ROMS_sky2 += 11864b02

# NIC	dge550t	1186,4b03	D-Link DGE-550T
DRIVER_dge550t = sky2
ROM_TYPE_dge550t = pci
ROM_DESCRIPTION_dge550t = "D-Link DGE-550T"
PCI_VENDOR_dge550t = 0x1186
PCI_DEVICE_dge550t = 0x4b03
ROMS += dge550t
ROMS_sky2 += dge550t

# NIC	11864b03	1186,4b03	D-Link DGE-550T
DRIVER_11864b03 = sky2
ROM_TYPE_11864b03 = pci
ROM_DESCRIPTION_11864b03 = "D-Link DGE-550T"
PCI_VENDOR_11864b03 = 0x1186
PCI_DEVICE_11864b03 = 0x4b03
ROMS += 11864b03
ROMS_sky2 += 11864b03

# NIC	m88e8021	11ab,4340	Marvell 88E8021
DRIVER_m88e8021 = sky2
ROM_TYPE_m88e8021 = pci
ROM_DESCRIPTION_m88e8021 = "Marvell 88E8021"
PCI_VENDOR_m88e8021 = 0x11ab
PCI_DEVICE_m88e8021 = 0x4340
ROMS += m88e8021
ROMS_sky2 += m88e8021

# NIC	11ab4340	11ab,4340	Marvell 88E8021
DRIVER_11ab4340 = sky2
ROM_TYPE_11ab4340 = pci
ROM_DESCRIPTION_11ab4340 = "Marvell 88E8021"
PCI_VENDOR_11ab4340 = 0x11ab
PCI_DEVICE_11ab4340 = 0x4340
ROMS += 11ab4340
ROMS_sky2 += 11ab4340

# NIC	m88e8022	11ab,4341	Marvell 88E8022
DRIVER_m88e8022 = sky2
ROM_TYPE_m88e8022 = pci
ROM_DESCRIPTION_m88e8022 = "Marvell 88E8022"
PCI_VENDOR_m88e8022 = 0x11ab
PCI_DEVICE_m88e8022 = 0x4341
ROMS += m88e8022
ROMS_sky2 += m88e8022

# NIC	11ab4341	11ab,4341	Marvell 88E8022
DRIVER_11ab4341 = sky2
ROM_TYPE_11ab4341 = pci
ROM_DESCRIPTION_11ab4341 = "Marvell 88E8022"
PCI_VENDOR_11ab4341 = 0x11ab
PCI_DEVICE_11ab4341 = 0x4341
ROMS += 11ab4341
ROMS_sky2 += 11ab4341

# NIC	m88e8061	11ab,4342	Marvell 88E8061
DRIVER_m88e8061 = sky2
ROM_TYPE_m88e8061 = pci
ROM_DESCRIPTION_m88e8061 = "Marvell 88E8061"
PCI_VENDOR_m88e8061 = 0x11ab
PCI_DEVICE_m88e8061 = 0x4342
ROMS += m88e8061
ROMS_sky2 += m88e8061

# NIC	11ab4342	11ab,4342	Marvell 88E8061
DRIVER_11ab4342 = sky2
ROM_TYPE_11ab4342 = pci
ROM_DESCRIPTION_11ab4342 = "Marvell 88E8061"
PCI_VENDOR_11ab4342 = 0x11ab
PCI_DEVICE_11ab4342 = 0x4342
ROMS += 11ab4342
ROMS_sky2 += 11ab4342

# NIC	m88e8062	11ab,4343	Marvell 88E8062
DRIVER_m88e8062 = sky2
ROM_TYPE_m88e8062 = pci
ROM_DESCRIPTION_m88e8062 = "Marvell 88E8062"
PCI_VENDOR_m88e8062 = 0x11ab
PCI_DEVICE_m88e8062 = 0x4343
ROMS += m88e8062
ROMS_sky2 += m88e8062

# NIC	11ab4343	11ab,4343	Marvell 88E8062
DRIVER_11ab4343 = sky2
ROM_TYPE_11ab4343 = pci
ROM_DESCRIPTION_11ab4343 = "Marvell 88E8062"
PCI_VENDOR_11ab4343 = 0x11ab
PCI_DEVICE_11ab4343 = 0x4343
ROMS += 11ab4343
ROMS_sky2 += 11ab4343

# NIC	m88e8021b	11ab,4344	Marvell 88E8021
DRIVER_m88e8021b = sky2
ROM_TYPE_m88e8021b = pci
ROM_DESCRIPTION_m88e8021b = "Marvell 88E8021"
PCI_VENDOR_m88e8021b = 0x11ab
PCI_DEVICE_m88e8021b = 0x4344
ROMS += m88e8021b
ROMS_sky2 += m88e8021b

# NIC	11ab4344	11ab,4344	Marvell 88E8021
DRIVER_11ab4344 = sky2
ROM_TYPE_11ab4344 = pci
ROM_DESCRIPTION_11ab4344 = "Marvell 88E8021"
PCI_VENDOR_11ab4344 = 0x11ab
PCI_DEVICE_11ab4344 = 0x4344
ROMS += 11ab4344
ROMS_sky2 += 11ab4344

# NIC	m88e8022b	11ab,4345	Marvell 88E8022
DRIVER_m88e8022b = sky2
ROM_TYPE_m88e8022b = pci
ROM_DESCRIPTION_m88e8022b = "Marvell 88E8022"
PCI_VENDOR_m88e8022b = 0x11ab
PCI_DEVICE_m88e8022b = 0x4345
ROMS += m88e8022b
ROMS_sky2 += m88e8022b

# NIC	11ab4345	11ab,4345	Marvell 88E8022
DRIVER_11ab4345 = sky2
ROM_TYPE_11ab4345 = pci
ROM_DESCRIPTION_11ab4345 = "Marvell 88E8022"
PCI_VENDOR_11ab4345 = 0x11ab
PCI_DEVICE_11ab4345 = 0x4345
ROMS += 11ab4345
ROMS_sky2 += 11ab4345

# NIC	m88e8061b	11ab,4346	Marvell 88E8061
DRIVER_m88e8061b = sky2
ROM_TYPE_m88e8061b = pci
ROM_DESCRIPTION_m88e8061b = "Marvell 88E8061"
PCI_VENDOR_m88e8061b = 0x11ab
PCI_DEVICE_m88e8061b = 0x4346
ROMS += m88e8061b
ROMS_sky2 += m88e8061b

# NIC	11ab4346	11ab,4346	Marvell 88E8061
DRIVER_11ab4346 = sky2
ROM_TYPE_11ab4346 = pci
ROM_DESCRIPTION_11ab4346 = "Marvell 88E8061"
PCI_VENDOR_11ab4346 = 0x11ab
PCI_DEVICE_11ab4346 = 0x4346
ROMS += 11ab4346
ROMS_sky2 += 11ab4346

# NIC	m88e8062b	11ab,4347	Marvell 88E8062
DRIVER_m88e8062b = sky2
ROM_TYPE_m88e8062b = pci
ROM_DESCRIPTION_m88e8062b = "Marvell 88E8062"
PCI_VENDOR_m88e8062b = 0x11ab
PCI_DEVICE_m88e8062b = 0x4347
ROMS += m88e8062b
ROMS_sky2 += m88e8062b

# NIC	11ab4347	11ab,4347	Marvell 88E8062
DRIVER_11ab4347 = sky2
ROM_TYPE_11ab4347 = pci
ROM_DESCRIPTION_11ab4347 = "Marvell 88E8062"
PCI_VENDOR_11ab4347 = 0x11ab
PCI_DEVICE_11ab4347 = 0x4347
ROMS += 11ab4347
ROMS_sky2 += 11ab4347

# NIC	m88e8035	11ab,4350	Marvell 88E8035
DRIVER_m88e8035 = sky2
ROM_TYPE_m88e8035 = pci
ROM_DESCRIPTION_m88e8035 = "Marvell 88E8035"
PCI_VENDOR_m88e8035 = 0x11ab
PCI_DEVICE_m88e8035 = 0x4350
ROMS += m88e8035
ROMS_sky2 += m88e8035

# NIC	11ab4350	11ab,4350	Marvell 88E8035
DRIVER_11ab4350 = sky2
ROM_TYPE_11ab4350 = pci
ROM_DESCRIPTION_11ab4350 = "Marvell 88E8035"
PCI_VENDOR_11ab4350 = 0x11ab
PCI_DEVICE_11ab4350 = 0x4350
ROMS += 11ab4350
ROMS_sky2 += 11ab4350

# NIC	m88e8036	11ab,4351	Marvell 88E8036
DRIVER_m88e8036 = sky2
ROM_TYPE_m88e8036 = pci
ROM_DESCRIPTION_m88e8036 = "Marvell 88E8036"
PCI_VENDOR_m88e8036 = 0x11ab
PCI_DEVICE_m88e8036 = 0x4351
ROMS += m88e8036
ROMS_sky2 += m88e8036

# NIC	11ab4351	11ab,4351	Marvell 88E8036
DRIVER_11ab4351 = sky2
ROM_TYPE_11ab4351 = pci
ROM_DESCRIPTION_11ab4351 = "Marvell 88E8036"
PCI_VENDOR_11ab4351 = 0x11ab
PCI_DEVICE_11ab4351 = 0x4351
ROMS += 11ab4351
ROMS_sky2 += 11ab4351

# NIC	m88e8038	11ab,4352	Marvell 88E8038
DRIVER_m88e8038 = sky2
ROM_TYPE_m88e8038 = pci
ROM_DESCRIPTION_m88e8038 = "Marvell 88E8038"
PCI_VENDOR_m88e8038 = 0x11ab
PCI_DEVICE_m88e8038 = 0x4352
ROMS += m88e8038
ROMS_sky2 += m88e8038

# NIC	11ab4352	11ab,4352	Marvell 88E8038
DRIVER_11ab4352 = sky2
ROM_TYPE_11ab4352 = pci
ROM_DESCRIPTION_11ab4352 = "Marvell 88E8038"
PCI_VENDOR_11ab4352 = 0x11ab
PCI_DEVICE_11ab4352 = 0x4352
ROMS += 11ab4352
ROMS_sky2 += 11ab4352

# NIC	m88e8039	11ab,4353	Marvell 88E8039
DRIVER_m88e8039 = sky2
ROM_TYPE_m88e8039 = pci
ROM_DESCRIPTION_m88e8039 = "Marvell 88E8039"
PCI_VENDOR_m88e8039 = 0x11ab
PCI_DEVICE_m88e8039 = 0x4353
ROMS += m88e8039
ROMS_sky2 += m88e8039

# NIC	11ab4353	11ab,4353	Marvell 88E8039
DRIVER_11ab4353 = sky2
ROM_TYPE_11ab4353 = pci
ROM_DESCRIPTION_11ab4353 = "Marvell 88E8039"
PCI_VENDOR_11ab4353 = 0x11ab
PCI_DEVICE_11ab4353 = 0x4353
ROMS += 11ab4353
ROMS_sky2 += 11ab4353

# NIC	m88e8040	11ab,4354	Marvell 88E8040
DRIVER_m88e8040 = sky2
ROM_TYPE_m88e8040 = pci
ROM_DESCRIPTION_m88e8040 = "Marvell 88E8040"
PCI_VENDOR_m88e8040 = 0x11ab
PCI_DEVICE_m88e8040 = 0x4354
ROMS += m88e8040
ROMS_sky2 += m88e8040

# NIC	11ab4354	11ab,4354	Marvell 88E8040
DRIVER_11ab4354 = sky2
ROM_TYPE_11ab4354 = pci
ROM_DESCRIPTION_11ab4354 = "Marvell 88E8040"
PCI_VENDOR_11ab4354 = 0x11ab
PCI_DEVICE_11ab4354 = 0x4354
ROMS += 11ab4354
ROMS_sky2 += 11ab4354

# NIC	m88e8040t	11ab,4355	Marvell 88E8040T
DRIVER_m88e8040t = sky2
ROM_TYPE_m88e8040t = pci
ROM_DESCRIPTION_m88e8040t = "Marvell 88E8040T"
PCI_VENDOR_m88e8040t = 0x11ab
PCI_DEVICE_m88e8040t = 0x4355
ROMS += m88e8040t
ROMS_sky2 += m88e8040t

# NIC	11ab4355	11ab,4355	Marvell 88E8040T
DRIVER_11ab4355 = sky2
ROM_TYPE_11ab4355 = pci
ROM_DESCRIPTION_11ab4355 = "Marvell 88E8040T"
PCI_VENDOR_11ab4355 = 0x11ab
PCI_DEVICE_11ab4355 = 0x4355
ROMS += 11ab4355
ROMS_sky2 += 11ab4355

# NIC	m88ec033	11ab,4356	Marvel 88EC033
DRIVER_m88ec033 = sky2
ROM_TYPE_m88ec033 = pci
ROM_DESCRIPTION_m88ec033 = "Marvel 88EC033"
PCI_VENDOR_m88ec033 = 0x11ab
PCI_DEVICE_m88ec033 = 0x4356
ROMS += m88ec033
ROMS_sky2 += m88ec033

# NIC	11ab4356	11ab,4356	Marvel 88EC033
DRIVER_11ab4356 = sky2
ROM_TYPE_11ab4356 = pci
ROM_DESCRIPTION_11ab4356 = "Marvel 88EC033"
PCI_VENDOR_11ab4356 = 0x11ab
PCI_DEVICE_11ab4356 = 0x4356
ROMS += 11ab4356
ROMS_sky2 += 11ab4356

# NIC	m88e8042	11ab,4357	Marvell 88E8042
DRIVER_m88e8042 = sky2
ROM_TYPE_m88e8042 = pci
ROM_DESCRIPTION_m88e8042 = "Marvell 88E8042"
PCI_VENDOR_m88e8042 = 0x11ab
PCI_DEVICE_m88e8042 = 0x4357
ROMS += m88e8042
ROMS_sky2 += m88e8042

# NIC	11ab4357	11ab,4357	Marvell 88E8042
DRIVER_11ab4357 = sky2
ROM_TYPE_11ab4357 = pci
ROM_DESCRIPTION_11ab4357 = "Marvell 88E8042"
PCI_VENDOR_11ab4357 = 0x11ab
PCI_DEVICE_11ab4357 = 0x4357
ROMS += 11ab4357
ROMS_sky2 += 11ab4357

# NIC	m88e8048	11ab,435a	Marvell 88E8048
DRIVER_m88e8048 = sky2
ROM_TYPE_m88e8048 = pci
ROM_DESCRIPTION_m88e8048 = "Marvell 88E8048"
PCI_VENDOR_m88e8048 = 0x11ab
PCI_DEVICE_m88e8048 = 0x435a
ROMS += m88e8048
ROMS_sky2 += m88e8048

# NIC	11ab435a	11ab,435a	Marvell 88E8048
DRIVER_11ab435a = sky2
ROM_TYPE_11ab435a = pci
ROM_DESCRIPTION_11ab435a = "Marvell 88E8048"
PCI_VENDOR_11ab435a = 0x11ab
PCI_DEVICE_11ab435a = 0x435a
ROMS += 11ab435a
ROMS_sky2 += 11ab435a

# NIC	m88e8052	11ab,4360	Marvell 88E8052
DRIVER_m88e8052 = sky2
ROM_TYPE_m88e8052 = pci
ROM_DESCRIPTION_m88e8052 = "Marvell 88E8052"
PCI_VENDOR_m88e8052 = 0x11ab
PCI_DEVICE_m88e8052 = 0x4360
ROMS += m88e8052
ROMS_sky2 += m88e8052

# NIC	11ab4360	11ab,4360	Marvell 88E8052
DRIVER_11ab4360 = sky2
ROM_TYPE_11ab4360 = pci
ROM_DESCRIPTION_11ab4360 = "Marvell 88E8052"
PCI_VENDOR_11ab4360 = 0x11ab
PCI_DEVICE_11ab4360 = 0x4360
ROMS += 11ab4360
ROMS_sky2 += 11ab4360

# NIC	m88e8050	11ab,4361	Marvell 88E8050
DRIVER_m88e8050 = sky2
ROM_TYPE_m88e8050 = pci
ROM_DESCRIPTION_m88e8050 = "Marvell 88E8050"
PCI_VENDOR_m88e8050 = 0x11ab
PCI_DEVICE_m88e8050 = 0x4361
ROMS += m88e8050
ROMS_sky2 += m88e8050

# NIC	11ab4361	11ab,4361	Marvell 88E8050
DRIVER_11ab4361 = sky2
ROM_TYPE_11ab4361 = pci
ROM_DESCRIPTION_11ab4361 = "Marvell 88E8050"
PCI_VENDOR_11ab4361 = 0x11ab
PCI_DEVICE_11ab4361 = 0x4361
ROMS += 11ab4361
ROMS_sky2 += 11ab4361

# NIC	m88e8053	11ab,4362	Marvell 88E8053
DRIVER_m88e8053 = sky2
ROM_TYPE_m88e8053 = pci
ROM_DESCRIPTION_m88e8053 = "Marvell 88E8053"
PCI_VENDOR_m88e8053 = 0x11ab
PCI_DEVICE_m88e8053 = 0x4362
ROMS += m88e8053
ROMS_sky2 += m88e8053

# NIC	11ab4362	11ab,4362	Marvell 88E8053
DRIVER_11ab4362 = sky2
ROM_TYPE_11ab4362 = pci
ROM_DESCRIPTION_11ab4362 = "Marvell 88E8053"
PCI_VENDOR_11ab4362 = 0x11ab
PCI_DEVICE_11ab4362 = 0x4362
ROMS += 11ab4362
ROMS_sky2 += 11ab4362

# NIC	m88e8055	11ab,4363	Marvell 88E8055
DRIVER_m88e8055 = sky2
ROM_TYPE_m88e8055 = pci
ROM_DESCRIPTION_m88e8055 = "Marvell 88E8055"
PCI_VENDOR_m88e8055 = 0x11ab
PCI_DEVICE_m88e8055 = 0x4363
ROMS += m88e8055
ROMS_sky2 += m88e8055

# NIC	11ab4363	11ab,4363	Marvell 88E8055
DRIVER_11ab4363 = sky2
ROM_TYPE_11ab4363 = pci
ROM_DESCRIPTION_11ab4363 = "Marvell 88E8055"
PCI_VENDOR_11ab4363 = 0x11ab
PCI_DEVICE_11ab4363 = 0x4363
ROMS += 11ab4363
ROMS_sky2 += 11ab4363

# NIC	m88e8056	11ab,4364	Marvell 88E8056
DRIVER_m88e8056 = sky2
ROM_TYPE_m88e8056 = pci
ROM_DESCRIPTION_m88e8056 = "Marvell 88E8056"
PCI_VENDOR_m88e8056 = 0x11ab
PCI_DEVICE_m88e8056 = 0x4364
ROMS += m88e8056
ROMS_sky2 += m88e8056

# NIC	11ab4364	11ab,4364	Marvell 88E8056
DRIVER_11ab4364 = sky2
ROM_TYPE_11ab4364 = pci
ROM_DESCRIPTION_11ab4364 = "Marvell 88E8056"
PCI_VENDOR_11ab4364 = 0x11ab
PCI_DEVICE_11ab4364 = 0x4364
ROMS += 11ab4364
ROMS_sky2 += 11ab4364

# NIC	m88e8070	11ab,4365	Marvell 88E8070
DRIVER_m88e8070 = sky2
ROM_TYPE_m88e8070 = pci
ROM_DESCRIPTION_m88e8070 = "Marvell 88E8070"
PCI_VENDOR_m88e8070 = 0x11ab
PCI_DEVICE_m88e8070 = 0x4365
ROMS += m88e8070
ROMS_sky2 += m88e8070

# NIC	11ab4365	11ab,4365	Marvell 88E8070
DRIVER_11ab4365 = sky2
ROM_TYPE_11ab4365 = pci
ROM_DESCRIPTION_11ab4365 = "Marvell 88E8070"
PCI_VENDOR_11ab4365 = 0x11ab
PCI_DEVICE_11ab4365 = 0x4365
ROMS += 11ab4365
ROMS_sky2 += 11ab4365

# NIC	m88ec036	11ab,4366	Marvell 88EC036
DRIVER_m88ec036 = sky2
ROM_TYPE_m88ec036 = pci
ROM_DESCRIPTION_m88ec036 = "Marvell 88EC036"
PCI_VENDOR_m88ec036 = 0x11ab
PCI_DEVICE_m88ec036 = 0x4366
ROMS += m88ec036
ROMS_sky2 += m88ec036

# NIC	11ab4366	11ab,4366	Marvell 88EC036
DRIVER_11ab4366 = sky2
ROM_TYPE_11ab4366 = pci
ROM_DESCRIPTION_11ab4366 = "Marvell 88EC036"
PCI_VENDOR_11ab4366 = 0x11ab
PCI_DEVICE_11ab4366 = 0x4366
ROMS += 11ab4366
ROMS_sky2 += 11ab4366

# NIC	m88ec032	11ab,4367	Marvell 88EC032
DRIVER_m88ec032 = sky2
ROM_TYPE_m88ec032 = pci
ROM_DESCRIPTION_m88ec032 = "Marvell 88EC032"
PCI_VENDOR_m88ec032 = 0x11ab
PCI_DEVICE_m88ec032 = 0x4367
ROMS += m88ec032
ROMS_sky2 += m88ec032

# NIC	11ab4367	11ab,4367	Marvell 88EC032
DRIVER_11ab4367 = sky2
ROM_TYPE_11ab4367 = pci
ROM_DESCRIPTION_11ab4367 = "Marvell 88EC032"
PCI_VENDOR_11ab4367 = 0x11ab
PCI_DEVICE_11ab4367 = 0x4367
ROMS += 11ab4367
ROMS_sky2 += 11ab4367

# NIC	m88ec034	11ab,4368	Marvell 88EC034
DRIVER_m88ec034 = sky2
ROM_TYPE_m88ec034 = pci
ROM_DESCRIPTION_m88ec034 = "Marvell 88EC034"
PCI_VENDOR_m88ec034 = 0x11ab
PCI_DEVICE_m88ec034 = 0x4368
ROMS += m88ec034
ROMS_sky2 += m88ec034

# NIC	11ab4368	11ab,4368	Marvell 88EC034
DRIVER_11ab4368 = sky2
ROM_TYPE_11ab4368 = pci
ROM_DESCRIPTION_11ab4368 = "Marvell 88EC034"
PCI_VENDOR_11ab4368 = 0x11ab
PCI_DEVICE_11ab4368 = 0x4368
ROMS += 11ab4368
ROMS_sky2 += 11ab4368

# NIC	m88ec042	11ab,4369	Marvell 88EC042
DRIVER_m88ec042 = sky2
ROM_TYPE_m88ec042 = pci
ROM_DESCRIPTION_m88ec042 = "Marvell 88EC042"
PCI_VENDOR_m88ec042 = 0x11ab
PCI_DEVICE_m88ec042 = 0x4369
ROMS += m88ec042
ROMS_sky2 += m88ec042

# NIC	11ab4369	11ab,4369	Marvell 88EC042
DRIVER_11ab4369 = sky2
ROM_TYPE_11ab4369 = pci
ROM_DESCRIPTION_11ab4369 = "Marvell 88EC042"
PCI_VENDOR_11ab4369 = 0x11ab
PCI_DEVICE_11ab4369 = 0x4369
ROMS += 11ab4369
ROMS_sky2 += 11ab4369

# NIC	m88e8058	11ab,436a	Marvell 88E8058
DRIVER_m88e8058 = sky2
ROM_TYPE_m88e8058 = pci
ROM_DESCRIPTION_m88e8058 = "Marvell 88E8058"
PCI_VENDOR_m88e8058 = 0x11ab
PCI_DEVICE_m88e8058 = 0x436a
ROMS += m88e8058
ROMS_sky2 += m88e8058

# NIC	11ab436a	11ab,436a	Marvell 88E8058
DRIVER_11ab436a = sky2
ROM_TYPE_11ab436a = pci
ROM_DESCRIPTION_11ab436a = "Marvell 88E8058"
PCI_VENDOR_11ab436a = 0x11ab
PCI_DEVICE_11ab436a = 0x436a
ROMS += 11ab436a
ROMS_sky2 += 11ab436a

# NIC	m88e8071	11ab,436b	Marvell 88E8071
DRIVER_m88e8071 = sky2
ROM_TYPE_m88e8071 = pci
ROM_DESCRIPTION_m88e8071 = "Marvell 88E8071"
PCI_VENDOR_m88e8071 = 0x11ab
PCI_DEVICE_m88e8071 = 0x436b
ROMS += m88e8071
ROMS_sky2 += m88e8071

# NIC	11ab436b	11ab,436b	Marvell 88E8071
DRIVER_11ab436b = sky2
ROM_TYPE_11ab436b = pci
ROM_DESCRIPTION_11ab436b = "Marvell 88E8071"
PCI_VENDOR_11ab436b = 0x11ab
PCI_DEVICE_11ab436b = 0x436b
ROMS += 11ab436b
ROMS_sky2 += 11ab436b

# NIC	m88e8072	11ab,436c	Marvell 88E8072
DRIVER_m88e8072 = sky2
ROM_TYPE_m88e8072 = pci
ROM_DESCRIPTION_m88e8072 = "Marvell 88E8072"
PCI_VENDOR_m88e8072 = 0x11ab
PCI_DEVICE_m88e8072 = 0x436c
ROMS += m88e8072
ROMS_sky2 += m88e8072

# NIC	11ab436c	11ab,436c	Marvell 88E8072
DRIVER_11ab436c = sky2
ROM_TYPE_11ab436c = pci
ROM_DESCRIPTION_11ab436c = "Marvell 88E8072"
PCI_VENDOR_11ab436c = 0x11ab
PCI_DEVICE_11ab436c = 0x436c
ROMS += 11ab436c
ROMS_sky2 += 11ab436c

# NIC	m88e8055b	11ab,436d	Marvell 88E8055
DRIVER_m88e8055b = sky2
ROM_TYPE_m88e8055b = pci
ROM_DESCRIPTION_m88e8055b = "Marvell 88E8055"
PCI_VENDOR_m88e8055b = 0x11ab
PCI_DEVICE_m88e8055b = 0x436d
ROMS += m88e8055b
ROMS_sky2 += m88e8055b

# NIC	11ab436d	11ab,436d	Marvell 88E8055
DRIVER_11ab436d = sky2
ROM_TYPE_11ab436d = pci
ROM_DESCRIPTION_11ab436d = "Marvell 88E8055"
PCI_VENDOR_11ab436d = 0x11ab
PCI_DEVICE_11ab436d = 0x436d
ROMS += 11ab436d
ROMS_sky2 += 11ab436d

# NIC	m88e8075	11ab,4370	Marvell 88E8075
DRIVER_m88e8075 = sky2
ROM_TYPE_m88e8075 = pci
ROM_DESCRIPTION_m88e8075 = "Marvell 88E8075"
PCI_VENDOR_m88e8075 = 0x11ab
PCI_DEVICE_m88e8075 = 0x4370
ROMS += m88e8075
ROMS_sky2 += m88e8075

# NIC	11ab4370	11ab,4370	Marvell 88E8075
DRIVER_11ab4370 = sky2
ROM_TYPE_11ab4370 = pci
ROM_DESCRIPTION_11ab4370 = "Marvell 88E8075"
PCI_VENDOR_11ab4370 = 0x11ab
PCI_DEVICE_11ab4370 = 0x4370
ROMS += 11ab4370
ROMS_sky2 += 11ab4370

# NIC	m88e8057	11ab,4380	Marvell 88E8057
DRIVER_m88e8057 = sky2
ROM_TYPE_m88e8057 = pci
ROM_DESCRIPTION_m88e8057 = "Marvell 88E8057"
PCI_VENDOR_m88e8057 = 0x11ab
PCI_DEVICE_m88e8057 = 0x4380
ROMS += m88e8057
ROMS_sky2 += m88e8057

# NIC	11ab4380	11ab,4380	Marvell 88E8057
DRIVER_11ab4380 = sky2
ROM_TYPE_11ab4380 = pci
ROM_DESCRIPTION_11ab4380 = "Marvell 88E8057"
PCI_VENDOR_11ab4380 = 0x11ab
PCI_DEVICE_11ab4380 = 0x4380
ROMS += 11ab4380
ROMS_sky2 += 11ab4380
