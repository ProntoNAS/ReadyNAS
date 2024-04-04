libmod_privsep.la: mod_privsep.lo util_privsep_client.lo
	$(MOD_LINK) mod_privsep.lo util_privsep_client.lo $(MOD_PRIVSEP_LDADD)
DISTCLEAN_TARGETS = modules.mk
static =  libmod_privsep.la
shared = 
