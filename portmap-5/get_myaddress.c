 /*
  * get_myaddress - alternative version that picks the loopback interface.
  * 
  * Author: Wietse Venema (wietse@wzv.win.tue.nl), dept. of Mathematics and
  * Computing Science, Eindhoven University of Technology, The Netherlands.
  */

#ifndef lint
static char sccsid[] = "@(#) get_myaddress.c 1.1 23:00:53";
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK ntohl(inet_addr("127.0.0.1"))
#endif

void    get_myaddress(addrp)
struct sockaddr_in *addrp;
{
    memset((char *) addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_port = htons(PMAPPORT);
    addrp->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

#ifdef TEST

#include <stdio.h>

main(argc, argv)
int     argc;
char  **argv;
{
    struct sockaddr_in addr;

    get_myaddress(&addr);
    printf("%s\n", inet_ntoa(addr.sin_addr));
    exit(0);
}

#endif
