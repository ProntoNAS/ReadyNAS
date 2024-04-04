/* Some compiler optimizations may transform loops into memset/memmove
   calls and without proper declaration it may generate PLT calls.  */
#if !defined __ASSEMBLER__ && !defined NOT_IN_libc && defined SHARED
#include <config.h>
#  if !defined NO_HIDDEN
asm ("memmove = __GI_memmove");
asm ("memset = __GI_memset");
#  endif
#endif
