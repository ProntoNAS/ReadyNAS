$(BIN)/%.hd : $(BIN)/%.hd.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_hd) 
	$(Q)$(FINALISE_hd)
