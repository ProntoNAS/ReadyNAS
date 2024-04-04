#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <md5.h>

/*
 * vsscanf.c
 *
 * vsscanf(), from which the rest of the scanf()
 * family is built
 */

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <sys/bitops.h>

#ifndef LONG_BIT
#define LONG_BIT (CHAR_BIT*sizeof(long))
#endif

enum flags {
    FL_SPLAT = 0x01,		/* Drop the value, do not assign */
    FL_WIDTH = 0x02,		/* Field width specified */
    FL_MINUS = 0x04,		/* Negative number */
};

enum ranks {
    rank_char = -2,
    rank_short = -1,
    rank_int = 0,
    rank_long = 1,
    rank_longlong = 2,
    rank_ptr = INT_MAX		/* Special value used for pointers */
};

#define MIN_RANK	rank_char
#define MAX_RANK	rank_longlong

#define INTMAX_RANK	rank_longlong
#define SIZE_T_RANK	rank_long
#define PTRDIFF_T_RANK	rank_long

enum bail {
    bail_none = 0,		/* No error condition */
    bail_eof,			/* Hit EOF */
    bail_err			/* Conversion mismatch */
};

static const char *_skipspace(const char *p)
{
	while (isspace(*p))
		p++;
	return p;
}

static int _vsscanf(const char *buffer, const char *format, va_list ap)
{
    const char *p = format;
    char ch;
    const char *q = buffer;
    const char *qq;
    uintmax_t val = 0;
    int rank = rank_int;	/* Default rank */
    unsigned int width = UINT_MAX;
    int base;
    enum flags flags = 0;
    enum {
	st_normal,		/* Ground state */
	st_flags,		/* Special flags */
	st_width,		/* Field width */
	st_modifiers,		/* Length or conversion modifiers */
	st_match_init,		/* Initial state of %[ sequence */
	st_match,		/* Main state of %[ sequence */
	st_match_range,		/* After - in a %[ sequence */
    } state = st_normal;
    char *sarg = NULL;		/* %s %c or %[ string argument */
    enum bail bail = bail_none;
    int converted = 0;		/* Successful conversions */
    unsigned long matchmap[((1 << CHAR_BIT) + (LONG_BIT - 1)) / LONG_BIT];
    int matchinv = 0;		/* Is match map inverted? */
    unsigned char range_start = 0;

    while ((ch = *p++) && !bail) {
	switch (state) {
	case st_normal:
	    if (ch == '%') {
		state = st_flags;
		flags = 0;
		rank = rank_int;
		width = UINT_MAX;
	    } else if (isspace((unsigned char)ch)) {
		q = _skipspace(q);
	    } else {
		if (*q == ch)
		    q++;
		else
		    bail = bail_err;	/* Match failure */
	    }
	    break;

	case st_flags:
	    switch (ch) {
	    case '*':
		flags |= FL_SPLAT;
		break;
	    case '0' ... '9':
		width = (ch - '0');
		state = st_width;
		flags |= FL_WIDTH;
		break;
	    default:
		state = st_modifiers;
		p--;		/* Process this character again */
		break;
	    }
	    break;

	case st_width:
	    if (ch >= '0' && ch <= '9') {
		width = width * 10 + (ch - '0');
	    } else {
		state = st_modifiers;
		p--;		/* Process this character again */
	    }
	    break;

	case st_modifiers:
	    switch (ch) {
		/* Length modifiers - nonterminal sequences */
	    case 'h':
		rank--;		/* Shorter rank */
		break;
	    case 'l':
		rank++;		/* Longer rank */
		break;
	    case 'j':
		rank = INTMAX_RANK;
		break;
	    case 'z':
		rank = SIZE_T_RANK;
		break;
	    case 't':
		rank = PTRDIFF_T_RANK;
		break;
	    case 'L':
	    case 'q':
		rank = rank_longlong;	/* long double/long long */
		break;

	    default:
		/* Output modifiers - terminal sequences */
		state = st_normal;	/* Next state will be normal */
		if (rank < MIN_RANK)	/* Canonicalize rank */
		    rank = MIN_RANK;
		else if (rank > MAX_RANK)
		    rank = MAX_RANK;

		switch (ch) {
		case 'P':	/* Upper case pointer */
		case 'p':	/* Pointer */
#if 0				/* Enable this to allow null pointers by name */
		    q = _skipspace(q);
		    if (!isdigit((unsigned char)*q)) {
			static const char *const nullnames[] =
			    { "null", "nul", "nil", "(null)", "(nul)", "(nil)",
0 };
			const char *const *np;

			/* Check to see if it's a null pointer by name */
			for (np = nullnames; *np; np++) {
			    if (!strncasecmp(q, *np, strlen(*np))) {
				val = (uintmax_t) ((void *)NULL);
				goto set_integer;
			    }
			}
			/* Failure */
			bail = bail_err;
			break;
		    }
		    /* else */
#endif
		    rank = rank_ptr;
		    base = 0;
		    goto scan_int;

		case 'i':	/* Base-independent integer */
		    base = 0;
		    goto scan_int;

		case 'd':	/* Decimal integer */
		    base = 10;
		    goto scan_int;

		case 'o':	/* Octal integer */
		    base = 8;
		    goto scan_int;

		case 'u':	/* Unsigned decimal integer */
		    base = 10;
		    goto scan_int;

		case 'x':	/* Hexadecimal integer */
		case 'X':
		    base = 16;
		    goto scan_int;

		case 'n':	/* Number of characters consumed */
		    val = (q - buffer);
		    goto set_integer;

scan_int:
		    q = _skipspace(q);
		    if (!*q) {
			bail = bail_eof;
			break;
		    }
		    val = strntoumax(q, (char **)&qq, base, width);
		    if (qq == q) {
			bail = bail_err;
			break;
		    }
		    q = qq;
		    converted++;
		    /* fall through */

set_integer:
		    if (!(flags & FL_SPLAT)) {
			switch (rank) {
			case rank_char:
			    *va_arg(ap, unsigned char *) = (unsigned char)val;
			    break;
			case rank_short:
			    *va_arg(ap, unsigned short *) = (unsigned short)val;
			    break;
			case rank_int:
			    *va_arg(ap, unsigned int *) = (unsigned int)val;
			    break;
			case rank_long:
			    *va_arg(ap, unsigned long *) = (unsigned long)val;
			    break;
			case rank_longlong:
			    *va_arg(ap, unsigned long long *) =
				(unsigned long long)val;
			    break;
			case rank_ptr:
			    *va_arg(ap, void **) = (void *)(uintptr_t) val;
			    break;
			}
		    }
		    break;

		case 'c':	/* Character */
		    width = (flags & FL_WIDTH) ? width : 1;	/* Default width == 1 */
		    sarg = va_arg(ap, char *);
		    while (width--) {
			if (!*q) {
			    bail = bail_eof;
			    break;
			}
			*sarg++ = *q++;
		    }
		    if (!bail)
			converted++;
		    break;

		case 's':	/* String */
		    {
			char *sp;
			sp = sarg = va_arg(ap, char *);
			while (width-- && *q && !isspace((unsigned char)*q)) {
			    *sp++ = *q++;
			}
			if (sarg != sp) {
			    *sp = '\0';	/* Terminate output */
			    converted++;
			} else {
			    bail = bail_eof;
			}
		    }
		    break;

		case '[':	/* Character range */
		    sarg = va_arg(ap, char *);
		    state = st_match_init;
		    matchinv = 0;
		    memset(matchmap, 0, sizeof matchmap);
		    break;

		case '%':	/* %% sequence */
		    if (*q == '%')
			q++;
		    else
			bail = bail_err;
		    break;

		default:	/* Anything else */
		    bail = bail_err;	/* Unknown sequence */
		    break;
		}
	    }
	    break;

	case st_match_init:	/* Initial state for %[ match */
	    if (ch == '^' && !matchinv) {
		matchinv = 1;
	    } else {
		range_start = (unsigned char)ch;
		set_bit((unsigned char)ch, matchmap);
		state = st_match;
	    }
	    break;

	case st_match:		/* Main state for %[ match */
	    if (ch == ']') {
		goto match_run;
	    } else if (ch == '-') {
		state = st_match_range;
	    } else {
		range_start = (unsigned char)ch;
		set_bit((unsigned char)ch, matchmap);
	    }
	    break;

	case st_match_range:	/* %[ match after - */
	    if (ch == ']') {
		set_bit((unsigned char)'-', matchmap);	/* - was last character */
		goto match_run;
	    } else {
		int i;
		for (i = range_start; i <= (unsigned char)ch; i++)
		    set_bit(i, matchmap);
		state = st_match;
	    }
	    break;

match_run:			/* Match expression finished */
	    qq = q;
	    while (width && *q
		   && test_bit((unsigned char)*q, matchmap) ^ matchinv) {
		*sarg++ = *q++;
	    }
	    if (q != qq) {
		*sarg = '\0';
		converted++;
	    } else {
		bail = *q ? bail_err : bail_eof;
	    }
	    break;
	}
    }

    if (bail == bail_eof && !converted)
	converted = -1;		/* Return EOF (-1) */

    return converted;
}

static int _sscanf(const char *str, const char *format, ...)
{
	va_list ap;
	int rv;

	va_start(ap, format);
	rv = _vsscanf(str, format, ap);
	va_end(ap);

	return rv;
}

const char *csumsfile = "csums.md5";
#define MD5STRLEN	(MD5_SIZE * 2)

static char *_fgets(char *buf, size_t len, FILE *fp)
{
	size_t i = 0;

	while (i < len - 1) {
		int c = getc(fp);

		switch (c) {
		case EOF:
			if (!i)
				return NULL;
			break;
		case '\n':
			buf[i++] = c;
			break;
		default:
			buf[i++] = c;
			continue;
		}
		break;
	}
	buf[i] = '\0';
	return buf;
}

#define printfs(fmt,...)	do {printf((fmt),__VA_ARGS__);sleep(1);}while(0)

/*
 * Compare MD5 signature with a record in meta file.
 * Returns -1 with EIO only if an anctual mismatch is found.
 */
int csum_md5(int fd, const char *pathname, int flags)
{
	if (!strcasecmp(csumsfile, pathname) || flags != O_RDONLY)
		return 0;

	int csf_fd = open(csumsfile, O_RDONLY);

	if (csf_fd < 0)
		return 0;
	FILE *fp = fdopen(csf_fd, "r");
	if (!fp) {
		close(csf_fd);
		return 0;
	}

	int found = 0;
	char buf[256], md5s[256];

	while (_fgets(buf, sizeof buf, fp)) {
		char pname[256];

		if (_sscanf(buf, "%s %s", md5s, pname) != 2)
			continue;

		if (strcasecmp(pname, pathname))
			continue;
		if (strlen(md5s) != MD5STRLEN)
			continue;

		char *p = md5s;
		while (*p) {
			if (!isxdigit(*p))
				break;
			p++;
		}
		if (!*p) {
			found = 1;
			break;
		}
	}
	fclose(fp);
	if (!found)
		return 0;

	if (!(fp = fdopen(fd, "r")))
		return 0;
	MD5_CTX ctx;
	MD5Init(&ctx);
	int c;
	while ((c = getc(fp)) != EOF) {
		unsigned char cc = c;
		MD5Update(&ctx, &cc, 1);
	}
	fclose(fp);
	MD5Final((unsigned char *)buf, &ctx);

	int i;
	for (i = 0; i < MD5_SIZE; i++) {
		char hex[3];

		sprintf(hex, "%02x", buf[i] & 0xff);
		if (strncasecmp(hex, &md5s[i * 2], 2)) {
			printf(" MD checksum error: %s\n", pathname);
			errno = EIO;
			return -1;
		}
	}
	return 0;
}
