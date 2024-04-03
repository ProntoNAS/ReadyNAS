/* $Id: upnpglobalvars.c,v 1.3 2007-11-07 03:31:38 jmaggard Exp $ */
/* project: MiniUPnP
 * Website: http://miniupnp.free.fr/
 * Author : Thomas Bernard
 * This software is subject to the conditions
 * detailed in the LICENCE file provided in this
 * distribution. */
#include <sys/types.h>
#include <netinet/in.h>
#include "upnpglobalvars.h"

/* network interface for internet */
//const char * ext_if_name = 0;

/* ip address of this interface */
//char ext_ip_addr[INET_ADDRSTRLEN];

/* LAN address */
const char * listen_addr = 0;

//unsigned long downstream_bitrate = 0;
//unsigned long upstream_bitrate = 0;

/* startup time */
time_t startup_time = 0;
/* use system uptime */
int sysuptime = 0;

/* log packets flag */
int logpackets = 0;

const char * pidfilename = "/var/run/miniupnpd.pid";

char uuidvalue[] = "uuid:UPnP-NAS-0000-0000-0000-000000000000";

char hostname[64];
char vendor[32] = "NETGEAR";
char model_name[32] = "ReadyNAS";
char model_num[32];
char serial[32] = "000da2000001";

/* presentation url :
 * http://nnn.nnn.nnn.nnn:ppppp/  => max 30 bytes including terminating 0 */
char presentationurl[PRESENTATIONURL_LEN];

