#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "k5-int.h"
#include "kprop.h"

int sockaddr2krbaddr(int family, struct sockaddr *sa, krb5_address *dest)
{
	struct sockaddr_in *sa4;
	struct sockaddr_in6 *sa6;

	if (family == AF_INET) {
		dest->addrtype = ADDRTYPE_INET;
		sa4 = (struct sockaddr_in *)sa;
		dest->length = sizeof(sa4->sin_addr);
		dest->contents = (krb5_octet *) malloc(sizeof(sa4->sin_addr));
		if (!dest->contents) {
			(void) fprintf(stderr, _("\nCouldn't allocate memory"));
			exit(1);
		}
		memcpy(dest->contents, &sa4->sin_addr, sizeof(sa4->sin_addr));
		return 0;
	} else if (family == AF_INET6) {
		dest->addrtype = ADDRTYPE_INET6;
		sa6 = (struct sockaddr_in6 *)sa;
		dest->length = sizeof(sa6->sin6_addr);
		dest->contents = (krb5_octet *) malloc(sizeof(sa6->sin6_addr));
		if (!dest->contents) {
			(void) fprintf(stderr, _("\nCouldn't allocate memory"));
			exit(1);
		}
		memcpy(dest->contents, &sa6->sin6_addr, sizeof(sa6->sin6_addr));
		return 0;
	}

	/* Bad address family */
	return 1;
}
