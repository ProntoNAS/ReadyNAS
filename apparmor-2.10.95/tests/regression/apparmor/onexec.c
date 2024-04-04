/*
 *	Copyright (C) 2002-2005 Novell/SUSE
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2 of the
 *	License.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/unistd.h>

#include <sys/apparmor.h>
#include "changehat.h"

int main(int argc, char *argv[])
{
	int rc = 0;

	extern char **environ;

	if (argc < 3){
		fprintf(stderr, "usage: %s profile executable args\n",
			argv[0]);
		return 1;
	}

	/* change profile if profile name != nochange */
	if (strcmp(argv[1], "nochange") != 0){
		rc = aa_change_onexec(argv[1]);
		if (rc == -1){
			fprintf(stderr, "FAIL: change_onexec %s failed - %s\n",
				argv[1], strerror(errno));
			exit(errno);
		}
	}

	/* stop after onexec and wait to for continue before exec so
	 * caller can introspect task */
	rc = kill(getpid(), SIGSTOP);
	if (rc == -1){
		fprintf(stderr, "FAIL: signal to self failed - %s\n",
			strerror(errno));
		exit(errno);
	}

	(void)execve(argv[2], &argv[2], environ);
	/* exec failed, kill outselves to flag parent */

	rc = errno;

	fprintf(stderr, "FAIL: exec to '%s' failed\n", argv[2]);

	return rc;
}
