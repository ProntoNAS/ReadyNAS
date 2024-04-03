
static const char cright_years_z[] =

/* uudecode utility.
   Copyright (C) */ "1994, 1995, 1996, 2002, 2005, 2006, 2007, 2010";

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

#include "local.h"
#include "liballoca.h"


#if HAVE_LOCALE_H
#else
# define setlocale(Category, Locale)
#endif
#define _(str) gettext (str)

/*=====================================================================\
| uudecode [FILE ...]						       |
| 								       |
| Create the specified FILE, decoding as you go.  Used with uuencode.  |
\=====================================================================*/

#include <pwd.h>

#define UU_MODE_BITS(_M)        ((_M) & (S_IRWXU | S_IRWXG | S_IRWXO))
#if HAVE_FCHMOD
#define UU_CHMOD(_n, _fd, _m)   fchmod ((_fd), UU_MODE_BITS(_m))
#else
#define UU_CHMOD(_n, _fd, _m)   chmod ((_n), UU_MODE_BITS(_m))
#endif

struct passwd *getpwnam ();

static struct option longopts[] =
{
  { "version", no_argument, NULL, 'v' },
  { "help", no_argument, NULL, 'h' },
  { "output-file", required_argument, NULL, 'o' },
  { NULL, 0, NULL, 0 }
};

static int read_stduu  __P ((const char *inname, const char *outname));
static int read_base64 __P ((const char *inname, const char *outname));
static int decode __P ((const char *, const char *));
static void usage __P ((int))
#if defined __GNUC__ && ((__GNUC__ == 2 && __GNUC_MINOR__ >= 5) || __GNUC__ > 2)
     __attribute__ ((noreturn))
#endif
;

#ifndef NUL
#define NUL '\0'
#endif

/* The name this program was run with. */
const char *program_name;

/* Single character decode.  */
#define	DEC(Char) (((Char) - ' ') & 077)

#if !defined S_ISLNK && defined S_IFLNK
# define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif

#define TRY_PUTCHAR(c) do { \
                         if (putchar (c) == EOF) { \
                           error (0, 0, _("%s: Write error"), outname); \
                           return 1; \
                         } \
                       } while (0)

static int
read_stduu (inname, outname)
     const char *inname, *outname;
{
  char buf[2 * BUFSIZ];

  while (1)
    {
      int n;
      char *p;

      if (fgets ((char *) buf, sizeof(buf), stdin) == NULL)
	{
	  error (0, 0, _("%s: Short file"), inname);
	  return EXIT_FAILURE;
	}
      p = buf;

      /* N is used to avoid writing out all the characters at the end of
	 the file.  */

      n = DEC (*p);
      if (n <= 0)
	break;
      for (++p; n >= 3; p += 4, n -= 3)
	{
	  TRY_PUTCHAR (DEC (p[0]) << 2 | DEC (p[1]) >> 4);
	  TRY_PUTCHAR (DEC (p[1]) << 4 | DEC (p[2]) >> 2);
	  TRY_PUTCHAR (DEC (p[2]) << 6 | DEC (p[3]));
        }
      if (n > 0)
	{
	  TRY_PUTCHAR (DEC (p[0]) << 2 | DEC (p[1]) >> 4);
	  if (n >= 2)
	    TRY_PUTCHAR (DEC (p[1]) << 4 | DEC (p[2]) >> 2);
	}
    }

  do {
    if (fgets (buf, sizeof(buf), stdin) == NULL)
      break;

    if (strcmp (buf, "end\n") == 0)
      return EXIT_SUCCESS;

    if (strcmp (buf, "end\r\n") == 0)
      return EXIT_SUCCESS;
  } while (0);

  error (0, 0, _("%s: No `end' line"), inname);
  return EXIT_FAILURE;
}

static int
read_base64 (inname, outname)
     const char *inname, *outname;
{
  static const char b64_tab[256] =
  {
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*000-007*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*010-017*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*020-027*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*030-037*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*040-047*/
    '\177', '\177', '\177', '\76',  '\177', '\177', '\177', '\77',  /*050-057*/
    '\64',  '\65',  '\66',  '\67',  '\70',  '\71',  '\72',  '\73',  /*060-067*/
    '\74',  '\75',  '\177', '\177', '\177', '\100', '\177', '\177', /*070-077*/
    '\177', '\0',   '\1',   '\2',   '\3',   '\4',   '\5',   '\6',   /*100-107*/
    '\7',   '\10',  '\11',  '\12',  '\13',  '\14',  '\15',  '\16',  /*110-117*/
    '\17',  '\20',  '\21',  '\22',  '\23',  '\24',  '\25',  '\26',  /*120-127*/
    '\27',  '\30',  '\31',  '\177', '\177', '\177', '\177', '\177', /*130-137*/
    '\177', '\32',  '\33',  '\34',  '\35',  '\36',  '\37',  '\40',  /*140-147*/
    '\41',  '\42',  '\43',  '\44',  '\45',  '\46',  '\47',  '\50',  /*150-157*/
    '\51',  '\52',  '\53',  '\54',  '\55',  '\56',  '\57',  '\60',  /*160-167*/
    '\61',  '\62',  '\63',  '\177', '\177', '\177', '\177', '\177', /*170-177*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*200-207*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*210-217*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*220-227*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*230-237*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*240-247*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*250-257*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*260-267*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*270-277*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*300-307*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*310-317*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*320-327*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*330-337*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*340-347*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*350-357*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*360-367*/
    '\177', '\177', '\177', '\177', '\177', '\177', '\177', '\177', /*370-377*/
  };
  char buf[2 * BUFSIZ];

  while (1)
    {
      int last_data = 0;
      unsigned char *p;

      if (fgets (buf, sizeof(buf), stdin) == NULL)
	{
	  error (0, 0, _("%s: Short file"), inname);
	  return EXIT_FAILURE;
	}
      p = (unsigned char *) buf;

      if (memcmp (buf, "====", 4) == 0)
	break;
      if (last_data != 0)
	{
	  error (0, 0, _("%s: data following `=' padding character"), inname);
	  return EXIT_FAILURE;
	}

      /* The following implementation of the base64 decoding might look
	 a bit clumsy but I only try to follow the POSIX standard:
	 ``All line breaks or other characters not found in the table
	   [with base64 characters] shall be ignored by decoding
	   software.''  */
      while (*p != '\n')
	{
	  char c1, c2, c3;

	  while ((b64_tab[*p] & '\100') != 0)
	    if (*p == '\n' || *p++ == '=')
	      break;
	  if (*p == '\n')
	    /* This leaves the loop.  */
	    continue;
	  c1 = b64_tab[*p++];

	  while ((b64_tab[*p] & '\100') != 0)
	    if (*p == '\n' || *p++ == '=')
	      {
		error (0, 0, _("%s: illegal line"), inname);
		return 1;
	      }
	  c2 = b64_tab[*p++];

	  while (b64_tab[*p] == '\177')
	    if (*p++ == '\n')
	      {
		error (0, 0, _("%s: illegal line"), inname);
		return 1;
	      }
	  if (*p == '=')
	    {
	      TRY_PUTCHAR (c1 << 2 | c2 >> 4);
	      last_data = 1;
	      break;
	    }
	  c3 = b64_tab[*p++];

	  while (b64_tab[*p] == '\177')
	    if (*p++ == '\n')
	      {
		error (0, 0, _("%s: illegal line"), inname);
		return 1;
	      }
	  TRY_PUTCHAR (c1 << 2 | c2 >> 4);
	  TRY_PUTCHAR (c2 << 4 | c3 >> 2);
	  if (*p == '=')
	    {
	      last_data = 1;
	      break;
	    }
	  else
	    TRY_PUTCHAR (c3 << 6 | b64_tab[*p++]);
	}
    }

  return EXIT_SUCCESS;
}


static char *
expand_tilde (char * buf)
{
  char * outname;
  int nm_len, n1;
  struct passwd *pw;
  char * pz = buf + 1;
  while (*pz != '/')
    ++pz;
  if (*pz == NUL)
    {
      error (0, 0, _("%s: Illegal file name: %s"), program_name, buf);
      return NULL;
    }
  *pz++ = NUL;
  pw = getpwnam (buf + 1);
  if (pw == NULL)
    {
      error (0, 0, _("No user `%s'"), buf + 1);
      return NULL;
    }
  nm_len = strlen (pw->pw_dir);
  n1 = strlen (pz);

  {
    size_t sz = nm_len + n1 + 2;
    outname = (char *) malloc (sz);
    if (outname == NULL)
      {
        error (0, 0, _("no memory for %d byte allocation"), (int)sz);
        return NULL;
      }
  }

  memcpy (outname, pw->pw_dir, (size_t) nm_len);
  outname[nm_len] = '/';
  memcpy (outname + nm_len + 1, pz, (size_t) (n1 + 1));
  return outname;
}

/*
 * Create output file and set mode.
 */
static int
reopen_output (char const * outname, int mode)
{
  struct stat attr;
  FILE* fp;

  /* Check out file if it exists */
  if (!access(outname, F_OK))
    {
      if (lstat(outname, &attr) == -1)
        {
          error (0, errno, _("cannot access %s"), outname);
          return EXIT_FAILURE;
        }
      if (S_ISFIFO(attr.st_mode))
        {
          error (0, errno, _("denied writing FIFO (%s)"), outname);
          return EXIT_FAILURE;
        }
      if (S_ISLNK(attr.st_mode))
        {
          error (0, errno, _("not following symlink (%s)"), outname);
          return EXIT_FAILURE;
        }
    }

  fp = freopen (outname, FOPEN_WRITE_BINARY, stdout);
  if (fp != stdout)
    {
      error (0, errno, _("freopen of %s"), outname);
      return EXIT_FAILURE;
    }

  if (UU_CHMOD(outname, fileno (fp), mode) != 0)
    {
      error (0, errno, _("chmod of %s"), outname);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

static int
decode (inname, forced_outname)
     const char *inname;
     const char *forced_outname;
{
  char *pz;
  int   mode;
  char  buf[2 * BUFSIZ] = { NUL };
  char *outname;
  int   do_base64 = 0;
  int   rval;
  int   allocated_outname = 0;

  /* Search for header line.  */

  while (1)
    {
      if (fgets (buf, sizeof (buf), stdin) == NULL)
	{
	  error (0, 0, _("%s: No `begin' line"), inname);
	  return EXIT_FAILURE;
	}

      if (strncmp (buf, "begin", 5) == 0)
	{
	  if (sscanf (buf+5, "-base64 %o %[^\n]", &mode, buf) == 2)
	    {
	      do_base64 = 1;
	      break;
	    }
	  if (sscanf (buf+5, " %o %[^\n]", &mode, buf) == 2)
	    break;
	}
    }

  /* If the output file name is given on the command line this rules.  */
  if (forced_outname != NULL)
    outname = (char *) forced_outname;
  else
    {
      if (buf[0] != '~')
	outname = buf;
      else
        {
          /* Handle ~user/file format.  */

          outname = expand_tilde (buf);
          if (outname == NULL)
            return EXIT_FAILURE;
          allocated_outname = 1;
        }

      /* Be certain there is no trailing white space */
      pz = outname + strlen (outname);
      while ((pz > outname) && isspace ((int)pz[-1]))
        pz--;
      *pz = NUL;
    }

  if (  (strcmp (outname, "/dev/stdout") != 0)
     && (strcmp (outname, "-") != 0) )
    {
      rval = reopen_output (outname, mode);
      if (rval != EXIT_SUCCESS)
        goto fail_return;
    }

  /* We differenciate decoding standard UU encoding and base64.  A
     common function would only slow down the program.  */

  /* For each input line:  */
  if (do_base64)
    rval = read_base64 (inname, outname);
  else
    rval = read_stduu (inname, outname);

  if (rval == EXIT_SUCCESS && (ferror(stdout) || fflush(stdout) != 0))
    {
      error (0, 0, _("%s: Write error"), outname);
      rval = EXIT_FAILURE;
    }

 fail_return:

  if (allocated_outname)
    free (outname);
  return rval;
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
      printf (_("Usage: %s [FILE]...\n"), program_name);
      fputs (_("\
Mandatory arguments to long options are mandatory to short options too.\n\
  -o, --output-file=FILE   direct output to FILE\n\
      --help               display this help and exit\n\
      --version            output version information and exit\n"), stdout);
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
  int exit_status;
  const char *outname;

  program_name = argv[0];
  outname = NULL;
  setlocale (LC_ALL, "");

  /* Set the text message domain.  */
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  while (opt = getopt_long (argc, argv, "o:", longopts, (int *) NULL),
	 opt != EOF)
    {
      switch (opt)
	{
	case 'h':
	  usage (EXIT_SUCCESS);

	case 'o':
	  outname = optarg;
	  break;

	case 'v':
	  printf ("%s (GNU %s) %s\n", basename (program_name),
		  PACKAGE, VERSION);
	  /* xgettext: no-wrap */
	  printf (_("Copyright (C) %s Free Software Foundation, Inc.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
"),
		  cright_years_z);
	  exit (EXIT_SUCCESS);

	case 0:
	  break;

	default:
	  usage (EXIT_FAILURE);
	}
    }

  if (optind == argc)
    exit_status = decode ("stdin", outname) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
  else
    {
      exit_status = EXIT_SUCCESS;
      do
	{
	  if (freopen (argv[optind], "r", stdin) != NULL)
	    {
	      exit_status |= decode (argv[optind], outname);
	    }
	  else
	    {
	      error (0, errno, "%s", argv[optind]);
	      exit_status = EXIT_FAILURE;
	    }
	  optind++;
	}
      while (optind < argc);
    }

  exit (exit_status);
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "gnu"
 * indent-tabs-mode: nil
 * End:
 * end of uudecode.c */
