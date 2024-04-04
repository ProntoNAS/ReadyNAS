$(BIN)/%.kpxe : $(BIN)/%.kpxe.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_kpxe) 
	$(Q)$(FINALISE_kpxe)
