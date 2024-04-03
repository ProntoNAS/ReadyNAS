/* $Id: upnpredirect.h,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $ */
/* (c) 2006 Thomas Bernard */
#ifndef __UPNPREDIRECT_H__
#define __UPNPREDIRECT_H__

/* upnp_redirect() calls OS/fw dependant implementation
 * of the redirection.
 * protocol should be the string "TCP" or "UDP"
 * returns 0 on success.
 */
int
upnp_redirect(unsigned short eport, 
              const char * iaddr, unsigned short iport,
              const char * protocol, const char * desc);

int
upnp_get_redirection_infos(unsigned short eport, const char * protocol,
                           unsigned short * iport, char * iaddr, int iaddrlen,
                           const char * * desc);

int
upnp_get_redirection_infos_by_index(int index,
                                    unsigned short * eport, char * protocol,
                                    unsigned short * iport, 
                                    char * iaddr, int iaddrlen,
                                    const char * * desc);

int
upnp_delete_redirection(unsigned short eport, const char * protocol);

#endif


