$(BIN)/%.raw : $(BIN)/%.raw.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_raw) 
	$(Q)$(FINALISE_raw)
