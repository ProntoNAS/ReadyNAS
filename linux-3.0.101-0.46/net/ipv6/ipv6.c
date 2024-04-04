/*
 *	This is a hack. SLES 11 turns off ipv6 by blacklisting the ipv6
 *	module. However, since bonding, bridging and other drivers need bits of
 *	the ipv6 code, their modules would normally depend on ipv6. Thus, when
 *	ipv6 is blacklisted, these drivers cannot beloaded.
 *
 *	The original ipv6 module is now renamed to ipv6_lib, and drivers that
 *	depend on it can load it even if ipv6 is blacklisted. The ipv6_lib
 *	module will do nothing on module_init and all the initialization is
 *	performed only once this tiny ipv6 module is loaded.
 *
 *	Functions originally in the .init.text section need to be put in standard 
 *	.text in ipv6_lib because the initialization is now done after the module load.
 *
 *	Authors: Jiri Bohac <jbohac@suse.cz>
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the
 *	Free Software Foundation; either version 2 of the License, or (at your
 *	option) any later version.
 */

#include <linux/module.h>
#include <net/ipv6.h>

MODULE_AUTHOR("Cast of dozens");
MODULE_DESCRIPTION("IPv6 protocol stack for Linux");
MODULE_LICENSE("GPL");

module_param_named(disable, disable_ipv6_mod, int, 0444);
MODULE_PARM_DESC(disable, "Disable IPv6 module such that it is non-functional");

module_param_named(disable_ipv6, ipv6_defaults.disable_ipv6, int, 0444);
MODULE_PARM_DESC(disable_ipv6, "Disable IPv6 on all interfaces");

module_param_named(autoconf, ipv6_defaults.autoconf, int, 0444);
MODULE_PARM_DESC(autoconf, "Enable IPv6 address autoconfiguration on all interfaces");

void inet6_exit_real(void);
int inet6_init_real(void);

static int  ipv6_init(void)
{
        __module_get(THIS_MODULE); /* ipv6 cannot be unloaded */
	return inet6_init_real();
}
module_init(ipv6_init);

static void __exit ipv6_exit(void)
{
	inet6_exit_real();
}
module_exit(ipv6_exit);

MODULE_ALIAS_NETPROTO(PF_INET6);

