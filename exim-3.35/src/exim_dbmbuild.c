/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* A small freestanding program to build dbm databases from serial input. For
alias files, this program fulfils the function of the newaliases program used
by other mailers, but it can be used for other dbm data files too. It operates
by writing a new file or files, and then renaming; otherwise old entries can
never get flushed out.

This program is clever enough to cope with ndbm, which creates two files called
<name>.dir and <name>.pag, or with db, which creates a single file called
<name>.db. If native db is in use (USE_DB defined) or tdb is in use (USE_TDB
defined) there is no extension to the output filename. This is also handled. If
there are any other variants, the program won't cope.

The first argument to the program is the name of the serial file; the second
is the base name for the DBM file(s). When native db is in use, these must be
different.

Input lines beginning with # are ignored, as are blank lines. Entries begin
with a key terminated by a colon or end of line or whitespace and continue with
indented lines. */


#include "exim.h"


#define maxsize 10000


#ifdef STRERROR_FROM_ERRLIST
/* Some old-fashioned systems still around (e.g. SunOS4) don't have strerror()
in their libraries, but can provide the same facility by this simple
alternative function. */

char *
strerror(int n)
{
if (n < 0 || n >= sys_nerr) return "unknown error number";
return sys_errlist[n];
}
#endif /* STRERROR_FROM_ERRLIST */


int main(int argc, char **argv)
{
int started;
int count = 0;
int dupcount = 0;
int yield = 0;
int arg = 1;
int add_zero = 1;
BOOL lowercase = TRUE;
BOOL warn = TRUE;
BOOL duperr = TRUE;
BOOL lastdup = FALSE;
#if !defined (USE_DB) && !defined(USE_TDB)
int is_db = 0;
struct stat statbuf;
#endif
FILE *f;
EXIM_DB *d;
EXIM_DATUM key, content;
char *bptr;
char  keybuffer[256];
char  temp_dbmname[256];
char  real_dbmname[256];
char *buffer = malloc(maxsize);
char *line = malloc(maxsize);

while (argc > 1)
  {
  if      (strcmp(argv[arg], "-nolc") == 0)     lowercase = FALSE;
  else if (strcmp(argv[arg], "-nowarn") == 0)   warn = FALSE;
  else if (strcmp(argv[arg], "-lastdup") == 0)  lastdup = TRUE;
  else if (strcmp(argv[arg], "-noduperr") == 0) duperr = FALSE;
  else if (strcmp(argv[arg], "-nozero") == 0)   add_zero = 0;
  else break;
  arg++;
  argc--;
  }

if (argc != 3)
  {
  printf("usage: exim_dbmbuild [-nolc] <source file> <dbm base name>\n");
  exit(1);
  }

if (strcmp(argv[arg], "-") == 0) f = stdin; else
  {
  f = fopen(argv[arg], "r");
  if (f == NULL)
    {
    printf("exim_dbmbuild: unable to open %s: %s\n", argv[arg], strerror(errno));
    exit(1);
    }
  }

/* By default Berkeley db does not put extensions on... which
can be painful! */

#if defined(USE_DB) || defined(USE_TDB)
if (strcmp(argv[arg], argv[arg+1]) == 0)
  {
  printf("exim_dbmbuild: input and output filenames are the same\n");
  exit(1);
  }
#endif

strcpy(temp_dbmname, argv[arg+1]);
strcat(temp_dbmname, ".dbmbuild_temp");

/* It is apparently necessary to open with O_RDWR for this to work
with gdbm-1.7.3, though no reading is actually going to be done. */

EXIM_DBOPEN(temp_dbmname, O_RDWR|O_CREAT|O_EXCL, 0644, &d);

if (d == NULL)
  {
  printf("exim_dbmbuild: unable to create %s: %s\n", temp_dbmname,
    strerror(errno));
  fclose(f);
  exit(1);
  }

/* Unless using native db calls, see if we have created <name>.db; if not,
assume .dir & .pag */

#if !defined(USE_DB) && !defined(USE_TDB)
sprintf(real_dbmname, "%s.db", temp_dbmname);
is_db = stat(real_dbmname, &statbuf) == 0;
#endif

/* Now do the business */

bptr = buffer;
started = 0;

while (fgets(line, maxsize, f) != NULL)
  {
  char *p;

  if (line[0] == '#') continue;
  p = line + (int)strlen(line);
  while (p > line && isspace((uschar)p[-1])) p--;
  *p = 0;
  if (line[0] == 0) continue;

  /* A continuation line is valid only if there was a previous first
  line. */

  if (isspace((uschar)line[0]))
    {
    char *s = line;
    if (!started)
      {
      printf("Unexpected continuation line ignored\n%s\n\n", line);
      continue;
      }
    while (isspace((uschar)*s)) s++;
    *(--s) = ' ';
    strcpy (bptr, s);
    bptr += p - s;
    }

  /* A first line must have a name followed by a colon or whitespace or
  end of line, but first finish with a previous line. The key is lower
  cased by default - this is what the newaliases program for sendmail does.
  However, there's an option not to do this. */

  else
    {
    int i, rc;
    char *s = line;

    if (started)
      {
      EXIM_DATUM_INIT(content);
      EXIM_DATUM_DATA(content) = buffer;
      EXIM_DATUM_SIZE(content) = bptr - buffer + add_zero;

      switch(rc = EXIM_DBPUTB(d, key, content))
        {
        case EXIM_DBPUTB_OK:
        count++;
        break;

        case EXIM_DBPUTB_DUP:
        if (warn) fprintf(stderr, "** Duplicate key \"%s\"\n",
          keybuffer);
        dupcount++;
        if(duperr) yield = 1;
        if (lastdup) EXIM_DBPUT(d, key, content);
        break;

        default:
        fprintf(stderr, "Error %d while writing key %s: errno=%d\n", rc,
          keybuffer, errno);
        yield = 2;
        goto TIDYUP;
        }

      bptr = buffer;
      }

    while (*s != 0 && *s != ':' && !isspace((uschar)*s)) s++;
    EXIM_DATUM_INIT(key);
    EXIM_DATUM_DATA(key) = keybuffer;
    EXIM_DATUM_SIZE(key) = s - line + add_zero;

    if (EXIM_DATUM_SIZE(key) > 256)
      {
      printf("Keys longer than 255 characters cannot be handled\n");
      started = 0;
      yield = 2;
      goto TIDYUP;
      }

    if (lowercase)
      {
      for (i = 0; i < EXIM_DATUM_SIZE(key) - add_zero; i++)
        keybuffer[i] = tolower(line[i]);
      }
    else
      {
      for (i = 0; i < EXIM_DATUM_SIZE(key) - add_zero; i++)
        keybuffer[i] = line[i];
      }

    keybuffer[i] = 0;
    started = 1;

    while (isspace((uschar)*s))s++;
    if (*s == ':')
      {
      s++;
      while (isspace((uschar)*s))s++;
      }
    if (*s != 0)
      {
      strcpy(bptr, s);
      bptr += p - s;
      }
    else buffer[0] = 0;
    }
  }

if (started)
  {
  int rc;
  EXIM_DATUM_INIT(content);
  EXIM_DATUM_DATA(content) = buffer;
  EXIM_DATUM_SIZE(content) = bptr - buffer + add_zero;

  switch(rc = EXIM_DBPUTB(d, key, content))
    {
    case EXIM_DBPUTB_OK:
    count++;
    break;

    case EXIM_DBPUTB_DUP:
    if (warn) fprintf(stderr, "** Duplicate key \"%s\"\n", keybuffer);
    dupcount++;
    if (duperr) yield = 1;
    if (lastdup) EXIM_DBPUT(d, key, content);
    break;

    default:
    fprintf(stderr, "Error %d while writing key %s: errno=%d\n", rc,
      keybuffer, errno);
    yield = 2;
    break;
    }
  }

/* Close files, rename or abandon the temporary files, and exit */

TIDYUP:

EXIM_DBCLOSE(d);
fclose(f);

/* If successful, output the number of entries and rename the temporary
files. */

if (yield == 0 || yield == 1)
  {
  printf("%d entr%s written\n", count, (count == 1)? "y" : "ies");
  if (dupcount > 0)
    {
    printf("%d duplicate key%s \n", dupcount, (dupcount > 1)? "s" : "");
    }

  #if defined(USE_DB) || defined(USE_TDB)
  strcpy(real_dbmname, temp_dbmname);
  strcpy(buffer, argv[arg+1]);
  if (rename(real_dbmname, buffer) != 0)
    {
    printf("Unable to rename %s as %s\n", real_dbmname, buffer);
    return 1;
    }
  #else

  /* Rename a single .db file */

  if (is_db)
    {
    sprintf(real_dbmname, "%s.db", temp_dbmname);
    sprintf(buffer, "%s.db", argv[arg+1]);
    if (rename(real_dbmname, buffer) != 0)
      {
      printf("Unable to rename %s as %s\n", real_dbmname, buffer);
      return 1;
      }
    }

  /* Rename .dir and .pag files */

  else
    {
    sprintf(real_dbmname, "%s.dir", temp_dbmname);
    sprintf(buffer, "%s.dir", argv[arg+1]);
    if (rename(real_dbmname, buffer) != 0)
      {
      printf("Unable to rename %s as %s\n", real_dbmname, buffer);
      return 1;
      }

    sprintf(real_dbmname, "%s.pag", temp_dbmname);
    sprintf(buffer, "%s.pag", argv[arg+1]);
    if (rename(real_dbmname, buffer) != 0)
      {
      printf("Unable to rename %s as %s\n", real_dbmname, buffer);
      return 1;
      }
    }

  #endif /* USE_DB || USE_TDB */
  }

/* Otherwise unlink the temporary files. */

else
  {
  printf("dbmbuild abandoned\n");
  #if defined(USE_DB) || defined(USE_TDB)
  unlink(temp_dbmname);
  #else
  if (is_db)
    {
    sprintf(real_dbmname, "%s.db", temp_dbmname);
    unlink(real_dbmname);
    }
  else
    {
    sprintf(real_dbmname, "%s.dir", temp_dbmname);
    unlink(real_dbmname);
    sprintf(real_dbmname, "%s.pag", temp_dbmname);
    unlink(real_dbmname);
    }
  #endif /* USE_DB || USE_TDB */
  }

return yield;
}

/* End of exim_dbmbuild.c */
