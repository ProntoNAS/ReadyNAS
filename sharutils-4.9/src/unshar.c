
static const char cright_years_z[] =

/* Handle so called `shell archives'.
   Copyright (C) */ "1994, 1995, 1996, 2002, 2005, 2006, 2007, 2010";

/* Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see:  http://www.gnu.org/licenses.
*/

/* Unpackage one or more shell archive files.  The `unshar' program is a
   filter which removes the front part of a file and passes the rest to
   the `sh' command.  It understands phrases like "cut here", and also
   knows about shell comment characters and the Unix commands `echo',
   `cat', and `sed'.  */

#include "local.h"

#include "xgetcwd.h"

#if HAVE_LOCALE_H
#else
# define setlocale(Category, Locale)
#endif
#define _(str) gettext (str)

/* Buffer size for shell process input.  */
#define SHELL_BUFFER_SIZE 8196

#define EOL '\n'

/* The name this program was run with. */
const char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help = 0;

/* If non-zero, print the version on standard output and exit.  */
static int show_version = 0;

static int pass_c_flag = 0;
static int continue_reading = 0;
static const char *exit_string = "exit 0";
static size_t exit_string_length;
static char *current_directory;

/*-------------------------------------------------------------------.
| Match the leftmost part of a string.  Returns 1 if initial	     |
| characters of DATA match PATTERN exactly; else 0.  This was	     |
| formerly a function.  But because we always have a constant string |
| as the seconf argument and the length of the second argument is a  |
| lot of shorter than the buffer the first argument is pointing at,  |
| we simply use `memcmp'.  And one more point: even if the `memcmp'  |
| function does not work correct for 8 bit characters it does not    |
| matter here.  We are only interested in equal or not equal	     |
| information.							     |
`-------------------------------------------------------------------*/

#define starting_with(data, pattern)					    \
  (memcmp (data, pattern, sizeof (pattern) - 1) == 0)

static FILE* load_stdin (char **p_name_buf);

/*-------------------------------------------------------------------------.
| For a DATA string and a PATTERN containing one or more embedded	   |
| asterisks (matching any number of characters), return non-zero if the	   |
| match succeeds, and set RESULT_ARRAY[I] to the characters matched by the |
| I'th *.								   |
`-------------------------------------------------------------------------*/

static int
matched_by (data, pattern, result_array)
     const char *data;
     const char *pattern;
     char **result_array;
{
  const char *pattern_cursor = NULL;
  const char *data_cursor = NULL;
  char *result_cursor = NULL;
  int number_of_results = 0;

  while (1)
    if (*pattern == '*')
      {
	pattern_cursor = ++pattern;
	data_cursor = data;
	result_cursor = result_array[number_of_results++];
	*result_cursor = '\0';
      }
    else if (*data == *pattern)
      {
	if (*pattern == '\0')
	  /* The pattern matches.  */
	  return 1;

	pattern++;
	data++;
      }
    else
      {
	if (*data == '\0')
	  /* The pattern fails: no more data.  */
	  return 0;

	if (pattern_cursor == NULL)
	  /* The pattern fails: no star to adjust.  */
	  return 0;

	/* Restart pattern after star.  */

	pattern = pattern_cursor;
	*result_cursor++ = *data_cursor;
	*result_cursor = '\0';

	/* Rescan after copied char.  */

	data = ++data_cursor;
      }
}

/*------------------------------------------------------------------------.
| Associated with a given file NAME, position FILE at the start of the	  |
| shell command portion of a shell archive file.  Scan file from position |
| START.								  |
`------------------------------------------------------------------------*/

static int
find_archive (name, file, start)
     const char *name;
     FILE *file;
     off_t start;
{
  char buffer[BUFSIZ];
  off_t position;

  /* Results from star matcher.  */

  static char res1[BUFSIZ], res2[BUFSIZ], res3[BUFSIZ], res4[BUFSIZ];
  static char *result[] = {res1, res2, res3, res4};

  fseeko (file, start, SEEK_SET);

  while (1)
    {

      /* Record position of the start of this line.  */

      position = ftello (file);

      /* Read next line, fail if no more and no previous process.  */

      if (!fgets (buffer, BUFSIZ, file))
	{
	  if (!start)
	    error (0, 0, _("Found no shell commands in %s"), name);
	  return 0;
	}

      /* Bail out if we see C preprocessor commands or C comments.  */

      if (starting_with (buffer, "#include")
	  || starting_with (buffer, "# include")
	  || starting_with (buffer, "#define")
	  || starting_with (buffer, "# define")
	  || starting_with (buffer, "#ifdef")
	  || starting_with (buffer, "# ifdef")
	  || starting_with (buffer, "#ifndef")
	  || starting_with (buffer, "# ifndef")
	  || starting_with (buffer, "/*"))
	{
	  error (0, 0, _("%s looks like raw C code, not a shell archive"),
		 name);
	  return 0;
	}

      /* Does this line start with a shell command or comment.  */

      if (starting_with (buffer, "#")
	  || starting_with (buffer, ":")
	  || starting_with (buffer, "echo ")
	  || starting_with (buffer, "sed ")
	  || starting_with (buffer, "cat ")
	  || starting_with (buffer, "if "))
	{
	  fseeko (file, position, SEEK_SET);
	  return 1;
	}

      /* Does this line say "Cut here".  */

      if (matched_by (buffer, "*CUT*HERE*", result) ||
	  matched_by (buffer, "*cut*here*", result) ||
	  matched_by (buffer, "*TEAR*HERE*", result) ||
	  matched_by (buffer, "*tear*here*", result) ||
	  matched_by (buffer, "*CUT*CUT*", result) ||
	  matched_by (buffer, "*cut*cut*", result))
	{

	  /* Read next line after "cut here", skipping blank lines.  */

	  while (1)
	    {
	      position = ftello (file);

	      if (!fgets (buffer, BUFSIZ, file))
		{
		  error (0, 0, _("Found no shell commands after `cut' in %s"),
			 name);
		  return 0;
		}

	      if (*buffer != '\n')
		break;
	    }

	  /* Win if line starts with a comment character of lower case
	     letter.  */

	  if (*buffer == '#' || *buffer == ':'
	      || (('a' <= *buffer) && ('z' >= *buffer)))
	    {
	      fseeko (file, position, SEEK_SET);
	      return 1;
	    }

	  /* Cut here message lied to us.  */

	  error (0, 0, _("%s is probably not a shell archive"), name);
	  error (0, 0, _("The `cut' line was followed by: %s"), buffer);
	  return 0;
	}
    }
}

/*-----------------------------------------------------------------.
| Unarchive a shar file provided on file NAME.  The file itself is |
| provided on the already opened FILE.				   |
`-----------------------------------------------------------------*/

static void
unarchive_shar_file (name, file)
     const char *name;
     FILE *file;
{
  char buffer[SHELL_BUFFER_SIZE];
  FILE *shell_process;
  off_t current_position = 0;
  char *more_to_read;

  while (find_archive (name, file, current_position))
    {
      printf ("%s:\n", name);
      shell_process = popen (pass_c_flag ? "sh -s - -c" : "sh", "w");
      if (!shell_process)
	error (EXIT_FAILURE, errno, _("Starting `sh' process"));

      if (!continue_reading)
	{
	  size_t len;

	  while ((len = fread (buffer, 1, SHELL_BUFFER_SIZE, file)) != 0)
	    fwrite (buffer, 1, len, shell_process);
#if 0
	  /* Don't know whether a test is appropriate here.  */
	  if (ferror (shell_process) != 0)
	    fwrite (buffer, length, 1, shell_process);
#endif
	  pclose (shell_process);
	  break;
	}
      else
	{
	  while (more_to_read = fgets (buffer, SHELL_BUFFER_SIZE, file),
		 more_to_read != NULL)
	    {
	      fputs (buffer, shell_process);
	      if (!strncmp (exit_string, buffer, exit_string_length))
		break;
	    }
	  pclose (shell_process);

	  if (more_to_read)
	    current_position = ftello (file);
	  else
	    break;
	}
    }
}

/*-----------------------------.
| Explain how to use program.  |
`-----------------------------*/

static void
usage (status)
     int status;
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
	     program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [FILE]...\n"), program_name);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
\n\
  -d, --directory=DIRECTORY   change to DIRECTORY before unpacking\n\
  -c, --overwrite             pass -c to shar script for overwriting files\n\
  -e, --exit-0                same as `--split-at=\"exit 0\"'\n\
  -E, --split-at=STRING       split concatenated shars after STRING\n\
  -f, --force                 same as `-c'\n\
      --help                  display this help and exit\n\
      --version               output version information and exit\n\
\n\
If no FILE, standard input is read.\n"),
	     stdout);
      /* TRANSLATORS: add the contact address for your translation team! */
      printf (_("Report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  exit (status);
}

/*--------------------------------------.
| Decode options and launch execution.  |
`--------------------------------------*/

static const struct option long_options[] =
{
  {"directory", required_argument, NULL, 'd'},
  {"exit-0", no_argument, NULL, 'e'},
  {"force", no_argument, NULL, 'f'},
  {"overwrite", no_argument, NULL, 'c'},
  {"split-at", required_argument, NULL, 'E'},

  {"help", no_argument, &show_help, 1},
  {"version", no_argument, &show_version, 1},

  { NULL, 0, NULL, 0 },
};

int
main (argc, argv)
     int argc;
     char *const *argv;
{
  FILE *file;
  char* name_buffer = NULL;
  int optchar;

  program_name = argv[0];
  setlocale (LC_ALL, "");

  /* Set the text message domain.  */
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

#ifdef __MSDOS__
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
#endif

  if (current_directory = xgetcwd (), !current_directory)
    error (EXIT_FAILURE, errno, _("Cannot get current directory name"));

  /* Process options.  */

  while (optchar = getopt_long (argc, argv, "E:cd:ef", long_options, NULL),
	 optchar != EOF)
    switch (optchar)
      {
      case '\0':
	break;

      case 'c':
      case 'f':
	pass_c_flag = 1;
	break;

      case 'd':
	if (chdir (optarg) == -1)
	  error (2, 0, _("Cannot chdir to `%s'"), optarg);
	break;

      case 'E':
	exit_string = optarg;
	/* Fall through.  */

      case 'e':
	continue_reading = 1;
	exit_string_length = strlen (exit_string);
	break;

      default:
	usage (EXIT_FAILURE);
      }

  if (show_version)
    {
      printf ("%s (GNU %s) %s\n", basename (program_name), PACKAGE, VERSION);
      /* xgettext: no-wrap */
      printf (_("Copyright (C) %s Free Software Foundation, Inc.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
"),
	      cright_years_z);
      exit (EXIT_SUCCESS);
    }

  if (show_help)
    usage (EXIT_SUCCESS);

  if (optind < argc)
    {
      size_t buflen = 0;
      argv += optind;

      for (;;)
        {
          char* arg = *(argv++);
          size_t sz;
          if (arg == NULL)
            break;

          sz = strlen (current_directory) + strlen (arg) + 2;
          if (sz > buflen)
            {
              buflen = sz + 32;
              name_buffer = (name_buffer == NULL)
                ? malloc (buflen)
                : realloc (name_buffer, buflen);
              if (name_buffer == NULL)
                error (EXIT_FAILURE, ENOMEM, _("allocate file name buffer"));
            }

          if (*arg == '/')
            strcpy (name_buffer, arg);
          else
            {
              char *cp = stpcpy (name_buffer, current_directory);
              *cp++ = '/';
              strcpy (cp, arg);
            }
          if (file = fopen (name_buffer, "r"), !file)
            error (EXIT_FAILURE, errno, name_buffer);
          unarchive_shar_file (name_buffer, file);
          fclose (file);
        }
    }
  else
    {
      file = load_stdin (&name_buffer);

      unarchive_shar_file (_("standard input"), file);

      fclose (file);
      unlink (name_buffer);
    }

  exit (EXIT_SUCCESS);
}


static FILE*
load_stdin (char **p_name_buf)
{
  static const char z_tmpfile[] = "unsh.XXXXXX";
  char *pz_fname;
  FILE *fp;

  /*
   * FIXME: actually configure this stuff:
   */
#if defined(_SC_PAGESIZE)
  long pg_sz = sysconf (_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
  long pg_sz = sysconf (_SC_PAGE_SIZE);
#elif defined(HAVE_GETPAGESIZE)
  long pg_sz = getpagesize();
#else
# define pg_sz 8192
#endif

  {
    size_t name_size;
    char *pz_tmp = getenv ("TMPDIR");

    if (pz_tmp == NULL)
      pz_tmp = "/tmp";

    name_size = strlen (pz_tmp) + sizeof (z_tmpfile) + 1;
    *p_name_buf = pz_fname = malloc (name_size);

    if (pz_fname == NULL)
      error (EXIT_FAILURE, ENOMEM, _("allocate file name buffer"));

    sprintf (pz_fname, "%s/%s", pz_tmp, z_tmpfile);
  }

  {
    int fd = mkstemp (pz_fname);
    if (fd < 0)
      error (EXIT_FAILURE, errno, pz_fname);

    fp = fdopen (fd, "w+");
  }

  if (fp == NULL)
    error (EXIT_FAILURE, errno, pz_fname);

  {
    char *buf = malloc (pg_sz);
    size_t size_read;

    if (buf == NULL)
      error (EXIT_FAILURE, ENOMEM, _("allocate file buffer"));

    while (size_read = fread (buf, 1, pg_sz, stdin),
           size_read != 0)
      fwrite (buf, size_read, 1, fp);

    free (buf);
  }

  rewind (fp);

  return fp;
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "gnu"
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 * end of agen5/autogen.c */
