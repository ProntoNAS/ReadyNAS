#ifndef _CONFIG_H
#define _CONFIG_H

#define PACKAGE "libidn"

#define strcasecmp stricmp
#define strncasecmp strnicmp

extern int strverscmp (const char *, const char *);

#define LOCALEDIR "."

#if _MSC_VER && !__cplusplus
# define inline __inline
#endif

#endif /* _CONFIG_H */
