$(BIN)/%.hrom : $(BIN)/%.hrom.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_hrom) 
	$(Q)$(FINALISE_hrom)
