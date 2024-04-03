/*
 * libdpkg - Debian packaging suite library routines
 * parsehelp.c - helpful routines for parsing and writing
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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/string.h>
#include <dpkg/parsedump.h>

static void
parse_error_msg(struct parsedb_state *ps, const struct pkginfo *pigp,
                const char *type, char *buf)
{
  if (pigp && pigp->name)
    sprintf(buf, _("%s, in file '%.255s' near line %d package '%.255s':\n "),
            type, ps->filename, ps->lno, pigp->name);
  else
    sprintf(buf, _("%s, in file '%.255s' near line %d:\n "),
            type, ps->filename, ps->lno);
}

void
parse_error(struct parsedb_state *ps,
            const struct pkginfo *pigp, const char *fmt, ...)
{
  va_list args;
  char buf1[768], buf2[1000], *q;

  parse_error_msg(ps, pigp, _("parse error"), buf1);
  q = str_escape_fmt(buf2, buf1);
  strcat(q,fmt);

  va_start(args, fmt);
  ohshitv(buf2, args);
}

void
parse_warn(struct parsedb_state *ps,
           const struct pkginfo *pigp, const char *fmt, ...)
{
  va_list args;
  char buf1[768], buf2[1000], *q;

  parse_error_msg(ps, pigp, _("warning"), buf1);
  q = str_escape_fmt(buf2, buf1);
  strcat(q, fmt);

  va_start(args, fmt);
  ps->warncount++;
  if (ps->warnto) {
    strcat(q,"\n");
    if (vfprintf(ps->warnto, buf2, args) == EOF)
      ohshite(_("failed to write parsing warning"));
  }
  va_end(args);
}

#define NAMEVALUE_DEF(n, v) \
	[v] = { .name = n, .value = v, .length = sizeof(n) - 1 }

const struct namevalue booleaninfos[] = {
  NAMEVALUE_DEF("no",  false),
  NAMEVALUE_DEF("yes", true),
  { .name = NULL }
};

const struct namevalue priorityinfos[] = {
  NAMEVALUE_DEF("required",                      pri_required),
  NAMEVALUE_DEF("important",                     pri_important),
  NAMEVALUE_DEF("standard",                      pri_standard),
  NAMEVALUE_DEF("optional",                      pri_optional),
  NAMEVALUE_DEF("extra",                         pri_extra),
  NAMEVALUE_DEF("this is a bug - please report", pri_other),
  NAMEVALUE_DEF("unknown",                       pri_unknown),
  { .name = NULL }
};

const struct namevalue statusinfos[] = {
  NAMEVALUE_DEF("not-installed",    stat_notinstalled),
  NAMEVALUE_DEF("config-files",     stat_configfiles),
  NAMEVALUE_DEF("half-installed",   stat_halfinstalled),
  NAMEVALUE_DEF("unpacked",         stat_unpacked),
  NAMEVALUE_DEF("half-configured",  stat_halfconfigured),
  NAMEVALUE_DEF("triggers-awaited", stat_triggersawaited),
  NAMEVALUE_DEF("triggers-pending", stat_triggerspending),
  NAMEVALUE_DEF("installed",        stat_installed),
  { .name = NULL }
};

const struct namevalue eflaginfos[] = {
  NAMEVALUE_DEF("ok",        eflag_ok),
  NAMEVALUE_DEF("reinstreq", eflag_reinstreq),
  { .name = NULL }
};

const struct namevalue wantinfos[] = {
  NAMEVALUE_DEF("unknown",   want_unknown),
  NAMEVALUE_DEF("install",   want_install),
  NAMEVALUE_DEF("hold",      want_hold),
  NAMEVALUE_DEF("deinstall", want_deinstall),
  NAMEVALUE_DEF("purge",     want_purge),
  { .name = NULL }
};

const char *illegal_packagename(const char *p, const char **ep) {
  static const char alsoallowed[]= "-+._"; /* _ is deprecated */
  static char buf[150];
  int c;
  
  if (!*p) return _("may not be empty string");
  if (!isalnum(*p)) return _("must start with an alphanumeric");
  while ((c = *p++) != '\0')
    if (!isalnum(c) && !strchr(alsoallowed,c)) break;
  if (!c) return NULL;
  if (isspace(c) && ep) {
    while (isspace(*p)) p++;
    *ep= p; return NULL;
  }
  snprintf(buf, sizeof(buf), _(
	   "character `%c' not allowed (only letters, digits and characters `%s')"),
	   c, alsoallowed);
  return buf;
}

const struct nickname nicknames[]= {
  /* NB: capitalisation of these strings is important. */
  { .nick = "Recommended",      .canon = "Recommends" },
  { .nick = "Optional",         .canon = "Suggests" },
  { .nick = "Class",            .canon = "Priority" },
  { .nick = "Package-Revision", .canon = "Revision" },
  { .nick = "Package_Revision", .canon = "Revision" },
  { .nick = NULL }
};

bool
informativeversion(const struct versionrevision *version)
{
  return (version->epoch ||
          (version->version && *version->version) ||
          (version->revision && *version->revision));
}

void varbufversion
(struct varbuf *vb,
 const struct versionrevision *version,
 enum versiondisplayepochwhen vdew) 
{
  switch (vdew) {
  case vdew_never:
    break;
  case vdew_nonambig:
    if (!version->epoch &&
        (!version->version || !strchr(version->version,':')) &&
        (!version->revision || !strchr(version->revision,':'))) break;
  case vdew_always: /* FALL THROUGH */
    varbufprintf(vb,"%lu:",version->epoch);
    break;
  default:
    internerr("unknown versiondisplayepochwhen '%d'", vdew);
  }
  if (version->version) varbufaddstr(vb,version->version);
  if (version->revision && *version->revision) {
    varbufaddc(vb,'-');
    varbufaddstr(vb,version->revision);
  }
}

const char *versiondescribe
(const struct versionrevision *version,
 enum versiondisplayepochwhen vdew)
{
  static struct varbuf bufs[10];
  static int bufnum=0;

  struct varbuf *vb;
  
  if (!informativeversion(version)) return _("<none>");

  vb= &bufs[bufnum]; bufnum++; if (bufnum == 10) bufnum= 0;
  varbufreset(vb);
  varbufversion(vb,version,vdew);
  varbufaddc(vb,0);

  return vb->buf;
}

static const char *
parseversion_lax(struct versionrevision *rversion, const char *string)
{
  char *hyphen, *colon, *eepochcolon;
  const char *end, *ptr;
  unsigned long epoch;

  if (!*string) return _("version string is empty");

  /* trim leading and trailing space */
  while (*string && isblank(*string))
    string++;
  /* string now points to the first non-whitespace char */
  end = string;
  /* find either the end of the string, or a whitespace char */
  while (*end && !isblank(*end))
    end++;
  /* check for extra chars after trailing space */
  ptr = end;
  while (*ptr && isblank(*ptr))
    ptr++;
  if (*ptr) return _("version string has embedded spaces");

  colon= strchr(string,':');
  if (colon) {
    epoch= strtoul(string,&eepochcolon,10);
    if (colon != eepochcolon) return _("epoch in version is not number");
    if (!*++colon) return _("nothing after colon in version number");
    string= colon;
    rversion->epoch= epoch;
  } else {
    rversion->epoch= 0;
  }
  rversion->version= nfstrnsave(string,end-string);
  hyphen= strrchr(rversion->version,'-');
  if (hyphen)
    *hyphen++ = '\0';
  rversion->revision= hyphen ? hyphen : "";

  return NULL;
}

/**
 * Check for invalid syntax in version structure.
 *
 * The rest of the syntax has been already checked in parseversion_lax(). So
 * we only do the stricter checks here.
 *
 * @param rversion The version to verify.
 *
 * @return An error string, or NULL if eveyrthing was ok.
 */
static const char *
version_strict_check(struct versionrevision *rversion)
{
  const char *ptr;

  /* XXX: Would be faster to use something like cisversion and cisrevision. */
  for (ptr = rversion->version; *ptr; ptr++) {
    if (!cisdigit(*ptr) && !cisalpha(*ptr) && strchr(".-+~:", *ptr) == NULL)
      return _("invalid character in version number");
  }
  for (ptr = rversion->revision; *ptr; ptr++) {
    if (!cisdigit(*ptr) && !cisalpha(*ptr) && strchr(".-+~", *ptr) == NULL)
      return _("invalid character in revision number");
  }

  return NULL;
}

const char *
parseversion(struct versionrevision *rversion, const char *string)
{
  const char *emsg;

  emsg = parseversion_lax(rversion, string);
  if (emsg)
    return emsg;

  return version_strict_check(rversion);
}

/**
 * Parse a version string coming from a database file.
 *
 * It parses a version string, and prints a warning or an error depending
 * on the parse options.
 *
 * @param ps The parsedb state.
 * @param pkg The package being parsed.
 * @param version The version to parse into.
 * @param value The version string to parse from.
 * @param fmt The error format string.
 */
void
parse_db_version(struct parsedb_state *ps, const struct pkginfo *pkg,
                 struct versionrevision *version, const char *value,
                 const char *fmt, ...)
{
  const char *msg;
  bool warn_msg = false;

  msg = parseversion_lax(version, value);
  if (msg == NULL) {
    msg = version_strict_check(version);
    if (ps->flags & pdb_lax_parser)
      warn_msg = true;
  }

  if (msg) {
    va_list args;
    char buf[1000];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);

    if (warn_msg)
      parse_warn(ps, pkg, "%s: %.250s", buf, msg);
    else
      parse_error(ps, pkg, "%s: %.250s", buf, msg);

    va_end(args);
  }
}

void
parse_must_have_field(struct parsedb_state *ps,
                      const struct pkginfo *pigp,
                      const char *value, const char *what)
{
  if (!value)
    parse_error(ps, pigp, _("missing %s"), what);
  else if (!*value)
    parse_error(ps, pigp, _("empty value for %s"), what);
}

void
parse_ensure_have_field(struct parsedb_state *ps,
                        const struct pkginfo *pigp,
                        const char **value, const char *what)
{
  static const char empty[] = "";

  if (!*value) {
    parse_warn(ps, pigp, _("missing %s"), what);
    *value = empty;
  } else if (!**value) {
    parse_warn(ps, pigp, _("empty value for %s"), what);
  }
}

