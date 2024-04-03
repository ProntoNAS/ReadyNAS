#include <stdio.h>
#include <sys/types.h>

/* keep this in sync with the patch to apr_file_info.h */
#ifdef __alpha__
typedef unsigned int              apr_ino_t;
#else
typedef unsigned long int         apr_ino_t;
#endif

int main (void)
{
	size_t s0 = sizeof(ino_t), s1 = sizeof(apr_ino_t);
	if (s0 == s1)
		return 0;
	fprintf(stderr, "***\n"
			"*** 'ino_t' size is %zu, expected %zu\n"
			"*** Please report this to the Debian Apache maintainers\n"
			"***\n", s0, s1);
	return 1;
}
