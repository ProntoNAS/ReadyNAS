/*
 * dpkg-split - splitting and joining of multipart *.deb archives
 * main.c - main program
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <config.h>
#include <compat.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <limits.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/macros.h>
#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/myopt.h>

#include "dpkg-split.h"

static void DPKG_ATTR_NORET
printversion(const struct cmdinfo *cip, const char *value)
{
  printf(_("Debian `%s' package split/join tool; version %s.\n"),
         SPLITTER, DPKG_VERSION_ARCH);
  printf(_("Copyright (C) 1994-1996 Ian Jackson.\n"));
  printf(_(
"This is free software; see the GNU General Public License version 2 or\n"
"later for copying conditions. There is NO warranty.\n"));

  m_output(stdout, _("<standard output>"));

  exit(0);
}

static void DPKG_ATTR_NORET
usage(const struct cmdinfo *cip, const char *value)
{
  printf(_(
"Usage: %s [<option> ...] <command>\n"
"\n"), SPLITTER);

  printf(_(
"Commands:\n"
"  -s|--split <file> [<prefix>]     Split an archive.\n"
"  -j|--join <part> <part> ...      Join parts together.\n"
"  -I|--info <part> ...             Display info about a part.\n"
"  -a|--auto -o <complete> <part>   Auto-accumulate parts.\n"
"  -l|--listq                       List unmatched pieces.\n"
"  -d|--discard [<filename> ...]    Discard unmatched pieces.\n"
"\n"));

  printf(_(
"  -h|--help                        Show this help message.\n"
"  --version                        Show the version.\n"
"\n"));

  printf(_(
"Options:\n"
"  --depotdir <directory>           Use <directory> instead of %s/%s.\n"
"  -S|--partsize <size>             In KiB, for -s (default is 450).\n"
"  -o|--output <file>               For -j (default is <package>-<version>.deb).\n"
"  -Q|--npquiet                     Be quiet when -a is not a part.\n"
"  --msdos                          Generate 8.3 filenames.\n"
"\n"
"Exit status: 0 = OK;  1 = -a is not a part;  2 = trouble!\n"),
         ADMINDIR, PARTSDIR);

  m_output(stdout, _("<standard output>"));

  exit(0);
}

const char thisname[]= SPLITTER;
const char printforhelp[]= N_("Type dpkg-split --help for help.");

dofunction *action=NULL;
const struct cmdinfo *cipaction=NULL;
struct partqueue *queue= NULL;

long opt_maxpartsize = SPLITPARTDEFMAX;
const char *opt_depotdir = ADMINDIR "/" PARTSDIR;
const char *opt_outputfile = NULL;
int opt_npquiet = 0;
int opt_msdos = 0;

void rerr(const char *fn) {
  ohshite(_("error reading %.250s"), fn);
}

void rerreof(FILE *f, const char *fn) {
  if (ferror(f)) ohshite(_("error reading %.250s"),fn);
  ohshit(_("unexpected end of file in %.250s"),fn);
}

static void setaction(const struct cmdinfo *cip, const char *value);

static void setpartsize(const struct cmdinfo *cip, const char *value) {
  long newpartsize;
  char *endp;

  newpartsize= strtol(value,&endp,10);
  if (value == endp || *endp)
    badusage(_("invalid integer for --%s: `%.250s'"), cip->olong, value);
  if (newpartsize <= 0 || newpartsize > (INT_MAX >> 10))
    badusage(_("part size is far too large or is not positive"));

  opt_maxpartsize = newpartsize << 10;
  if (opt_maxpartsize <= HEADERALLOWANCE)
    badusage(_("part size must be at least %d KiB (to allow for header)"),
             (HEADERALLOWANCE >> 10) + 1);
}

static dofunction *const dofunctions[]= {
  do_split,
  do_join,
  do_info,
  do_auto,
  do_queue,
  do_discard,
};

/* NB: the entries using setaction must appear first and be in the
 * same order as dofunctions:
 */
static const struct cmdinfo cmdinfos[]= {
  { "split",        's',  0,  NULL, NULL,             setaction           },
  { "join",         'j',  0,  NULL, NULL,             setaction           },
  { "info",         'I',  0,  NULL, NULL,             setaction           },
  { "auto",         'a',  0,  NULL, NULL,             setaction           },
  { "listq",        'l',  0,  NULL, NULL,             setaction           },
  { "discard",      'd',  0,  NULL, NULL,             setaction           },
  { "help",         'h',  0,  NULL, NULL,             usage               },
  { "version",       0,   0,  NULL, NULL,             printversion        },
  { "depotdir",      0,   1,  NULL, &opt_depotdir,    NULL                },
  { "partsize",     'S',  1,  NULL, NULL,             setpartsize         },
  { "output",       'o',  1,  NULL, &opt_outputfile,  NULL                },
  { "npquiet",      'Q',  0,  &opt_npquiet, NULL,     NULL,           1   },
  { "msdos",         0,   0,  &opt_msdos, NULL,       NULL,           1   },
  {  NULL,              0                                              }
};

static void setaction(const struct cmdinfo *cip, const char *value) {
  if (cipaction)
    badusage(_("conflicting actions -%c (--%s) and -%c (--%s)"),
             cip->oshort, cip->olong, cipaction->oshort, cipaction->olong);
  cipaction= cip;
  assert((int)(cip - cmdinfos) < (int)(array_count(dofunctions)));
  action= dofunctions[cip-cmdinfos];
}

int main(int argc, const char *const *argv) {
  jmp_buf ejbuf;
  int l;
  char *p;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  standard_startup(&ejbuf);
  myopt(&argv, cmdinfos);

  if (!cipaction) badusage(_("need an action option"));

  l = strlen(opt_depotdir);
  if (l && opt_depotdir[l - 1] != '/') {
    p= nfmalloc(l+2);
    strcpy(p, opt_depotdir);
    strcpy(p+l,"/");
    opt_depotdir = p;
  }

  setvbuf(stdout,NULL,_IONBF,0);
  action(argv);

  m_output(stderr, _("<standard error>"));
  
  standard_shutdown();
  exit(0);
}
