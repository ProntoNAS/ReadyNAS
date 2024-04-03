/*
 * dowall.c	Write to all users on the system.
 *
 * Author:	Miquel van Smoorenburg, miquels@cistron.nl
 * 
 * Version:	@(#)dowall.c  2.84-3  27-May-2002  miquels@cistron.nl
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 1991-2002 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <utmp.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf jbuf;

/* display pid of rpc.rwalld if wall() is called with 'remote' non-zero */
#define RWALLDPID

/*
 *	Alarm handler
 */
/*ARGSUSED*/
static void handler(int arg)
{
	longjmp(jbuf, 1);
}


/*
 *	Print a text, escape all characters not in Latin-1.
 */
void feputs(char *line, FILE *fp)
{
	unsigned char *p;

	for (p = (unsigned char *)line; *p; p++) {
		if (strchr("\t\r\n", *p) ||
		    (*p >= 32 && *p <= 127) || (*p >= 160)) {
			fputc(*p, fp);
		} else {
			fprintf(fp, "^%c", (*p & 0x1f) + 'A' - 1);
		}
	}
	fflush(fp);
}


/*
 *	Wall function.
 */
void wall(char *text, int fromshutdown, int remote)
{
	FILE *tp;
	char line[81];
	char term[UT_LINESIZE+6];
	static char *user, ttynm[32], *date;
	char uidbuf[32];
	static int fd, init = 0;
	struct passwd *pwd;
	struct sigaction sa;
	char *tty, *p;
	time_t t;
	struct utmp *utmp;
	uid_t uid;

	setutent();

	if (init == 0) {
		uid = getuid();
		pwd = getpwuid(uid);
		sprintf (uidbuf, "uid %d", (int)uid);
		/*
		 *	display uid if no corresponding username found in
		 *	password file. superuser (uid 0) is always "root",
		 *	not "uid 0".
		 */
		user = pwd ? pwd->pw_name : (uid == 0 ? "root" : uidbuf);
		if ((tty = ttyname(0)) != NULL) {
			if (strncmp(tty, "/dev/", 5) == 0)
				tty += 5;
			sprintf(ttynm, "(%.28s) ", tty);	
		} else
			ttynm[0] = 0;
		init++;
		sa.sa_handler = handler;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaction(SIGALRM, &sa, NULL);
	}
	
	/* Get the time */
	time(&t);
	date = ctime(&t);
	for(p = date; *p && *p != '\n'; p++)
		;
	*p = 0;
	
	if (remote) {
		snprintf(line, sizeof(line),
			"\007\r\nRemote broadcast message (%s):\r\n\r\n",
			date);
	} else {
		snprintf(line, sizeof(line),
			"\007\r\nBroadcast message from %s %s(%s):\r\n\r\n",
			user, ttynm, date);
	}

	/*
	 *	Fork to avoid us hanging in a write()
	 */
	if (fork() != 0)
		return;
	
	siginterrupt (SIGALRM, 1);
	while ((utmp = getutent()) != NULL) {
		if(utmp->ut_type != USER_PROCESS ||
		   utmp->ut_user[0] == 0) continue;
		term[sizeof(term) - 1] = 0;
		if (strncmp(utmp->ut_line, "/dev/", 5) == 0)
			strncpy(term, utmp->ut_line, sizeof(term)-1);
		else
			snprintf(term, sizeof(term), "/dev/%s",
				utmp->ut_line);
		if (strstr(term, "/../")) continue;

		/*
		 *	Sometimes the open/write hangs in spite of the O_NDELAY
		 */
		alarm(2);

		/*
		 *	Open it non-delay
		 */
		if (sigsetjmp(jbuf, 1) == 0 &&
		    (fd = open(term, O_WRONLY|O_NDELAY|O_NOCTTY)) > 0) {
			if (isatty(fd) && (tp = fdopen(fd, "w")) != NULL) {
				fputs(line, tp);
				feputs(text, tp);
				fclose(tp);
			} else
				close(fd);
			fd = -1;
			alarm(0);
		}
		if (fd >= 0) close(fd);
		alarm(0);
	}
	endutent();
	exit(0);
}

