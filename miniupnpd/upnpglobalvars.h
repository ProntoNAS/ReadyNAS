/* $Id: upnpglobalvars.h,v 1.2 2007-01-16 03:42:47 jmaggard Exp $ */
/* project : MiniUPnP
 * website : http://miniupnp.free.fr
 * Author : Thomas Bernard
 * This software is subject to the conditions detailed in
 * the LICENCE file provided within this distribution */
#ifndef __UPNPGLOBALVARS_H__
#define __UPNPGLOBALVARS_H__

#include <time.h>

/* name of the network interface used to acces internet */
//extern const char * ext_if_name;

/* IP address of this interface */
//extern char ext_ip_addr[];

/* LAN address */
extern const char * listen_addr;

/* parameters to return to upnp client when asked */

//extern unsigned long downstream_bitrate;
//extern unsigned long upstream_bitrate;

/* log packets flag */
extern int logpackets;

/* statup time */
extern time_t startup_time;
/* use system uptime */
extern int sysuptime;

extern const char * pidfilename;

extern char uuidvalue[];

extern char hostname[64];
extern char vendor[32];
extern char model_name[32];
extern char model_num[32];
extern char serial[32];

#define PRESENTATIONURL_LEN (32)
extern char presentationurl[];

#endif

