$(BIN)/%.dsk : $(BIN)/%.dsk.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_dsk) 
	$(Q)$(FINALISE_dsk)
