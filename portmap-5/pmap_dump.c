 /*
  * pmap_dump - dump portmapper table in format readable by pmap_set
  * 
  * Author: Wietse Venema (wietse@wzv.win.tue.nl), dept. of Mathematics and
  * Computing Science, Eindhoven University of Technology, The Netherlands.
  */

#ifndef lint
static char sccsid[] = "@(#) pmap_dump.c 1.1 92/06/11 22:53:15";
#endif

#include <stdio.h>
#include <sys/types.h>
#ifdef SYSV40
#include <netinet/in.h>
#include <rpc/rpcent.h>
#else
#include <netdb.h>
#endif
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_prot.h>

static char *protoname();

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK ntohl(inet_addr("127.0.0.1"))
#endif

static void    get_myloopaddress(addrp)
struct sockaddr_in *addrp;
{
    memset((char *) addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_port = htons(PMAPPORT);
    addrp->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

main(argc, argv)
int     argc;
char  **argv;
{
    struct sockaddr_in addr;
    register struct pmaplist *list;
    register struct rpcent *rpc;

    get_myloopaddress(&addr);

    for (list = pmap_getmaps(&addr); list; list = list->pml_next) {
	rpc = getrpcbynumber((int) list->pml_map.pm_prog);
	printf("%10lu %4lu %5s %6lu  %s\n",
	       list->pml_map.pm_prog,
	       list->pml_map.pm_vers,
	       protoname(list->pml_map.pm_prot),
	       list->pml_map.pm_port,
	       rpc ? rpc->r_name : "");
    }
#undef perror
    return (fclose(stdout) ? (perror(argv[0]), 1) : 0);
}

static char *protoname(proto)
u_long  proto;
{
    static char buf[BUFSIZ];

    switch (proto) {
    case IPPROTO_UDP:
	return ("udp");
    case IPPROTO_TCP:
	return ("tcp");
    default:
	sprintf(buf, "%lu", proto);
	return (buf);
    }
}
