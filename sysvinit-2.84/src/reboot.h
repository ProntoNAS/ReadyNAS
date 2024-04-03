/*
 * reboot.h	Headerfile that defines how to handle
 *		the reboot() system call.
 *
 * Version:	@(#)reboot.h  1.00  23-Jul-1996  miquels@cistron.nl
 *
 */

#if defined(__GLIBC__)
#  include <sys/reboot.h>
#endif

#define BMAGIC_HARD	0x89ABCDEF
#define BMAGIC_SOFT	0
#define BMAGIC_REBOOT	0x01234567
#define BMAGIC_HALT	0xCDEF0123
#define BMAGIC_POWEROFF	0x4321FEDC

#if defined(__GLIBC__)
  #define init_reboot(magic) reboot(magic)
#else
  #define init_reboot(magic) reboot(0xfee1dead, 672274793, magic)
#endif
