#include <stdio.h>
#include <sys/types.h>
#include "apr_file_info.h"

/* this was the old definition of apr_ino_t until 1.2.11-1 */
#if defined(__alpha__) || defined(__FreeBSD_kernel__)
typedef unsigned int              old_apr_ino_t;
#else
typedef unsigned long int         old_apr_ino_t;
#endif

int main (void)
{
	size_t s0 = sizeof(apr_ino_t), s1 = sizeof(old_apr_ino_t);
	if (s0 == s1)
		return 0;
	fprintf(stderr, "***\n"
			"*** 'apr_ino_t' size is %zu, expected %zu\n"
			"*** Please report this to the Debian Apache maintainers\n"
			"***\n", s0, s1);
	return 1;
}
