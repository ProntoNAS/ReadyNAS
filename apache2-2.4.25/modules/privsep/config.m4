dnl APACHE_MODULE(name, helptext[, objects[, structname[, default[, config]]]])

APACHE_MODPATH_INIT(privsep)

privsep_objects="mod_privsep.lo util_privsep_client.lo"
APACHE_MODULE(privsep, Privilege Separation, $privsep_objects, , no, [
  APR_SETVAR(MOD_PRIVSEP_LDADD, [-lpam])
])

APACHE_MODPATH_FINISH
