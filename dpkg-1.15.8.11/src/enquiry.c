/*
 * dpkg - main program for package management
 * enquiry.c - status enquiry and listing options
 *
 * Copyright © 1995,1996 Ian Jackson <ian@chiark.greenend.org.uk>
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

/* FIXME: per-package audit */
#include <config.h>
#include <compat.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/termios.h>

#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/pkg-show.h>
#include <dpkg/myopt.h>

#include "filesdb.h"
#include "main.h"

struct badstatinfo {
  bool (*yesno)(struct pkginfo *, const struct badstatinfo *bsi);
  int value;
  const char *explanation;
};

static bool
bsyn_reinstreq(struct pkginfo *pkg, const struct badstatinfo *bsi)
{
  return pkg->eflag &= eflag_reinstreq;
}

static bool
bsyn_status(struct pkginfo *pkg, const struct badstatinfo *bsi)
{
  if (pkg->eflag &= eflag_reinstreq)
    return false;
  return (int)pkg->status == bsi->value;
}

static const struct badstatinfo badstatinfos[]= {
  {
    .yesno = bsyn_reinstreq,
    .value = 0,
    .explanation = N_(
    "The following packages are in a mess due to serious problems during\n"
    "installation.  They must be reinstalled for them (and any packages\n"
    "that depend on them) to function properly:\n")
  }, {
    .yesno = bsyn_status,
    .value = stat_unpacked,
    .explanation = N_(
    "The following packages have been unpacked but not yet configured.\n"
    "They must be configured using dpkg --configure or the configure\n"
    "menu option in dselect for them to work:\n")
  }, {
    .yesno = bsyn_status,
    .value = stat_halfconfigured,
    .explanation = N_(
    "The following packages are only half configured, probably due to problems\n"
    "configuring them the first time.  The configuration should be retried using\n"
    "dpkg --configure <package> or the configure menu option in dselect:\n")
  }, {
    .yesno = bsyn_status,
    .value = stat_halfinstalled,
    .explanation = N_(
    "The following packages are only half installed, due to problems during\n"
    "installation.  The installation can probably be completed by retrying it;\n"
    "the packages can be removed using dselect or dpkg --remove:\n")
  }, {
    .yesno = bsyn_status,
    .value = stat_triggersawaited,
    .explanation = N_(
    "The following packages are awaiting processing of triggers that they\n"
    "have activated in other packages.  This processing can be requested using\n"
    "dselect or dpkg --configure --pending (or dpkg --triggers-only):\n")
  }, {
    .yesno = bsyn_status,
    .value = stat_triggerspending,
    .explanation = N_(
    "The following packages have been triggered, but the trigger processing\n"
    "has not yet been done.  Trigger processing can be requested using\n"
    "dselect or dpkg --configure --pending (or dpkg --triggers-only):\n")
  }, {
    .yesno = NULL
  }
};

static void describebriefly(struct pkginfo *pkg) {
  int maxl, l;
  const char *pdesc;

  maxl= 57;
  l= strlen(pkg->name);
  if (l>20) maxl -= (l-20);

  pdesc = pkg_summary(pkg, &l);
  l = min(l, maxl);

  printf(" %-20s %.*s\n",pkg->name,l,pdesc);
}

void audit(const char *const *argv) {
  const struct badstatinfo *bsi;
  bool head_running = false;

  if (*argv)
    badusage(_("--%s takes no arguments"), cipaction->olong);

  modstatdb_init(admindir,msdbrw_readonly);

  for (bsi= badstatinfos; bsi->yesno; bsi++) {
    struct pkgiterator *it;
    struct pkginfo *pkg;
    bool head = false;

    it= iterpkgstart(); 
    while ((pkg= iterpkgnext(it))) {
      if (!bsi->yesno(pkg,bsi)) continue;
      if (!head_running) {
        if (modstatdb_is_locked(admindir))
          puts(_(
"Another process has locked the database for writing, and might currently be\n"
"modifying it, some of the following problems might just be due to that.\n"));
        head_running = true;
      }
      if (!head) {
        fputs(gettext(bsi->explanation),stdout);
        head = true;
      }
      describebriefly(pkg);
    }
    iterpkgend(it);
    if (head) putchar('\n');
  }

  m_output(stdout, _("<standard output>"));
}

struct sectionentry {
  struct sectionentry *next;
  const char *name;
  int count;
};

static bool
yettobeunpacked(struct pkginfo *pkg, const char **thissect)
{
  if (pkg->want != want_install)
    return false;

  switch (pkg->status) {
  case stat_unpacked: case stat_installed: case stat_halfconfigured:
  case stat_triggerspending:
  case stat_triggersawaited:
    return false;
  case stat_notinstalled: case stat_halfinstalled: case stat_configfiles:
    if (thissect)
      *thissect= pkg->section && *pkg->section ? pkg->section : _("<unknown>");
    return true;
  default:
    internerr("unknown package status '%d'", pkg->status);
  }
  return false;
}

void unpackchk(const char *const *argv) {
  int totalcount, sects;
  struct sectionentry *sectionentries, *se, **sep;
  struct pkgiterator *it;
  struct pkginfo *pkg;
  const char *thissect;
  char buf[20];
  int width;
  
  if (*argv)
    badusage(_("--%s takes no arguments"), cipaction->olong);

  modstatdb_init(admindir,msdbrw_readonly|msdbrw_noavail);

  totalcount= 0;
  sectionentries = NULL;
  sects= 0;
  it= iterpkgstart(); 
  while ((pkg= iterpkgnext(it))) {
    if (!yettobeunpacked(pkg, &thissect)) continue;
    for (se= sectionentries; se && strcasecmp(thissect,se->name); se= se->next);
    if (!se) {
      se= nfmalloc(sizeof(struct sectionentry));
      for (sep= &sectionentries;
           *sep && strcasecmp(thissect,(*sep)->name) > 0;
           sep= &(*sep)->next);
      se->name= thissect;
      se->count= 0;
      se->next= *sep;
      *sep= se;
      sects++;
    }
    se->count++; totalcount++;
  }
  iterpkgend(it);

  if (totalcount == 0) exit(0);
  
  if (totalcount <= 12) {
    it= iterpkgstart(); 
    while ((pkg= iterpkgnext(it))) {
      if (!yettobeunpacked(pkg, NULL))
        continue;
      describebriefly(pkg);
    }
    iterpkgend(it);
  } else if (sects <= 12) {
    for (se= sectionentries; se; se= se->next) {
      sprintf(buf,"%d",se->count);
      printf(_(" %d in %s: "),se->count,se->name);
      width= 70-strlen(se->name)-strlen(buf);
      while (width > 59) { putchar(' '); width--; }
      it= iterpkgstart(); 
      while ((pkg= iterpkgnext(it))) {
        if (!yettobeunpacked(pkg,&thissect)) continue;
        if (strcasecmp(thissect,se->name)) continue;
        width -= strlen(pkg->name); width--;
        if (width < 4) { printf(" ..."); break; }
        printf(" %s",pkg->name);
      }
      iterpkgend(it);
      putchar('\n');
    }
  } else {
    printf(P_(" %d package, from the following section:",
              " %d packages, from the following sections:", totalcount),
           totalcount);
    width= 0;
    for (se= sectionentries; se; se= se->next) {
      sprintf(buf,"%d",se->count);
      width -= (6 + strlen(se->name) + strlen(buf));
      if (width < 0) { putchar('\n'); width= 73 - strlen(se->name) - strlen(buf); }
      printf("   %s (%d)",se->name,se->count);
    }
    putchar('\n');
  }

  m_output(stdout, _("<standard output>"));
}

static void
assert_version_support(const char *const *argv,
                       struct versionrevision *version,
                       const char *feature_name)
{
  struct pkginfo *pkg;

  if (*argv)
    badusage(_("--%s takes no arguments"), cipaction->olong);

  modstatdb_init(admindir,msdbrw_readonly|msdbrw_noavail);

  pkg= findpackage("dpkg");
  switch (pkg->status) {
  case stat_installed:
  case stat_triggerspending:
    break;
  case stat_unpacked: case stat_halfconfigured: case stat_halfinstalled:
  case stat_triggersawaited:
    if (versionsatisfied3(&pkg->configversion, version, dvr_laterequal))
      break;
    printf(_("Version of dpkg with working %s support not yet configured.\n"
             " Please use 'dpkg --configure dpkg', and then try again.\n"),
           feature_name);
    exit(1);
  default:
    printf(_("dpkg not recorded as installed, cannot check for %s support!\n"),
           feature_name);
    exit(1);
  }
}

void assertpredep(const char *const *argv) {
  struct versionrevision version = { 0, "1.1.0", NULL };
  assert_version_support(argv, &version, _("Pre-Depends field"));
}

void assertepoch(const char *const *argv) {
  struct versionrevision version = { 0, "1.4.0.7", NULL };
  assert_version_support(argv, &version, _("epoch"));
}

void assertlongfilenames(const char *const *argv) {
  struct versionrevision version = { 0, "1.4.1.17", NULL };
  assert_version_support(argv, &version, _("long filenames"));
}

void assertmulticonrep(const char *const *argv) {
  struct versionrevision version = { 0, "1.4.1.19", NULL };
  assert_version_support(argv, &version, _("multiple Conflicts and Replaces"));
}

void predeppackage(const char *const *argv) {
  /* Print a single package which:
   *  (a) is the target of one or more relevant predependencies.
   *  (b) has itself no unsatisfied pre-dependencies.
   * If such a package is present output is the Packages file entry,
   *  which can be massaged as appropriate.
   * Exit status:
   *  0 = a package printed, OK
   *  1 = no suitable package available
   *  2 = error
   */
  static struct varbuf vb;
  
  struct pkgiterator *it;
  struct pkginfo *pkg = NULL, *startpkg, *trypkg;
  struct dependency *dep;
  struct deppossi *possi, *provider;

  if (*argv)
    badusage(_("--%s takes no arguments"), cipaction->olong);

  modstatdb_init(admindir,msdbrw_readonly);
  clear_istobes(); /* We use clientdata->istobe to detect loops */

  dep = NULL;
  it = iterpkgstart();
  while (!dep && (pkg = iterpkgnext(it))) {
    if (pkg->want != want_install) continue; /* Ignore packages user doesn't want */
    if (!pkg->files) continue; /* Ignore packages not available */
    pkg->clientdata->istobe= itb_preinstall;
    for (dep= pkg->available.depends; dep; dep= dep->next) {
      if (dep->type != dep_predepends) continue;
      if (depisok(dep, &vb, NULL, true))
        continue;
      break; /* This will leave dep non-NULL, and so exit the loop. */
    }
    pkg->clientdata->istobe= itb_normal;
    /* If dep is NULL we go and get the next package. */
  }
  iterpkgend(it);

  if (!dep) exit(1); /* Not found */
  assert(pkg);
  startpkg= pkg;
  pkg->clientdata->istobe= itb_preinstall;

  /* OK, we have found an unsatisfied predependency.
   * Now go and find the first thing we need to install, as a first step
   * towards satisfying it.
   */
  do {
    /* We search for a package which would satisfy dep, and put it in pkg */
    for (possi = dep->list, pkg = NULL;
         !pkg && possi;
         possi=possi->next) {
      trypkg= possi->ed;
      if (trypkg->files && versionsatisfied(&trypkg->available,possi)) {
        if (trypkg->clientdata->istobe == itb_normal) { pkg= trypkg; break; }
      }
      if (possi->verrel != dvr_none) continue;
      for (provider=possi->ed->available.depended;
           !pkg && provider;
           provider=provider->next) {
        if (provider->up->type != dep_provides) continue;
        trypkg= provider->up->up;
        if (!trypkg->files)
          continue;
        if (trypkg->clientdata->istobe == itb_normal) { pkg= trypkg; break; }
      }
    }
    if (!pkg) {
      varbufreset(&vb);
      describedepcon(&vb,dep);
      varbufaddc(&vb,0);
      fprintf(stderr, _("dpkg: cannot see how to satisfy pre-dependency:\n %s\n"),vb.buf);
      ohshit(_("cannot satisfy pre-dependencies for %.250s (wanted due to %.250s)"),
             dep->up->name,startpkg->name);
    }
    pkg->clientdata->istobe= itb_preinstall;
    for (dep= pkg->available.depends; dep; dep= dep->next) {
      if (dep->type != dep_predepends) continue;
      if (depisok(dep, &vb, NULL, true))
        continue;
      break; /* This will leave dep non-NULL, and so exit the loop. */
    }
  } while (dep);

  /* OK, we've found it - pkg has no unsatisfied pre-dependencies ! */
  writerecord(stdout, _("<standard output>"), pkg, &pkg->available);

  m_output(stdout, _("<standard output>"));
}

void printarch(const char *const *argv) {
  if (*argv)
    badusage(_("--%s takes no arguments"), cipaction->olong);

  printf("%s\n", architecture);

  m_output(stdout, _("<standard output>"));
}

void
printinstarch(const char *const *argv)
{
  warning(_("obsolete option '--%s', please use '--%s' instead."),
          "print-installation-architecture", "print-architecture");
  printarch(argv);
}

void cmpversions(const char *const *argv) {
  struct relationinfo {
    const char *string;
    /* These values are exit status codes, so 0=true, 1=false */
    int if_lesser, if_equal, if_greater;
    int if_none_a, if_none_both, if_none_b;
  };

  static const struct relationinfo relationinfos[]= {
   /*              < = > !a!2!b  */
    { "le",        0,0,1, 0,0,1  },
    { "lt",        0,1,1, 0,1,1  },
    { "eq",        1,0,1, 1,0,1  },
    { "ne",        0,1,0, 0,1,0  },
    { "ge",        1,0,0, 1,0,0  },
    { "gt",        1,1,0, 1,1,0  },
    { "le-nl",     0,0,1, 1,0,0  }, /* Here none        */
    { "lt-nl",     0,1,1, 1,1,0  }, /* is counted       */
    { "ge-nl",     1,0,0, 0,0,1  }, /* than any version.*/
    { "gt-nl",     1,1,0, 0,1,1  }, /*                  */
    { "<",         0,0,1, 0,0,1  }, /* For compatibility*/
    { "<=",        0,0,1, 0,0,1  }, /* with dpkg        */
    { "<<",        0,1,1, 0,1,1  }, /* control file     */
    { "=",         1,0,1, 1,0,1  }, /* syntax           */
    { ">",         1,0,0, 1,0,0  }, /*                  */
    { ">=",        1,0,0, 1,0,0  },
    { ">>",        1,1,0, 1,1,0  },
    { NULL                       }
  };

  const struct relationinfo *rip;
  const char *emsg;
  struct versionrevision a, b;
  int r;
  
  if (!argv[0] || !argv[1] || !argv[2] || argv[3])
    badusage(_("--compare-versions takes three arguments:"
             " <version> <relation> <version>"));

  for (rip=relationinfos; rip->string && strcmp(rip->string,argv[1]); rip++);

  if (!rip->string) badusage(_("--compare-versions bad relation"));

  if (*argv[0] && strcmp(argv[0],"<unknown>")) {
    emsg= parseversion(&a,argv[0]);
    if (emsg)
      ohshit(_("version '%s' has bad syntax: %s"), argv[0], emsg);
  } else {
    blankversion(&a);
  }
  if (*argv[2] && strcmp(argv[2],"<unknown>")) {
    emsg= parseversion(&b,argv[2]);
    if (emsg)
      ohshit(_("version '%s' has bad syntax: %s"), argv[2], emsg);
  } else {
    blankversion(&b);
  }
  if (!informativeversion(&a)) {
    exit(informativeversion(&b) ? rip->if_none_a : rip->if_none_both);
  } else if (!informativeversion(&b)) {
    exit(rip->if_none_b);
  }
  r= versioncompare(&a,&b);
  debug(dbg_general,"cmpversions a=`%s' b=`%s' r=%d",
        versiondescribe(&a,vdew_always),
        versiondescribe(&b,vdew_always),
        r);
  if (r>0) exit(rip->if_greater);
  else if (r<0) exit(rip->if_lesser);
  else exit(rip->if_equal);
}
