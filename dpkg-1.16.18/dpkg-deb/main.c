/*
 * dpkg-deb - construction and deconstruction of *.deb archives
 * main.c - main program
 *
 * Copyright © 1994,1995 Ian Jackson <ian@chiark.greenend.org.uk>
 * Copyright © 2006-2012 Guillem Jover <guillem@debian.org>
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
#include <sys/wait.h>

#include <assert.h>
#include <limits.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/macros.h>
#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/compress.h>
#include <dpkg/options.h>

#include "dpkg-deb.h"

const char *showformat = "${Package}\t${Version}\n";

static void DPKG_ATTR_NORET
printversion(const struct cmdinfo *cip, const char *value)
{
  printf(_("Debian `%s' package archive backend version %s.\n"),
         BACKEND, DPKG_VERSION_ARCH);
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
"\n"), BACKEND);

  printf(_(
"Commands:\n"
"  -b|--build <directory> [<deb>]   Build an archive.\n"
"  -c|--contents <deb>              List contents.\n"
"  -I|--info <deb> [<cfile> ...]    Show info to stdout.\n"
"  -W|--show <deb>                  Show information on package(s)\n"
"  -f|--field <deb> [<cfield> ...]  Show field(s) to stdout.\n"
"  -e|--control <deb> [<directory>] Extract control info.\n"
"  -x|--extract <deb> <directory>   Extract files.\n"
"  -X|--vextract <deb> <directory>  Extract & list files.\n"
"  -R|--raw-extract <deb> <directory>\n"
"                                   Extract control info and files.\n"
"  --fsys-tarfile <deb>             Output filesystem tarfile.\n"
"\n"));

  printf(_(
"  -?, --help                       Show this help message.\n"
"      --version                    Show the version.\n"
"\n"));

  printf(_(
"<deb> is the filename of a Debian format archive.\n"
"<cfile> is the name of an administrative file component.\n"
"<cfield> is the name of a field in the main `control' file.\n"
"\n"));

  printf(_(
"Options:\n"
"  --showformat=<format>            Use alternative format for --show.\n"
"  -v, --verbose                    Enable verbose output.\n"
"  -D                               Enable debugging output.\n"
"  --old, --new                     Select archive format.\n"
"  --nocheck                        Suppress control file check (build bad\n"
"                                     packages).\n"
"  -z#                              Set the compression level when building.\n"
"  -Z<type>                         Set the compression type used when building.\n"
"                                     Allowed types: gzip, xz, bzip2, none.\n"
"  -S<strategy>                     Set the compression strategy when building.\n"
"                                     Allowed values: none, extreme (xz).\n"
"\n"));

  printf(_(
"Format syntax:\n"
"  A format is a string that will be output for each package. The format\n"
"  can include the standard escape sequences \\n (newline), \\r (carriage\n"
"  return) or \\\\ (plain backslash). Package information can be included\n"
"  by inserting variable references to package fields using the ${var[;width]}\n"
"  syntax. Fields will be right-aligned unless the width is negative in which\n"
"  case left alignment will be used.\n"));

  printf(_(
"\n"
"Use `dpkg' to install and remove packages from your system, or\n"
"`dselect' or `aptitude' for user-friendly package management.  Packages\n"
"unpacked using `dpkg-deb --extract' will be incorrectly installed !\n"));

  m_output(stdout, _("<standard output>"));

  exit(0);
}

static const char printforhelp[] =
  N_("Type dpkg-deb --help for help about manipulating *.deb files;\n"
     "Type dpkg --help for help about installing and deinstalling packages.");

int debugflag = 0;
int nocheckflag = 0;
int oldformatflag = 0;
int opt_verbose = 0;
struct compress_params compress_params = {
  .type = compressor_type_gzip,
  .strategy = compressor_strategy_none,
  .level = -1,
};

static void
set_compress_level(const struct cmdinfo *cip, const char *value)
{
  long level;
  char *end;

  errno = 0;
  level = strtol(value, &end, 0);
  if (value == end || *end || errno != 0)
    badusage(_("invalid integer for -%c: '%.250s'"), cip->oshort, value);

  if (level < 0 || level > 9)
    badusage(_("invalid compression level for -%c: %ld'"), cip->oshort, level);

  compress_params.level = level;
}

static void
set_compress_strategy(const struct cmdinfo *cip, const char *value)
{
  compress_params.strategy = compressor_get_strategy(value);
  if (compress_params.strategy == compressor_strategy_unknown)
    ohshit(_("unknown compression strategy '%s'!"), value);
}

static void
setcompresstype(const struct cmdinfo *cip, const char *value)
{
  compress_params.type = compressor_find_by_name(value);
  if (compress_params.type == compressor_type_unknown)
    ohshit(_("unknown compression type `%s'!"), value);
  if (compress_params.type == compressor_type_lzma)
    warning(_("deprecated compression type '%s'; use xz instead"), value);
}

static const struct cmdinfo cmdinfos[]= {
  ACTION("build",         'b', 0, do_build),
  ACTION("contents",      'c', 0, do_contents),
  ACTION("control",       'e', 0, do_control),
  ACTION("info",          'I', 0, do_info),
  ACTION("field",         'f', 0, do_field),
  ACTION("extract",       'x', 0, do_extract),
  ACTION("vextract",      'X', 0, do_vextract),
  ACTION("raw-extract",   'R', 0, do_raw_extract),
  ACTION("fsys-tarfile",  0,   0, do_fsystarfile),
  ACTION("show",          'W', 0, do_showinfo),

  { "new",           0,   0, &oldformatflag, NULL,         NULL,          0 },
  { "old",           0,   0, &oldformatflag, NULL,         NULL,          1 },
  { "debug",         'D', 0, &debugflag,     NULL,         NULL,          1 },
  { "verbose",       'v', 0, &opt_verbose,   NULL,         NULL,          1 },
  { "nocheck",       0,   0, &nocheckflag,   NULL,         NULL,          1 },
  { NULL,            'z', 1, NULL,           NULL,         set_compress_level },
  { NULL,            'Z', 1, NULL,           NULL,         setcompresstype  },
  { NULL,            'S', 1, NULL,           NULL,         set_compress_strategy },
  { "showformat",    0,   1, NULL,           &showformat,  NULL             },
  { "help",          '?', 0, NULL,           NULL,         usage            },
  { "version",       0,   0, NULL,           NULL,         printversion     },
  {  NULL,           0,   0, NULL,           NULL,         NULL             }
};

int main(int argc, const char *const *argv) {
  struct dpkg_error err;
  int ret;

  setlocale(LC_NUMERIC, "POSIX");
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  dpkg_set_progname(BACKEND);
  standard_startup();
  myopt(&argv, cmdinfos, printforhelp);

  if (!cipaction) badusage(_("need an action option"));

  if (!compressor_check_params(&compress_params, &err))
    badusage(_("invalid compressor parameters: %s"), err.str);

  unsetenv("GZIP");

  ret = cipaction->action(argv);

  standard_shutdown();

  return ret;
}

/* vi: sw=2
 */
