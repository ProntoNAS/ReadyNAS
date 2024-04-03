/*
 * dpkg-statoverride - override ownership and mode of files
 *
 * Copyright © 2000, 2001 Wichert Akkerman <wakkerma@debian.org>
 * Copyright © 2006-2009 Guillem Jover <guillem@debian.org>
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
 *
 */

#include <config.h>
#include <compat.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <fnmatch.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/path.h>
#include <dpkg/dir.h>
#include <dpkg/myopt.h>

#include "main.h"
#include "glob.h"
#include "filesdb.h"

const char thisname[] = "dpkg-statoverride";
const char printforhelp[] = N_("Use --help for help about querying packages.");

static void DPKG_ATTR_NORET
printversion(const struct cmdinfo *cip, const char *value)
{
	printf(_("Debian %s version %s.\n"), thisname, DPKG_VERSION_ARCH);

	printf(_(
"Copyright (C) 2000, 2001 Wichert Akkerman.\n"
"Copyright (C) 2006-2009 Guillem Jover.\n"));

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
"\n"), thisname);

	printf(_(
"Commands:\n"
"  --add <owner> <group> <mode> <file>\n"
"                           add a new entry into the database.\n"
"  --remove <file>          remove file from the database.\n"
"  --list [<glob-pattern>]  list current overrides in the database.\n"
"\n"));

	printf(_(
"Options:\n"
"  --admindir <directory>   set the directory with the statoverride file.\n"
"  --update                 immediately update file permissions.\n"
"  --force                  force an action even if a sanity check fails.\n"
"  --quiet                  quiet operation, minimal output.\n"
"  --help                   show this help message.\n"
"  --version                show the version.\n"
"\n"));

	m_output(stdout, _("<standard output>"));

	exit(0);
}

const struct cmdinfo *cipaction = NULL;
const char *admindir = ADMINDIR;

static int opt_verbose = 1;
static int opt_force = 0;
static int opt_update = 0;

static void
setaction(const struct cmdinfo *cip, const char *value)
{
	if (cipaction)
		badusage(_("conflicting actions -%c (--%s) and -%c (--%s)"),
		         cip->oshort, cip->olong,
		         cipaction->oshort, cipaction->olong);
	cipaction = cip;
}

static char *
path_cleanup(const char *path)
{
	char *new_path = m_strdup(path);

	path_rtrim_slash_slashdot(new_path);
	if (opt_verbose && strcmp(path, new_path) != 0)
		warning(_("stripping trailing /"));

	return new_path;
}

static struct filestatoverride *
statdb_node_new(const char *user, const char *group, const char *mode)
{
	struct filestatoverride *filestat;

	filestat = nfmalloc(sizeof(*filestat));

	filestat->uid = statdb_parse_uid(user);
	filestat->gid = statdb_parse_gid(group);
	filestat->mode = statdb_parse_mode(mode);

	return filestat;
}

static struct filestatoverride **
statdb_node_find(const char *filename)
{
	struct filenamenode *file;

	file = findnamenode(filename, 0);

	return &file->statoverride;
}

static int
statdb_node_remove(const char *filename)
{
	struct filenamenode *file;

	file = findnamenode(filename, fnn_nonew);
	if (!file || (file && !file->statoverride))
		return 0;

	file->statoverride = NULL;

	return 1;
}

static void
statdb_node_apply(const char *filename, struct filestatoverride *filestat)
{
	if (chown(filename, filestat->uid, filestat->gid) < 0)
		ohshite(_("error setting ownership of `%.255s'"), filename);
	if (chmod(filename, filestat->mode))
		ohshite(_("error setting permissions of `%.255s'"), filename);
}

static void
statdb_node_print(FILE *out, struct filenamenode *file)
{
	struct filestatoverride *filestat = file->statoverride;
	struct passwd *pw;
	struct group *gr;

	if (!filestat)
		return;

	pw = getpwuid(filestat->uid);
	if (pw)
		fprintf(out, "%s ", pw->pw_name);
	else
		fprintf(out, "#%d ", filestat->uid);

	gr = getgrgid(filestat->gid);
	if (gr)
		fprintf(out, "%s ", gr->gr_name);
	else
		fprintf(out, "#%d ", filestat->gid);

	fprintf(out, "%o %s\n", filestat->mode, file->name);
}

static void
statdb_write(void)
{
	FILE *dbfile;
	struct fileiterator *i;
	struct filenamenode *file;
	struct varbuf dbname = VARBUF_INIT;
	struct varbuf dbname_new = VARBUF_INIT;
	struct varbuf dbname_old = VARBUF_INIT;

	varbufaddstr(&dbname, admindir);
	varbufaddstr(&dbname, "/" STATOVERRIDEFILE);
	varbufaddc(&dbname, '\0');

	varbufaddstr(&dbname_new, dbname.buf);
	varbufaddstr(&dbname_new, NEWDBEXT);
	varbufaddc(&dbname_new, '\0');

	varbufaddstr(&dbname_old, dbname.buf);
	varbufaddstr(&dbname_old, OLDDBEXT);
	varbufaddc(&dbname_old, '\0');

	dbfile = fopen(dbname_new.buf, "w");
	if (!dbfile)
		ohshite(_("cannot open new statoverride file"));

	i = iterfilestart();
	while ((file = iterfilenext(i)))
		statdb_node_print(dbfile, file);
	iterfileend(i);

	if (fflush(dbfile))
		ohshite(_("unable to flush file '%s'"), dbname_new.buf);
	if (fsync(fileno(dbfile)))
		ohshite(_("unable to sync file '%s'"), dbname_new.buf);
	fclose(dbfile);

	chmod(dbname_new.buf, 0644);
	if (unlink(dbname_old.buf) && errno != ENOENT)
		ohshite(_("error removing statoverride-old"));
	if (link(dbname.buf, dbname_old.buf) && errno != ENOENT)
		ohshite(_("error creating new statoverride-old"));
	if (rename(dbname_new.buf, dbname.buf))
		ohshite(_("error installing new statoverride"));

	dir_sync_path(admindir);

	varbuf_destroy(&dbname);
	varbuf_destroy(&dbname_new);
	varbuf_destroy(&dbname_old);
}

static int
statoverride_add(const char *const *argv)
{
	const char *user = argv[0];
	const char *group = argv[1];
	const char *mode = argv[2];
	const char *path = argv[3];
	char *filename;
	struct filestatoverride **filestat;

	if (!user || !group || !mode || !path || argv[4])
		badusage(_("--add needs four arguments"));

	if (strchr(path, '\n'))
		badusage(_("file may not contain newlines"));

	filename = path_cleanup(path);

	filestat = statdb_node_find(filename);
	if (*filestat != NULL) {
		if (opt_force)
			warning(_("An override for '%s' already exists, "
			          "but --force specified so will be ignored."),
			        filename);
		else
			ohshit(_("An override for '%s' already exists, "
			         "aborting."), filename);
	}

	*filestat = statdb_node_new(user, group, mode);

	if (opt_update) {
		struct stat st;

		if (stat(filename, &st) == 0)
			statdb_node_apply(filename, *filestat);
		else if (opt_verbose)
			warning(_("--update given but %s does not exist"),
			        filename);
	}

	statdb_write();

	free(filename);

	return 0;
}

static int
statoverride_remove(const char *const *argv)
{
	const char *path = argv[0];
	char *filename;

	if (!path || argv[1])
		badusage(_("--%s needs a single argument"), "remove");

	filename = path_cleanup(path);

	if (!statdb_node_remove(filename)) {
		if (opt_verbose)
			warning(_("No override present."));
		if (opt_force)
			exit(0);
		else
			exit(2);
	}

	if (opt_update && opt_verbose)
		warning(_("--update is useless for --remove"));

	statdb_write();

	free(filename);

	return 0;
}

static int
statoverride_list(const char *const *argv)
{
	struct fileiterator *i;
	struct filenamenode *file;
	const char *thisarg;
	struct glob_node *glob_list = NULL;
	int ret = 1;

	while ((thisarg = *argv++)) {
		char *pattern = path_cleanup(thisarg);

		glob_list_prepend(&glob_list, pattern);
	}
	if (glob_list == NULL)
		glob_list_prepend(&glob_list, m_strdup("*"));

	i = iterfilestart();
	while ((file = iterfilenext(i))) {
		struct glob_node *g;

		for (g = glob_list; g; g = g->next) {
			if (fnmatch(g->pattern, file->name, 0) == 0) {
				statdb_node_print(stdout, file);
				ret = 0;
				break;
			}
		}
	}
	iterfileend(i);

	glob_list_free(glob_list);

	return ret;
}

#define ACTION(longopt, shortopt, code, function) \
 { longopt, shortopt, 0, 0, 0, setaction, code, 0, (voidfnp)function }

static const struct cmdinfo cmdinfos[] = {
	ACTION("add",    0, act_install,   statoverride_add),
	ACTION("remove", 0, act_remove,    statoverride_remove),
	ACTION("list",   0, act_listfiles, statoverride_list),

	{ "admindir",   0,   1,  NULL,         &admindir, NULL          },
	{ "quiet",      0,   0,  &opt_verbose, NULL,      NULL, 0       },
	{ "force",      0,   0,  &opt_force,   NULL,      NULL, 1       },
	{ "update",     0,   0,  &opt_update,  NULL,      NULL, 1       },
	{ "help",       'h', 0,  NULL,         NULL,      usage         },
	{ "version",    0,   0,  NULL,         NULL,      printversion  },
	{  NULL,        0                                               }
};

int
main(int argc, const char *const *argv)
{
	jmp_buf ejbuf;
	int (*actionfunction)(const char *const *argv);
	int ret;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	standard_startup(&ejbuf);
	myopt(&argv, cmdinfos);

	if (!cipaction)
		badusage(_("need an action option"));

	setvbuf(stdout, NULL, _IONBF, 0);

	filesdbinit();
	ensure_statoverrides();

	actionfunction = (int (*)(const char *const *))cipaction->farg;
	ret = actionfunction(argv);

	standard_shutdown();

	return ret;
}

