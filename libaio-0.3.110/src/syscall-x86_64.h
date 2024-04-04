#ifndef __NR_io_setup
#define __NR_io_setup		206
#endif
#ifndef __NR_io_destroy
#define __NR_io_destroy		207
#endif
#ifndef __NR_io_getevents
#define __NR_io_getevents	208
#endif
#ifndef __NR_io_submit
#define __NR_io_submit		209
#endif
#ifndef __NR_io_cancel
#define __NR_io_cancel		210
#endif

#define __syscall_clobber "r11","rcx","memory" 
#define __syscall "syscall"

#define io_syscall1(type,fname,sname,type1,arg1)			\
type fname(type1 arg1)							\
{									\
long __res;								\
__asm__ volatile (__syscall						\
	: "=a" (__res)							\
	: "0" (__NR_##sname),"D" ((long)(arg1)) : __syscall_clobber );	\
return __res;								\
}

#define io_syscall2(type,fname,sname,type1,arg1,type2,arg2)		\
type fname(type1 arg1,type2 arg2)					\
{									\
long __res;								\
__asm__ volatile (__syscall						\
	: "=a" (__res)							\
	: "0" (__NR_##sname),"D" ((long)(arg1)),"S" ((long)(arg2)) : __syscall_clobber ); \
return __res;								\
}

#define io_syscall3(type,fname,sname,type1,arg1,type2,arg2,type3,arg3)	\
type fname(type1 arg1,type2 arg2,type3 arg3)				\
{									\
long __res;								\
__asm__ volatile (__syscall						\
	: "=a" (__res)							\
	: "0" (__NR_##sname),"D" ((long)(arg1)),"S" ((long)(arg2)),	\
		  "d" ((long)(arg3)) : __syscall_clobber);		\
return __res;								\
}

#define io_syscall4(type,fname,sname,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type fname (type1 arg1, type2 arg2, type3 arg3, type4 arg4)		\
{									\
long __res;								\
register long __a4 asm ("r10") = (long) arg4;                           \
__asm__ volatile (__syscall						\
	: "=a" (__res)							\
	: "0" (__NR_##sname),"D" ((long)(arg1)),"S" ((long)(arg2)),	\
	  "d" ((long)(arg3)),"r" (__a4)); \
return __res;								\
} 

#define io_syscall5(type,fname,sname,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5)							\
type fname (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5)	\
{									\
long __res;								\
register long __a4 asm ("r10") = (long) arg4;				\
register long __a5 asm ("r8") = (long) arg5;                            \
__asm__ volatile (__syscall						\
	: "=a" (__res)							\
	: "0" (__NR_##sname),"D" ((long)(arg1)),"S" ((long)(arg2)),	\
	  "d" ((long)(arg3)),"r" (__a4),"r" (__a5)); \
return __res;								\
}
