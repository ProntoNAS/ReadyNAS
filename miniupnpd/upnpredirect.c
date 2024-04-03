/* $Id: upnpredirect.c,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $ */
/* miniupnp project : http://miniupnp.free.fr/
 * 
 * (c) 2006 Thomas Bernard
 * this software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution
 */
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "upnpredirect.h"
#include "upnpglobalvars.h"
#if defined(__linux__)
#include "linux/iptcrdr.h"
#else
#include "openbsd/obsdrdr.h"
#endif

static int proto_atoi(const char * protocol)
{
	int proto = IPPROTO_TCP;
	if(strcmp(protocol, "UDP") == 0)
		proto = IPPROTO_UDP;
	return proto;
}

int
upnp_redirect(unsigned short eport, 
              const char * iaddr, unsigned short iport,
              const char * protocol, const char * desc)
{
	int proto, r;
	char iaddr_old[32];
	unsigned short iport_old;
	proto = proto_atoi(protocol);

	r = get_redirect_rule(ext_if_name, eport, proto,
	                      iaddr_old, sizeof(iaddr_old), &iport_old, 0);
	if(r==0)
	{
		syslog(LOG_INFO, "port %hu protocol %s allready redirected to %s:%hu",
	    	             eport, protocol, iaddr_old, iport_old);
		return -1;
	}
	else
	{
		syslog(LOG_INFO, "redirecting port %hu to %s:%hu protocol %s for: %s",
	    	             eport, iaddr, iport, protocol, desc);
		add_redirect_rule2(ext_if_name, eport, iaddr, iport, proto, desc);
		syslog(LOG_INFO, "create pass rule to %s:%hu protocol %s for: %s",
	    	             iaddr, iport, protocol, desc);
		add_filter_rule2(ext_if_name, iaddr, eport, proto, desc);
	}

	return 0;
}

int
upnp_get_redirection_infos(unsigned short eport, const char * protocol,
                           unsigned short * iport,
                           char * iaddr, int iaddrlen,
                           const char * * desc)
{
	int proto, r;
	static char desc_buf[64];
	desc_buf[0] = '\0';
	proto = proto_atoi(protocol);
	r = get_redirect_rule(ext_if_name, eport, proto,
	                      iaddr, iaddrlen, iport, desc_buf);
	*desc = desc_buf;/*"miniupnpd";*/
	return r;
}

int
upnp_get_redirection_infos_by_index(int index,
                                    unsigned short * eport, char * protocol,
                                    unsigned short * iport, 
                                    char * iaddr, int iaddrlen,
                                    const char * * desc)
{
	char ifname[IFNAMSIZ];
	int proto = 0;
	static char desc_buf[64];
	desc_buf[0] = '\0';
	if(get_redirect_rule_by_index(index, ifname, eport, iaddr, iaddrlen,
	                              iport, &proto, desc_buf) < 0)
		return -1;
	else
	{
		if(proto == IPPROTO_TCP)
			memcpy(protocol, "TCP", 4);
		else
			memcpy(protocol, "UDP", 4);
		*desc = desc_buf;/*"miniupnpd";*/
		return 0;
	}
}

int
upnp_delete_redirection(unsigned short eport, const char * protocol)
{
	int proto, r;
	proto = proto_atoi(protocol);
	syslog(LOG_INFO, "removing redirect rule port %hu %s", eport, protocol);
#if defined(__linux__)
	r = delete_redirect_and_filter_rules(eport, proto);
#else
	r = delete_redirect_rule(ext_if_name, eport, proto);
	delete_filter_rule(ext_if_name, eport, proto);
#endif
	return r;
}

