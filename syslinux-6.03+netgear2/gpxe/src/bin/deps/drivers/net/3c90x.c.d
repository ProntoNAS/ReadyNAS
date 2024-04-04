3c90x_DEPS = drivers/net/3c90x.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/stdlib.h include/assert.h include/stddef.h include/string.h \
 arch/x86/include/bits/string.h include/unistd.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/ethernet.h include/gpxe/if_ether.h include/gpxe/io.h \
 config/ioapi.h include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/iobuf.h include/gpxe/list.h \
 include/gpxe/malloc.h include/gpxe/netdevice.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/pci.h \
 include/gpxe/device.h include/gpxe/pci_io.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/pci_ids.h \
 include/gpxe/nvs.h drivers/net/3c90x.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/stdlib.h:

include/assert.h:

include/stddef.h:

include/string.h:

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

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ethernet.h:

include/gpxe/if_ether.h:

include/gpxe/io.h:

config/ioapi.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/iobuf.h:

include/gpxe/list.h:

include/gpxe/malloc.h:

include/gpxe/netdevice.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/nvs.h:

drivers/net/3c90x.h:

$(BIN)/3c90x.o : drivers/net/3c90x.c $(MAKEDEPS) $(POST_O_DEPS) $(3c90x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/3c90x.o
 
$(BIN)/3c90x.dbg%.o : drivers/net/3c90x.c $(MAKEDEPS) $(POST_O_DEPS) $(3c90x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/3c90x.dbg%.o
 
$(BIN)/3c90x.c : drivers/net/3c90x.c $(MAKEDEPS) $(POST_O_DEPS) $(3c90x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/3c90x.c
 
$(BIN)/3c90x.s : drivers/net/3c90x.c $(MAKEDEPS) $(POST_O_DEPS) $(3c90x_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/3c90x.s
 
bin/deps/drivers/net/3c90x.c.d : $(3c90x_DEPS)
 
TAGS : $(3c90x_DEPS)


# NIC	
# NIC	family	drivers/net/3c90x
DRIVERS += 3c90x

# NIC	3c556	10b7,6055	3C556
DRIVER_3c556 = 3c90x
ROM_TYPE_3c556 = pci
ROM_DESCRIPTION_3c556 = "3C556"
PCI_VENDOR_3c556 = 0x10b7
PCI_DEVICE_3c556 = 0x6055
ROMS += 3c556
ROMS_3c90x += 3c556

# NIC	10b76055	10b7,6055	3C556
DRIVER_10b76055 = 3c90x
ROM_TYPE_10b76055 = pci
ROM_DESCRIPTION_10b76055 = "3C556"
PCI_VENDOR_10b76055 = 0x10b7
PCI_DEVICE_10b76055 = 0x6055
ROMS += 10b76055
ROMS_3c90x += 10b76055

# NIC	3c905-tpo	10b7,9000	3Com900-TPO
DRIVER_3c905-tpo = 3c90x
ROM_TYPE_3c905-tpo = pci
ROM_DESCRIPTION_3c905-tpo = "3Com900-TPO"
PCI_VENDOR_3c905-tpo = 0x10b7
PCI_DEVICE_3c905-tpo = 0x9000
ROMS += 3c905-tpo
ROMS_3c90x += 3c905-tpo

# NIC	10b79000	10b7,9000	3Com900-TPO
DRIVER_10b79000 = 3c90x
ROM_TYPE_10b79000 = pci
ROM_DESCRIPTION_10b79000 = "3Com900-TPO"
PCI_VENDOR_10b79000 = 0x10b7
PCI_DEVICE_10b79000 = 0x9000
ROMS += 10b79000
ROMS_3c90x += 10b79000

# NIC	3c905-t4	10b7,9001	3Com900-Combo
DRIVER_3c905-t4 = 3c90x
ROM_TYPE_3c905-t4 = pci
ROM_DESCRIPTION_3c905-t4 = "3Com900-Combo"
PCI_VENDOR_3c905-t4 = 0x10b7
PCI_DEVICE_3c905-t4 = 0x9001
ROMS += 3c905-t4
ROMS_3c90x += 3c905-t4

# NIC	10b79001	10b7,9001	3Com900-Combo
DRIVER_10b79001 = 3c90x
ROM_TYPE_10b79001 = pci
ROM_DESCRIPTION_10b79001 = "3Com900-Combo"
PCI_VENDOR_10b79001 = 0x10b7
PCI_DEVICE_10b79001 = 0x9001
ROMS += 10b79001
ROMS_3c90x += 10b79001

# NIC	3c905-tpo100	10b7,9050	3Com905-TX
DRIVER_3c905-tpo100 = 3c90x
ROM_TYPE_3c905-tpo100 = pci
ROM_DESCRIPTION_3c905-tpo100 = "3Com905-TX"
PCI_VENDOR_3c905-tpo100 = 0x10b7
PCI_DEVICE_3c905-tpo100 = 0x9050
ROMS += 3c905-tpo100
ROMS_3c90x += 3c905-tpo100

# NIC	10b79050	10b7,9050	3Com905-TX
DRIVER_10b79050 = 3c90x
ROM_TYPE_10b79050 = pci
ROM_DESCRIPTION_10b79050 = "3Com905-TX"
PCI_VENDOR_10b79050 = 0x10b7
PCI_DEVICE_10b79050 = 0x9050
ROMS += 10b79050
ROMS_3c90x += 10b79050

# NIC	3c905-combo	10b7,9051	3Com905-T4
DRIVER_3c905-combo = 3c90x
ROM_TYPE_3c905-combo = pci
ROM_DESCRIPTION_3c905-combo = "3Com905-T4"
PCI_VENDOR_3c905-combo = 0x10b7
PCI_DEVICE_3c905-combo = 0x9051
ROMS += 3c905-combo
ROMS_3c90x += 3c905-combo

# NIC	10b79051	10b7,9051	3Com905-T4
DRIVER_10b79051 = 3c90x
ROM_TYPE_10b79051 = pci
ROM_DESCRIPTION_10b79051 = "3Com905-T4"
PCI_VENDOR_10b79051 = 0x10b7
PCI_DEVICE_10b79051 = 0x9051
ROMS += 10b79051
ROMS_3c90x += 10b79051

# NIC	3c905b-tpo	10b7,9004	3Com900B-TPO
DRIVER_3c905b-tpo = 3c90x
ROM_TYPE_3c905b-tpo = pci
ROM_DESCRIPTION_3c905b-tpo = "3Com900B-TPO"
PCI_VENDOR_3c905b-tpo = 0x10b7
PCI_DEVICE_3c905b-tpo = 0x9004
ROMS += 3c905b-tpo
ROMS_3c90x += 3c905b-tpo

# NIC	10b79004	10b7,9004	3Com900B-TPO
DRIVER_10b79004 = 3c90x
ROM_TYPE_10b79004 = pci
ROM_DESCRIPTION_10b79004 = "3Com900B-TPO"
PCI_VENDOR_10b79004 = 0x10b7
PCI_DEVICE_10b79004 = 0x9004
ROMS += 10b79004
ROMS_3c90x += 10b79004

# NIC	3c905b-combo	10b7,9005	3Com900B-Combo
DRIVER_3c905b-combo = 3c90x
ROM_TYPE_3c905b-combo = pci
ROM_DESCRIPTION_3c905b-combo = "3Com900B-Combo"
PCI_VENDOR_3c905b-combo = 0x10b7
PCI_DEVICE_3c905b-combo = 0x9005
ROMS += 3c905b-combo
ROMS_3c90x += 3c905b-combo

# NIC	10b79005	10b7,9005	3Com900B-Combo
DRIVER_10b79005 = 3c90x
ROM_TYPE_10b79005 = pci
ROM_DESCRIPTION_10b79005 = "3Com900B-Combo"
PCI_VENDOR_10b79005 = 0x10b7
PCI_DEVICE_10b79005 = 0x9005
ROMS += 10b79005
ROMS_3c90x += 10b79005

# NIC	3c905b-tpb2	10b7,9006	3Com900B-2/T
DRIVER_3c905b-tpb2 = 3c90x
ROM_TYPE_3c905b-tpb2 = pci
ROM_DESCRIPTION_3c905b-tpb2 = "3Com900B-2/T"
PCI_VENDOR_3c905b-tpb2 = 0x10b7
PCI_DEVICE_3c905b-tpb2 = 0x9006
ROMS += 3c905b-tpb2
ROMS_3c90x += 3c905b-tpb2

# NIC	10b79006	10b7,9006	3Com900B-2/T
DRIVER_10b79006 = 3c90x
ROM_TYPE_10b79006 = pci
ROM_DESCRIPTION_10b79006 = "3Com900B-2/T"
PCI_VENDOR_10b79006 = 0x10b7
PCI_DEVICE_10b79006 = 0x9006
ROMS += 10b79006
ROMS_3c90x += 10b79006

# NIC	3c905b-fl	10b7,900a	3Com900B-FL
DRIVER_3c905b-fl = 3c90x
ROM_TYPE_3c905b-fl = pci
ROM_DESCRIPTION_3c905b-fl = "3Com900B-FL"
PCI_VENDOR_3c905b-fl = 0x10b7
PCI_DEVICE_3c905b-fl = 0x900a
ROMS += 3c905b-fl
ROMS_3c90x += 3c905b-fl

# NIC	10b7900a	10b7,900a	3Com900B-FL
DRIVER_10b7900a = 3c90x
ROM_TYPE_10b7900a = pci
ROM_DESCRIPTION_10b7900a = "3Com900B-FL"
PCI_VENDOR_10b7900a = 0x10b7
PCI_DEVICE_10b7900a = 0x900a
ROMS += 10b7900a
ROMS_3c90x += 10b7900a

# NIC	3c905b-tpo100	10b7,9055	3Com905B-TX
DRIVER_3c905b-tpo100 = 3c90x
ROM_TYPE_3c905b-tpo100 = pci
ROM_DESCRIPTION_3c905b-tpo100 = "3Com905B-TX"
PCI_VENDOR_3c905b-tpo100 = 0x10b7
PCI_DEVICE_3c905b-tpo100 = 0x9055
ROMS += 3c905b-tpo100
ROMS_3c90x += 3c905b-tpo100

# NIC	10b79055	10b7,9055	3Com905B-TX
DRIVER_10b79055 = 3c90x
ROM_TYPE_10b79055 = pci
ROM_DESCRIPTION_10b79055 = "3Com905B-TX"
PCI_VENDOR_10b79055 = 0x10b7
PCI_DEVICE_10b79055 = 0x9055
ROMS += 10b79055
ROMS_3c90x += 10b79055

# NIC	3c905b-t4	10b7,9056	3Com905B-T4
DRIVER_3c905b-t4 = 3c90x
ROM_TYPE_3c905b-t4 = pci
ROM_DESCRIPTION_3c905b-t4 = "3Com905B-T4"
PCI_VENDOR_3c905b-t4 = 0x10b7
PCI_DEVICE_3c905b-t4 = 0x9056
ROMS += 3c905b-t4
ROMS_3c90x += 3c905b-t4

# NIC	10b79056	10b7,9056	3Com905B-T4
DRIVER_10b79056 = 3c90x
ROM_TYPE_10b79056 = pci
ROM_DESCRIPTION_10b79056 = "3Com905B-T4"
PCI_VENDOR_10b79056 = 0x10b7
PCI_DEVICE_10b79056 = 0x9056
ROMS += 10b79056
ROMS_3c90x += 10b79056

# NIC	3c905b-9058	10b7,9058	3Com905B-9058
DRIVER_3c905b-9058 = 3c90x
ROM_TYPE_3c905b-9058 = pci
ROM_DESCRIPTION_3c905b-9058 = "3Com905B-9058"
PCI_VENDOR_3c905b-9058 = 0x10b7
PCI_DEVICE_3c905b-9058 = 0x9058
ROMS += 3c905b-9058
ROMS_3c90x += 3c905b-9058

# NIC	10b79058	10b7,9058	3Com905B-9058
DRIVER_10b79058 = 3c90x
ROM_TYPE_10b79058 = pci
ROM_DESCRIPTION_10b79058 = "3Com905B-9058"
PCI_VENDOR_10b79058 = 0x10b7
PCI_DEVICE_10b79058 = 0x9058
ROMS += 10b79058
ROMS_3c90x += 10b79058

# NIC	3c905b-fx	10b7,905a	3Com905B-FL
DRIVER_3c905b-fx = 3c90x
ROM_TYPE_3c905b-fx = pci
ROM_DESCRIPTION_3c905b-fx = "3Com905B-FL"
PCI_VENDOR_3c905b-fx = 0x10b7
PCI_DEVICE_3c905b-fx = 0x905a
ROMS += 3c905b-fx
ROMS_3c90x += 3c905b-fx

# NIC	10b7905a	10b7,905a	3Com905B-FL
DRIVER_10b7905a = 3c90x
ROM_TYPE_10b7905a = pci
ROM_DESCRIPTION_10b7905a = "3Com905B-FL"
PCI_VENDOR_10b7905a = 0x10b7
PCI_DEVICE_10b7905a = 0x905a
ROMS += 10b7905a
ROMS_3c90x += 10b7905a

# NIC	3c905c-tpo	10b7,9200	3Com905C-TXM
DRIVER_3c905c-tpo = 3c90x
ROM_TYPE_3c905c-tpo = pci
ROM_DESCRIPTION_3c905c-tpo = "3Com905C-TXM"
PCI_VENDOR_3c905c-tpo = 0x10b7
PCI_DEVICE_3c905c-tpo = 0x9200
ROMS += 3c905c-tpo
ROMS_3c90x += 3c905c-tpo

# NIC	10b79200	10b7,9200	3Com905C-TXM
DRIVER_10b79200 = 3c90x
ROM_TYPE_10b79200 = pci
ROM_DESCRIPTION_10b79200 = "3Com905C-TXM"
PCI_VENDOR_10b79200 = 0x10b7
PCI_DEVICE_10b79200 = 0x9200
ROMS += 10b79200
ROMS_3c90x += 10b79200

# NIC	3c920b-emb-ati	10b7,9202	3c920B-EMB-WNM (ATI Radeon 9100 IGP)
DRIVER_3c920b-emb-ati = 3c90x
ROM_TYPE_3c920b-emb-ati = pci
ROM_DESCRIPTION_3c920b-emb-ati = "3c920B-EMB-WNM (ATI Radeon 9100 IGP)"
PCI_VENDOR_3c920b-emb-ati = 0x10b7
PCI_DEVICE_3c920b-emb-ati = 0x9202
ROMS += 3c920b-emb-ati
ROMS_3c90x += 3c920b-emb-ati

# NIC	10b79202	10b7,9202	3c920B-EMB-WNM (ATI Radeon 9100 IGP)
DRIVER_10b79202 = 3c90x
ROM_TYPE_10b79202 = pci
ROM_DESCRIPTION_10b79202 = "3c920B-EMB-WNM (ATI Radeon 9100 IGP)"
PCI_VENDOR_10b79202 = 0x10b7
PCI_DEVICE_10b79202 = 0x9202
ROMS += 10b79202
ROMS_3c90x += 10b79202

# NIC	3c920b-emb-wnm	10b7,9210	3Com20B-EMB WNM
DRIVER_3c920b-emb-wnm = 3c90x
ROM_TYPE_3c920b-emb-wnm = pci
ROM_DESCRIPTION_3c920b-emb-wnm = "3Com20B-EMB WNM"
PCI_VENDOR_3c920b-emb-wnm = 0x10b7
PCI_DEVICE_3c920b-emb-wnm = 0x9210
ROMS += 3c920b-emb-wnm
ROMS_3c90x += 3c920b-emb-wnm

# NIC	10b79210	10b7,9210	3Com20B-EMB WNM
DRIVER_10b79210 = 3c90x
ROM_TYPE_10b79210 = pci
ROM_DESCRIPTION_10b79210 = "3Com20B-EMB WNM"
PCI_VENDOR_10b79210 = 0x10b7
PCI_DEVICE_10b79210 = 0x9210
ROMS += 10b79210
ROMS_3c90x += 10b79210

# NIC	3c980	10b7,9800	3Com980-Cyclone
DRIVER_3c980 = 3c90x
ROM_TYPE_3c980 = pci
ROM_DESCRIPTION_3c980 = "3Com980-Cyclone"
PCI_VENDOR_3c980 = 0x10b7
PCI_DEVICE_3c980 = 0x9800
ROMS += 3c980
ROMS_3c90x += 3c980

# NIC	10b79800	10b7,9800	3Com980-Cyclone
DRIVER_10b79800 = 3c90x
ROM_TYPE_10b79800 = pci
ROM_DESCRIPTION_10b79800 = "3Com980-Cyclone"
PCI_VENDOR_10b79800 = 0x10b7
PCI_DEVICE_10b79800 = 0x9800
ROMS += 10b79800
ROMS_3c90x += 10b79800

# NIC	3c9805	10b7,9805	3Com9805
DRIVER_3c9805 = 3c90x
ROM_TYPE_3c9805 = pci
ROM_DESCRIPTION_3c9805 = "3Com9805"
PCI_VENDOR_3c9805 = 0x10b7
PCI_DEVICE_3c9805 = 0x9805
ROMS += 3c9805
ROMS_3c90x += 3c9805

# NIC	10b79805	10b7,9805	3Com9805
DRIVER_10b79805 = 3c90x
ROM_TYPE_10b79805 = pci
ROM_DESCRIPTION_10b79805 = "3Com9805"
PCI_VENDOR_10b79805 = 0x10b7
PCI_DEVICE_10b79805 = 0x9805
ROMS += 10b79805
ROMS_3c90x += 10b79805

# NIC	3csoho100-tx	10b7,7646	3CSOHO100-TX
DRIVER_3csoho100-tx = 3c90x
ROM_TYPE_3csoho100-tx = pci
ROM_DESCRIPTION_3csoho100-tx = "3CSOHO100-TX"
PCI_VENDOR_3csoho100-tx = 0x10b7
PCI_DEVICE_3csoho100-tx = 0x7646
ROMS += 3csoho100-tx
ROMS_3c90x += 3csoho100-tx

# NIC	10b77646	10b7,7646	3CSOHO100-TX
DRIVER_10b77646 = 3c90x
ROM_TYPE_10b77646 = pci
ROM_DESCRIPTION_10b77646 = "3CSOHO100-TX"
PCI_VENDOR_10b77646 = 0x10b7
PCI_DEVICE_10b77646 = 0x7646
ROMS += 10b77646
ROMS_3c90x += 10b77646

# NIC	3c450	10b7,4500	3Com450 HomePNA Tornado
DRIVER_3c450 = 3c90x
ROM_TYPE_3c450 = pci
ROM_DESCRIPTION_3c450 = "3Com450 HomePNA Tornado"
PCI_VENDOR_3c450 = 0x10b7
PCI_DEVICE_3c450 = 0x4500
ROMS += 3c450
ROMS_3c90x += 3c450

# NIC	10b74500	10b7,4500	3Com450 HomePNA Tornado
DRIVER_10b74500 = 3c90x
ROM_TYPE_10b74500 = pci
ROM_DESCRIPTION_10b74500 = "3Com450 HomePNA Tornado"
PCI_VENDOR_10b74500 = 0x10b7
PCI_DEVICE_10b74500 = 0x4500
ROMS += 10b74500
ROMS_3c90x += 10b74500

# NIC	3c982a	10b7,1201	3Com982A
DRIVER_3c982a = 3c90x
ROM_TYPE_3c982a = pci
ROM_DESCRIPTION_3c982a = "3Com982A"
PCI_VENDOR_3c982a = 0x10b7
PCI_DEVICE_3c982a = 0x1201
ROMS += 3c982a
ROMS_3c90x += 3c982a

# NIC	10b71201	10b7,1201	3Com982A
DRIVER_10b71201 = 3c90x
ROM_TYPE_10b71201 = pci
ROM_DESCRIPTION_10b71201 = "3Com982A"
PCI_VENDOR_10b71201 = 0x10b7
PCI_DEVICE_10b71201 = 0x1201
ROMS += 10b71201
ROMS_3c90x += 10b71201

# NIC	3c982b	10b7,1202	3Com982B
DRIVER_3c982b = 3c90x
ROM_TYPE_3c982b = pci
ROM_DESCRIPTION_3c982b = "3Com982B"
PCI_VENDOR_3c982b = 0x10b7
PCI_DEVICE_3c982b = 0x1202
ROMS += 3c982b
ROMS_3c90x += 3c982b

# NIC	10b71202	10b7,1202	3Com982B
DRIVER_10b71202 = 3c90x
ROM_TYPE_10b71202 = pci
ROM_DESCRIPTION_10b71202 = "3Com982B"
PCI_VENDOR_10b71202 = 0x10b7
PCI_DEVICE_10b71202 = 0x1202
ROMS += 10b71202
ROMS_3c90x += 10b71202
