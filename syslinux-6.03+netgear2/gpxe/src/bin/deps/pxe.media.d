$(BIN)/%.pxe : $(BIN)/%.pxe.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_pxe) 
	$(Q)$(FINALISE_pxe)
