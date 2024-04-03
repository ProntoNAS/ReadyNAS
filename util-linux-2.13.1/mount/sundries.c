/*
 * Support functions.  Exported functions are prototyped in sundries.h.
 * sundries.c,v 1.1.1.1 1993/11/18 08:40:51 jrs Exp
 *
 * added fcntl locking by Kjetil T. (kjetilho@math.uio.no) - aeb, 950927
 *
 * 1999-02-22 Arkadiusz Mi�kiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <mntent.h>		/* for MNTTYPE_SWAP */
#include "fstab.h"
#include "sundries.h"
#include "realpath.h"
#include "xmalloc.h"
#include "nls.h"

char *
xstrndup (const char *s, int n) {
     char *t;

     if (s == NULL)
	  die (EX_SOFTWARE, _("bug in xstrndup call"));

     t = xmalloc(n+1);
     strncpy(t,s,n);
     t[n] = 0;

     return t;
}

/* reallocates its first arg - typical use: s = xstrconcat3(s,t,u); */
char *
xstrconcat3 (char *s, const char *t, const char *u) {
     size_t len = 0;

     len = (s ? strlen(s) : 0) + (t ? strlen(t) : 0) + (u ? strlen(u) : 0);

     if (!len)
	     return NULL;
     if (!s) {
	     s = xmalloc(len + 1);
	     *s = '\0';
     }
     else
	     s = xrealloc(s, len + 1);
     if (t)
	     strcat(s, t);
     if (u)
	     strcat(s, u);
     return s;
}

/* frees its first arg - typical use: s = xstrconcat4(s,t,u,v); */
char *
xstrconcat4 (char *s, const char *t, const char *u, const char *v) {
     size_t len = 0;

     len = (s ? strlen(s) : 0) + (t ? strlen(t) : 0) +
		(u ? strlen(u) : 0) + (v ? strlen(v) : 0);

     if (!len)
	     return NULL;
     if (!s) {
	     s = xmalloc(len + 1);
	     *s = '\0';
     }
     else
	     s = xrealloc(s, len + 1);
     if (t)
	     strcat(s, t);
     if (u)
	     strcat(s, u);
     if (v)
	     strcat(s, v);
     return s;


}

/* Call this with SIG_BLOCK to block and SIG_UNBLOCK to unblock.  */
void
block_signals (int how) {
     sigset_t sigs;

     sigfillset (&sigs);
     sigdelset(&sigs, SIGTRAP);
     sigdelset(&sigs, SIGSEGV);
     sigprocmask (how, &sigs, (sigset_t *) 0);
}


/* Non-fatal error.  Print message and return.  */
/* (print the message in a single printf, in an attempt
    to avoid mixing output of several threads) */
void
error (const char *fmt, ...) {
     va_list args;

     if (mount_quiet)
	  return;
     va_start (args, fmt);
     vfprintf (stderr, fmt, args);
     va_end (args);
     fputc('\n', stderr);
}

/* True if fstypes match.  Null *TYPES means match anything,
   except that swap types always return false. */
/* Accept nonfs,proc,devpts and nonfs,noproc,nodevpts
   with the same meaning. */
int
matching_type (const char *type, const char *types) {
     int no;			/* negated types list */
     int len;
     const char *p;

     if (streq (type, MNTTYPE_SWAP))
	  return 0;
     if (types == NULL)
	  return 1;

     no = 0;
     if (!strncmp(types, "no", 2)) {
	  no = 1;
	  types += 2;
     }

     /* Does type occur in types, separated by commas? */
     len = strlen(type);
     p = types;
     while(1) {
	     if (!strncmp(p, "no", 2) && !strncmp(p+2, type, len) &&
		 (p[len+2] == 0 || p[len+2] == ','))
		     return 0;
	     if (strncmp(p, type, len) == 0 &&
		 (p[len] == 0 || p[len] == ','))
		     return !no;
	     p = index(p,',');
	     if (!p)
		     break;
	     p++;
     }
     return no;
}

/* Returns 1 if needle found or noneedle not found in haystack
 * Otherwise returns 0
 */
static int
check_option(const char *haystack, const char *needle) {
     const char *p, *r;
     int len, needle_len, this_len;
     int no;

     no = 0;
     if (!strncmp(needle, "no", 2)) {
	  no = 1;
	  needle += 2;
     }
     needle_len = strlen(needle);
     len = strlen(haystack);

     for (p = haystack; p < haystack+len; p++) {
	  r = strchr(p, ',');
	  if (r) {
	       this_len = r-p;
	  } else {
	       this_len = strlen(p);
	  }
	  if (this_len != needle_len) {
	       p += this_len;
	       continue;
	  }
	  if (strncmp(p, needle, this_len) == 0)
	       return !no; /* foo or nofoo was found */
	  p += this_len;
     }

     return no;  /* foo or nofoo was not found */
}


/* Returns 1 if each of the test_opts options agrees with the entire
 * list of options.
 * Returns 0 if any noopt is found in test_opts and opt is found in options.
 * Returns 0 if any opt is found in test_opts but is not found in options.
 * Unlike fs type matching, nonetdev,user and nonetdev,nouser have
 * DIFFERENT meanings; each option is matched explicitly as specified.
 */
int
matching_opts (const char *options, const char *test_opts) {
     const char *p, *r;
     char *q;
     int len, this_len;

     if (test_opts == NULL)
	  return 1;

     len = strlen(test_opts);
     q = alloca(len+1);
     if (q == NULL)
          die (EX_SYSERR, _("not enough memory"));
     
     for (p = test_opts; p < test_opts+len; p++) {
	  r = strchr(p, ',');
	  if (r) {
	       this_len = r-p;
	  } else {
	       this_len = strlen(p);
	  }
	  if (!this_len) continue; /* if two ',' appear in a row */
	  strncpy(q, p, this_len);
	  q[this_len] = '\0';
	  if (!check_option(options, q))
	       return 0; /* any match failure means failure */
	  p += this_len;
     }

     /* no match failures in list means success */
     return 1;
}

/* Make a canonical pathname from PATH.  Returns a freshly malloced string.
   It is up the *caller* to ensure that the PATH is sensible.  i.e.
   canonicalize ("/dev/fd0/.") returns "/dev/fd0" even though ``/dev/fd0/.''
   is not a legal pathname for ``/dev/fd0''.  Anything we cannot parse
   we return unmodified.   */
char *
canonicalize (const char *path) {
	char canonical[PATH_MAX+2];

	if (path == NULL)
		return NULL;

#if 1
	if (streq(path, "none") ||
	    streq(path, "proc") ||
	    streq(path, "devpts"))
		return xstrdup(path);
#endif
#if 1 //JM - NETGEAR:  Don't do this for device-mapper items.
	if (myrealpath (path, canonical, PATH_MAX+1)) {
		if( strncmp(canonical, "/dev/mapper/", 12) == 0 )
			return xstrdup(path);
		else
			return xstrdup(canonical);
	}
#else
	if (myrealpath (path, canonical, PATH_MAX+1))
		return xstrdup(canonical);
#endif

	return xstrdup(path);
}


/*
 * Parses NAME=value, returns -1 on parse error, 0 success. The success is also
 * when the 'spec' doesn't contain name=value pair (because the spec could be
 * a devname too). In particular case the pointer 'name' is set to NULL.

 * The result is a new allocated string (the 'name' pointer).
 */
int
parse_spec(const char *spec, char **name, char **value)
{
	char *vl, *tk, *cp;

	*name = NULL;
	*value = NULL;

	if (!(cp = strchr(spec, '=')))
		return 0;				/* no name= */

	tk = xstrdup(spec);
	vl = tk + (cp - spec);
	*vl++ = '\0';

	if (*vl == '"' || *vl == '\'') {
		if (!(cp = strrchr(vl+1, *vl))) {
			free(tk);
			return -1;			/* parse error */
		}
		vl++;
		*cp = '\0';
	}

	*name = tk;
	*value = vl;
	return 0;
}

