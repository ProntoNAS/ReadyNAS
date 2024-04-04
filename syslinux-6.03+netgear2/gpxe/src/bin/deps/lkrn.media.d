$(BIN)/%.lkrn : $(BIN)/%.lkrn.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_lkrn) 
	$(Q)$(FINALISE_lkrn)
