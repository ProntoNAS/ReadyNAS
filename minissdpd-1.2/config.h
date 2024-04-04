/* $Id: config.h,v 1.5 2012/03/05 19:36:25 nanard Exp $ */
/*  MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* use BSD daemon() ? */
#define USE_DAEMON

/* set the syslog facility to use. See man syslog(3) and syslog.conf(5). */
#define LOG_MINISSDPD	LOG_DAEMON

/* enable IPv6 */
#define ENABLE_IPV6

/* Maximum number of network interface we can listen on */
#define MAX_IF_ADDR 8

#endif
