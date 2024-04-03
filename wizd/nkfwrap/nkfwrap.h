#ifndef _NKFWRAP_H_
#define _NKFWRAP_H_
/* just prototypes */

/* len; output buffer size. */
int nkf(const char *in, char *out, size_t len, const char *opts);
int nkf_continue(const char *in, char *out, size_t len);
int nkf_wrapper(char *dst, int dst_size, const char *src, const char *opts);

char *nkf_version();

#endif
