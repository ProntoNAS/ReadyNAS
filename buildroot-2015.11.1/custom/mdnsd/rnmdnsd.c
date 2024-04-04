#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>

#include "mdns.h"
#include "mdnsd.h"

static int
get_ipv4(uint32_t *ip_addr)
{
	struct ifaddrs *ifap, *p;
	struct sockaddr_in *addr;
	int ret = -1;

	if (getifaddrs(&ifap) != 0)
	{
		fprintf(stderr, "failed to get interface addresses (%s)",
			strerror(errno));
		return -1;
	}

	for (p = ifap; p != NULL; p = p->ifa_next)
	{
		addr = (struct sockaddr_in *)p->ifa_addr;
		if (p->ifa_flags & (IFF_LOOPBACK | IFF_SLAVE))
			continue;
		if (strncmp(p->ifa_name, "virbr", 5) == 0)
			continue;
		if (!addr || p->ifa_addr->sa_family != AF_INET)
			continue;
		memcpy(ip_addr, &addr->sin_addr, 4);
		ret = 0;
		break;
	}
	freeifaddrs(ifap);

	return ret;
}

int main(int argc, char **argv)
{
	struct mdnsd *svr;
	struct mdns_service *svc;
        const char *txt[] = { "", NULL };
	char hostname[32];
	uint32_t ip_addr;

	gethostname(hostname, sizeof(hostname) - 8);
	strcat(hostname, ".local");

	svr = mdnsd_start();
	if (svr == NULL) {
		printf("mdnsd_start() error\n");
		return 1;
	}

	get_ipv4(&ip_addr);
	mdnsd_set_hostname(svr, hostname, ip_addr);

	gethostname(hostname, sizeof(hostname));
	svc = mdnsd_register_svc(svr, hostname, "_workstation._tcp.local", 9, NULL, txt);

	pause();

	mdns_service_destroy(svc);
	mdnsd_stop(svr);

	return 0;
}

