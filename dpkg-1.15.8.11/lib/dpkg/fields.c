/*
 * libdpkg - Debian packaging suite library routines
 * fields.c - parsing of all the different fields, when reading in
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 * Copyright © 2001 Wichert Akkerman
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
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/path.h>
#include <dpkg/parsedump.h>
#include <dpkg/triglib.h>

static int
convert_string(struct parsedb_state *ps, const char *what, int otherwise,
               const struct pkginfo *pigp,
               const char *startp, const struct namevalue *ivip,
               const char **endpp)
{
  const char *ep;
  const struct namevalue *nvip = ivip;

  if (!*startp)
    parse_error(ps, pigp, _("%s is missing"), what);
  while (nvip->name) {
    if (strncasecmp(nvip->name, startp, nvip->length))
      nvip++;
    else
      break;
  }
  if (!nvip->name) {
    if (otherwise != -1) return otherwise;
    parse_error(ps, pigp, _("`%.*s' is not allowed for %s"),
                (int)strnlen(startp, 50), startp, what);
  }

  ep = startp + nvip->length;
  while (isspace(*ep))
    ep++;
  if (*ep && !endpp)
    parse_error(ps, pigp, _("junk after %s"), what);
  if (endpp) *endpp= ep;
  return nvip->value;
}

void
f_name(struct pkginfo *pigp, struct pkginfoperfile *pifp,
       struct parsedb_state *ps,
       const char *value, const struct fieldinfo *fip)
{
  const char *e;
  if ((e= illegal_packagename(value,NULL)) != NULL)
    parse_error(ps, pigp, _("invalid package name (%.250s)"), e);
  pigp->name= findpackage(value)->name;
  /* We use the new name, as findpackage() may have
     done a tolower for us.
   */
}

void f_filecharf(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                 struct parsedb_state *ps,
                 const char *value, const struct fieldinfo *fip) {
  struct filedetails *fdp, **fdpp;
  char *cpos, *space;
  int allowextend;

  if (!*value)
    parse_error(ps, pigp, _("empty file details field `%s'"), fip->name);
  if (!(ps->flags & pdb_recordavailable))
    parse_error(ps, pigp,
                _("file details field `%s' not allowed in status file"),
               fip->name);
  allowextend= !pigp->files;
  fdpp= &pigp->files;
  cpos= nfstrsave(value);
  while (*cpos) {
    space= cpos; while (*space && !isspace(*space)) space++;
    if (*space)
      *space++ = '\0';
    fdp= *fdpp;
    if (!fdp) {
      if (!allowextend)
        parse_error(ps, pigp,
                    _("too many values in file details field `%s' "
                      "(compared to others)"), fip->name);
      fdp= nfmalloc(sizeof(struct filedetails));
      fdp->next= NULL;
      fdp->name= fdp->msdosname= fdp->size= fdp->md5sum= NULL;
      *fdpp= fdp;
    }
    FILEFFIELD(fdp,fip->integer,const char*)= cpos;
    fdpp= &fdp->next;
    while (*space && isspace(*space)) space++;
    cpos= space;
  }
  if (*fdpp)
    parse_error(ps, pigp,
                _("too few values in file details field `%s' "
                  "(compared to others)"), fip->name);
}

void f_charfield(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                 struct parsedb_state *ps,
                 const char *value, const struct fieldinfo *fip) {
  if (*value) PKGPFIELD(pifp,fip->integer,char*)= nfstrsave(value);
}

void f_boolean(struct pkginfo *pigp, struct pkginfoperfile *pifp,
               struct parsedb_state *ps,
               const char *value, const struct fieldinfo *fip) {
  bool boolean;

  if (!*value)
    return;

  boolean = convert_string(ps, _("yes/no in boolean field"),
                           -1, pigp, value, booleaninfos, NULL);
  PKGPFIELD(pifp, fip->integer, bool) = boolean;
}

void f_section(struct pkginfo *pigp, struct pkginfoperfile *pifp,
               struct parsedb_state *ps,
               const char *value, const struct fieldinfo *fip) {
  if (!*value) return;
  pigp->section= nfstrsave(value);
}

void f_priority(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                struct parsedb_state *ps,
                const char *value, const struct fieldinfo *fip) {
  if (!*value) return;
  pigp->priority = convert_string(ps, _("word in `priority' field"),
                                  pri_other, pigp, value, priorityinfos, NULL);
  if (pigp->priority == pri_other) pigp->otherpriority= nfstrsave(value);
}

void f_status(struct pkginfo *pigp, struct pkginfoperfile *pifp,
              struct parsedb_state *ps,
              const char *value, const struct fieldinfo *fip) {
  const char *ep;

  if (ps->flags & pdb_rejectstatus)
    parse_error(ps, pigp,
                _("value for `status' field not allowed in this context"));
  if (ps->flags & pdb_recordavailable)
    return;
  
  pigp->want = convert_string(ps, _("first (want) word in `status' field"),
                              -1, pigp, value, wantinfos, &ep);
  pigp->eflag = convert_string(ps, _("second (error) word in `status' field"),
                               -1, pigp, ep, eflaginfos, &ep);
  pigp->status = convert_string(ps, _("third (status) word in `status' field"),
                                -1, pigp, ep, statusinfos, NULL);
}

void f_version(struct pkginfo *pigp, struct pkginfoperfile *pifp,
               struct parsedb_state *ps,
               const char *value, const struct fieldinfo *fip) {
  parse_db_version(ps, pigp, &pifp->version, value,
                   _("error in Version string '%.250s'"), value);
}  

void f_revision(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                struct parsedb_state *ps,
                const char *value, const struct fieldinfo *fip) {
  char *newversion;

  parse_warn(ps, pigp,
             _("obsolete `Revision' or `Package-Revision' field used"));
  if (!*value) return;
  if (pifp->version.revision && *pifp->version.revision) {
    newversion= nfmalloc(strlen(pifp->version.version)+strlen(pifp->version.revision)+2);
    sprintf(newversion,"%s-%s",pifp->version.version,pifp->version.revision);
    pifp->version.version= newversion;
  }
  pifp->version.revision= nfstrsave(value);
}  

void f_configversion(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                     struct parsedb_state *ps,
                     const char *value, const struct fieldinfo *fip) {
  if (ps->flags & pdb_rejectstatus)
    parse_error(ps, pigp,
                _("value for `config-version' field not allowed in this context"));
  if (ps->flags & pdb_recordavailable)
    return;

  parse_db_version(ps, pigp, &pigp->configversion, value,
                   _("error in Config-Version string '%.250s'"), value);

}

static void conffvalue_lastword(const char *value, const char *from,
                                const char *endent,
                                const char **word_start_r, int *word_len_r,
                                const char **new_from_r,
                                struct parsedb_state *ps,
                                struct pkginfo *pigp)
{
  /* the code in f_conffiles ensures that value[-1]==' ', which is helpful */
  const char *lastspc;
  
  if (from <= value+1) goto malformed;
  for (lastspc= from-1; *lastspc != ' '; lastspc--);
  if (lastspc <= value+1 || lastspc >= endent-1) goto malformed;

  *new_from_r= lastspc;
  *word_start_r= lastspc + 1;
  *word_len_r= (int)(from - *word_start_r);
  return;

malformed:
  parse_error(ps, pigp,
              _("value for `conffiles' has malformatted line `%.*s'"),
              (int)min(endent - value, 250), value);
}

void f_conffiles(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                 struct parsedb_state *ps,
                 const char *value, const struct fieldinfo *fip) {
  static const char obsolete_str[]= "obsolete";
  struct conffile **lastp, *newlink;
  const char *endent, *endfn, *hashstart;
  int c, namelen, hashlen, obsolete;
  char *newptr;
  
  lastp= &pifp->conffiles;
  while (*value) {
    c= *value++;
    if (c == '\n') continue;
    if (c != ' ')
      parse_error(ps, pigp,
                  _("value for `conffiles' has line starting with non-space `%c'"),
                  c);
    for (endent = value; (c = *endent) != '\0' && c != '\n'; endent++) ;
    conffvalue_lastword(value, endent, endent,
			&hashstart, &hashlen, &endfn,
                        ps, pigp);
    obsolete= (hashlen == sizeof(obsolete_str)-1 &&
	       !memcmp(hashstart, obsolete_str, hashlen));
    if (obsolete)
      conffvalue_lastword(value, endfn, endent,
			  &hashstart, &hashlen, &endfn,
			  ps, pigp);
    newlink= nfmalloc(sizeof(struct conffile));
    value = path_skip_slash_dotslash(value);
    namelen= (int)(endfn-value);
    if (namelen <= 0)
      parse_error(ps, pigp,
                  _("root or null directory is listed as a conffile"));
    newptr = nfmalloc(namelen+2);
    newptr[0]= '/';
    memcpy(newptr+1,value,namelen);
    newptr[namelen+1] = '\0';
    newlink->name= newptr;
    newptr= nfmalloc(hashlen+1);
    memcpy(newptr, hashstart, hashlen);
    newptr[hashlen] = '\0';
    newlink->hash= newptr;
    newlink->obsolete= obsolete;
    newlink->next =NULL;
    *lastp= newlink;
    lastp= &newlink->next;
    value= endent;
  }
}

void f_dependency(struct pkginfo *pigp, struct pkginfoperfile *pifp,
                  struct parsedb_state *ps,
                  const char *value, const struct fieldinfo *fip) {
  char c1, c2;
  const char *p, *emsg;
  const char *depnamestart, *versionstart;
  int depnamelength, versionlength;
  static struct varbuf depname, version;

  struct dependency *dyp, **ldypp;
  struct deppossi *dop, **ldopp;

  if (!*value) return; /* empty fields are ignored */
  p= value;
  ldypp= &pifp->depends; while (*ldypp) ldypp= &(*ldypp)->next;
  for (;;) { /* loop creating new struct dependency's */
    dyp= nfmalloc(sizeof(struct dependency));
    dyp->up= NULL; /* Set this to zero for now, as we don't know what our real
                 * struct pkginfo address (in the database) is going to be yet.
                 */
    dyp->next= NULL; *ldypp= dyp; ldypp= &dyp->next;
    dyp->list= NULL; ldopp= &dyp->list;
    dyp->type= fip->integer;
    for (;;) { /* loop creating new struct deppossi's */
      depnamestart= p;
/* skip over package name characters */
      while (*p && !isspace(*p) && *p != '(' && *p != ',' && *p != '|') {
	p++;
      }
      depnamelength= p - depnamestart ;
      varbufreset(&depname);
      varbufaddbuf(&depname, depnamestart, depnamelength);
      varbufaddc(&depname, '\0');
      if (!depname.buf[0])
        parse_error(ps, pigp,
                    _("`%s' field, missing package name, or garbage where "
                      "package name expected"), fip->name);
      emsg = illegal_packagename(depname.buf, NULL);
      if (emsg)
        parse_error(ps, pigp,
                    _("`%s' field, invalid package name `%.255s': %s"),
                    fip->name, depname.buf, emsg);
      dop= nfmalloc(sizeof(struct deppossi));
      dop->up= dyp;
      dop->ed = findpackage(depname.buf);
      dop->next= NULL; *ldopp= dop; ldopp= &dop->next;

      /* Don't link this (which is after all only ‘newpig’ from
       * the main parsing loop in parsedb) into the depended on
       * packages' lists yet. This will be done later when we
       * install this (in parse.c). For the moment we do the
       * ‘forward’ links in deppossi (‘ed’) only, and the ‘backward’
       * links from the depended on packages to dop are left undone. */
      dop->rev_next = NULL;
      dop->rev_prev = NULL;

      dop->cyclebreak = false;
/* skip whitespace after packagename */
      while (isspace(*p)) p++;
      if (*p == '(') {			/* if we have a versioned relation */
        p++; while (isspace(*p)) p++;
        c1= *p;
        if (c1 == '<' || c1 == '>') {
          c2= *++p;
          dop->verrel= (c1 == '<') ? dvrf_earlier : dvrf_later;
          if (c2 == '=') {
            dop->verrel |= (dvrf_orequal | dvrf_builtup);
            p++;
          } else if (c2 == c1) {
            dop->verrel |= (dvrf_strict | dvrf_builtup);
            p++;
          } else if (c2 == '<' || c2 == '>') {
            parse_error(ps, pigp,
                        _("`%s' field, reference to `%.255s':\n"
                          " bad version relationship %c%c"),
                        fip->name, depname.buf, c1, c2);
            dop->verrel= dvr_none;
          } else {
            parse_warn(ps, pigp,
                       _("`%s' field, reference to `%.255s':\n"
                         " `%c' is obsolete, use `%c=' or `%c%c' instead"),
                       fip->name, depname.buf, c1, c1, c1, c1);
            dop->verrel |= (dvrf_orequal | dvrf_builtup);
          }
        } else if (c1 == '=') {
          dop->verrel= dvr_exact;
          p++;
        } else {
          parse_warn(ps, pigp,
                     _("`%s' field, reference to `%.255s':\n"
                       " implicit exact match on version number, "
                       "suggest using `=' instead"),
                     fip->name, depname.buf);
          dop->verrel= dvr_exact;
        }
	if ((dop->verrel!=dvr_exact) && (fip->integer==dep_provides))
          parse_warn(ps, pigp,
                     _("Only exact versions may be used for Provides"));

        if (!isspace(*p) && !isalnum(*p)) {
          parse_warn(ps, pigp,
                     _("`%s' field, reference to `%.255s':\n"
                       " version value starts with non-alphanumeric, "
                       "suggest adding a space"),
                     fip->name, depname.buf);
        }
/* skip spaces between the relation and the version */
        while (isspace(*p)) p++;

	versionstart= p;
        while (*p && *p != ')' && *p != '(') {
          if (isspace(*p)) break;
          p++;
        }
	versionlength= p - versionstart;
        while (isspace(*p)) p++;
        if (*p == '(')
          parse_error(ps, pigp,
                      _("`%s' field, reference to `%.255s': "
                        "version contains `%c'"), fip->name, depname.buf, ')');
        else if (*p != ')')
          parse_error(ps, pigp,
                      _("`%s' field, reference to `%.255s': "
                        "version contains `%c'"), fip->name, depname.buf, ' ');
        else if (*p == '\0')
          parse_error(ps, pigp,
                      _("`%s' field, reference to `%.255s': "
                        "version unterminated"), fip->name, depname.buf);
        varbufreset(&version);
        varbufaddbuf(&version, versionstart, versionlength);
        varbufaddc(&version, '\0');
        parse_db_version(ps, pigp, &dop->version, version.buf,
                         _("'%s' field, reference to '%.255s': "
                           "error in version"), fip->name, depname.buf);
        p++; while (isspace(*p)) p++;
      } else {
        dop->verrel= dvr_none;
        blankversion(&dop->version);
      }
      if (!*p || *p == ',') break;
      if (*p != '|')
        parse_error(ps, pigp,
                    _("`%s' field, syntax error after reference to package `%.255s'"),
                    fip->name, dop->ed->name);
      if (fip->integer == dep_conflicts ||
          fip->integer == dep_breaks ||
          fip->integer == dep_provides ||
          fip->integer == dep_replaces)
        parse_error(ps, pigp,
                    _("alternatives (`|') not allowed in %s field"), fip->name);
      p++; while (isspace(*p)) p++;
    }
    if (!*p) break;
    p++; while (isspace(*p)) p++;
  }
}

static const char *
scan_word(const char **valp)
{
  static struct varbuf word;
  const char *p, *start, *end;

  p = *valp;
  for (;;) {
    if (!*p) {
      *valp = p;
      return NULL;
    }
    if (cisspace(*p)) {
      p++;
      continue;
    }
    start = p;
    break;
  }
  for (;;) {
    if (*p && !cisspace(*p)) {
      p++;
      continue;
    }
    end = p;
    break;
  }

  varbufreset(&word);
  varbufaddbuf(&word, start, end - start);
  varbufaddc(&word, '\0');

  *valp = p;

  return word.buf;
}

void
f_trigpend(struct pkginfo *pend, struct pkginfoperfile *pifp,
           struct parsedb_state *ps,
           const char *value, const struct fieldinfo *fip)
{
  const char *word, *emsg;

  if (ps->flags & pdb_rejectstatus)
    parse_error(ps, pend,
                _("value for `triggers-pending' field not allowed in "
                  "this context"));

  while ((word = scan_word(&value))) {
    emsg = illegal_triggername(word);
    if (emsg)
      parse_error(ps, pend,
                  _("illegal pending trigger name `%.255s': %s"), word, emsg);

    if (!trig_note_pend_core(pend, nfstrsave(word)))
      parse_error(ps, pend,
                  _("duplicate pending trigger `%.255s'"), word);
  }
}

void
f_trigaw(struct pkginfo *aw, struct pkginfoperfile *pifp,
         struct parsedb_state *ps,
         const char *value, const struct fieldinfo *fip)
{
  const char *word, *emsg;
  struct pkginfo *pend;

  if (ps->flags & pdb_rejectstatus)
    parse_error(ps, aw,
                _("value for `triggers-awaited' field not allowed in "
                  "this context"));

  while ((word = scan_word(&value))) {
    emsg = illegal_packagename(word, NULL);
    if (emsg)
      parse_error(ps, aw,
                  _("illegal package name in awaited trigger `%.255s': %s"),
                  word, emsg);
    pend = findpackage(word);

    if (!trig_note_aw(pend, aw))
      parse_error(ps, aw,
                  _("duplicate awaited trigger package `%.255s'"), word);

    trig_enqueue_awaited_pend(pend);
  }
}

