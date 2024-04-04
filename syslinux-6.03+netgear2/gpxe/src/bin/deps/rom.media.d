$(BIN)/%.rom : $(BIN)/%.rom.zbin 
	$(QM)echo "  [FINISH] $@" 
	$(Q)$(CP) $< $@ 
	$(Q)$(PAD_rom) 
	$(Q)$(FINALISE_rom)
