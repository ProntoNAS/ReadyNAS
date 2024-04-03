/*
 * nkfwrap.c - a simple C interface to nkf.
 *
 * What is it?
 *  - This is a wrapper allows you to replace a nkf() function of libnkf.
 *  - An interface to the Nkf Wrapper, is also available.
 *
 * What is the difference from libnkf?
 *  - In contrast the libnkf's nkf() returns 0 if succeeed,
 *    this nkf() returns how much characters were written.
 *  - nkf_continue(), preserve previous nkf() states, is available.
 *  - You can use another version of nkf by swapping the nkf.c/utf8tbl.c.
 *
 * What is the difference from Nkf Wrapper 0.1?
 *  - You can specify long options in a single 'opts' argument.
 *    (options must be separated by space, and '--' prefix is always needed.)
 *  - You can use nkf_continue().
 *  - When you want to use nkf 1.92, you dont need to modify this file.
 *    You just copy nkf.c into nkf/ directory. :)
 *
 * Why did you decide to write the code?
 *  - The libnkf's page has been gone.
 *    probably it wouldnt be maintenanced anymore.
 *  - in libnkf, you cannot use original nkf.
 *    I wanted to use current great nkf.
 *  - and... I didnt know about the Nkf Wrapper. :(
 *
 * License:
 *   Copyright Dec, 2004  Kikuchan
 *   You can use/redistribute/modify as long as under the term of nkf license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This idea is stolen from NKF module implementation. thx. */
#undef getc
#undef ungetc
#define getc(f)		(*input_ptr ? *input_ptr++ : -1)
#define ungetc(c,f)	input_ptr--, 0
#undef putchar
#define putchar(c)	nkf_putchar(c)

static unsigned char *input_ptr;
static unsigned char *output_buf;
static size_t output_len;
static size_t max_output_buffer_len;

static int
nkf_putchar(int c)
{
	if (output_len >= max_output_buffer_len) {
		return -1;
	}
	output_buf[output_len++] = c;
	return 0;
}

/* now let's include nkf.c file :) */
#define PERL_XS 1
#include "nkf/nkf.c"
#include "nkf/utf8tbl.c"

static void
nkf_parse_options(const char *opt_orig)
{
	char opt_buf[1024];
	char opt_tmp[1024];
	char *next_ptr;
	char *opts;

	if (opt_orig == NULL) return ;

	strncpy(opt_buf, opt_orig, sizeof(opt_buf));
	opts = opt_buf;

	do {
		next_ptr = strrchr(opts, ' ');
		if (next_ptr != NULL) {
			*next_ptr++ = '\0';
		}

		if (*opts && *opts != '-') {
			/* backward compatibility with libnkf */
			snprintf(opt_tmp, sizeof(opt_tmp), "-%s", opts);
			opts = opt_tmp;
		}
		options((unsigned char*)opts);
	} while ((opts = next_ptr) != NULL);
}

/*
 *  it convert kanji using previous internal state.
 */
int
nkf_continue(const char *in, char *out, size_t len)
{
	if (len <= 0) return -1;

	input_ptr = (unsigned char*)in;
	output_buf = (unsigned char*)out;
	output_len = 0;
	max_output_buffer_len = len;

#ifdef WISH_TRUE
	if (x0201_f == WISH_TRUE) {
		x0201_f = ((!iso2022jp_f)? TRUE : NO_X0201);
	}
#endif

	kanji_convert(NULL);

	nkf_putchar(0);		/* Null terminator */
	out[len-1] = '\0';	/* Force NUL termination for safe */

	return output_len;
}

/*
 *  An interface to the libnkf's nkf().
 */
int
nkf(const char *in, char *out, size_t len, const char *opts)
{
	reinit();
	nkf_parse_options(opts);
	return nkf_continue(in, out, len);
}

/*
 *  An interface to the Nkf Wrapper.
 */
int
nkf_wrapper(char *dst, int dst_size, const char *src, const char *opts)
{
	reinit();
	nkf_parse_options(opts);
	return nkf_continue(src, dst, dst_size);
}

/*
 * it returns nkfwrap's version. :)
 */
char *nkf_version()
{
	static char buf[1024];

	snprintf(buf, sizeof(buf),
		"NKF wrapper by Kikuchan.  Based on...\n"
		"  Network Kanji Filter Version %s (%s)\n"
		"  %s\n"
#ifdef NKF_VERSION
		, NKF_VERSION, NKF_RELEASE_DATE, CopyRight);
#else
		, Version, Patchlevel, CopyRight);
#endif

	return buf;
}
