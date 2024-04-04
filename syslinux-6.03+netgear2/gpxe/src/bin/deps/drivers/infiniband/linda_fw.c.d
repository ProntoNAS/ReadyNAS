linda_fw_DEPS = drivers/infiniband/linda_fw.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h drivers/infiniband/linda.h \
 include/gpxe/bitops.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h drivers/infiniband/qib_7220_regs.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

drivers/infiniband/linda.h:

include/gpxe/bitops.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

drivers/infiniband/qib_7220_regs.h:

$(BIN)/linda_fw.o : drivers/infiniband/linda_fw.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_fw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/linda_fw.o
 
$(BIN)/linda_fw.dbg%.o : drivers/infiniband/linda_fw.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_fw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/linda_fw.dbg%.o
 
$(BIN)/linda_fw.c : drivers/infiniband/linda_fw.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_fw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/linda_fw.c
 
$(BIN)/linda_fw.s : drivers/infiniband/linda_fw.c $(MAKEDEPS) $(POST_O_DEPS) $(linda_fw_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/linda_fw.s
 
bin/deps/drivers/infiniband/linda_fw.c.d : $(linda_fw_DEPS)
 
TAGS : $(linda_fw_DEPS)

