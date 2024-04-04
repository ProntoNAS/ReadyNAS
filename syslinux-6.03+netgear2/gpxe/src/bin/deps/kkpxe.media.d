$(BIN)/%.kkpxe : $(BIN)/%.kkpxe.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_kkpxe) 
	$(Q)$(FINALISE_kkpxe)
