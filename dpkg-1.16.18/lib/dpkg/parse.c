/*
 * libdpkg - Debian packaging suite library routines
 * parse.c - database file parsing, main package/field loop
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 * Copyright © 2006,2008-2012 Guillem Jover <guillem@debian.org>
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
#ifdef USE_MMAP
#include <sys/mman.h>
#endif

#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/macros.h>
#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/string.h>
#include <dpkg/pkg.h>
#include <dpkg/parsedump.h>
#include <dpkg/fdio.h>

/**
 * Fields information.
 */
const struct fieldinfo fieldinfos[]= {
  /* Note: Capitalization of field name strings is important. */
  { FIELD("Package"),          f_name,            w_name                                     },
  { FIELD("Essential"),        f_boolean,         w_booleandefno,   PKGIFPOFF(essential)     },
  { FIELD("Status"),           f_status,          w_status                                   },
  { FIELD("Priority"),         f_priority,        w_priority                                 },
  { FIELD("Section"),          f_section,         w_section                                  },
  { FIELD("Installed-Size"),   f_charfield,       w_charfield,      PKGIFPOFF(installedsize) },
  { FIELD("Origin"),           f_charfield,       w_charfield,      PKGIFPOFF(origin)        },
  { FIELD("Maintainer"),       f_charfield,       w_charfield,      PKGIFPOFF(maintainer)    },
  { FIELD("Bugs"),             f_charfield,       w_charfield,      PKGIFPOFF(bugs)          },
  { FIELD("Architecture"),     f_architecture,    w_architecture                             },
  { FIELD("Multi-Arch"),       f_multiarch,       w_multiarch,      PKGIFPOFF(multiarch)     },
  { FIELD("Source"),           f_charfield,       w_charfield,      PKGIFPOFF(source)        },
  { FIELD("Version"),          f_version,         w_version,        PKGIFPOFF(version)       },
  { FIELD("Revision"),         f_revision,        w_null                                     },
  { FIELD("Config-Version"),   f_configversion,   w_configversion                            },
  { FIELD("Replaces"),         f_dependency,      w_dependency,     dep_replaces             },
  { FIELD("Provides"),         f_dependency,      w_dependency,     dep_provides             },
  { FIELD("Depends"),          f_dependency,      w_dependency,     dep_depends              },
  { FIELD("Pre-Depends"),      f_dependency,      w_dependency,     dep_predepends           },
  { FIELD("Recommends"),       f_dependency,      w_dependency,     dep_recommends           },
  { FIELD("Suggests"),         f_dependency,      w_dependency,     dep_suggests             },
  { FIELD("Breaks"),           f_dependency,      w_dependency,     dep_breaks               },
  { FIELD("Conflicts"),        f_dependency,      w_dependency,     dep_conflicts            },
  { FIELD("Enhances"),         f_dependency,      w_dependency,     dep_enhances             },
  { FIELD("Conffiles"),        f_conffiles,       w_conffiles                                },
  { FIELD("Filename"),         f_filecharf,       w_filecharf,      FILEFOFF(name)           },
  { FIELD("Size"),             f_filecharf,       w_filecharf,      FILEFOFF(size)           },
  { FIELD("MD5sum"),           f_filecharf,       w_filecharf,      FILEFOFF(md5sum)         },
  { FIELD("MSDOS-Filename"),   f_filecharf,       w_filecharf,      FILEFOFF(msdosname)      },
  { FIELD("Description"),      f_charfield,       w_charfield,      PKGIFPOFF(description)   },
  { FIELD("Triggers-Pending"), f_trigpend,        w_trigpend                                 },
  { FIELD("Triggers-Awaited"), f_trigaw,          w_trigaw                                   },
  /* Note that aliases are added to the nicknames table. */
  {  NULL                                                                             }
};

static const struct nickname nicknames[] = {
  /* Note: Capitalization of these strings is important. */
  { NICK("Recommended"),      .canon = "Recommends" },
  { NICK("Optional"),         .canon = "Suggests" },
  { NICK("Class"),            .canon = "Priority" },
  { NICK("Package-Revision"), .canon = "Revision" },
  { NICK("Package_Revision"), .canon = "Revision" },
  { .nick = NULL }
};

/**
 * Package object being parsed.
 *
 * Structure used to hold the parsed data for the package being constructed,
 * before it gets properly inserted into the package database.
 */
struct pkg_parse_object {
  struct pkginfo *pkg;
  struct pkgbin *pkgbin;
};

/**
 * Parse the field and value into the package being constructed.
 */
static void
pkg_parse_field(struct parsedb_state *ps, struct field_state *fs,
                void *parse_obj)
{
  struct pkg_parse_object *pkg_obj = parse_obj;
  const struct nickname *nick;
  const struct fieldinfo *fip;
  int *ip;

  for (nick = nicknames; nick->nick; nick++)
    if (nick->nicklen == (size_t)fs->fieldlen &&
        strncasecmp(nick->nick, fs->fieldstart, fs->fieldlen) == 0)
      break;
  if (nick->nick) {
    fs->fieldstart = nick->canon;
    fs->fieldlen = strlen(fs->fieldstart);
  }

  for (fip = fieldinfos, ip = fs->fieldencountered; fip->name; fip++, ip++)
    if (fip->namelen == (size_t)fs->fieldlen &&
        strncasecmp(fip->name, fs->fieldstart, fs->fieldlen) == 0)
      break;
  if (fip->name) {
    if ((*ip)++)
      parse_error(ps,
                  _("duplicate value for `%s' field"), fip->name);

    varbuf_reset(&fs->value);
    varbuf_add_buf(&fs->value, fs->valuestart, fs->valuelen);
    varbuf_end_str(&fs->value);

    fip->rcall(pkg_obj->pkg, pkg_obj->pkgbin, ps, fs->value.buf, fip);
  } else {
    struct arbitraryfield *arp, **larpp;

    if (fs->fieldlen < 2)
      parse_error(ps,
                  _("user-defined field name `%.*s' too short"),
                  fs->fieldlen, fs->fieldstart);
    larpp = &pkg_obj->pkgbin->arbs;
    while ((arp = *larpp) != NULL) {
      if (strncasecmp(arp->name, fs->fieldstart, fs->fieldlen) == 0 &&
          strlen(arp->name) == (size_t)fs->fieldlen)
        parse_error(ps,
                   _("duplicate value for user-defined field `%.*s'"),
                   fs->fieldlen, fs->fieldstart);
      larpp = &arp->next;
    }
    arp = nfmalloc(sizeof(struct arbitraryfield));
    arp->name = nfstrnsave(fs->fieldstart, fs->fieldlen);
    arp->value = nfstrnsave(fs->valuestart, fs->valuelen);
    arp->next = NULL;
    *larpp = arp;
  }
}

/**
 * Verify and fixup the package structure being constructed.
 */
static void
pkg_parse_verify(struct parsedb_state *ps,
                 struct pkginfo *pkg, struct pkgbin *pkgbin)
{
  struct dependency *dep;
  struct deppossi *dop;

  parse_must_have_field(ps, pkg->set->name, "package name");

  /* XXX: We need to check for status != stat_halfinstalled as while
   * unpacking an unselected package, it will not have yet all data in
   * place. But we cannot check for > stat_halfinstalled as stat_configfiles
   * always should have those fields. */
  if ((ps->flags & pdb_recordavailable) ||
      (pkg->status != stat_notinstalled &&
       pkg->status != stat_halfinstalled)) {
    parse_ensure_have_field(ps, &pkgbin->description, "description");
    parse_ensure_have_field(ps, &pkgbin->maintainer, "maintainer");
    parse_must_have_field(ps, pkgbin->version.version, "version");
  }

  /* XXX: Versions before dpkg 1.10.19 did not preserve the Architecture
   * field in the status file. So there's still live systems with packages
   * in stat_configfiles, ignore those too for now. */
  if ((ps->flags & pdb_recordavailable) ||
      pkg->status > stat_halfinstalled) {
    /* We always want usable architecture information (as long as the package
     * is in such a state that it make sense), so that it can be used safely
     * on string comparisons and the like. */
    if (pkgbin->arch->type == arch_none)
      parse_warn(ps, _("missing %s"), "architecture");
    else if (pkgbin->arch->type == arch_empty)
      parse_warn(ps, _("empty value for %s"), "architecture");
  }
  /* Mark missing architectures as empty, to distinguish these from
   * unused slots in the db. */
  if (pkgbin->arch->type == arch_none)
    pkgbin->arch = dpkg_arch_get(arch_empty);

  if (pkgbin->arch->type == arch_empty && pkgbin->multiarch == multiarch_same)
    parse_error(ps, _("package has field '%s' but is missing architecture"),
                "Multi-Arch: same");
  if (pkgbin->arch->type == arch_all && pkgbin->multiarch == multiarch_same)
    parse_error(ps, _("package has field '%s' but is architecture all"),
                "Multi-Arch: same");

  /* Initialize deps to be arch-specific unless stated otherwise. */
  for (dep = pkgbin->depends; dep; dep = dep->next)
    for (dop = dep->list; dop; dop = dop->next)
      if (!dop->arch)
        dop->arch = pkgbin->arch;

  /*
   * Check the Config-Version information:
   *
   * If there is a Config-Version it is definitely to be used, but there
   * should not be one if the package is ‘installed’ or ‘triggers-pending’
   * (in which case the Version will be copied) or if the package is
   * ‘not-installed’ (in which case there is no Config-Version).
   */
  if (!(ps->flags & pdb_recordavailable)) {
    if (pkg->configversion.version) {
      if (pkg->status == stat_installed ||
          pkg->status == stat_notinstalled ||
          pkg->status == stat_triggerspending)
        parse_error(ps,
                    _("Configured-Version for package with inappropriate Status"));
    } else {
      if (pkg->status == stat_installed ||
          pkg->status == stat_triggerspending)
        pkg->configversion = pkgbin->version;
    }
  }

  if (pkg->trigaw.head &&
      (pkg->status <= stat_configfiles ||
       pkg->status >= stat_triggerspending))
    parse_error(ps,
                _("package has status %s but triggers are awaited"),
                statusinfos[pkg->status].name);
  else if (pkg->status == stat_triggersawaited && !pkg->trigaw.head)
    parse_error(ps,
                _("package has status triggers-awaited but no triggers awaited"));

  if (pkg->trigpend_head &&
      !(pkg->status == stat_triggerspending ||
        pkg->status == stat_triggersawaited))
    parse_error(ps,
                _("package has status %s but triggers are pending"),
                statusinfos[pkg->status].name);
  else if (pkg->status == stat_triggerspending && !pkg->trigpend_head)
    parse_error(ps,
                _("package has status triggers-pending but no triggers "
                  "pending"));

  /* FIXME: There was a bug that could make a not-installed package have
   * conffiles, so we check for them here and remove them (rather than
   * calling it an error, which will do at some point). */
  if (!(ps->flags & pdb_recordavailable) &&
      pkg->status == stat_notinstalled &&
      pkgbin->conffiles) {
    parse_warn(ps,
               _("Package which in state not-installed has conffiles, "
                 "forgetting them"));
    pkgbin->conffiles = NULL;
  }

  /* XXX: Mark not-installed leftover packages for automatic removal on
   * next database dump. This code can be removed after dpkg 1.16.x, when
   * there's guarantee that no leftover is found on the status file on
   * major distributions. */
  if (!(ps->flags & pdb_recordavailable) &&
      pkg->status == stat_notinstalled &&
      pkg->eflag == eflag_ok &&
      (pkg->want == want_purge ||
       pkg->want == want_deinstall ||
       pkg->want == want_hold)) {
    pkg_set_want(pkg, want_unknown);
  }

  /* XXX: Mark not-installed non-arch-qualified selections for automatic
   * removal, as they do not make sense in a multiarch enabled world, and
   * might cause those selections to be unreferencable from command-line
   * interfaces when there's other more specific selections. */
  if (ps->type == pdb_file_status &&
      pkg->status == stat_notinstalled &&
      pkg->eflag == eflag_ok &&
      pkg->want == want_install &&
      pkgbin->arch->type == arch_empty)
    pkg->want = want_unknown;
}

struct pkgcount {
  int single;
  int multi;
  int total;
};

static void
parse_count_pkg_instance(struct pkgcount *count,
                         struct pkginfo *pkg, struct pkgbin *pkgbin)
{
  if (pkg->status == stat_notinstalled)
     return;

  if (pkgbin->multiarch == multiarch_same)
    count->multi++;
  else
    count->single++;

  count->total++;
}

/**
 * Lookup the package set slot for the parsed package.
 *
 * Perform various checks, to make sure the database is always in a sane
 * state, and to not allow breaking it.
 */
static struct pkgset *
parse_find_set_slot(struct parsedb_state *ps,
                    struct pkginfo *new_pkg, struct pkgbin *new_pkgbin)
{
  struct pkgcount count = { .single = 0, .multi = 0, .total = 0 };
  struct pkgset *set;
  struct pkginfo *pkg;

  set = pkg_db_find_set(new_pkg->set->name);

  /* Sanity checks: verify that the db is in a consistent state. */

  if (ps->type == pdb_file_status)
    parse_count_pkg_instance(&count, new_pkg, new_pkgbin);

  count.total = 0;

  for (pkg = &set->pkg; pkg; pkg = pkg->arch_next)
    parse_count_pkg_instance(&count, pkg, &pkg->installed);

  if (count.single > 1)
    parse_error(ps, _("multiple non-coinstallable package instances present; "
                      "most probably due to an upgrade from an unofficial dpkg"));

  if (count.single > 0 && count.multi > 0)
    parse_error(ps, _("mixed non-coinstallable and coinstallable package "
                      "instances present; most probably due to an upgrade "
                      "from an unofficial dpkg"));

  if (pkgset_installed_instances(set) != count.total)
    internerr("in-core pkgset '%s' with inconsistent number of instances",
              set->name);

  return set;
}

/**
 * Lookup the package slot for the parsed package.
 *
 * Cross-grading (i.e. switching arch) is only possible when parsing an
 * update entry or when installing a new package.
 *
 * Most of the time each pkginfo in a pkgset has the same architecture for
 * both the installed and available pkgbin members. But when cross-grading
 * there's going to be a temporary discrepancy, because we reuse the single
 * instance and fill the available pkgbin with the candidate pkgbin, until
 * that is copied over the installed pkgbin.
 *
 * If there's 0 or > 1 package instances, then we match against the pkginfo
 * slot architecture, because cross-grading is just not possible.
 *
 * If there's 1 instance, we are cross-grading and both installed and
 * candidate are not multiarch_same, we have to reuse the existing single
 * slot regardless of the arch differing between the two. If we are not
 * cross-grading, then we use the entry with the matching arch.
 */
static struct pkginfo *
parse_find_pkg_slot(struct parsedb_state *ps,
                    struct pkginfo *new_pkg, struct pkgbin *new_pkgbin)
{
  struct pkgset *db_set;
  struct pkginfo *db_pkg;

  db_set = parse_find_set_slot(ps, new_pkg, new_pkgbin);

  if (ps->type == pdb_file_available) {
    /* If there's a single package installed and the new package is not
     * “Multi-Arch: same”, then we preserve the previous behaviour of
     * possible architecture switch, for example from native to all. */
    if (pkgset_installed_instances(db_set) == 1 &&
        new_pkgbin->multiarch != multiarch_same)
      return pkg_db_get_singleton(db_set);
    else
      return pkg_db_get_pkg(db_set, new_pkgbin->arch);
  } else {
    bool selection = false;

    /* If the package is part of the status file, and it's not installed
     * then this means it's just a selection. */
    if (ps->type == pdb_file_status && new_pkg->status == stat_notinstalled)
      selection = true;

    /* Verify we don't allow something that will mess up the db. */
    if (pkgset_installed_instances(db_set) > 1 &&
        !selection && new_pkgbin->multiarch != multiarch_same)
      ohshit(_("%s %s (Multi-Arch: %s) is not co-installable with "
               "%s which has multiple installed instances"),
             pkgbin_name(new_pkg, new_pkgbin, pnaw_always),
             versiondescribe(&new_pkgbin->version, vdew_nonambig),
             multiarchinfos[new_pkgbin->multiarch].name, db_set->name);

    /* If we are parsing the status file, use a slot per arch. */
    if (ps->type == pdb_file_status)
      return pkg_db_get_pkg(db_set, new_pkgbin->arch);

    /* If we are doing an update, from the log or a new package, then
     * handle cross-grades. */
    if (pkgset_installed_instances(db_set) == 1) {
      db_pkg = pkg_db_get_singleton(db_set);

      if (db_pkg->installed.multiarch == multiarch_same &&
          new_pkgbin->multiarch == multiarch_same)
        return pkg_db_get_pkg(db_set, new_pkgbin->arch);
      else
        return db_pkg;
    } else {
      return pkg_db_get_pkg(db_set, new_pkgbin->arch);
    }
  }
}

/**
 * Copy into the in-core database the package being constructed.
 */
static void
pkg_parse_copy(struct parsedb_state *ps,
               struct pkginfo *dst_pkg, struct pkgbin *dst_pkgbin,
               struct pkginfo *src_pkg, struct pkgbin *src_pkgbin)
{
  /* Copy the priority and section across, but don't overwrite existing
   * values if the pdb_weakclassification flag is set. */
  if (str_is_set(src_pkg->section) &&
      !((ps->flags & pdb_weakclassification) &&
        str_is_set(dst_pkg->section)))
    dst_pkg->section = src_pkg->section;
  if (src_pkg->priority != pri_unknown &&
      !((ps->flags & pdb_weakclassification) &&
        dst_pkg->priority != pri_unknown)) {
    dst_pkg->priority = src_pkg->priority;
    if (src_pkg->priority == pri_other)
      dst_pkg->otherpriority = src_pkg->otherpriority;
  }

  /* Sort out the dependency mess. */
  copy_dependency_links(dst_pkg, &dst_pkgbin->depends, src_pkgbin->depends,
                        (ps->flags & pdb_recordavailable) ? true : false);

  /* Copy across data. */
  memcpy(dst_pkgbin, src_pkgbin, sizeof(struct pkgbin));
  if (!(ps->flags & pdb_recordavailable)) {
    struct trigaw *ta;

    pkg_set_want(dst_pkg, src_pkg->want);
    pkg_copy_eflags(dst_pkg, src_pkg);
    pkg_set_status(dst_pkg, src_pkg->status);
    dst_pkg->configversion = src_pkg->configversion;
    dst_pkg->files = NULL;

    dst_pkg->trigpend_head = src_pkg->trigpend_head;
    dst_pkg->trigaw = src_pkg->trigaw;
    for (ta = dst_pkg->trigaw.head; ta; ta = ta->sameaw.next) {
      assert(ta->aw == src_pkg);
      ta->aw = dst_pkg;
      /* ->othertrigaw_head is updated by trig_note_aw in *(pkg_db_find())
       * rather than in dst_pkg. */
    }
  } else if (!(ps->flags & pdb_ignorefiles)) {
    dst_pkg->files = src_pkg->files;
  }
}

/**
 * Return a descriptive parser type.
 */
static enum parsedbtype
parse_get_type(struct parsedb_state *ps, enum parsedbflags flags)
{
  if (flags & pdb_recordavailable) {
    if (flags & pdb_deb_control)
      return pdb_file_control;
    else
      return pdb_file_available;
  } else {
    if (flags & pdb_deb_control)
      return pdb_file_update;
    else
      return pdb_file_status;
  }
}

/**
 * Open a file for RFC-822 parsing.
 */
void
parse_open(struct parsedb_state *ps, const char *filename,
           enum parsedbflags flags)
{
  static int fd;
  struct stat st;

  ps->filename = filename;
  ps->type = parse_get_type(ps, flags);
  ps->flags = flags;
  ps->lno = 0;
  ps->pkg = NULL;
  ps->pkgbin = NULL;

  fd = open(filename, O_RDONLY);
  if (fd == -1)
    ohshite(_("failed to open package info file `%.255s' for reading"),
            filename);

  push_cleanup(cu_closefd, ~ehflag_normaltidy, NULL, 0, 1, &fd);

  if (fstat(fd, &st) == -1)
    ohshite(_("can't stat package info file `%.255s'"), filename);

  if (st.st_size > 0) {
#ifdef USE_MMAP
    ps->dataptr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ps->dataptr == MAP_FAILED)
      ohshite(_("can't mmap package info file `%.255s'"), filename);
#else
    ps->dataptr = m_malloc(st.st_size);

    if (fd_read(fd, ps->dataptr, st.st_size) < 0)
      ohshite(_("reading package info file '%.255s'"), filename);
#endif
    ps->data = ps->dataptr;
    ps->endptr = ps->dataptr + st.st_size;
  } else {
    ps->data = ps->dataptr = ps->endptr = NULL;
  }

  pop_cleanup(ehflag_normaltidy);

  if (close(fd))
    ohshite(_("failed to close after read: `%.255s'"), filename);
}

/**
 * Parse an RFC-822 style stanza.
 */
bool
parse_stanza(struct parsedb_state *ps, struct field_state *fs,
             parse_field_func *parse_field, void *parse_obj)
{
  int c;

  /* Skip adjacent new lines. */
  while (!parse_EOF(ps)) {
    c = parse_getc(ps);
    if (c != '\n' && c != MSDOS_EOF_CHAR)
      break;
    ps->lno++;
  }

  /* Nothing relevant parsed, bail out. */
  if (parse_EOF(ps))
    return false;

  /* Loop per field. */
  for (;;) {
    bool blank_line;

    /* Scan field name. */
    fs->fieldstart = ps->dataptr - 1;
    while (!parse_EOF(ps) && !isspace(c) && c != ':' && c != MSDOS_EOF_CHAR)
      c = parse_getc(ps);
    fs->fieldlen = ps->dataptr - fs->fieldstart - 1;
    if (fs->fieldlen == 0)
      parse_error(ps,  _("empty field name"));

    /* Skip spaces before ‘:’. */
    while (!parse_EOF(ps) && c != '\n' && isspace(c))
      c = parse_getc(ps);

    /* Validate ‘:’. */
    if (parse_EOF(ps))
      parse_error(ps,
                  _("EOF after field name `%.*s'"), fs->fieldlen, fs->fieldstart);
    if (c == '\n')
      parse_error(ps,
                  _("newline in field name `%.*s'"), fs->fieldlen, fs->fieldstart);
    if (c == MSDOS_EOF_CHAR)
      parse_error(ps,
                  _("MSDOS EOF (^Z) in field name `%.*s'"),
                  fs->fieldlen, fs->fieldstart);
    if (c != ':')
      parse_error(ps,
                  _("field name `%.*s' must be followed by colon"),
                  fs->fieldlen, fs->fieldstart);

    /* Skip space after ‘:’ but before value and EOL. */
    while (!parse_EOF(ps)) {
      c = parse_getc(ps);
      if (c == '\n' || !isspace(c))
        break;
    }
    if (parse_EOF(ps))
      parse_error(ps,
                  _("EOF before value of field `%.*s' (missing final newline)"),
                  fs->fieldlen, fs->fieldstart);
    if (c == MSDOS_EOF_CHAR)
      parse_error(ps,
                  _("MSDOS EOF char in value of field `%.*s' (missing newline?)"),
                  fs->fieldlen, fs->fieldstart);

    blank_line = false;

    /* Scan field value. */
    fs->valuestart = ps->dataptr - 1;
    for (;;) {
      if (c == '\n' || c == MSDOS_EOF_CHAR) {
        if (blank_line)
          parse_error(ps,
                      _("blank line in value of field '%.*s'"),
                      fs->fieldlen, fs->fieldstart);
        ps->lno++;

        if (parse_EOF(ps))
          break;
        c = parse_getc(ps);

        /* Found double EOL, or start of new field. */
        if (parse_EOF(ps) || c == '\n' || !isspace(c))
          break;

        parse_ungetc(c, ps);
        c = '\n';
        blank_line = true;
      } else if (blank_line && !isspace(c)) {
        blank_line = false;
      }

      if (parse_EOF(ps))
        parse_error(ps,
                    _("EOF during value of field `%.*s' (missing final newline)"),
                    fs->fieldlen, fs->fieldstart);

      c = parse_getc(ps);
    }
    fs->valuelen = ps->dataptr - fs->valuestart - 1;

    /* Trim ending space on value. */
    while (fs->valuelen && isspace(*(fs->valuestart + fs->valuelen - 1)))
      fs->valuelen--;

    parse_field(ps, fs, parse_obj);

    if (parse_EOF(ps) || c == '\n' || c == MSDOS_EOF_CHAR)
      break;
  } /* Loop per field. */

  if (c == '\n')
    ps->lno++;

  return true;
}

/**
 * Close an RFC-822 parser context.
 */
void
parse_close(struct parsedb_state *ps)
{
  if (ps->data != NULL) {
#ifdef USE_MMAP
    munmap(ps->data, ps->endptr - ps->data);
#else
    free(ps->data);
#endif
  }
}

/**
 * Parse an RFC-822 style file.
 *
 * donep may be NULL.
 * If donep is not NULL only one package's information is expected.
 */
int parsedb(const char *filename, enum parsedbflags flags,
            struct pkginfo **donep)
{
  struct pkgset tmp_set;
  struct pkginfo *new_pkg, *db_pkg;
  struct pkgbin *new_pkgbin, *db_pkgbin;
  struct pkg_parse_object pkg_obj;
  int fieldencountered[array_count(fieldinfos)];
  int pdone;
  struct parsedb_state ps;
  struct field_state fs;

  memset(&fs, 0, sizeof(fs));
  fs.fieldencountered = fieldencountered;

  parse_open(&ps, filename, flags);

  new_pkg = &tmp_set.pkg;
  if (flags & pdb_recordavailable)
    new_pkgbin = &new_pkg->available;
  else
    new_pkgbin = &new_pkg->installed;

  ps.pkg = new_pkg;
  ps.pkgbin = new_pkgbin;

  pkg_obj.pkg = new_pkg;
  pkg_obj.pkgbin = new_pkgbin;

  pdone= 0;

  /* Loop per package. */
  for (;;) {
    memset(fieldencountered, 0, sizeof(fieldencountered));
    pkgset_blank(&tmp_set);

    if (!parse_stanza(&ps, &fs, pkg_parse_field, &pkg_obj))
      break;

    if (pdone && donep)
      parse_error(&ps,
                  _("several package info entries found, only one allowed"));

    pkg_parse_verify(&ps, new_pkg, new_pkgbin);

    db_pkg = parse_find_pkg_slot(&ps, new_pkg, new_pkgbin);
    if (flags & pdb_recordavailable)
      db_pkgbin = &db_pkg->available;
    else
      db_pkgbin = &db_pkg->installed;

    if (((flags & pdb_ignoreolder) || ps.type == pdb_file_available) &&
        dpkg_version_is_informative(&db_pkgbin->version) &&
        dpkg_version_compare(&new_pkgbin->version, &db_pkgbin->version) < 0)
      continue;

    pkg_parse_copy(&ps, db_pkg, db_pkgbin, new_pkg, new_pkgbin);

    if (donep)
      *donep = db_pkg;
    pdone++;
    if (parse_EOF(&ps))
      break;
  }

  parse_close(&ps);

  varbuf_destroy(&fs.value);
  if (donep && !pdone) ohshit(_("no package information in `%.255s'"),filename);

  return pdone;
}

/**
 * Copy dependency links structures.
 *
 * This routine is used to update the ‘reverse’ dependency pointers when
 * new ‘forwards’ information has been constructed. It first removes all
 * the links based on the old information. The old information starts in
 * *updateme; after much brou-ha-ha the reverse structures are created
 * and *updateme is set to the value from newdepends.
 *
 * @param pkg The package we're doing this for. This is used to construct
 *        correct uplinks.
 * @param updateme The forwards dependency pointer that we are to update.
 *        This starts out containing the old forwards info, which we use to
 *        unthread the old reverse links. After we're done it is updated.
 * @param newdepends The value that we ultimately want to have in updateme.
 * @param available The pkgbin to modify, available or installed.
 *
 * It is likely that the backward pointer for the package in question
 * (‘depended’) will be updated by this routine, but this will happen by
 * the routine traversing the dependency data structures. It doesn't need
 * to be told where to update that; I just mention it as something that
 * one should be cautious about.
 */
void copy_dependency_links(struct pkginfo *pkg,
                           struct dependency **updateme,
                           struct dependency *newdepends,
                           bool available)
{
  struct dependency *dyp;
  struct deppossi *dop, **revdeps;

  /* Delete ‘backward’ (‘depended’) links from other packages to
   * dependencies listed in old version of this one. We do this by
   * going through all the dependencies in the old version of this
   * one and following them down to find which deppossi nodes to
   * remove. */
  for (dyp= *updateme; dyp; dyp= dyp->next) {
    for (dop= dyp->list; dop; dop= dop->next) {
      if (dop->rev_prev)
        dop->rev_prev->rev_next = dop->rev_next;
      else
        if (available)
          dop->ed->depended.available = dop->rev_next;
        else
          dop->ed->depended.installed = dop->rev_next;
      if (dop->rev_next)
        dop->rev_next->rev_prev = dop->rev_prev;
    }
  }

  /* Now fill in new ‘ed’ links from other packages to dependencies
   * listed in new version of this one, and set our uplinks correctly. */
  for (dyp= newdepends; dyp; dyp= dyp->next) {
    dyp->up= pkg;
    for (dop= dyp->list; dop; dop= dop->next) {
      revdeps = available ? &dop->ed->depended.available :
                            &dop->ed->depended.installed;
      dop->rev_next = *revdeps;
      dop->rev_prev = NULL;
      if (*revdeps)
        (*revdeps)->rev_prev = dop;
      *revdeps = dop;
    }
  }

  /* Finally, we fill in the new value. */
  *updateme= newdepends;
}
