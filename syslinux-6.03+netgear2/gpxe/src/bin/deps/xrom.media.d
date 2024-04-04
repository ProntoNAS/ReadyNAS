$(BIN)/%.xrom : $(BIN)/%.xrom.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_xrom) 
	$(Q)$(FINALISE_xrom)
