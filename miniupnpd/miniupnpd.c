/* project: miniUPnP
 * webpage: http://miniupnp.free.fr/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in the distribution */
/* Modified 2007/01/15 by Justin Maggard at Infrant Technologies.
 * Changes made:
 *  - Set up some default config options by reading system information.
 *  - Strip out some IGW code, since this is advertising a more simple device.
 *  - Remove many of the syslog messages to reduce clutter.
 *  - Automatically fill in our IP address if none is specified.
 */
/* $Id: miniupnpd.c,v 1.11 2009-09-05 01:15:54 jmaggard Exp $ */
/* system or libc include : */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
/* for BSD's sysctl */
#include <sys/param.h>
#include <sys/sysctl.h>

/* miniupnp includes : */
#include "upnpglobalvars.h"
#include "upnphttp.h"
#include "upnpdescgen.h"
#include "miniupnpdpath.h"
#include "getifaddr.h"
#include "upnpsoap.h"

/* ip et port pour le SSDP */
#define PORT (1900)
#define UPNP_MCAST_ADDR ("239.255.255.250")

#define MAX_ADD_LISTEN_ADDR (4)

static volatile int quitting = 0;

static int AddMulticastMembership(int s, const char * ifaddr)
{
	struct ip_mreq imr;	/* Ip multicast membership */
	/*
	 * setting up imr structure
	 */
	imr.imr_multiaddr.s_addr = inet_addr(UPNP_MCAST_ADDR);
	/*imr.imr_interface.s_addr = htonl(INADDR_ANY);*/
	imr.imr_interface.s_addr = inet_addr(ifaddr);
	
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP|SO_REUSEADDR, (void *)&imr, sizeof(struct ip_mreq)) < 0 &&
	    (errno != EADDRINUSE))
	{
		syslog(LOG_ERR, "setsockopt - IP_ADD_MEMBERSHIP: %m");
		return -1;
	}
	return 0;
}

static int OpenAndConfUdpSocket(const char * ifaddr)
{
	int s;
	struct sockaddr_in sockname;
	int i = 1;
	
	s = socket(PF_INET, SOCK_DGRAM, 0);
	
        if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
        {
                syslog(LOG_WARNING, "setsockopt(udp, SO_REUSEADDR): %m");
        }

	memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(PORT);
	/* NOTE : it seems it doesnt work when binding on the specific address */
    /*sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);*/
    sockname.sin_addr.s_addr = htonl(INADDR_ANY);
    /*sockname.sin_addr.s_addr = inet_addr(ifaddr);*/

    if (bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
	{
		syslog(LOG_ERR, "bind(udp): %m");
		close(s);
		return -1;
    }

	if(AddMulticastMembership(s, ifaddr) < 0)
	{
		close(s);
		return -1;
	}

	return s;
}

/* open the UDP socket used to send SSDP notifications to
 * the multicast group reserved for them */
static int OpenAndConfNotifySocket(const char * addr)
{
	int s;
	unsigned char loopchar = 0;
	struct in_addr mc_if;
	struct sockaddr_in sockname;
	
	s = socket(PF_INET, SOCK_DGRAM, 0);
	mc_if.s_addr = inet_addr(addr);

	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopchar, sizeof(loopchar)) < 0)
	{
		syslog(LOG_ERR, "setsockopt - IP_MULTICAST_LOOP: %m");
		close(s);
		return -1;
	}

	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&mc_if, sizeof(mc_if)) < 0)
	{
		syslog(LOG_ERR, "setsockopt - IP_MULTICAST_IF: %m");
		close(s);
		return -1;
	}

	memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(50001);
    sockname.sin_addr.s_addr = inet_addr(addr);

    if (bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
	{
		syslog(LOG_ERR, "bind(udp): %m");
		close(s);
		return -1;
    }
	return s;
}

static int OpenAndConfHTTPSocket(const char * addr, unsigned short port)
{
	int s;
	int i = 1;
	struct sockaddr_in listenname;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if(s<0)
	{
		syslog(LOG_ERR, "socket(http): %m");
		return s;
	}
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
	{
		syslog(LOG_WARNING, "setsockopt(http, SO_REUSEADDR): %m");
	}
	memset(&listenname, 0, sizeof(struct sockaddr_in));
	listenname.sin_family = AF_INET;
	listenname.sin_port = htons(port);
	listenname.sin_addr.s_addr = inet_addr(addr);
	if(bind(s, (struct sockaddr *)&listenname, sizeof(struct sockaddr_in)) < 0)
	{
		syslog(LOG_ERR, "bind(http): %m");
		return -1;
	}
	if(listen(s, 6) < 0)
	{
		syslog(LOG_ERR, "listen(http): %m");
		return -1;
	}
	return s;
}

/*
 * response from a LiveBox (Wanadoo)
HTTP/1.1 200 OK
CACHE-CONTROL: max-age=1800
DATE: Thu, 01 Jan 1970 04:03:23 GMT
EXT:
LOCATION: http://192.168.0.1:49152/gatedesc.xml
SERVER: Linux/2.4.17, UPnP/1.0, Intel SDK for UPnP devices /1.2
ST: upnp:rootdevice
USN: uuid:75802409-bccb-40e7-8e6c-fa095ecce13e::upnp:rootdevice

 * response from a Linksys 802.11b :
HTTP/1.1 200 OK
Cache-Control:max-age=120
Location:http://192.168.5.1:5678/rootDesc.xml
Server:NT/5.0 UPnP/1.0
ST:upnp:rootdevice
USN:uuid:upnp-InternetGatewayDevice-1_0-0090a2777777::upnp:rootdevice
EXT:
 */

/* not really an SSDP "announce" as it is the response
 * to a SSDP "M-SEARCH" */
static void SendSSDPAnnounce2(int s, struct sockaddr_in sockname,
                              const char * st, int st_len,
							  const char * host, unsigned short port)
{
	int l, n;
	char buf[512];
	/* TODO :
	 * follow guideline from document "UPnP Device Architecture 1.0"
	 * put in uppercase.
	 * DATE: is recommended
	 * SERVER: OS/ver UPnP/1.0 miniupnpd/1.0
	 * */
	l = snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
		"Cache-Control: max-age=120\r\n"
		"ST: %.*s\r\n"
		"USN: %s::%.*s\r\n"
		"EXT:\r\n"
		"Server: " MINIUPNPD_SERVER_STRING "\r\n"
		"Location: http://%s:%u" ROOTDESC_PATH "\r\n"
		"\r\n",
		st_len, st,
		uuidvalue, st_len, st,
		host, (unsigned int)port);
	n = sendto(s, buf, l, 0,
	           (struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
#if 0 //JM: Don't fill up syslog, even in error condition
	if(n<0)
	{
		syslog(LOG_ERR, "sendto: %m");
	}
#endif
}

static const char * const known_service_types[] =
{
	"upnp:rootdevice",
#if 0 //JM: We don't really do anything for these
	"urn:schemas-upnp-org:device:InternetGatewayDevice:",
	"urn:schemas-upnp-org:device:WANConnectionDevice:",
	"urn:schemas-upnp-org:device:WANDevice:",
	"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:",
	"urn:schemas-upnp-org:service:WANIPConnection:",
	"urn:schemas-upnp-org:service:WANPPPConnection:",
	"urn:schemas-upnp-org:service:Layer3Forwarding:",
#endif
	0
};

static void SendSSDPNotifies(int s, const char * host, unsigned short port)
{
	struct sockaddr_in sockname;
	int n,i=0;
	char bufr[512];

	memset(&sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

	while(known_service_types[i])
	{
		snprintf(bufr, sizeof(bufr), 
				"NOTIFY * HTTP/1.1\r\n"
				"HOST:%s:%d\r\n"
				"Cache-Control:max-age=120\r\n"
				"Location:http://%s:%d" ROOTDESC_PATH"\r\n"
				/*"Server:miniupnpd/1.0 UPnP/1.0\r\n"*/
				"Server: " MINIUPNPD_SERVER_STRING "\r\n"
				"NT:%s%s\r\n"
				"USN:%s::%s%s\r\n"
				"NTS:ssdp:alive\r\n"
				"\r\n",
				UPNP_MCAST_ADDR, PORT,
				host, port,
				known_service_types[i], (i==0?"":"1"),
				uuidvalue, known_service_types[i], (i==0?"":"1") );
		n = sendto(s, bufr, strlen(bufr), 0,
			(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
#if 0 //JM: Don't fill up syslog, even in error condition
		if(n<0)
		{
			syslog(LOG_ERR, "sendto: %m");
		}
#endif
		i++;
	}

}

void ProcessSSDPRequest(int s, const char * host, unsigned short port)
{
	int n;
	char bufr[2048];
	socklen_t len_r;
	struct sockaddr_in sendername;
	int i, l;
	char * st = 0;
	int st_len = 0;
	len_r = sizeof(struct sockaddr_in);
	n = recvfrom(s, bufr, sizeof(bufr), 0,
	             (struct sockaddr *)&sendername, &len_r);
	if(n<0)
	{
		syslog(LOG_ERR, "recvfrom: %m");
		return;
	}
	if(memcmp(bufr, "NOTIFY", 6) == 0)
	{
		/* ignore NOTIFY packets. We could log the sender and device type */
		return;
	}
	else if(memcmp(bufr, "M-SEARCH", 8) == 0)
	{
		i = 0;
		while(i<n)
		{
			while(bufr[i] != '\r' || bufr[i+1] != '\n')
				i++;
			i += 2;
			if(strncasecmp(bufr+i, "st:", 3) == 0)
			{
				st = bufr+i+3;
				st_len = 0;
				while(*st == ' ' || *st == '\t') st++;
				while(st[st_len]!='\r' && st[st_len]!='\n') st_len++;
				/*syslog(LOG_INFO, "ST: %.*s", st_len, st);*/
				/*j = 0;*/
				/*while(bufr[i+j]!='\r') j++;*/
				/*syslog(LOG_INFO, "%.*s", j, bufr+i);*/
			}
		}
		/*syslog(LOG_INFO, "SSDP M-SEARCH packet received from %s:%d",
	           inet_ntoa(sendername.sin_addr),
	           ntohs(sendername.sin_port) );*/
		if(st)
		{
			/* TODO : doesnt answer at once but wait for a random time */
			/*syslog(LOG_INFO, "ST: %.*s", st_len, st);*/
			//JM syslog(LOG_INFO, "SSDP M-SEARCH from %s:%d ST: %.*s",
	        	//JM   inet_ntoa(sendername.sin_addr),
	           	//JM   ntohs(sendername.sin_port),
			//JM	   st_len, st);
			i = 0;
			while(known_service_types[i])
			{
				l = (int)strlen(known_service_types[i]);
				if(l<=st_len && (0 == memcmp(st, known_service_types[i], l)))
				{
					SendSSDPAnnounce2(s, sendername, st, st_len, host, port);
					break;
				}
				i++;
			}
			l = (int)strlen(uuidvalue);
			if(l==st_len && (0 == memcmp(st, uuidvalue, l)))
			{
				SendSSDPAnnounce2(s, sendername, st, st_len, host, port);
			}
		}
		else
		{
			//JM syslog(LOG_INFO, "invalid SSDP M-SEARCH from %s:%d",
	        	//JM    inet_ntoa(sendername.sin_addr),
	           	//JM    ntohs(sendername.sin_port) );
		}
	}
	else
	{
		syslog(LOG_NOTICE, "Unknown udp packet received from %s:%d",
		       inet_ntoa(sendername.sin_addr),
			   ntohs(sendername.sin_port) );
	}
}

/* Handler for the SIGTERM signal (kill) */
void sigterm(int sig)
{
	/*int save_errno = errno;*/
	signal(sig, SIG_IGN);

	syslog(LOG_NOTICE, "received signal %d, exiting", sig);

	quitting = 1;
	/*errno = save_errno;*/
}


/* This will broadcast ssdp:byebye notifications to inform 
 * the network that UPnP is going down. */
void miniupnpdShutdown()
{
	struct sockaddr_in sockname;
    	int n,i=0;
    	char bufr[512];
	int s;
	struct in_addr local_if;

	s = socket(PF_INET, SOCK_DGRAM, 0);

	i = 1;
        if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
        {
                syslog(LOG_WARNING, "setsockopt(udp, SO_REUSEADDR): %m");
        }
	i = 0;

	local_if.s_addr = inet_addr(listen_addr);
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&local_if, sizeof(local_if)) < 0)
	{
		syslog(LOG_ERR, "setsockopt - IP_MULTICAST_IF: %m");
	}

    memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_addr.s_addr = inet_addr(listen_addr);/*INADDR_ANY;*/
	if(bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
		syslog(LOG_ERR, "bind: %m");

    memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(PORT);
    sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

    while(known_service_types[i])
    {
        snprintf(bufr, sizeof(bufr),
                 "NOTIFY * HTTP/1.1\r\n"
                 "HOST:%s:%d\r\n"
                 "NT:%s%s\r\n"
                 "USN:%s::%s%s\r\n"
                 "NTS:ssdp:byebye\r\n"
                 "\r\n",
                 UPNP_MCAST_ADDR, PORT,
				 known_service_types[i], (i==0?"":"1"),
                 uuidvalue, known_service_types[i], (i==0?"":"1"));
        n = sendto(s, bufr, strlen(bufr), 0,
                   (struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
		if(n<0)
		{
			syslog(LOG_ERR, "shutdown: sendto: %m");
		}
        i++;
    }
	close(s);
	if(unlink(pidfilename) < 0)
	{
		syslog(LOG_ERR, "failed to remove %s : %m", pidfilename);
	}
	closelog();
	exit(0);
}

/* Write the pid to a file */
static void
writepidfile(const char * fname, int pid)
{
	char *pidstring;
	int pidstringlen;
	int pidfile;

	if(!fname || (strlen(fname) == 0))
		return;
	
	pidfile = open(fname, O_WRONLY|O_CREAT, 0666);
	if(pidfile < 0)
	{
		syslog(LOG_ERR, "Unable to write to pidfile %s: %m", fname);
	}
	else
	{
		pidstringlen = asprintf(&pidstring, "%d\n", pid);
		if(pidstringlen < 0)
		{
			syslog(LOG_ERR,
			       "asprintf failed, Unable to write to pidfile %s",
				   fname);
		}
		else
		{
			write(pidfile, pidstring, pidstringlen);
			free(pidstring);
		}
		close(pidfile);
	}
}

int getinfo(char *val, char *which, int word) {
	FILE *boot_info;
	char buffer[501];
	char item[16];
	char value[32];

	if( !(boot_info = fopen("/proc/sys/dev/boot/info", "r")) ) {
		return(0);
	}

	fgets(buffer, 500, boot_info);
	while (!feof(boot_info)) {
		if (sscanf(buffer, "%[^:]:\t%[^\n]", item, value)) {
			if(strcmp(item,which) == 0) {
				fclose(boot_info);
				if(word == 1) {
					sscanf(value, "%s", value);
				}
				else if(word == 2) {
					sscanf(value, "%s %s", item, value);
				}
				strncpy(val,value,32);
				return(1);
			}
		}
		fgets(buffer, 500, boot_info);
	}
	fclose(boot_info);
	return(0);
}

#include <net/if.h>
#include <sys/ioctl.h>
char mac_addr[16];
void get_mac_address(char *iface) {
        unsigned char *mac = malloc(8);
        struct ifreq ifr;
        int fd;

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if( fd ) {
                strcpy(ifr.ifr_name, iface);
                if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
                        memmove( mac, ifr.ifr_hwaddr.sa_data, 6);
                close(fd);
        }
        sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        free(mac);
}

/* === main === */
/* call procession of HTTP or SSDP requests */
int main(int argc, char * * argv)
{
	int i;
	int pid;
	int sudp, shttpl, snotify;
	int port = -1;
	LIST_HEAD(httplisthead, upnphttp) upnphttphead;
	struct upnphttp * e = 0;
	struct upnphttp * next;
	fd_set readset;	/* for select() */
	const char * logfilename = 0;
	char logname[16] = {'\0'};
	const char * add_listen_addr[MAX_ADD_LISTEN_ADDR];
//	const char * use_ext_ip_addr = 0;	/* forced external IP address to use */
	int n_add_listen_addr = 0;
	int notify_interval = 30;	/* seconds between SSDP announces */
	struct timeval timeout, timeofday, lasttimeofday = {0, 0};
	int debug_flag = 0;
	int openlog_option;
	struct sigaction sa;
	const char * presurl = 0;
	char *buf;

	/* command line arguments processing */
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			if( strncmp(argv[i], "eth", 3) == 0 ||
			    strncmp(argv[i], "bond", 4) == 0 )
			{
				/* A network interface was specified, so figure out its IP */
				asprintf(&buf, "/var/run/upnpd-%s.pid", argv[i]);
				pidfilename = buf;
				sprintf(logname, "upnpd(%s)", argv[i]);
				buf = calloc(1, INET_ADDRSTRLEN);
				if(getifaddr(argv[i], buf, INET_ADDRSTRLEN) < 0)
				{
					asprintf(&buf, "%s: Could not get interface info. EXITING\n", argv[i]);
					fprintf(stderr, "%s", buf);
					syslog(LOG_ERR, buf);
					return 1;
				}
				add_listen_addr[n_add_listen_addr++] = buf;
				listen_addr = buf;
			}
			else
			{
				fprintf(stderr, "%s: Unknown option\n", argv[i]);
			}
		}
		else switch(argv[i][1])
		{
		case 'o':
			//use_ext_ip_addr = argv[++i];
			break;
		case 't':
			notify_interval = atoi(argv[++i]);
			break;
		case 'u':
			strncpy(uuidvalue+5, argv[++i], strlen(uuidvalue+5) + 1);
			break;
		case 'U':
			sysuptime = 1;
			break;
		case 'l':
			logfilename = argv[++i];
			break;
		case 'L':
			logpackets = 1;
			break;
		case 'i':
			//ext_if_name = argv[++i];
			break;
		case 'p':
			port = atoi(argv[++i]);
			break;
		case 'P':
			pidfilename = argv[++i];
			break;
		case 'd':
			debug_flag = 1;
			break;
		case 'w':
			presurl = argv[++i];
			break;
		case 'B':
			//downstream_bitrate = strtoul(argv[++i], 0, 0);
			//upstream_bitrate = strtoul(argv[++i], 0, 0);
			break;
		case 'a':
			i++;
			if(listen_addr)
			{
				if(n_add_listen_addr < MAX_ADD_LISTEN_ADDR)
					add_listen_addr[n_add_listen_addr++] = argv[i];
				else
					fprintf(stderr,
				            "Too many listening ips, ignoring %s\n",
					        argv[i]);
			}
			else
				listen_addr = argv[i];
			break;
		default:
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
	}
	//if(!ext_if_name || !listen_addr || port<=0)
	if(!listen_addr)
	{
		char hn[32];
		gethostname(hn, sizeof(hn));
		struct hostent *ht = gethostbyname(hn);
		if(ht->h_addr_list[0] != NULL) {
			listen_addr = inet_ntoa(*((struct in_addr *)ht->h_addr_list[0]));
		}
	}

	if(!listen_addr)
	{
		fprintf(stderr, "Usage:\n\t"
		        "%s -a listening_ip -p port "
				/*"[-l logfile] " not functionnal */
				"[-u uuid] [-t notify interval] "
				"[-P pidfilename] [-d] [-L] [-U] [-B down up] "
				"[-w url]\n"
		        "\nNotes:\n\tThere can be one or several listening_ips\n"
		        "\tNotify interval is in seconds. Default is 30sec.\n"
				"\tDefault pid file is %s\n"
				"\tWith -d option miniupnpd will run as a standard program\n"
				"\t-L option set packet log in pf on.\n"
				"\t-U option makes miniupnpd report system uptime instead "
				"of daemon uptime.\n"
				//"\t-B option sets bitrates reported by daemon in Bytes "
				"per second.\n"
				"\t-w sets the presentation url. Default is http address on port 80\n",
		        argv[0], pidfilename);
		return 1;
	}
	if(port<=0) {
		port = 50000;
	}

	if(debug_flag)
	{
		pid = getpid();
	}
	else
	{
		daemon(0, 0);
		pid = getpid();
	}

	/* TODO : change LOG_LOCAL0 to LOG_DAEMON */
	openlog_option = LOG_PID|LOG_CONS;
	if(debug_flag)
		openlog_option |= LOG_PERROR;	/* also log on stderr */
	if( !*logname )
		strcpy(logname, "upnpd");
	openlog(logname, openlog_option, LOG_DAEMON);

	writepidfile(pidfilename, pid);

	/* record the startup time, for returning uptime */
	startup_time = time(NULL);
	if(sysuptime)
	{
		/* use system uptime instead of daemon uptime */
#if defined(__linux__)
		char buff[64];
		int uptime, fd;
		fd = open("/proc/uptime", O_RDONLY);
		if(fd<0)
		{
			syslog(LOG_ERR, "open(\"/proc/uptime\" : %m");
		}
		else
		{
			memset(buff, 0, sizeof(buff));
			read(fd, buff, sizeof(buff) - 1);
			uptime = atoi(buff);
			//JM syslog(LOG_INFO, "system uptime is %d seconds", uptime);
			close(fd);
			startup_time -= uptime;
		}
#else
		struct timeval boottime;
		size_t size = sizeof(boottime);
		int name[2] = { CTL_KERN, KERN_BOOTTIME };
		if(sysctl(name, 2, &boottime, &size, NULL, 0) < 0)
		{
			syslog(LOG_ERR, "sysctl(\"kern.boottime\") failed");
		}
		else
		{
			startup_time = boottime.tv_sec;
		}
#endif
	}

#if 0
	if(use_ext_ip_addr)
	{
		strncpy(ext_ip_addr, use_ext_ip_addr, INET_ADDRSTRLEN);
		syslog(LOG_INFO, "using %s as external address (for interface %s)",
		       ext_ip_addr, ext_if_name);
	}
	else if(getifaddr(ext_if_name, ext_ip_addr, INET_ADDRSTRLEN) < 0)
	{
		syslog(LOG_ERR, "Failed to get ip address for interface %s. EXITING",
			ext_if_name);
		return 1;
	}
#endif
	syslog(LOG_INFO, "Listening on %s:%d", listen_addr, port);

	/* presentation url */
	if(presurl)
	{
		strncpy(presentationurl, presurl, PRESENTATIONURL_LEN);
		presentationurl[PRESENTATIONURL_LEN-1] = '\0';
	}
	else
	{
		snprintf(presentationurl, PRESENTATIONURL_LEN,
		         "https://%s/admin/", listen_addr);
	}

	getinfo(vendor,"vendor",0);
	getinfo(model_name,"model",1);
	getinfo(model_num,"model",2);
	strcat(hostname,model_name);
	strcat(hostname," ");
	strcat(hostname,model_num);
	strcat(hostname," [");
	gethostname(hostname+strlen(hostname), 32);
	strcat(hostname,"]");
	getinfo(serial,"serial",0);
	get_mac_address("eth0");
	if(strlen(serial) <= 4)
		strcpy(serial, mac_addr);
	sprintf(uuidvalue, "uuid:a524ec76-0b8c-4f26-ac61-%s", mac_addr);
	//strncpy(uuidvalue+14, serial, strlen(uuidvalue+14) + 1);

	if( !strcasecmp(vendor,"Infrant") || !strlen(vendor) ) {
		strcpy(vendor, "NETGEAR");
	}
	//JM syslog(LOG_INFO, "hostname = %s", hostname);
	//JM syslog(LOG_INFO, "vendor = %s", vendor);
	//JM syslog(LOG_INFO, "model = %s %s", model_name, model_num);
	//JM syslog(LOG_INFO, "serial = %s", serial);
	//JM syslog(LOG_INFO, "uuid = %s", uuidvalue);

	LIST_INIT(&upnphttphead);
	/* socket d'ecoute pour le SSDP */
	sudp = OpenAndConfUdpSocket(listen_addr);
	if(sudp < 0)
	{
		syslog(LOG_ERR, "Failed to open socket for SSDP. EXITING");
		return 1;
	}
	while(n_add_listen_addr>0)
	{
		n_add_listen_addr--;
		if(AddMulticastMembership(sudp, add_listen_addr[n_add_listen_addr]) < 0)
			syslog(LOG_WARNING,
			       "Failed to add membership for address %s", 
			       add_listen_addr[n_add_listen_addr] );
	}
	/* socket d'ecoute des connections HTTP */
	shttpl = OpenAndConfHTTPSocket(listen_addr, port);
	if(shttpl < 0)
	{
		syslog(LOG_ERR, "Failed to open socket for HTTP. EXITING");
		return 1;
	}
	/* open socket for sending notifications */
	snotify = OpenAndConfNotifySocket(listen_addr);
	if(snotify < 0)
	{
		syslog(LOG_ERR, "Failed to open socket for SSDP notify messages");
	}
	/* set signal handler */
	memset(&sa, 0, sizeof(struct sigaction));

	sa.sa_handler = sigterm;
	if (sigaction(SIGTERM, &sa, NULL))
	{
		syslog(LOG_ERR, "Failed to set SIGTERM handler. EXITING");
		return 1;
	}
	if (sigaction(SIGINT, &sa, NULL))
	{
		syslog(LOG_ERR, "Failed to set SIGTERM handler. EXITING");
		return 1;
	}
	while (!quitting)
	{
		/* Check if we need to send SSDP NOTIFY messages and do it if
		 * needed */
		if(gettimeofday(&timeofday, 0) < 0)
		{
			syslog(LOG_ERR, "gettimeofday: %m");
			timeout.tv_sec = notify_interval;
			timeout.tv_usec = 0;
		}
		else
		{
			/* the comparaison is not very precise but who cares ? */
			if(timeofday.tv_sec >= (lasttimeofday.tv_sec + notify_interval))
			{
				SendSSDPNotifies(snotify, listen_addr, (unsigned short)port);
				memcpy(&lasttimeofday, &timeofday, sizeof(struct timeval));
				timeout.tv_sec = notify_interval;
				timeout.tv_usec = 0;
			}
			else
			{
				timeout.tv_sec = lasttimeofday.tv_sec + notify_interval
				                 - timeofday.tv_sec;
				if(timeofday.tv_usec > lasttimeofday.tv_usec)
				{
					timeout.tv_usec = 1000000 + lasttimeofday.tv_usec
					                  - timeofday.tv_usec;
					timeout.tv_sec--;
				}
				else
				{
					timeout.tv_usec = lasttimeofday.tv_usec - timeofday.tv_usec;
				}
			}
		}
		/* select open sockets (SSDP, HTTP listen and all HTTP soap sockets) */
		FD_ZERO(&readset);
		FD_SET(sudp, &readset);
		FD_SET(shttpl, &readset);
		i = 0;	/* active HTTP connections count */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if((e->socket >= 0) && (e->state <= 2))
			{
				FD_SET(e->socket, &readset);
				i++;
			}
		}
		/* for debug */
		if(i>1)
		{
			syslog(LOG_WARNING, "%d active incoming HTTP connections", i);
		}
		if(select(FD_SETSIZE, &readset, 0, 0, &timeout) < 0)
		{
			if(quitting) goto shutdown;
			syslog(LOG_ERR, "select: %m");
			syslog(LOG_ERR, "Exiting...");
			exit(1);	/* very serious cas of error */
		}
		/* process SSDP packets */
		if(FD_ISSET(sudp, &readset))
		{
			/*syslog(LOG_INFO, "Received UDP Packet");*/
			ProcessSSDPRequest(sudp, listen_addr, (unsigned short)port);
		}
		/* process incoming HTTP connections */
		if(FD_ISSET(shttpl, &readset))
		{
			int shttp;
			socklen_t	clientnamelen;
			struct sockaddr_in clientname;
			struct upnphttp * tmp = 0;
			clientnamelen = sizeof(struct sockaddr_in);
			shttp = accept(shttpl, (struct sockaddr *)&clientname, &clientnamelen);
			if(shttp<0)
			{
				syslog(LOG_ERR, "accept: %m");
			}
			else
			{
				//JM syslog(LOG_INFO, "HTTP connection from %s:%d",
				//JM        inet_ntoa(clientname.sin_addr),
				//JM    	   ntohs(clientname.sin_port) );
				/*if (fcntl(shttp, F_SETFL, O_NONBLOCK) < 0) {
					syslog(LOG_ERR, "fcntl F_SETFL, O_NONBLOCK");
				}*/
				/* Create a new upnphttp object and add it to
				 * the active upnphttp object list */
				tmp = New_upnphttp(shttp);
				LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
			}
		}
		/* process active HTTP connections */
		/* LIST_FOREACH is not available under linux */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if(  (e->socket >= 0) && (e->state <= 2)
			   &&(FD_ISSET(e->socket, &readset)) )
				Process_upnphttp(e);
		}
		/* delete finished HTTP connections */
		for(e = upnphttphead.lh_first; e != NULL; )
		{
			next = e->entries.le_next;
			if(e->state >= 100)
			{
				LIST_REMOVE(e, entries);
				Delete_upnphttp(e);
			}
			e = next;
		}
	}

shutdown:
	/* close out open sockets */
	while(upnphttphead.lh_first != NULL)
	{
		e = upnphttphead.lh_first;
		LIST_REMOVE(e, entries);
		Delete_upnphttp(e);
	}
	close(sudp);
	close(shttpl);
	close(snotify);
	
	miniupnpdShutdown();
	
	return 0;
}

