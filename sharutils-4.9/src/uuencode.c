
static const char cright_years_z[] =

/* uuencode utility.
   Copyright (C) */ "1994, 1995, 1996, 2002, 2005, 2007, 2010";

/* Free Software Foundation, Inc.

   This product is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This product is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see:  http://www.gnu.org/licenses.
*/

/* Copyright (c) 1983 Regents of the University of California.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. All advertising materials mentioning features or use of this software
      must display the following acknowledgement:
	 This product includes software developed by the University of
	 California, Berkeley and its contributors.
   4. Neither the name of the University nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.  */

/* Reworked to GNU style by Ian Lance Taylor, ian@airs.com, August 93.  */

/*=======================================================\
| uuencode [INPUT] OUTPUT				 |
| 							 |
| Encode a file so it can be mailed to a remote system.	 |
\=======================================================*/

#include "local.h"

#if HAVE_LOCALE_H
#else
# define setlocale(Category, Locale)
#endif
#define _(str) gettext (str)

#if __CYGWIN__
# include <fcntl.h>
# include <io.h>
#endif

#define	RW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

static struct option longopts[] =
{
  { "base64", 0, 0, 'm' },
  { "version", 0, 0, 'v' },
  { "help", 0, 0, 'h' },
  { NULL, 0, 0, 0 }
};

static inline void try_putchar __P ((int));
static void encode __P ((void));
static void usage __P ((int))
#if defined __GNUC__ && ((__GNUC__ == 2 && __GNUC_MINOR__ >= 5) || __GNUC__ > 2)
     __attribute__ ((noreturn))
#endif
;

/* The name this program was run with. */
const char *program_name;

/* Pointer to the translation table we currently use.  */
const char *trans_ptr;

/* The two currently defined translation tables.  The first is the
   standard uuencoding, the second is base64 encoding.  */
const char uu_std[64] =
{
  '`', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\', ']', '^', '_'
};

const char uu_base64[64] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

/* ENC is the basic 1 character encoding function to make a char printing.  */
#define ENC(Char) (trans_ptr[(Char) & 077])

static inline void
try_putchar (c)
         int c;
{
  if (putchar (c) == EOF)
    error (EXIT_FAILURE, 0, _("Write error"));
}

/*------------------------------------------------.
| Copy from IN to OUT, encoding as you go along.  |
`------------------------------------------------*/

static void
encode ()
{
  register int n;
  int finishing = 0;
  register char *p;
  char buf[46];  /* 45 should be enough, but one never knows... */

  while ( !finishing && (n = fread (buf, 1, 45, stdin)) > 0 )
    {
      if (n < 45)
        {
          if (feof (stdin))
            finishing = 1;
          else
            error (EXIT_FAILURE, 0, _("Read error"));
        }

      if (trans_ptr == uu_std)
        putchar (ENC (n));

      for (p = buf; n > 2; n -= 3, p += 3)
        {
          try_putchar (ENC (*p >> 2));
          try_putchar (ENC (((*p << 4) & 060) | ((p[1] >> 4) & 017)));
          try_putchar (ENC (((p[1] << 2) & 074) | ((p[2] >> 6) & 03)));
          try_putchar (ENC (p[2] & 077));
        }

      if (n > 0)  /* encode the last one or two chars */
        {
          char tail = trans_ptr == uu_std ? ENC ('\0') : '=';

          if (n == 1)
            p[1] = '\0';

          try_putchar (ENC (*p >> 2));
          try_putchar (ENC (((*p << 4) & 060) | ((p[1] >> 4) & 017)));
          try_putchar (n == 1 ? tail : ENC ((p[1] << 2) & 074));
          try_putchar (tail);
        }

      try_putchar ('\n');
    }

  if (ferror (stdin))
    error (EXIT_FAILURE, 0, _("Read error"));
  if (fclose (stdin) != 0)
    error (EXIT_FAILURE, errno, _("Read error"));

  if (trans_ptr == uu_std)
    {
      try_putchar (ENC ('\0'));
      try_putchar ('\n');
    }
}

static void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
	     program_name);
  else
    {
      printf (_("Usage: %s [INFILE] REMOTEFILE\n"), program_name);
      fputs (_("\n\
  -m, --base64    use base64 encoding as of RFC1521\n\
      --help      display this help and exit\n\
      --version   output version information and exit\n"), stdout);
      /* TRANSLATORS: add the contact address for your translation team! */
      printf (_("Report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  exit (status);
}

int
main (argc, argv)
     int argc;
     char *const *argv;
{
  int opt;
  struct stat sb;
  int mode;

  /* Set global variables.  */
  trans_ptr = uu_std;		/* Standard encoding is old uu format.  */

  program_name = argv[0];
  setlocale (LC_ALL, "");

  /* Set the text message domain.  */
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  while (opt = getopt_long (argc, argv, "hm", longopts, (int *) NULL),
	 opt != EOF)
    {
      switch (opt)
	{
	case 'h':
	  usage (EXIT_SUCCESS);

	case 'm':
	  trans_ptr = uu_base64;
	  break;

	case 'v':
	  printf ("%s (GNU %s) %s\n", basename (program_name),
		  PACKAGE, VERSION);
	  /* xgettext: no-wrap */
	  printf (_("Copyright (C) %s Free Software Foundation, Inc.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"),
		  cright_years_z);
	  exit (EXIT_SUCCESS);

	case 0:
	  break;

	default:
	  usage (EXIT_FAILURE);
	}
    }

  switch (argc - optind)
    {
    case 2:
      /* Optional first argument is input file.  */
      {
#if S_IRWXU != 0700
        choke me - Must translate mode argument
#endif

	FILE *fp = freopen (argv[optind], FOPEN_READ_BINARY, stdin);
	if (fp != stdin)
	  error (EXIT_FAILURE, errno, _("fopen-ing %s"), argv[optind]);
	if (fstat (fileno (stdin), &sb) != 0)
	  error (EXIT_FAILURE, errno, _("fstat-ing %s"), argv[optind]);
	mode = sb.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	optind++;
	break;
      }

    case 1:
#if __CYGWIN__
      if (! isatty (STDIN_FILENO))
       setmode (STDIN_FILENO, O_BINARY);
#endif

      mode = RW & ~umask (RW);
      break;

    case 0:
    default:
      usage (EXIT_FAILURE);
    }

  if (printf ("begin%s %o %s\n", trans_ptr == uu_std ? "" : "-base64",
	      mode, argv[optind]) < 0)
    error (EXIT_FAILURE, errno, _("Write error"));

  encode ();

  if (ferror (stdout) ||
      printf (trans_ptr == uu_std ? "end\n" : "====\n") < 0 ||
      fclose (stdout) != 0)
    error (EXIT_FAILURE, errno, _("Write error"));

  exit (EXIT_SUCCESS);
}
