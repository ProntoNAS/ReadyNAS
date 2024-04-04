skge_DEPS = drivers/net/skge.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/stdio.h include/stdarg.h \
 include/unistd.h include/stddef.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/netdevice.h include/gpxe/list.h include/assert.h \
 include/gpxe/tables.h include/gpxe/refcnt.h include/gpxe/settings.h \
 include/gpxe/ethernet.h include/gpxe/if_ether.h include/gpxe/iobuf.h \
 include/gpxe/malloc.h include/stdlib.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/pci_io.h config/ioapi.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/io.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 drivers/net/skge.h

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

include/gpxe/netdevice.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/iobuf.h:

include/gpxe/malloc.h:

include/stdlib.h:

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

drivers/net/skge.h:

$(BIN)/skge.o : drivers/net/skge.c $(MAKEDEPS) $(POST_O_DEPS) $(skge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/skge.o
 
$(BIN)/skge.dbg%.o : drivers/net/skge.c $(MAKEDEPS) $(POST_O_DEPS) $(skge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/skge.dbg%.o
 
$(BIN)/skge.c : drivers/net/skge.c $(MAKEDEPS) $(POST_O_DEPS) $(skge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/skge.c
 
$(BIN)/skge.s : drivers/net/skge.c $(MAKEDEPS) $(POST_O_DEPS) $(skge_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/skge.s
 
bin/deps/drivers/net/skge.c.d : $(skge_DEPS)
 
TAGS : $(skge_DEPS)


# NIC	
# NIC	family	drivers/net/skge
DRIVERS += skge

# NIC	3C940	10b7,1700	3COM 3C940
DRIVER_3C940 = skge
ROM_TYPE_3C940 = pci
ROM_DESCRIPTION_3C940 = "3COM 3C940"
PCI_VENDOR_3C940 = 0x10b7
PCI_DEVICE_3C940 = 0x1700
ROMS += 3C940
ROMS_skge += 3C940

# NIC	10b71700	10b7,1700	3COM 3C940
DRIVER_10b71700 = skge
ROM_TYPE_10b71700 = pci
ROM_DESCRIPTION_10b71700 = "3COM 3C940"
PCI_VENDOR_10b71700 = 0x10b7
PCI_DEVICE_10b71700 = 0x1700
ROMS += 10b71700
ROMS_skge += 10b71700

# NIC	3C940B	10b7,80eb	3COM 3C940
DRIVER_3C940B = skge
ROM_TYPE_3C940B = pci
ROM_DESCRIPTION_3C940B = "3COM 3C940"
PCI_VENDOR_3C940B = 0x10b7
PCI_DEVICE_3C940B = 0x80eb
ROMS += 3C940B
ROMS_skge += 3C940B

# NIC	10b780eb	10b7,80eb	3COM 3C940
DRIVER_10b780eb = skge
ROM_TYPE_10b780eb = pci
ROM_DESCRIPTION_10b780eb = "3COM 3C940"
PCI_VENDOR_10b780eb = 0x10b7
PCI_DEVICE_10b780eb = 0x80eb
ROMS += 10b780eb
ROMS_skge += 10b780eb

# NIC	GE	1148,4300	Syskonnect GE
DRIVER_GE = skge
ROM_TYPE_GE = pci
ROM_DESCRIPTION_GE = "Syskonnect GE"
PCI_VENDOR_GE = 0x1148
PCI_DEVICE_GE = 0x4300
ROMS += GE
ROMS_skge += GE

# NIC	11484300	1148,4300	Syskonnect GE
DRIVER_11484300 = skge
ROM_TYPE_11484300 = pci
ROM_DESCRIPTION_11484300 = "Syskonnect GE"
PCI_VENDOR_11484300 = 0x1148
PCI_DEVICE_11484300 = 0x4300
ROMS += 11484300
ROMS_skge += 11484300

# NIC	YU	1148,4320	Syskonnect YU
DRIVER_YU = skge
ROM_TYPE_YU = pci
ROM_DESCRIPTION_YU = "Syskonnect YU"
PCI_VENDOR_YU = 0x1148
PCI_DEVICE_YU = 0x4320
ROMS += YU
ROMS_skge += YU

# NIC	11484320	1148,4320	Syskonnect YU
DRIVER_11484320 = skge
ROM_TYPE_11484320 = pci
ROM_DESCRIPTION_11484320 = "Syskonnect YU"
PCI_VENDOR_11484320 = 0x1148
PCI_DEVICE_11484320 = 0x4320
ROMS += 11484320
ROMS_skge += 11484320

# NIC	DGE510T	1186,4c00	DLink DGE-510T
DRIVER_DGE510T = skge
ROM_TYPE_DGE510T = pci
ROM_DESCRIPTION_DGE510T = "DLink DGE-510T"
PCI_VENDOR_DGE510T = 0x1186
PCI_DEVICE_DGE510T = 0x4c00
ROMS += DGE510T
ROMS_skge += DGE510T

# NIC	11864c00	1186,4c00	DLink DGE-510T
DRIVER_11864c00 = skge
ROM_TYPE_11864c00 = pci
ROM_DESCRIPTION_11864c00 = "DLink DGE-510T"
PCI_VENDOR_11864c00 = 0x1186
PCI_DEVICE_11864c00 = 0x4c00
ROMS += 11864c00
ROMS_skge += 11864c00

# NIC	DGE530T	1186,4b01	DLink DGE-530T
DRIVER_DGE530T = skge
ROM_TYPE_DGE530T = pci
ROM_DESCRIPTION_DGE530T = "DLink DGE-530T"
PCI_VENDOR_DGE530T = 0x1186
PCI_DEVICE_DGE530T = 0x4b01
ROMS += DGE530T
ROMS_skge += DGE530T

# NIC	11864b01	1186,4b01	DLink DGE-530T
DRIVER_11864b01 = skge
ROM_TYPE_11864b01 = pci
ROM_DESCRIPTION_11864b01 = "DLink DGE-530T"
PCI_VENDOR_11864b01 = 0x1186
PCI_DEVICE_11864b01 = 0x4b01
ROMS += 11864b01
ROMS_skge += 11864b01

# NIC	id4320	11ab,4320	Marvell id4320
DRIVER_id4320 = skge
ROM_TYPE_id4320 = pci
ROM_DESCRIPTION_id4320 = "Marvell id4320"
PCI_VENDOR_id4320 = 0x11ab
PCI_DEVICE_id4320 = 0x4320
ROMS += id4320
ROMS_skge += id4320

# NIC	11ab4320	11ab,4320	Marvell id4320
DRIVER_11ab4320 = skge
ROM_TYPE_11ab4320 = pci
ROM_DESCRIPTION_11ab4320 = "Marvell id4320"
PCI_VENDOR_11ab4320 = 0x11ab
PCI_DEVICE_11ab4320 = 0x4320
ROMS += 11ab4320
ROMS_skge += 11ab4320

# NIC	id5005	11ab,5005	Marvell id5005
DRIVER_id5005 = skge
ROM_TYPE_id5005 = pci
ROM_DESCRIPTION_id5005 = "Marvell id5005"
PCI_VENDOR_id5005 = 0x11ab
PCI_DEVICE_id5005 = 0x5005
ROMS += id5005
ROMS_skge += id5005

# NIC	11ab5005	11ab,5005	Marvell id5005
DRIVER_11ab5005 = skge
ROM_TYPE_11ab5005 = pci
ROM_DESCRIPTION_11ab5005 = "Marvell id5005"
PCI_VENDOR_11ab5005 = 0x11ab
PCI_DEVICE_11ab5005 = 0x5005
ROMS += 11ab5005
ROMS_skge += 11ab5005

# NIC	Gigacard	1371,434e	CNET Gigacard
DRIVER_Gigacard = skge
ROM_TYPE_Gigacard = pci
ROM_DESCRIPTION_Gigacard = "CNET Gigacard"
PCI_VENDOR_Gigacard = 0x1371
PCI_DEVICE_Gigacard = 0x434e
ROMS += Gigacard
ROMS_skge += Gigacard

# NIC	1371434e	1371,434e	CNET Gigacard
DRIVER_1371434e = skge
ROM_TYPE_1371434e = pci
ROM_DESCRIPTION_1371434e = "CNET Gigacard"
PCI_VENDOR_1371434e = 0x1371
PCI_DEVICE_1371434e = 0x434e
ROMS += 1371434e
ROMS_skge += 1371434e

# NIC	EG1064	1737,1064	Linksys EG1064
DRIVER_EG1064 = skge
ROM_TYPE_EG1064 = pci
ROM_DESCRIPTION_EG1064 = "Linksys EG1064"
PCI_VENDOR_EG1064 = 0x1737
PCI_DEVICE_EG1064 = 0x1064
ROMS += EG1064
ROMS_skge += EG1064

# NIC	17371064	1737,1064	Linksys EG1064
DRIVER_17371064 = skge
ROM_TYPE_17371064 = pci
ROM_DESCRIPTION_17371064 = "Linksys EG1064"
PCI_VENDOR_17371064 = 0x1737
PCI_DEVICE_17371064 = 0x1064
ROMS += 17371064
ROMS_skge += 17371064

# NIC	id_any	1737,ffff	Linksys [any]
DRIVER_id_any = skge
ROM_TYPE_id_any = pci
ROM_DESCRIPTION_id_any = "Linksys [any]"
PCI_VENDOR_id_any = 0x1737
PCI_DEVICE_id_any = 0xffff
ROMS += id_any
ROMS_skge += id_any

# NIC	1737ffff	1737,ffff	Linksys [any]
DRIVER_1737ffff = skge
ROM_TYPE_1737ffff = pci
ROM_DESCRIPTION_1737ffff = "Linksys [any]"
PCI_VENDOR_1737ffff = 0x1737
PCI_DEVICE_1737ffff = 0xffff
ROMS += 1737ffff
ROMS_skge += 1737ffff
