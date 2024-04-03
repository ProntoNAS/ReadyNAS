#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "config.h"
#ifndef HAVE_RESULTPROC_T
typedef void *resultproc_t;
#endif

#ifndef __GNUC__
#define __attribute__(a) /**/
#endif

#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__
#endif

#if (defined(__sun__) || defined(sun)) && !defined(__svr4__)

/* Stupid SunOS 4 doesn't have prototypes in the header files */

/* Some includes just to make the compiler be quiet */
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <rpc/types.h>

/* should be declared in /usr/include/rpc/pmap_clnt.h */
#ifdef HAVE_RESULTPROC_T
typedef bool_t (*resultproc_t) (void *, void *);
#endif

extern int fprintf(FILE *, const char *, ...);
extern int fscanf(FILE *, const char *, ...);
extern int fclose(FILE *);
extern int _flsbuf(unsigned char c, FILE *fp);
extern int puts(const char *str);
extern int printf(const char *format, ...);
extern int vfprintf( FILE *stream, char  *format,  va_list ap);
extern int vsyslog(int type, char *format, va_list ap);
extern int syslog(int, const char *format, ...);
extern void openlog( char *ident, int option, int  facility);
extern char *strerror(int errnum);
extern int rename(char *path1, char *patht2);
extern char tolower(char c);

extern int chdir(const char *path);
extern int perror(const char *str);
extern int atoi(const char *str);
extern int gettimeofday(struct timeval *tp, struct timezone *tpz);
extern int gethostname(char *buf, int bufsize);

extern int socket (int af, int type, int protocol);
extern int bind (int s, struct sockaddr *name, int namelen);
extern int chdir (const char *path);
extern int select(int  n,  fd_set  *readfds,  fd_set  *writefds,
		  fd_set *exceptfds, struct timeval *timeout);
extern pid_t wait3(int *status, int options, struct rusage *rusage);
extern pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);

#endif

#if (defined(__sun__) || defined(sun)) && defined(__svr4__)
/* Solaris 2.x */

#define __EXTENSIONS__
#define NEED_SVCSOC_H

#endif

#ifndef inline
#define inline __inline__
#endif


#if defined(hpux) || defined(__hpux__)

/* HP is really... Ah well. */

#define _INCLUDE_HPUX_SOURCE
#define _INCLUDE_XOPEN_SOURCE
#define _INCLUDE_POSIX_SOURCE
#define _INCLUDE_AES_SOURCE

extern void svcerr_systemerr();
#endif

#if defined(linux) || defined(__linux__)

/* Need this because some header files doesn't check for __linux__ */
#if !defined(linux)
#define linux linux
#endif

/* Needed for non-ANSI prototypes */
#ifndef _SVID_SOURCE
#define _SVID_SOURCE
#endif

/* Needed for gethostname() */
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <stdio.h>

#if !(defined (__GLIBC__) && __GLIBC__ >= 2)
extern int snprintf (char *, unsigned int, const char *, ...);
#endif

#endif /* linux */

#if !defined(HAVE_RES_GETHOSTBYNAME)
#if defined(HAVE__DNS_GETHOSTBYNAME) || defined(linux)
#define res_gethostbyname _dns_gethostbyname
extern struct hostent *_dns_gethostbyname(const char *);
#define res_gethostbyaddr _dns_gethostbyaddr
extern struct hostent *_dns_gethostbyaddr(const char *, int, int);
#else
/* !defined(HAVE_RES_GETHOSTBYNAME) && !defined(HAVE__DNS_GETHOSTBYNAME) */
#define res_gethostbyname gethostbyname
#define res_gethostbyaddr gethostbyaddr
#endif
#else
/* defined(HAVE_RES_GETHOSTBYNAME) */
extern struct hostent *res_gethostbyname(const char *);
extern struct hostent *res_gethostbyaddr(const char *, int, int);
#endif

#endif
