/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * raidard.c is based on                                                    
 * iproxy-server.c                                            July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
 *
 * iproxy
 * UDP - TCP proxy
 * Copyright (C) 2001  Andrew Tridgell <tridge@samba.org>
 *                     Horms <horms@vergenet.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 **********************************************************************/



#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "util.h"
#include "raidard.h"
#include "packet.h"
#include "interfaces.h"
#include "interface.h"

static unsigned short raidard_port = RAIDARD_PORT;
static uint32 raidard_sid = RAIDARD_SID;
static unsigned short dest_port = WEB_PORT;
uint32 mcast_ttl = TTL_MULTICAST;
static char *xcast_ip = ADDR_BROADCAST;
unsigned short raidard_mode = MODE_BROADCAST|MODE_SERVER;
int raidard_verbose;
int ifaceCount=0;
int factory_test=0;
int quitting;

#define PIDFILE "/var/run/raidard.pid"
#define SELECT_TIMEOUT 600

static void do_proxy_server()
{
	struct packet packet;
	size_t n;
	struct sockaddr saddr;
        //struct in_addr xcast_addr;
        struct timeval tv;
	socklen_t socklen;
        fd_set read_template;
        int status;
        int fd;
	FILE *factory_setting;
	FILE *stop_timer;
        extern struct iface_struct iface[MAX_INTERFACES];
        extern int iface_count;
	while (!quitting) {
                  FD_ZERO(&read_template);

                  interface_add_to_read_template(&read_template);

                  tv.tv_sec=SELECT_TIMEOUT;
                  tv.tv_usec=0;

                  status=select(FD_SETSIZE, &read_template, NULL, NULL, &tv);
                  if(status<0){
                    if(errno!=EINTR){
                      perror("do_proxy: select");
                      exit(-1);
                    }
                    continue; /* Ignore EINTR */
                  }
                  else if(status==0){
                    continue; /* Skip to start of loop on timeout */
                  }

                  interface_set_fd_start();
                  while((fd=interface_set_fd_next(&read_template))>=0){
                    /* Read packet from UDP socket */
                    memset(&packet, '\0', sizeof(packet));
                    socklen = sizeof(saddr);
                    n = recvfrom(fd, &packet, sizeof(packet),
			     0, (struct sockaddr *)&saddr, &socklen);
                    if(raidard_verbose) 
                        printf("received request of size %d from sid=0x%x "
                                        "cid=0x%x\n", n, ntohl(packet.sid), 
                                        ntohl(packet.cid));
		   //inet_pton(AF_INET, xcast_ip, &xcast_addr); 
               //    if (ntohl(packet.sid) == RAIDARD_LIST_SID) break;
                    if (packet.version != htonl(RAIDARD_PROTO_VERSION)){
                            if(raidard_verbose)
                                printf("[%d] packet for unknown raidard_sid "
                                                "version\n", getpid());
                            continue;
                    }

                    if(htonl(packet.checksum) != packet_checksum(&packet)){
                            printf("checksum missmatch 0x%x!=0x%x\n",
                                            ntohl(packet.checksum),
                                            packet_checksum(&packet));
                            continue;
                    }

                    if (ntohl(packet.length) != n) continue;
                    if (ntohl(packet.sid) != RAIDARD_LIST_SID &&
                            ntohl(packet.sid) != raidard_sid) continue;
                    if (ntohl(packet.flag) == RAIDARD_LOCATE_FLAG) {
                       system("/frontview/bin/blink all");
                       continue;
                    }
		    if (ntohl(packet.flag) == RAIDARD_INSTALL_FLAG) {
                            if ((factory_setting = fopen ("/.os_factory_setting", "w")) != NULL)
                            {
                                fputs(packet.buf, factory_setting);
                                if (fclose(factory_setting) != 0)
                                {
                                   printf("Error closing input file: factory_setting\n");
                                }
                             }
			    
		    }
		    if (ntohl(packet.flag) == RAIDARD_STOPTIMER_FLAG) {
			    if((stop_timer = fopen ("/.os_stop_timer", "w")) != NULL)
				    
		            {
				 fputs(packet.buf, stop_timer);
                                 if (fclose(stop_timer) != 0)
                                {
                                   printf("Error closing input file: stop_timer\n");
                                }
                             }
			    
		    }
                    
                    if (ntohl(packet.sid) == RAIDARD_LIST_SID) {
		      int i;
                      int boot=0;
                      FILE *test_status;
		      FILE *test_start;
                      FILE *boot_status;
		      FILE *enclosure_log;
		      FILE *osversion_file;
		      char timestamp[12]="none"; 
		      char status[8]="none";
		      char os_version[60]="";
		      char sysInfo[3072]="";
                      char ip[INET6_ADDRSTRLEN] = "";
		      time_t  clock;
		      time(&clock);	  
                      if ((boot_status = fopen ("/.os_status", "r")) != NULL)
                      {
                              fgets(os_version,60,boot_status);
                              boot=1;
                              fclose(boot_status);
                      }

		      if ((test_status = fopen ("/tmp/.status", "r")) != NULL)
		      {
   	                       fgets(status,8,test_status);
			       fclose(test_status);
		      } 
		      if ((test_start = fopen ("/tmp/.start", "r")) != NULL)
		      {
			      fgets(timestamp,11,test_start);
			      fclose(test_start);
		      } 

                      if ((enclosure_log = fopen ("/var/log/frontview/enclosure.log", "r")) != NULL)
                      {
                              i = fread(&sysInfo, 1, sizeof(sysInfo)-1, enclosure_log);
                              sysInfo[i] = '\0';
			      fclose(enclosure_log);
                      }

                      if (boot ==0) 
                      {
                             if (access("/var/log/raidiator_version", F_OK) == 0) {
                                   osversion_file = fopen("/var/log/raidiator_version", "r");
                             } else {
                                   osversion_file = fopen ("/etc/raidiator_version", "r");
                             }
			     if (osversion_file !=NULL) {
			         fgets(os_version,60,osversion_file);
			         fclose(osversion_file);
			      }
		      }
		      
                      for (i=0;i<iface_count-1;i++) {
		          get_ip_str(&(iface[i].sa), ip);
			  if (factory_test == 1)
		          {
                               n = snprintf(packet.buf, sizeof(packet.buf),"%s\t%s\t%s\t%s\t%s\t%u\t%s\t%s\t%u\t%d",
				            iface[i].mac_addr,iface[i].host_name,ip,sysInfo,os_version,(unsigned int)clock,status,timestamp,raidard_sid, factory_test);
			  
                          } else if (boot == 1){
				  n = snprintf(packet.buf, sizeof(packet.buf),"%s\t%s\t%s\t%s\t%s\t%u\t%d\t%d", iface[i].mac_addr,iface[i].host_name, ip,sysInfo,os_version,raidard_sid,boot,boot); 
                          } else {
				  n = snprintf(packet.buf, sizeof(packet.buf),"%s\t%s\t%s\t%s\t%s\t%u", iface[i].mac_addr,iface[i].host_name, ip,sysInfo,os_version,raidard_sid); 
                          }
                          packet.length = htonl(n + PACKET_HDR_LEN);
                          
                          //saddr.sin_addr = xcast_addr;
                          packet_send(&saddr, &packet);
                      }
                     
                    }

                  }
                  interface_set_fd_end();

        }
}

static void usage(int exit_status)
{
	fprintf(exit_status?stderr:stdout, "\n"
"raidard       [options]\n"
"    -h            help\n"
"    -p PORT       listen port number (UDP)\n"
"    -d PORT       dest port (TCP)\n"
"    -s SID        server id number\n"
"    -I IP         set IP to send packets to\n"
"    -T TTL        ttl for multicast packets\n"
"    -b            Use broadcast (default)\n"
"    -m            Use multicast\n"
"    -u            Use unicast\n"
"    -v            increase verbosity\n"
"    -f            factory test flag\n"
"    -D            debug mode (run in foreground)\n"
);

	exit(exit_status);
}


/*static void dumper(int sig){
        signal(sig, (void(*)(int))dumper);
        connection_table_dump();
}*/

static void reread(int sig){
	extern int iface_fd[MAX_INTERFACES+1];
	extern int iface_count;
	int i;
	for (i=0;i<iface_count;i++) {
		if (iface_fd[i] < 0)
			continue;
		close(iface_fd[i]);
	}
	interfaces_get();
	interface_open_fd(ADDR_ANY, xcast_ip, raidard_port);
}

static void quit(int sig){
	printf("quitting on signal %d...\n", sig);
	unlink(PIDFILE);
	quitting = 1;
}


int main(int argc, char *argv[])
{
	int c;
	char *p;
        int addr_set=0;
	int daemonize=1;
	FILE *pidfile;

	while ((c = getopt(argc, argv, "s:I:t:bmup:d:fvhD")) != -1) {
		switch (c) {
		case 's':
			raidard_sid = atoi(optarg);
			break;
                case 'I':
                        xcast_ip = optarg;
                        addr_set++;
                        break;
                case 't':
                        mcast_ttl = atoi(optarg);
                        break;
                case 'b':
                        raidard_mode = MODE_BROADCAST|MODE_SERVER;
                        if(!addr_set)
                                xcast_ip = ADDR_BROADCAST;
                        break;
                case 'm':
                        raidard_mode = MODE_MULTICAST|MODE_SERVER;
                        if(!addr_set)
                                xcast_ip = ADDR_MULTICAST;
                        break;
                case 'u':
                        raidard_mode = MODE_UNICAST|MODE_SERVER;
                        if(!addr_set)
                                xcast_ip = ADDR_ANY;
                        break;
		case 'p':
			raidard_port = atoi(optarg);
			break;
		case 'd':
			if ((p = strchr(optarg,':'))) {
				*p = 0;
				dest_port = atoi(p+1);
			} else {
				dest_port = atoi(optarg);
			}
			break;
		case 'v':
			raidard_verbose++;
			break;
		case 'f':
			factory_test = 1;
			break;
		case 'h':
			usage(0);
                        break;
		case 'D':
			daemonize = 0;
			break;
		default:
			usage(-1);
			break;
		}
		
	}
	if( daemonize )
	        daemon(0, 0);
       /* if(connection_table_init(xcast_ip)) {
                fprintf(stderr, "main: connection_table_init\n");
                return(-1);
        }
        signal(SIGUSR1, (void(*)(int))dumper);*/
        signal(SIGHUP, (void(*)(int))reread);
        signal(SIGTERM, (void(*)(int))quit);
        signal(SIGINT, (void(*)(int))quit);

	/* Write out a pid file */
	pidfile = fopen(PIDFILE, "w");
	if(pidfile) {
		fprintf(pidfile, "%d\n", getpid());
		fclose(pidfile);
	}

        ifaceCount=interfaces_get();
        if(ifaceCount<0){
                exit(1);
        }
               

        /* We can just pass the mcast_ip as this
         * argument isn't used unless we are in multicast mode
         */
        if(interface_open_fd(ADDR_ANY, xcast_ip, raidard_port)){
                fprintf(stderr, "do_proxy: interface_open_fd");
                exit(-1);
        }

        do_proxy_server();
        return 0;
}
