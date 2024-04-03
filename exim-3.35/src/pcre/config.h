/*************************************************
*           config.h for PCRE for Exim           *
*************************************************/

/* The PCRE sources include config.h, which for a free-standing PCRE build gets
set up by autoconf. The only configuration thing that matters for the PCRE
library itself is whether the memmove() function exists or not. It should be
present in all Standard C libraries, but is missing in SunOS4. PCRE expects
autoconf to set HAVE_MEMMOVE to 1 in config.h when memmove() is present. If
that is not set, it defines memmove() as a macro for bcopy().

Exim works differently. It handles this case by defining memmove() as a macro 
in its os.h-SunOS4 file. We interface this to PCRE by including the os.h file 
here, and then defining HAVE_MEMOVE so that PCRE's code in internal.h leaves 
things alone. */

#include "../os.h"
#define HAVE_MEMMOVE 1

/* End */
