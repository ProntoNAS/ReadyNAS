/*
  File: getfattr.c
  (Linux Extended Attributes)

  Copyright (C) 2001-2002 Andreas Gruenbacher <a.gruenbacher@computer.org>
  Copyright (C) 2001-2002 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <ftw.h>
#include <locale.h>

#include <attr/xattr.h>
#include "config.h"
#include "misc.h"

#define CMD_LINE_OPTIONS "n:de:m:hRLP"
#define CMD_LINE_SPEC "[-hRLP] [-n name|-d] [-e en] [-m pattern] path..."

struct option long_options[] = {
	{ "name",		1, 0, 'n' },
	{ "dump",		0, 0, 'd' },
	{ "encoding",		1, 0, 'e' },
	{ "match",		1, 0, 'm' },
	{ "only-values",	0, 0, 'v' },
	{ "no-dereference",	0, 0, 'h' },
	{ "absolute-names",	0, 0, 'a' },
	{ "recursive",		0, 0, 'R' },
	{ "logical",		0, 0, 'L' },
	{ "physical",		0, 0, 'P' },
	{ "version",		0, 0, 'V' },
	{ "help",		0, 0, 'H' },
	{ NULL,			0, 0, 0 }
};

int opt_recursive;  /* recurse into sub-directories? */
int opt_walk_logical;  /* always follow symbolic links */
int opt_walk_physical;  /* never follow symbolic links */
int opt_dump;  /* dump attribute values (or only list the names) */
int opt_deref = 1;  /* dereference symbolic links */
char *opt_name;  /* dump named attributes */
char *opt_name_pattern = "^user\\.";  /* include only matching names */
char *opt_encoding;  /* encode values automatically (NULL), or as "text",
                        "hex", or "base64" */
char opt_value_only;  /* dump the value only, without any decoration */
int opt_strip_leading_slash = 1;  /* strip leading '/' from path names */

const char *progname;
int absolute_warning;
int had_errors;
regex_t name_regex;


static const char *xquote(const char *str)
{
	const char *q = quote(str);
	if (q == NULL) {
		fprintf(stderr, "%s: %s\n", progname, strerror(errno));
		exit(1);
	}
	return q;
}

int do_getxattr(const char *path, const char *name, void *value, size_t size)
{
	return (opt_deref ? getxattr : lgetxattr)(path, name, value, size);
}

int do_listxattr(const char *path, char *list, size_t size)
{
	return (opt_deref ? listxattr : llistxattr)(path, list, size);
}

const char *strerror_ea(int err)
{
	if (err == ENODATA)
		return _("No such attribute");
	return strerror(err);
}

int pstrcmp(const void *a, const void *b)
{
	return strcmp(*(const char **)a, *(const char **)b);
}

int well_enough_printable(const char *value, size_t size)
{
	size_t n, nonpr = 0;

	for (n=0; n < size; n++)
		if (!isprint(*value++))
			nonpr++;

	return (size >= nonpr*8);  /* no more than 1/8 non-printable chars */
}

const char *encode(const char *value, size_t *size)
{
	static char *encoded;
	static size_t encoded_size;
	char *enc, *e;
	
	if (opt_encoding == NULL) {
		if (well_enough_printable(value, *size))
			enc = "text";
		else
			enc = "base64";
	} else
		enc = opt_encoding;

	if (strcmp(enc, "text") == 0) {
		size_t n, extra = 0;

		for (e=(char *)value; e < value + *size; e++) {
			if (!isprint(*e))
				extra += 4;
			else if (*e == '\\' || *e == '"')
				extra++;
		}
		if (high_water_alloc((void **)&encoded, &encoded_size,
				     *size + extra + 3)) {
			perror(progname);
			had_errors++;
			return NULL;
		}
		e = encoded;
		*e++='"';
		for (n = 0; n < *size; n++, value++) {
			if (!isprint(*value)) {
				*e++ = '\\';
				*e++ = '0' + ((unsigned char)*value >> 6);
				*e++ = '0' + (((unsigned char)*value & 070) >> 3);
				*e++ = '0' + ((unsigned char)*value & 07);
			} else if (*value == '\\' || *value == '"') {
				*e++ = '\\';
				*e++ = *value;
			} else {
				*e++ = *value;
			}
		}
		*e++ = '"';
		*e = '\0';
		*size = (e - encoded);
	} else if (strcmp(enc, "hex") == 0) {
		static const char *digits = "0123456789abcdef";
		size_t n;

		if (high_water_alloc((void **)&encoded, &encoded_size,
				     *size * 2 + 4)) {
			perror(progname);
			had_errors++;
			return NULL;
		}
		e = encoded;
		*e++='0'; *e++ = 'x';
		for (n = 0; n < *size; n++, value++) {
			*e++ = digits[((unsigned char)*value >> 4)];
			*e++ = digits[((unsigned char)*value & 0x0F)];
		}
		*e = '\0';
		*size = (e - encoded);
	} else if (strcmp(enc, "base64") == 0) {
		static const char *digits = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
					    "ghijklmnopqrstuvwxyz0123456789+/";
		size_t n;

		if (high_water_alloc((void **)&encoded, &encoded_size,
				     (*size + 2) / 3 * 4 + 1)) {
			perror(progname);
			had_errors++;
			return NULL;
		}
		e = encoded;
		*e++='0'; *e++ = 's';
		for (n=0; n + 2 < *size; n += 3) {
			*e++ = digits[(unsigned char)value[0] >> 2];
			*e++ = digits[(((unsigned char)value[0] & 0x03) << 4) |
			              (((unsigned char)value[1] & 0xF0) >> 4)];
			*e++ = digits[(((unsigned char)value[1] & 0x0F) << 2) |
			              ((unsigned char)value[2] >> 6)];
			*e++ = digits[(unsigned char)value[2] & 0x3F];
			value += 3;
		}
		if (*size - n == 2) {
			*e++ = digits[(unsigned char)value[0] >> 2];
			*e++ = digits[(((unsigned char)value[0] & 0x03) << 4) |
			              (((unsigned char)value[1] & 0xF0) >> 4)];
			*e++ = digits[((unsigned char)value[1] & 0x0F) << 2];
			*e++ = '=';
		} else if (*size - n == 1) {
			*e++ = digits[(unsigned char)value[0] >> 2];
			*e++ = digits[((unsigned char)value[0] & 0x03) << 4];
			*e++ = '=';
			*e++ = '=';
		}
		*e = '\0';
		*size = (e - encoded);
	}
	return encoded;
}

int print_attribute(const char *path, const char *name, int *header_printed)
{
	static char *value;
	static size_t value_size;
	ssize_t length = 0;

	if (opt_dump || opt_value_only) {
		length = do_getxattr(path, name, NULL, 0);
		if (length < 0) {
			fprintf(stderr, "%s: ", xquote(path));
			fprintf(stderr, "%s: %s\n", xquote(name),
				strerror_ea(errno));
			return 1;
		}
		if (high_water_alloc((void **)&value, &value_size, length)) {
			perror(progname);
			had_errors++;
			return 1;
		}
		length = do_getxattr(path, name, value, value_size);
		if (length < 0) {
			fprintf(stderr, "%s: ", xquote(path));
			fprintf(stderr, "%s: %s\n", xquote(name),
				strerror_ea(errno));
			return 1;
		}
	}

	if (opt_strip_leading_slash) {
		if (*path == '/') {
			if (!absolute_warning) {
				fprintf(stderr, _("%s: Removing leading '/' "
					"from absolute path names\n"),
					progname);
				absolute_warning = 1;
			}
			while (*path == '/')
				path++;
		} else if (*path == '.' && *(path+1) == '/')
			while (*++path == '/')
				/* nothing */ ;
		if (*path == '\0')
			path = ".";
	}

	if (!*header_printed && !opt_value_only) {
		printf("# file: %s\n", xquote(path));
		*header_printed = 1;
	}

	if (opt_value_only)
		fwrite(value, length, 1, stdout);
	else if (length) {
		const char *enc = encode(value, &length);
		
		if (enc)
			printf("%s=%s\n", xquote(name), enc);
	} else
		puts(xquote(name));

	return 0;
}

int list_attributes(const char *path, int *header_printed)
{
	static char *list;
	static size_t list_size;
	static char **names;
	static size_t names_size;
	int num_names = 0;
	ssize_t length;
	char *l;

	length = do_listxattr(path, NULL, 0);
	if (length < 0) {
		fprintf(stderr, "%s: %s: %s\n", progname, xquote(path),
			strerror_ea(errno));
		had_errors++;
		return 1;
	} else if (length == 0)
		return 0;
		
	if (high_water_alloc((void **)&list, &list_size, length)) {
		perror(progname);
		had_errors++;
		return 1;
	}

	length = do_listxattr(path, list, list_size);
	if (length < 0) {
		perror(xquote(path));
		had_errors++;
		return 1;
	}

	for (l = list; l != list + length; l = strchr(l, '\0')+1) {
		if (*l == '\0')	/* not a name, kernel bug */
			continue;

		if (regexec(&name_regex, l, 0, NULL, 0) != 0)
			continue;

		if (names_size < (num_names+1) * sizeof(*names)) {
			if (high_water_alloc((void **)&names, &names_size,
				             (num_names+1) * sizeof(*names))) {
				perror(progname);
				had_errors++;
				return 1;
			}
		}

		names[num_names++] = l;
	}

	qsort(names, num_names, sizeof(*names), pstrcmp);

	if (num_names) {
		int n;

		for (n = 0; n < num_names; n++)
			print_attribute(path, names[n], header_printed);
	}
	return 0;
}

int do_print(const char *path, const struct stat *stat,
             int flag, struct FTW *ftw)
{
	int saved_errno = errno;
	int header_printed = 0;

	/*
	 * Process the target of a symbolic link, and traverse the
	 * link, only if doing a logical walk, or if the symbolic link
	 * was specified on the command line. Always skip symbolic
	 * links if doing a physical walk.
	 */

	if (S_ISLNK(stat->st_mode) &&
	    (opt_walk_physical || (ftw->level > 0 && !opt_walk_logical)))
		return 0;

	if (opt_name)
		print_attribute(path, opt_name, &header_printed);
	else
		list_attributes(path, &header_printed);

	if (header_printed)
		puts("");

	if (flag == FTW_DNR && opt_recursive) {
		/* Item is a directory which can't be read. */
		fprintf(stderr, "%s: %s: %s\n", progname, xquote(path),
			strerror(saved_errno));
		return 0;
	}

	/*
	 * We also get here in non-recursive mode. In that case,
	 *  return something != 0 to abort nftw.
	 */

	if (!opt_recursive)
		return 1;
	return 0;
}

void help(void)
{
	printf(_("%s %s -- get extended attributes\n"),
	       progname, VERSION);
	printf(_("Usage: %s %s\n"),
	         progname, _(CMD_LINE_SPEC));
	printf(_(
"  -n, --name=name         get the named extended attribute value\n"
"  -d, --dump              get all extended attribute values\n"
"  -e, --encoding=...      encode values (as 'text', 'hex' or 'base64')\n"
"      --match=pattern     only get attributes with names matching pattern\n"
"      --only-values       print the bare values only\n"
"  -h, --no-dereference    do not dereference symbolic links\n"
"      --absolute-names    don't strip leading '/' in pathnames\n"
"  -R, --recursive         recurse into subdirectories\n"
"  -L, --logical           logical walk, follow symbolic links\n"
"  -P  --physical          physical walk, do not follow symbolic links\n"
"      --version           print version and exit\n"
"      --help              this help text\n"));
}


int main(int argc, char *argv[])
{
	int opt;

	progname = basename(argv[0]);

	setlocale(LC_CTYPE, "");
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	while ((opt = getopt_long(argc, argv, CMD_LINE_OPTIONS,
		                  long_options, NULL)) != -1) {
		switch(opt) {
			case 'a': /* absolute names */
				opt_strip_leading_slash = 0;
				break;

			case 'd': /* dump attribute values */
				opt_dump = 1;
				break;

			case 'e':  /* encoding */
				if (strcmp(optarg, "text") != 0 &&
				    strcmp(optarg, "hex") != 0 &&
				    strcmp(optarg, "base64") != 0)
					goto synopsis;
				opt_encoding = optarg;
				break;

			case 'H':
				help();
				return 0;

			case 'h': /* do not dereference symlinks */
				opt_deref = 0;
				break;

			case 'n':  /* get named attribute */
				opt_dump = 1;
				opt_name = optarg;
				break;

			case 'm':  /* regular expression for filtering names */
				opt_name_pattern = optarg;
				if (strcmp(opt_name_pattern, "-") == 0)
					opt_name_pattern = "";
				break;

			case 'v':  /* get attribute values only */
				opt_value_only = 1;
				break;

			case 'L':
				opt_walk_logical = 1;
				opt_walk_physical = 0;
				break;

			case 'P':
				opt_walk_logical = 0;
				opt_walk_physical = 1;
				break;

			case 'R':
				opt_recursive = 1;
				break;

			case 'V':
				printf("%s " VERSION "\n", progname);
				return 0;

			case ':':  /* option missing */
			case '?':  /* unknown option */
			default:
				goto synopsis;
		}
	}
	if (optind >= argc)
		goto synopsis;

	if (regcomp(&name_regex, opt_name_pattern,
	            REG_EXTENDED | REG_NOSUB) != 0) {
		fprintf(stderr, _("%s: invalid regular expression \"%s\"\n"),
			progname, opt_name_pattern);
		return 1;
	}

	while (optind < argc) {
		if (nftw(argv[optind], do_print, 0,
			 opt_walk_physical * FTW_PHYS) < 0) {
			fprintf(stderr, "%s: %s: %s\n", progname, argv[optind],
			        strerror_ea(errno));
			had_errors++;
		}
		optind++;
	}

	return (had_errors ? 1 : 0);

synopsis:
	fprintf(stderr, _("Usage: %s %s\n"
	                  "Try `%s --help' for more information.\n"),
		progname, CMD_LINE_SPEC, progname);
	return 2;
}

