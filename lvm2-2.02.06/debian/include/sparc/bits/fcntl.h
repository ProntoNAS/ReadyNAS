#include_next <bits/fcntl.h>

#ifdef __USE_GNU
# define O_NOATIME	0x200000 /* Do not set atime.  */
#endif
