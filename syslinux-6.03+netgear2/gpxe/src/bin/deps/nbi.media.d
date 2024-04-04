$(BIN)/%.nbi : $(BIN)/%.nbi.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_nbi) 
	$(Q)$(FINALISE_nbi)
