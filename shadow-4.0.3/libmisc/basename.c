/*
 * basename.c - not worth copyrighting :-).  Some versions of Linux libc
 * already have basename(), other versions don't.  To avoid confusion,
 * we will not use the function from libc and use a different name here.
 * --marekm
 */

#include <config.h>

#include "rcsid.h"
RCSID("$Id: basename.c 6 2005-03-20 15:34:28Z bubulle $")

#include "defines.h"
#include "prototypes.h"

char *
Basename(char *str)
{
	char *cp = strrchr(str, '/');

	return cp ? cp+1 : str;
}
