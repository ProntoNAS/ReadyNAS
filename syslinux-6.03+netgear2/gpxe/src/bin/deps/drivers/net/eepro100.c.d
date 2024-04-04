eepro100_DEPS = drivers/net/eepro100.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/errno.h include/gpxe/errfile.h \
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
 include/gpxe/spi_bit.h include/gpxe/spi.h include/gpxe/nvs.h \
 include/gpxe/bitbash.h include/gpxe/threewire.h \
 arch/i386/include/limits.h include/gpxe/netdevice.h \
 include/gpxe/refcnt.h include/gpxe/settings.h drivers/net/eepro100.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

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

include/gpxe/spi_bit.h:

include/gpxe/spi.h:

include/gpxe/nvs.h:

include/gpxe/bitbash.h:

include/gpxe/threewire.h:

arch/i386/include/limits.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

drivers/net/eepro100.h:

$(BIN)/eepro100.o : drivers/net/eepro100.c $(MAKEDEPS) $(POST_O_DEPS) $(eepro100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/eepro100.o
 
$(BIN)/eepro100.dbg%.o : drivers/net/eepro100.c $(MAKEDEPS) $(POST_O_DEPS) $(eepro100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/eepro100.dbg%.o
 
$(BIN)/eepro100.c : drivers/net/eepro100.c $(MAKEDEPS) $(POST_O_DEPS) $(eepro100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/eepro100.c
 
$(BIN)/eepro100.s : drivers/net/eepro100.c $(MAKEDEPS) $(POST_O_DEPS) $(eepro100_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/eepro100.s
 
bin/deps/drivers/net/eepro100.c.d : $(eepro100_DEPS)
 
TAGS : $(eepro100_DEPS)


# NIC	
# NIC	family	drivers/net/eepro100
DRIVERS += eepro100

# NIC	id1029	8086,1029	Intel EtherExpressPro100 ID1029
DRIVER_id1029 = eepro100
ROM_TYPE_id1029 = pci
ROM_DESCRIPTION_id1029 = "Intel EtherExpressPro100 ID1029"
PCI_VENDOR_id1029 = 0x8086
PCI_DEVICE_id1029 = 0x1029
ROMS += id1029
ROMS_eepro100 += id1029

# NIC	80861029	8086,1029	Intel EtherExpressPro100 ID1029
DRIVER_80861029 = eepro100
ROM_TYPE_80861029 = pci
ROM_DESCRIPTION_80861029 = "Intel EtherExpressPro100 ID1029"
PCI_VENDOR_80861029 = 0x8086
PCI_DEVICE_80861029 = 0x1029
ROMS += 80861029
ROMS_eepro100 += 80861029

# NIC	id1030	8086,1030	Intel EtherExpressPro100 ID1030
DRIVER_id1030 = eepro100
ROM_TYPE_id1030 = pci
ROM_DESCRIPTION_id1030 = "Intel EtherExpressPro100 ID1030"
PCI_VENDOR_id1030 = 0x8086
PCI_DEVICE_id1030 = 0x1030
ROMS += id1030
ROMS_eepro100 += id1030

# NIC	80861030	8086,1030	Intel EtherExpressPro100 ID1030
DRIVER_80861030 = eepro100
ROM_TYPE_80861030 = pci
ROM_DESCRIPTION_80861030 = "Intel EtherExpressPro100 ID1030"
PCI_VENDOR_80861030 = 0x8086
PCI_DEVICE_80861030 = 0x1030
ROMS += 80861030
ROMS_eepro100 += 80861030

# NIC	82801cam	8086,1031	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_82801cam = eepro100
ROM_TYPE_82801cam = pci
ROM_DESCRIPTION_82801cam = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_82801cam = 0x8086
PCI_DEVICE_82801cam = 0x1031
ROMS += 82801cam
ROMS_eepro100 += 82801cam

# NIC	80861031	8086,1031	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_80861031 = eepro100
ROM_TYPE_80861031 = pci
ROM_DESCRIPTION_80861031 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_80861031 = 0x8086
PCI_DEVICE_80861031 = 0x1031
ROMS += 80861031
ROMS_eepro100 += 80861031

# NIC	eepro100-1032	8086,1032	Intel PRO/100 VE Network Connection
DRIVER_eepro100-1032 = eepro100
ROM_TYPE_eepro100-1032 = pci
ROM_DESCRIPTION_eepro100-1032 = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_eepro100-1032 = 0x8086
PCI_DEVICE_eepro100-1032 = 0x1032
ROMS += eepro100-1032
ROMS_eepro100 += eepro100-1032

# NIC	80861032	8086,1032	Intel PRO/100 VE Network Connection
DRIVER_80861032 = eepro100
ROM_TYPE_80861032 = pci
ROM_DESCRIPTION_80861032 = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_80861032 = 0x8086
PCI_DEVICE_80861032 = 0x1032
ROMS += 80861032
ROMS_eepro100 += 80861032

# NIC	eepro100-1033	8086,1033	Intel PRO/100 VM Network Connection
DRIVER_eepro100-1033 = eepro100
ROM_TYPE_eepro100-1033 = pci
ROM_DESCRIPTION_eepro100-1033 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_eepro100-1033 = 0x8086
PCI_DEVICE_eepro100-1033 = 0x1033
ROMS += eepro100-1033
ROMS_eepro100 += eepro100-1033

# NIC	80861033	8086,1033	Intel PRO/100 VM Network Connection
DRIVER_80861033 = eepro100
ROM_TYPE_80861033 = pci
ROM_DESCRIPTION_80861033 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_80861033 = 0x8086
PCI_DEVICE_80861033 = 0x1033
ROMS += 80861033
ROMS_eepro100 += 80861033

# NIC	eepro100-1034	8086,1034	Intel PRO/100 VM Network Connection
DRIVER_eepro100-1034 = eepro100
ROM_TYPE_eepro100-1034 = pci
ROM_DESCRIPTION_eepro100-1034 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_eepro100-1034 = 0x8086
PCI_DEVICE_eepro100-1034 = 0x1034
ROMS += eepro100-1034
ROMS_eepro100 += eepro100-1034

# NIC	80861034	8086,1034	Intel PRO/100 VM Network Connection
DRIVER_80861034 = eepro100
ROM_TYPE_80861034 = pci
ROM_DESCRIPTION_80861034 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_80861034 = 0x8086
PCI_DEVICE_80861034 = 0x1034
ROMS += 80861034
ROMS_eepro100 += 80861034

# NIC	eepro100-1035	8086,1035	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_eepro100-1035 = eepro100
ROM_TYPE_eepro100-1035 = pci
ROM_DESCRIPTION_eepro100-1035 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_eepro100-1035 = 0x8086
PCI_DEVICE_eepro100-1035 = 0x1035
ROMS += eepro100-1035
ROMS_eepro100 += eepro100-1035

# NIC	80861035	8086,1035	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_80861035 = eepro100
ROM_TYPE_80861035 = pci
ROM_DESCRIPTION_80861035 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_80861035 = 0x8086
PCI_DEVICE_80861035 = 0x1035
ROMS += 80861035
ROMS_eepro100 += 80861035

# NIC	eepro100-1036	8086,1036	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_eepro100-1036 = eepro100
ROM_TYPE_eepro100-1036 = pci
ROM_DESCRIPTION_eepro100-1036 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_eepro100-1036 = 0x8086
PCI_DEVICE_eepro100-1036 = 0x1036
ROMS += eepro100-1036
ROMS_eepro100 += eepro100-1036

# NIC	80861036	8086,1036	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_80861036 = eepro100
ROM_TYPE_80861036 = pci
ROM_DESCRIPTION_80861036 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_80861036 = 0x8086
PCI_DEVICE_80861036 = 0x1036
ROMS += 80861036
ROMS_eepro100 += 80861036

# NIC	eepro100-1037	8086,1037	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_eepro100-1037 = eepro100
ROM_TYPE_eepro100-1037 = pci
ROM_DESCRIPTION_eepro100-1037 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_eepro100-1037 = 0x8086
PCI_DEVICE_eepro100-1037 = 0x1037
ROMS += eepro100-1037
ROMS_eepro100 += eepro100-1037

# NIC	80861037	8086,1037	Intel 82801CAM (ICH3) Chipset Ethernet Controller
DRIVER_80861037 = eepro100
ROM_TYPE_80861037 = pci
ROM_DESCRIPTION_80861037 = "Intel 82801CAM (ICH3) Chipset Ethernet Controller"
PCI_VENDOR_80861037 = 0x8086
PCI_DEVICE_80861037 = 0x1037
ROMS += 80861037
ROMS_eepro100 += 80861037

# NIC	id1038	8086,1038	Intel PRO/100 VM Network Connection
DRIVER_id1038 = eepro100
ROM_TYPE_id1038 = pci
ROM_DESCRIPTION_id1038 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_id1038 = 0x8086
PCI_DEVICE_id1038 = 0x1038
ROMS += id1038
ROMS_eepro100 += id1038

# NIC	80861038	8086,1038	Intel PRO/100 VM Network Connection
DRIVER_80861038 = eepro100
ROM_TYPE_80861038 = pci
ROM_DESCRIPTION_80861038 = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_80861038 = 0x8086
PCI_DEVICE_80861038 = 0x1038
ROMS += 80861038
ROMS_eepro100 += 80861038

# NIC	82562et	8086,1039	Intel PRO100 VE 82562ET
DRIVER_82562et = eepro100
ROM_TYPE_82562et = pci
ROM_DESCRIPTION_82562et = "Intel PRO100 VE 82562ET"
PCI_VENDOR_82562et = 0x8086
PCI_DEVICE_82562et = 0x1039
ROMS += 82562et
ROMS_eepro100 += 82562et

# NIC	80861039	8086,1039	Intel PRO100 VE 82562ET
DRIVER_80861039 = eepro100
ROM_TYPE_80861039 = pci
ROM_DESCRIPTION_80861039 = "Intel PRO100 VE 82562ET"
PCI_VENDOR_80861039 = 0x8086
PCI_DEVICE_80861039 = 0x1039
ROMS += 80861039
ROMS_eepro100 += 80861039

# NIC	id103a	8086,103a	Intel Corporation 82559 InBusiness 10/100
DRIVER_id103a = eepro100
ROM_TYPE_id103a = pci
ROM_DESCRIPTION_id103a = "Intel Corporation 82559 InBusiness 10/100"
PCI_VENDOR_id103a = 0x8086
PCI_DEVICE_id103a = 0x103a
ROMS += id103a
ROMS_eepro100 += id103a

# NIC	8086103a	8086,103a	Intel Corporation 82559 InBusiness 10/100
DRIVER_8086103a = eepro100
ROM_TYPE_8086103a = pci
ROM_DESCRIPTION_8086103a = "Intel Corporation 82559 InBusiness 10/100"
PCI_VENDOR_8086103a = 0x8086
PCI_DEVICE_8086103a = 0x103a
ROMS += 8086103a
ROMS_eepro100 += 8086103a

# NIC	82562etb	8086,103b	Intel PRO100 VE 82562ETB
DRIVER_82562etb = eepro100
ROM_TYPE_82562etb = pci
ROM_DESCRIPTION_82562etb = "Intel PRO100 VE 82562ETB"
PCI_VENDOR_82562etb = 0x8086
PCI_DEVICE_82562etb = 0x103b
ROMS += 82562etb
ROMS_eepro100 += 82562etb

# NIC	8086103b	8086,103b	Intel PRO100 VE 82562ETB
DRIVER_8086103b = eepro100
ROM_TYPE_8086103b = pci
ROM_DESCRIPTION_8086103b = "Intel PRO100 VE 82562ETB"
PCI_VENDOR_8086103b = 0x8086
PCI_DEVICE_8086103b = 0x103b
ROMS += 8086103b
ROMS_eepro100 += 8086103b

# NIC	eepro100-103c	8086,103c	Intel PRO/100 VM Network Connection
DRIVER_eepro100-103c = eepro100
ROM_TYPE_eepro100-103c = pci
ROM_DESCRIPTION_eepro100-103c = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_eepro100-103c = 0x8086
PCI_DEVICE_eepro100-103c = 0x103c
ROMS += eepro100-103c
ROMS_eepro100 += eepro100-103c

# NIC	8086103c	8086,103c	Intel PRO/100 VM Network Connection
DRIVER_8086103c = eepro100
ROM_TYPE_8086103c = pci
ROM_DESCRIPTION_8086103c = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_8086103c = 0x8086
PCI_DEVICE_8086103c = 0x103c
ROMS += 8086103c
ROMS_eepro100 += 8086103c

# NIC	eepro100-103d	8086,103d	Intel PRO/100 VE Network Connection
DRIVER_eepro100-103d = eepro100
ROM_TYPE_eepro100-103d = pci
ROM_DESCRIPTION_eepro100-103d = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_eepro100-103d = 0x8086
PCI_DEVICE_eepro100-103d = 0x103d
ROMS += eepro100-103d
ROMS_eepro100 += eepro100-103d

# NIC	8086103d	8086,103d	Intel PRO/100 VE Network Connection
DRIVER_8086103d = eepro100
ROM_TYPE_8086103d = pci
ROM_DESCRIPTION_8086103d = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_8086103d = 0x8086
PCI_DEVICE_8086103d = 0x103d
ROMS += 8086103d
ROMS_eepro100 += 8086103d

# NIC	eepro100-103e	8086,103e	Intel PRO/100 VM Network Connection
DRIVER_eepro100-103e = eepro100
ROM_TYPE_eepro100-103e = pci
ROM_DESCRIPTION_eepro100-103e = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_eepro100-103e = 0x8086
PCI_DEVICE_eepro100-103e = 0x103e
ROMS += eepro100-103e
ROMS_eepro100 += eepro100-103e

# NIC	8086103e	8086,103e	Intel PRO/100 VM Network Connection
DRIVER_8086103e = eepro100
ROM_TYPE_8086103e = pci
ROM_DESCRIPTION_8086103e = "Intel PRO/100 VM Network Connection"
PCI_VENDOR_8086103e = 0x8086
PCI_DEVICE_8086103e = 0x103e
ROMS += 8086103e
ROMS_eepro100 += 8086103e

# NIC	prove	8086,1051	Intel PRO/100 VE Network Connection
DRIVER_prove = eepro100
ROM_TYPE_prove = pci
ROM_DESCRIPTION_prove = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_prove = 0x8086
PCI_DEVICE_prove = 0x1051
ROMS += prove
ROMS_eepro100 += prove

# NIC	80861051	8086,1051	Intel PRO/100 VE Network Connection
DRIVER_80861051 = eepro100
ROM_TYPE_80861051 = pci
ROM_DESCRIPTION_80861051 = "Intel PRO/100 VE Network Connection"
PCI_VENDOR_80861051 = 0x8086
PCI_DEVICE_80861051 = 0x1051
ROMS += 80861051
ROMS_eepro100 += 80861051

# NIC	82551qm	8086,1059	Intel PRO/100 M Mobile Connection
DRIVER_82551qm = eepro100
ROM_TYPE_82551qm = pci
ROM_DESCRIPTION_82551qm = "Intel PRO/100 M Mobile Connection"
PCI_VENDOR_82551qm = 0x8086
PCI_DEVICE_82551qm = 0x1059
ROMS += 82551qm
ROMS_eepro100 += 82551qm

# NIC	80861059	8086,1059	Intel PRO/100 M Mobile Connection
DRIVER_80861059 = eepro100
ROM_TYPE_80861059 = pci
ROM_DESCRIPTION_80861059 = "Intel PRO/100 M Mobile Connection"
PCI_VENDOR_80861059 = 0x8086
PCI_DEVICE_80861059 = 0x1059
ROMS += 80861059
ROMS_eepro100 += 80861059

# NIC	82559er	8086,1209	Intel EtherExpressPro100 82559ER
DRIVER_82559er = eepro100
ROM_TYPE_82559er = pci
ROM_DESCRIPTION_82559er = "Intel EtherExpressPro100 82559ER"
PCI_VENDOR_82559er = 0x8086
PCI_DEVICE_82559er = 0x1209
ROMS += 82559er
ROMS_eepro100 += 82559er

# NIC	80861209	8086,1209	Intel EtherExpressPro100 82559ER
DRIVER_80861209 = eepro100
ROM_TYPE_80861209 = pci
ROM_DESCRIPTION_80861209 = "Intel EtherExpressPro100 82559ER"
PCI_VENDOR_80861209 = 0x8086
PCI_DEVICE_80861209 = 0x1209
ROMS += 80861209
ROMS_eepro100 += 80861209

# NIC	82865	8086,1227	Intel 82865 EtherExpress PRO/100A
DRIVER_82865 = eepro100
ROM_TYPE_82865 = pci
ROM_DESCRIPTION_82865 = "Intel 82865 EtherExpress PRO/100A"
PCI_VENDOR_82865 = 0x8086
PCI_DEVICE_82865 = 0x1227
ROMS += 82865
ROMS_eepro100 += 82865

# NIC	80861227	8086,1227	Intel 82865 EtherExpress PRO/100A
DRIVER_80861227 = eepro100
ROM_TYPE_80861227 = pci
ROM_DESCRIPTION_80861227 = "Intel 82865 EtherExpress PRO/100A"
PCI_VENDOR_80861227 = 0x8086
PCI_DEVICE_80861227 = 0x1227
ROMS += 80861227
ROMS_eepro100 += 80861227

# NIC	82556	8086,1228	Intel 82556 EtherExpress PRO/100 Smart
DRIVER_82556 = eepro100
ROM_TYPE_82556 = pci
ROM_DESCRIPTION_82556 = "Intel 82556 EtherExpress PRO/100 Smart"
PCI_VENDOR_82556 = 0x8086
PCI_DEVICE_82556 = 0x1228
ROMS += 82556
ROMS_eepro100 += 82556

# NIC	80861228	8086,1228	Intel 82556 EtherExpress PRO/100 Smart
DRIVER_80861228 = eepro100
ROM_TYPE_80861228 = pci
ROM_DESCRIPTION_80861228 = "Intel 82556 EtherExpress PRO/100 Smart"
PCI_VENDOR_80861228 = 0x8086
PCI_DEVICE_80861228 = 0x1228
ROMS += 80861228
ROMS_eepro100 += 80861228

# NIC	eepro100	8086,1229	Intel EtherExpressPro100
DRIVER_eepro100 = eepro100
ROM_TYPE_eepro100 = pci
ROM_DESCRIPTION_eepro100 = "Intel EtherExpressPro100"
PCI_VENDOR_eepro100 = 0x8086
PCI_DEVICE_eepro100 = 0x1229
ROMS += eepro100
ROMS_eepro100 += eepro100

# NIC	80861229	8086,1229	Intel EtherExpressPro100
DRIVER_80861229 = eepro100
ROM_TYPE_80861229 = pci
ROM_DESCRIPTION_80861229 = "Intel EtherExpressPro100"
PCI_VENDOR_80861229 = 0x8086
PCI_DEVICE_80861229 = 0x1229
ROMS += 80861229
ROMS_eepro100 += 80861229

# NIC	82562em	8086,2449	Intel EtherExpressPro100 82562EM
DRIVER_82562em = eepro100
ROM_TYPE_82562em = pci
ROM_DESCRIPTION_82562em = "Intel EtherExpressPro100 82562EM"
PCI_VENDOR_82562em = 0x8086
PCI_DEVICE_82562em = 0x2449
ROMS += 82562em
ROMS_eepro100 += 82562em

# NIC	80862449	8086,2449	Intel EtherExpressPro100 82562EM
DRIVER_80862449 = eepro100
ROM_TYPE_80862449 = pci
ROM_DESCRIPTION_80862449 = "Intel EtherExpressPro100 82562EM"
PCI_VENDOR_80862449 = 0x8086
PCI_DEVICE_80862449 = 0x2449
ROMS += 80862449
ROMS_eepro100 += 80862449

# NIC	82562-1	8086,2459	Intel 82562 based Fast Ethernet Connection
DRIVER_82562-1 = eepro100
ROM_TYPE_82562-1 = pci
ROM_DESCRIPTION_82562-1 = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_82562-1 = 0x8086
PCI_DEVICE_82562-1 = 0x2459
ROMS += 82562-1
ROMS_eepro100 += 82562-1

# NIC	80862459	8086,2459	Intel 82562 based Fast Ethernet Connection
DRIVER_80862459 = eepro100
ROM_TYPE_80862459 = pci
ROM_DESCRIPTION_80862459 = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_80862459 = 0x8086
PCI_DEVICE_80862459 = 0x2459
ROMS += 80862459
ROMS_eepro100 += 80862459

# NIC	82562-2	8086,245d	Intel 82562 based Fast Ethernet Connection
DRIVER_82562-2 = eepro100
ROM_TYPE_82562-2 = pci
ROM_DESCRIPTION_82562-2 = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_82562-2 = 0x8086
PCI_DEVICE_82562-2 = 0x245d
ROMS += 82562-2
ROMS_eepro100 += 82562-2

# NIC	8086245d	8086,245d	Intel 82562 based Fast Ethernet Connection
DRIVER_8086245d = eepro100
ROM_TYPE_8086245d = pci
ROM_DESCRIPTION_8086245d = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_8086245d = 0x8086
PCI_DEVICE_8086245d = 0x245d
ROMS += 8086245d
ROMS_eepro100 += 8086245d

# NIC	82562ez	8086,1050	Intel 82562EZ Network Connection
DRIVER_82562ez = eepro100
ROM_TYPE_82562ez = pci
ROM_DESCRIPTION_82562ez = "Intel 82562EZ Network Connection"
PCI_VENDOR_82562ez = 0x8086
PCI_DEVICE_82562ez = 0x1050
ROMS += 82562ez
ROMS_eepro100 += 82562ez

# NIC	80861050	8086,1050	Intel 82562EZ Network Connection
DRIVER_80861050 = eepro100
ROM_TYPE_80861050 = pci
ROM_DESCRIPTION_80861050 = "Intel 82562EZ Network Connection"
PCI_VENDOR_80861050 = 0x8086
PCI_DEVICE_80861050 = 0x1050
ROMS += 80861050
ROMS_eepro100 += 80861050

# NIC	eepro100-1051	8086,1051	Intel 82801EB/ER (ICH5/ICH5R) Chipset Ethernet Controller
DRIVER_eepro100-1051 = eepro100
ROM_TYPE_eepro100-1051 = pci
ROM_DESCRIPTION_eepro100-1051 = "Intel 82801EB/ER (ICH5/ICH5R) Chipset Ethernet Controller"
PCI_VENDOR_eepro100-1051 = 0x8086
PCI_DEVICE_eepro100-1051 = 0x1051
ROMS += eepro100-1051
ROMS_eepro100 += eepro100-1051

# NIC	80861051	8086,1051	Intel 82801EB/ER (ICH5/ICH5R) Chipset Ethernet Controller
DRIVER_80861051 = eepro100
ROM_TYPE_80861051 = pci
ROM_DESCRIPTION_80861051 = "Intel 82801EB/ER (ICH5/ICH5R) Chipset Ethernet Controller"
PCI_VENDOR_80861051 = 0x8086
PCI_DEVICE_80861051 = 0x1051
ROMS += 80861051
ROMS_eepro100 += 80861051

# NIC	82562-3	8086,1065	Intel 82562 based Fast Ethernet Connection
DRIVER_82562-3 = eepro100
ROM_TYPE_82562-3 = pci
ROM_DESCRIPTION_82562-3 = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_82562-3 = 0x8086
PCI_DEVICE_82562-3 = 0x1065
ROMS += 82562-3
ROMS_eepro100 += 82562-3

# NIC	80861065	8086,1065	Intel 82562 based Fast Ethernet Connection
DRIVER_80861065 = eepro100
ROM_TYPE_80861065 = pci
ROM_DESCRIPTION_80861065 = "Intel 82562 based Fast Ethernet Connection"
PCI_VENDOR_80861065 = 0x8086
PCI_DEVICE_80861065 = 0x1065
ROMS += 80861065
ROMS_eepro100 += 80861065

# NIC	eepro100-5200	8086,5200	Intel EtherExpress PRO/100 Intelligent Server
DRIVER_eepro100-5200 = eepro100
ROM_TYPE_eepro100-5200 = pci
ROM_DESCRIPTION_eepro100-5200 = "Intel EtherExpress PRO/100 Intelligent Server"
PCI_VENDOR_eepro100-5200 = 0x8086
PCI_DEVICE_eepro100-5200 = 0x5200
ROMS += eepro100-5200
ROMS_eepro100 += eepro100-5200

# NIC	80865200	8086,5200	Intel EtherExpress PRO/100 Intelligent Server
DRIVER_80865200 = eepro100
ROM_TYPE_80865200 = pci
ROM_DESCRIPTION_80865200 = "Intel EtherExpress PRO/100 Intelligent Server"
PCI_VENDOR_80865200 = 0x8086
PCI_DEVICE_80865200 = 0x5200
ROMS += 80865200
ROMS_eepro100 += 80865200

# NIC	eepro100-5201	8086,5201	Intel EtherExpress PRO/100 Intelligent Server
DRIVER_eepro100-5201 = eepro100
ROM_TYPE_eepro100-5201 = pci
ROM_DESCRIPTION_eepro100-5201 = "Intel EtherExpress PRO/100 Intelligent Server"
PCI_VENDOR_eepro100-5201 = 0x8086
PCI_DEVICE_eepro100-5201 = 0x5201
ROMS += eepro100-5201
ROMS_eepro100 += eepro100-5201

# NIC	80865201	8086,5201	Intel EtherExpress PRO/100 Intelligent Server
DRIVER_80865201 = eepro100
ROM_TYPE_80865201 = pci
ROM_DESCRIPTION_80865201 = "Intel EtherExpress PRO/100 Intelligent Server"
PCI_VENDOR_80865201 = 0x8086
PCI_DEVICE_80865201 = 0x5201
ROMS += 80865201
ROMS_eepro100 += 80865201
