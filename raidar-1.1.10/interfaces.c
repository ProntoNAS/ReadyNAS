/* 
  Unix SMB/Netbios implementation.   Version 2.0
  return a list of network interfaces
   Copyright (C) Andrew Tridgell 1998
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* This file is modified according to Richard Stevens's ideas.
   this code can be called directly from the autoconf tests. That also means
   this code cannot use any of the normal Samba debug stuff or defines.
   This is standalone code.
*/
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <net/if.h>
#include <sys/sysctl.h>
#include <netinet/if_ether.h>
#ifndef SIOCGIFCONF
#include <sys/sockio.h>
#endif
#define MAX_INTERFACES 16

#if __MACH__
#include <net/if_dl.h>
#endif
#define fatal_perror(X) do { perror(X), exit(1); } while(0)


#include "config.h"
#include "interfaces.h"


#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

int get_interfaces(struct iface_struct *ifaces)
{  
	struct ifconf ifc = {0};
	char buff[8192] = {0};
	int fd, i, n;
	struct ifreq *ifr=NULL;
	struct ifreq *item;
	int total = 0;
	char macAddr[18];

	if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		fatal_perror("socket");
		return -1;
	}

	ifc.ifc_len = sizeof(buff);
	ifc.ifc_buf = buff;

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
		fatal_perror("ioctl(SIOCGIFCONF)");
		close(fd);
		return -1;
	} 

	ifr = ifc.ifc_req;

	n = ifc.ifc_len / sizeof(struct ifreq);
	/* Loop through interfaces, looking for given IP address */
	for (i=n-1; i>= 0 && total < MAX_INTERFACES; i--) {
		item = &ifr[i]; 
		ifaces[total].sa =item->ifr_addr;

		if (ioctl(fd, SIOCGIFADDR, item) < 0) {
			fatal_perror("ioctl(OSIOCGIFADDR)");
			continue;
		}
		if(ioctl(fd, SIOCGIFFLAGS, item) < 0 ||
		   ifr[i].ifr_ifru.ifru_flags & IFF_LOOPBACK)
			continue;
		if(strcmp(ifr[i].ifr_name, "LeafNets") == 0 ||
		   strcmp(ifr[i].ifr_name, "tap0") == 0 ||
		   strncmp(ifr[i].ifr_name, "zt", 2) == 0 ||
		   strncmp(ifr[i].ifr_name, "veth", 4) == 0 ||
		   strncmp(ifr[i].ifr_name, "docker", 6) == 0)
			continue;
		if (ioctl(fd, SIOCGIFBRDADDR, item) != 0) {
			continue;
		} else {
			if (item->ifr_broadaddr.sa_family == AF_INET) {
				ifaces[total].broadaddr =item->ifr_broadaddr;
			}
		}
		if (ioctl(fd, SIOCGIFHWADDR, item) < 0) {
			fatal_perror("ioctl(SIOCGIFHWADDR)");		
			continue;
		} else {
			snprintf(macAddr, sizeof(macAddr),"%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned char)item->ifr_hwaddr.sa_data[0],
						(unsigned char)item->ifr_hwaddr.sa_data[1],
						(unsigned char)item->ifr_hwaddr.sa_data[2],
						(unsigned char)item->ifr_hwaddr.sa_data[3],
						(unsigned char)item->ifr_hwaddr.sa_data[4],
						(unsigned char)item->ifr_hwaddr.sa_data[5]);			     
	
		}
		strcpy(ifaces[total].mac_addr, macAddr);
		total++;
	}
	close(fd);

	return total;
}  

char *get_ip_str(const struct sockaddr *sa, char *ip)
{
	switch(sa->sa_family) {
	case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), ip, INET6_ADDRSTRLEN);
		break;

	case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),ip,INET6_ADDRSTRLEN);
		break;
	default:
		return NULL;
	}

	return ip;
}

int
open_netlink_sock(void)
{
#ifdef __linux__
	struct sockaddr_nl addr;
	int s;
	int ret;

	s = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (s < 0)
	{
		perror("couldn't open NETLINK_ROUTE socket");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_IPV4_IFADDR;

	ret = bind(s, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		perror("couldn't bind");
		close(s);
		return -1;
	}

	return s;
#else
	return -1;
#endif
}

/*****for test only
int main()
{
	struct iface_struct ifaces[MAX_INTERFACES];
	int total = get_interfaces(ifaces);
	int i;
	char  ip[INET6_ADDRSTRLEN] = {0};
	printf("got %d interfaces:\n", total);
	if (total <= 0) exit(1);

	for (i=0;i<total;i++) {
		//printf("hostname=%s ", ifaces[i].host_name);
		printf("IP=%s ", get_ip_str(&(ifaces[i].sa), ip));
		printf("Mac=%s ", ifaces[i].mac_addr);
		printf("BroadAddress=%s\n", get_ip_str(&(ifaces[i].broadaddr), ip));
	}
	return 0;
}*/ 

