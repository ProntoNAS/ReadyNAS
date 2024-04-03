/*
 * <pidfile.c>
 *
 * Copyright (C) 1999, 2000 Daniel Kobras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * $Id: pidfile.c,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
 *
 */

#include "pidfile.h"
#include "bug.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

struct pidfile_s {
	char	*pidfile;
};

/* 
 * Check for another noflushd already running at <pid>.
 * XXX: This routine is hacky and has quite some evil assumptions
 *      but hopefully will do for a while.
 */

static pid_t check_pid(pid_t pid)
{
	int fd = -1;
	FILE *fp = NULL;
	pid_t ret = 0;
	char buf[1024], *start;

	/* Danger, will only work for pid less than 10^1017! */
	snprintf(buf, 1024, "/proc/%u", pid);
	fd = open(".", O_RDONLY);
	if (chdir(buf) == -1 ||
	    !(fp = fopen("status", "r")))
		goto _out;
	
	while (fgets(buf, 1024, fp) && strncmp(buf, "Name:", 5));
	
	/* Huh!? No Name: field!? */
	if (strncmp(buf, "Name:", 5))
		goto _out;
	
	start = buf + 5;
	while (*start && isspace(*start))
		start++;

	/* Aieee, start still contains trailing newline. Ugly. */
	if (!strcmp(start, "noflushd\n"))
		ret = pid;

_out:
	if (fp)
		fclose(fp);
	if (fd != -1)
		fchdir(fd);
	else
		chdir("/");

	return ret;

}

/* Take pid from pid file (as in <fd>) and check if another noflushd
 * is running with this pid. If yes and fail is non-zero, terminate.
 */

static pid_t check_pid_file(int fd, int fail)
{
	FILE *fp;
	pid_t pid;

	if (!(fp = fdopen(fd, "r"))) {
		ERR("Failed to fdopen");
		return 0;
	}
	if (1 != fscanf(fp, "%u", &pid))
		return 0;
	if (fail && check_pid(pid)) {
		DEBUG("Another noflushd is running at pid %u. "
		      "This computer's too small for two of us!",
			pid);
		exit(0);
	}
	return pid;
}

/*
 * Save our pid to pidfile. Terminate if file exists and
 * another noflushd is running with indicated pid.
 */

pidfile_t pidfile_get(char *pidfile)
{
	int fd = -1;
	FILE *fp = NULL;
	pidfile_t pf = NULL;

	pf = malloc(sizeof(struct pidfile_s));
	if (!pf)
		goto _err;

	/* O_EXCL has a race via NFS. But then /var/run really shouldn't be
	 * shared anyway, so let's keep it simple. This is more of a 
	 * convenience check after all. Nothing too crucial.
	 */
	if ((fd = open(pidfile, O_CREAT | O_EXCL | O_RDWR, 
	               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
		if (errno == EEXIST) {
			fd = open(pidfile, O_CREAT | O_RDWR,
			          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			check_pid_file(fd, 1);
		} else {
			ERR("Cannot open pid file. Proceeding anyway. "
			     "Wish me luck");
			goto _err;
		}
	}
	/* Now it's okay to truncate the file. */
	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);
	if (!(fp = fdopen(fd, "w+"))) {
		ERR("fdopen failed");
		goto _err;
	}
	fprintf(fp, "%u\n", getpid());
	fflush(fp);
	close(fd);
	pf->pidfile=strdup(pidfile);
	return pf;
_err:
	if (fd != -1)
		close(fd);
	free(pf);
	return NULL;
	
}

/*
 * Remove pidfile.
 */

void pidfile_drop(pidfile_t pf)
{
	int fd;

	if (!pf)
		BUG("Invalid pidfile handle");

	if (!pf->pidfile || (fd = open(pf->pidfile, O_RDONLY)) == -1) {
		DEBUG("Cannot open pidfile.");
		goto _out;
	}
	if (check_pid_file(fd, 0) == getpid())
		unlink(pf->pidfile);
	else 
		DEBUG("Found alien pidfile, not removed.");
	
	close(fd);

_out:
	free(pf->pidfile);
	free(pf);
}
