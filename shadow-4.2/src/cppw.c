/*
  cppw, cpgr  copy with locking given file over the password or group file
  with -s will copy with locking given file over shadow or gshadow file

  Copyright (C) 1999 Stephen Frost <sfrost@snowman.net>

  Based on vipw, vigr by:
  Copyright (C) 1997 Guy Maor <maor@ece.utexas.edu>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */

#include <config.h>
#include "defines.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <utime.h>
#ifdef WITH_SELINUX
#include <selinux/selinux.h>
#endif				/* WITH_SELINUX */
#include "exitcodes.h"
#include "prototypes.h"
#include "pwio.h"
#include "shadowio.h"
#include "groupio.h"
#include "sgroupio.h"


const char *Prog;

const char *filename, *filenewname;
static bool filelocked = false;
static int (*unlock) (void);

/* local function prototypes */
static int create_copy (FILE *fp, const char *dest, struct stat *sb);
static void cppwexit (const char *msg, int syserr, int ret);
static void cppwcopy (const char *file,
                      const char *in_file,
                      int (*file_lock) (void),
                      int (*file_unlock) (void));

static int create_copy (FILE *fp, const char *dest, struct stat *sb)
{
	struct utimbuf ub;
	FILE *bkfp;
	int c;
	mode_t mask;

	mask = umask (077);
	bkfp = fopen (dest, "w");
	(void) umask (mask);
	if (NULL == bkfp) {
		return -1;
	}

	rewind (fp);
	while ((c = getc (fp)) != EOF) {
		if (putc (c, bkfp) == EOF) {
			break;
		}
	}

	if (   (c != EOF)
	    || (fflush (bkfp) != 0)) {
		(void) fclose (bkfp);
		(void) unlink (dest);
		return -1;
	}
	if (   (fsync (fileno (bkfp)) != 0)
	    || (fclose (bkfp) != 0)) {
		(void) unlink (dest);
		return -1;
	}

	ub.actime = sb->st_atime;
	ub.modtime = sb->st_mtime;
	if (   (utime (dest, &ub) != 0)
	    || (chmod (dest, sb->st_mode) != 0)
	    || (chown (dest, sb->st_uid, sb->st_gid) != 0)) {
		(void) unlink (dest);
		return -1;
	}
	return 0;
}

static void cppwexit (const char *msg, int syserr, int ret)
{
	int err = errno;
	if (filelocked) {
		(*unlock) ();
	}
	if (NULL != msg) {
		fprintf (stderr, "%s: %s", Prog, msg);
		if (0 != syserr) {
			fprintf (stderr, ": %s", strerror (err));
		}
		(void) fputs ("\n", stderr);
	}
	if (NULL != filename) {
		fprintf (stderr, _("%s: %s is unchanged\n"), Prog, filename);
	} else {
		fprintf (stderr, _("%s: no changes\n"), Prog);
	}

	exit (ret);
}

static void cppwcopy (const char *file,
                      const char *in_file,
                      int (*file_lock) (void),
                      int (*file_unlock) (void))
{
	struct stat st1;
	FILE *f;
	char filenew[1024];

	snprintf (filenew, sizeof filenew, "%s.new", file);
	unlock = file_unlock;
	filename = file;
	filenewname = filenew;

	if (access (file, F_OK) != 0) {
		cppwexit (file, 1, 1);
	}
#ifdef WITH_SELINUX
	/* if SE Linux is enabled then set the context of all new files
	 * to be the context of the file we are editing */
	if (is_selinux_enabled () > 0) {
		security_context_t passwd_context=NULL;
		int ret = 0;
		if (getfilecon (file, &passwd_context) < 0) {
			cppwexit (_("Couldn't get file context"), errno, 1);
		}
		ret = setfscreatecon (passwd_context);
		freecon (passwd_context);
		if (0 != ret) {
			cppwexit (_("setfscreatecon () failed"), errno, 1);
		}
	}
#endif				/* WITH_SELINUX */
	if (file_lock () == 0) {
		cppwexit (_("Couldn't lock file"), 0, 5);
	}
	filelocked = true;

	/* file to copy has same owners, perm */
	if (stat (file, &st1) != 0) {
		cppwexit (file, 1, 1);
	}
	f = fopen (in_file, "r");
	if (NULL == f) {
		cppwexit (in_file, 1, 1);
	}
	if (create_copy (f, filenew, &st1) != 0) {
		cppwexit (_("Couldn't make copy"), errno, 1);
	}

	/* XXX - here we should check filenew for errors; if there are any,
	 * fail w/ an appropriate error code and let the user manually fix
	 * it. Use pwck or grpck to do the check.  - Stephen (Shamelessly
	 * stolen from '--marekm's comment) */

	if (rename (filenew, file) != 0) {
		fprintf (stderr, _("%s: can't copy %s: %s)\n"),
                         Prog, filenew, strerror (errno));
		cppwexit (NULL,0,1);
	}

#ifdef WITH_SELINUX
	/* unset the fscreatecon */
	if (is_selinux_enabled () > 0) {
		if (setfscreatecon (NULL)) {
		cppwexit (_("setfscreatecon() failed"), errno, 1);
		}
	}
#endif				/* WITH_SELINUX */

	(*file_unlock) ();
}

int main (int argc, char **argv)
{
	int flag;
	bool cpshadow = false;
	char *in_file;
	int e = E_USAGE;
	bool do_cppw = true;

	(void) setlocale (LC_ALL, "");
	(void) bindtextdomain (PACKAGE, LOCALEDIR);
	(void) textdomain (PACKAGE);

	Prog = Basename (argv[0]);
	if (strcmp (Prog, "cpgr") == 0) {
		do_cppw = false;
	}

	while ((flag = getopt (argc, argv, "ghps")) != EOF) {
		switch (flag) {
		case 'p':
			do_cppw = true;
			break;
		case 'g':
			do_cppw = false;
			break;
		case 's':
			cpshadow = true;
			break;
		case 'h':
			e = E_SUCCESS;
			/*pass through*/
		default:
			(void) fputs (_("Usage:\n\
`cppw <file>' copys over /etc/passwd   `cppw -s <file>' copys over /etc/shadow\n\
`cpgr <file>' copys over /etc/group    `cpgr -s <file>' copys over /etc/gshadow\n\
"), (E_SUCCESS != e) ? stderr : stdout);
			exit (e);
		}
	}

	if (argc != optind + 1) {
		cppwexit (_("wrong number of arguments, -h for usage"),0,1);
	}

	in_file = argv[optind];

	if (do_cppw) {
		if (cpshadow) {
			cppwcopy (SHADOW_FILE, in_file, spw_lock, spw_unlock);
		} else {
			cppwcopy (PASSWD_FILE, in_file, pw_lock, pw_unlock);
		}
	} else {
#ifdef SHADOWGRP
		if (cpshadow) {
			cppwcopy (SGROUP_FILE, in_file, sgr_lock, sgr_unlock);
		} else
#endif				/* SHADOWGRP */
		{
			cppwcopy (GROUP_FILE, in_file, gr_lock, gr_unlock);
		}
	}

	return 0;
}

