/* Copyright 1988,1990,1993,1994 by Paul Vixie
 * All rights reserved
 *
 * Distribute freely, except: don't remove my name from the source or
 * documentation (don't take credit for my work), mark your changes (don't
 * get me blamed for your possible bugs), don't alter or remove this
 * notice.  May be sold if buildable source is provided to buyer.  No
 * warrantee of any kind, express or implied, is included with this
 * software; use at your own risk, responsibility for damages (if any) to
 * anyone resulting from the use of this software rests entirely with the
 * user.
 *
 * Send bug reports, bug fixes, enhancements, requests, flames, etc., and
 * I'll try to keep a version up to date.  I can be reached as follows:
 * Paul Vixie          <paul@vix.com>          uunet!decwrl!vixie!paul
 */

#if !defined(lint) && !defined(LINT)
static char rcsid[] = "$Id: database.c,v 2.8 1994/01/15 20:43:43 vixie Exp $";
#endif

/* vix 26jan87 [RCS has the log]
 */


#include "cron.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>


#define TMAX(a,b) ((a)>(b)?(a):(b))

/* Try to get maximum path name -- this isn't really correct, but we're
going to be lazy */

#ifndef PATH_MAX

#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN 
#else
#define PATH_MAX 2048
#endif

#endif /* ifndef PATH_MAX */

static	void		process_crontab __P((char *, char *, char *,
					     struct stat *,
					     cron_db *, cron_db *));
#ifdef DEBIAN
static int valid_name (char *filename);
static user *get_next_system_crontab __P((user *));
#endif
void
load_database(old_db)
	cron_db		*old_db;
{
        DIR		*dir;
	struct stat	statbuf;
	struct stat	syscron_stat;
	DIR_T   	*dp;
	cron_db		new_db;
	user		*u, *nu;
#ifdef DEBIAN
	struct stat     syscrond_stat;
	struct stat     syscrond_file_stat;
	
        char            syscrond_fname[PATH_MAX+1];
	int             syscrond_change = 0;
#endif

	Debug(DLOAD, ("[%d] load_database()\n", getpid()))

	/* before we start loading any data, do a stat on SPOOL_DIR
	 * so that if anything changes as of this moment (i.e., before we've
	 * cached any of the database), we'll see the changes next time.
	 */
	if (stat(SPOOL_DIR, &statbuf) < OK) {
		log_it("CRON", getpid(), "STAT FAILED", SPOOL_DIR);
		(void) exit(ERROR_EXIT);
	}

	/* track system crontab file
	 */
	if (stat(SYSCRONTAB, &syscron_stat) < OK)
		syscron_stat.st_mtime = 0;

#ifdef DEBIAN
	/* Check mod time of SYSCRONDIR. This won't tell us if a file
         * in it changed, but will capture deletions, which the individual
         * file check won't
	 */
	if (stat(SYSCRONDIR, &syscrond_stat) < OK) {
		log_it("CRON", getpid(), "STAT FAILED", SYSCRONDIR);
		(void) exit(ERROR_EXIT);
	}

	/* If SYSCRONDIR was modified, we know that something is changed and
	 * there is no need for any further checks. If it wasn't, we should
	 * pass through the old list of files in SYSCRONDIR and check their
	 * mod time. Therefore a stopped hard drive won't be spun up, since
	 * we avoid reading of SYSCRONDIR and don't change its access time.
	 * This is especially important on laptops with APM.
	 */
	if (old_db->sysd_mtime != syscrond_stat.st_mtime) {
	        syscrond_change = 1;
	} else {
	        /* Look through the individual files */
		user *systab;

		Debug(DLOAD, ("[%d] system dir mtime unch, check files now.\n",
			      getpid()))

		for (systab = old_db->head;
		     (systab = get_next_system_crontab (systab)) != NULL;
		     systab = systab->next) {

			sprintf(syscrond_fname, "%s/%s", SYSCRONDIR,
							 systab->name + 8);

			Debug(DLOAD, ("\t%s:", syscrond_fname))

			if (stat(syscrond_fname, &syscrond_file_stat) < OK)
				syscrond_file_stat.st_mtime = 0;

			if (syscrond_file_stat.st_mtime != systab->mtime) {
			        syscrond_change = 1;
                        }

			Debug(DLOAD, (" [checked]\n"))
		}
	}
#endif /* DEBIAN */

	/* if spooldir's mtime has not changed, we don't need to fiddle with
	 * the database.
	 *
	 * Note that old_db->mtime is initialized to 0 in main(), and
	 * so is guaranteed to be different than the stat() mtime the first
	 * time this function is called.
	 */
#ifdef DEBIAN
	if ((old_db->user_mtime == statbuf.st_mtime) &&
	    (old_db->sys_mtime == syscron_stat.st_mtime) &&
	    (!syscrond_change)) {
#else
	if ((old_db->user_mtime == statbuf.st_mtime) &&
	    (old_db->sys_mtime == syscron_stat.st_mtime)) {
#endif
		Debug(DLOAD, ("[%d] spool dir mtime unch, no load needed.\n",
			      getpid()))
		return;
	}

	/* something's different.  make a new database, moving unchanged
	 * elements from the old database, reloading elements that have
	 * actually changed.  Whatever is left in the old database when
	 * we're done is chaff -- crontabs that disappeared.
	 */
	new_db.user_mtime = statbuf.st_mtime;
	new_db.sys_mtime = syscron_stat.st_mtime;
#ifdef DEBIAN
	new_db.sysd_mtime = syscrond_stat.st_mtime;
#endif
	new_db.head = new_db.tail = NULL;

	if (syscron_stat.st_mtime) {
		process_crontab("root", "*system*",
				SYSCRONTAB, &syscron_stat,
				&new_db, old_db);
	}

#ifdef DEBIAN
	/* Read all the package crontabs. */
	if (!(dir = opendir(SYSCRONDIR))) {
		log_it("CRON", getpid(), "OPENDIR FAILED", SYSCRONDIR);
		(void) exit(ERROR_EXIT);
	}

	while (NULL != (dp = readdir(dir))) {
		char	fname[MAXNAMLEN+1],
		        tabname[PATH_MAX+1];


		/* avoid file names beginning with ".".  this is good
		 * because we would otherwise waste two guaranteed calls
		 * to stat() for . and .., and also because package names
		 * starting with a period are just too nasty to consider.
		 */
		if (dp->d_name[0] == '.')
			continue;

		/* skipfile names with letters outside the set
		 * [A-Za-z0-9_-], like run-parts.
		 */
		if (!valid_name(dp->d_name))
		  continue;

		/* Generate the "fname" */
		(void) strcpy(fname,"*system*");
		(void) strcat(fname, dp->d_name);
		sprintf(tabname,"%s/%s", SYSCRONDIR, dp->d_name);

		/* statbuf is used as working storage by process_crontab() --
		   current contents are irrelevant */
		process_crontab("root", fname, tabname,
				&statbuf, &new_db, old_db);

	}
	closedir(dir);
#endif

	/* we used to keep this dir open all the time, for the sake of
	 * efficiency.  however, we need to close it in every fork, and
	 * we fork a lot more often than the mtime of the dir changes.
	 */
	if (!(dir = opendir(SPOOL_DIR))) {
		log_it("CRON", getpid(), "OPENDIR FAILED", SPOOL_DIR);
		(void) exit(ERROR_EXIT);
	}

	while (NULL != (dp = readdir(dir))) {
		char	fname[MAXNAMLEN+1],
			tabname[PATH_MAX+1];

		/* avoid file names beginning with ".".  this is good
		 * because we would otherwise waste two guaranteed calls
		 * to getpwnam() for . and .., and also because user names
		 * starting with a period are just too nasty to consider.
		 */
		if (dp->d_name[0] == '.')
			continue;

		(void) strcpy(fname, dp->d_name);
		snprintf(tabname, PATH_MAX+1, CRON_TAB(fname));

		process_crontab(fname, fname, tabname,
				&statbuf, &new_db, old_db);
	}
	closedir(dir);

	/* if we don't do this, then when our children eventually call
	 * getpwnam() in do_command.c's child_process to verify MAILTO=,
	 * they will screw us up (and v-v).
	 */
	endpwent();

	/* whatever's left in the old database is now junk.
	 */
	Debug(DLOAD, ("unlinking old database:\n"))
	for (u = old_db->head;  u != NULL;  u = nu) {
		Debug(DLOAD, ("\t%s\n", u->name))
		nu = u->next;
		unlink_user(old_db, u);
		free_user(u);
	}

	/* overwrite the database control block with the new one.
	 */
	*old_db = new_db;
	Debug(DLOAD, ("load_database is done\n"))
}


void
link_user(db, u)
	cron_db	*db;
	user	*u;
{
	if (db->head == NULL)
		db->head = u;
	if (db->tail)
		db->tail->next = u;
	u->prev = db->tail;
	u->next = NULL;
	db->tail = u;
}


void
unlink_user(db, u)
	cron_db	*db;
	user	*u;
{
	if (u->prev == NULL)
		db->head = u->next;
	else
		u->prev->next = u->next;

	if (u->next == NULL)
		db->tail = u->prev;
	else
		u->next->prev = u->prev;
}


user *
find_user(db, name)
	cron_db	*db;
	char	*name;
{
	char	*env_get();
	user	*u;

	for (u = db->head;  u != NULL;  u = u->next)
		if (!strcmp(u->name, name))
			break;
	return u;
}


static void
process_crontab(uname, fname, tabname, statbuf, new_db, old_db)
	char		*uname;
	char		*fname;
	char		*tabname;
	struct stat	*statbuf;
	cron_db		*new_db;
	cron_db		*old_db;
{
	struct passwd	*pw = NULL;
	int		crontab_fd = OK - 1;
	user		*u;

#ifdef DEBIAN
	/* If the name begins with *system*, don't worry about password -
	 it's part of the system crontab */
	if (strncmp(fname, "*system*", 8) && !(pw = getpwnam(uname))) {
#else
	if (strcmp(fname, "*system*") && !(pw = getpwnam(uname))) {
#endif
		/* file doesn't have a user in passwd file.
		 */
		if (strncmp(fname, "tmp.", 4)) {
			/* don't log these temporary files */
			log_it(fname, getpid(), "ORPHAN", "no passwd entry");
		}
		goto next_crontab;
	}

	if ((crontab_fd = open(tabname, O_RDONLY, 0)) < OK) {
		/* crontab not accessible?
		 */
		log_it(fname, getpid(), "CAN'T OPEN", tabname);
		goto next_crontab;
	}

	if (fstat(crontab_fd, statbuf) < OK) {
		log_it(fname, getpid(), "FSTAT FAILED", tabname);
		goto next_crontab;
	}

	Debug(DLOAD, ("\t%s:", fname))
	u = find_user(old_db, fname);
	if (u != NULL) {
		/* if crontab has not changed since we last read it
		 * in, then we can just use our existing entry.
		 */
		if (u->mtime == statbuf->st_mtime) {
			Debug(DLOAD, (" [no change, using old data]"))
			unlink_user(old_db, u);
			link_user(new_db, u);
			goto next_crontab;
		}

		/* before we fall through to the code that will reload
		 * the user, let's deallocate and unlink the user in
		 * the old database.  This is more a point of memory
		 * efficiency than anything else, since all leftover
		 * users will be deleted from the old database when
		 * we finish with the crontab...
		 */
		Debug(DLOAD, (" [delete old data]"))
		unlink_user(old_db, u);
		free_user(u);
		log_it(fname, getpid(), "RELOAD", tabname);
	}
	u = load_user(crontab_fd, pw, fname);
	if (u != NULL) {
		u->mtime = statbuf->st_mtime;
		link_user(new_db, u);
	}

next_crontab:
	if (crontab_fd >= OK) {
		Debug(DLOAD, (" [done]\n"))
		close(crontab_fd);
	}
}

#ifdef DEBIAN

/* True or false? Is this a valid filename (upper/lower alpha, digits,
 * underscores, and hyphens only?)
 */
#include <ctype.h>
/* Same function, better compliance with ISO C */
static int valid_name (char *filename)
{
  while (*filename) {
    if (!(isalnum(*filename) ||
	  (*filename == '_') ||
	  (*filename == '-')))
      return 0;
    ++filename;
  }

  return 1;
}

static user *
get_next_system_crontab (curtab)
	user	*curtab;
{
	for ( ; curtab != NULL; curtab = curtab->next)
		if (!strncmp(curtab->name, "*system*", 8) && curtab->name [8])
			break;
	return curtab;
}

#endif
