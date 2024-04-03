/*
 * kilall5.c	Kill all processes except processes that have the
 *		same session id, so that the shell that called us
 *		won't be killed. Typically used in shutdown scripts.
 *
 * pidof.c	Tries to get the pid of the process[es] named.
 *
 * Version:	2.81 31-Jul-2001 MvS
 *
 * Usage:	killall5 [-][signal]
 *		pidof [-s] [-o omitpid [-o omitpid]] program [program..]
 *
 * Authors:	Miquel van Smoorenburg, miquels@cistron.nl
 *
 *		Riku Meskanen, <mesrik@jyu.fi>
 *		- return all running pids of given program name
 *		- single shot '-s' option for backwards combatibility
 *		- omit pid '-o' option and %PPID (parent pid metavariable)
 *		- syslog() only if not a connected to controlling terminal
 *		- swapped out programs pids are caught now
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 1991-2001 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <syslog.h>
#include <getopt.h>
#include <stdarg.h>

char *Version = "@(#)killall5 2.81 31-Jul-2001 miquels@cistron.nl";

/* Info about a process. */
typedef struct _proc_
{
  char *fullname;	/* Name as found out from argv[0] */
  char *basename;	/* Only the part after the last / */
  char *statname;	/* the statname without braces    */
  ino_t ino;		/* Inode number			  */
  dev_t dev;		/* Device it is on		  */
  pid_t pid;		/* Process ID.			  */
  int sid;		/* Session ID.			  */
  struct _proc_ *next;	/* Pointer to next struct. 	  */
} PROC;

/* pid queue */
typedef struct _pidq_ {
  struct _pidq_ *front;
  struct _pidq_ *next;
  struct _pidq_ *rear;
  PROC		*proc;
} PIDQ;

/* List of processes. */
PROC *plist;

/* Did we stop a number of processes? */
int stopped;

int scripts_too = 0;

char *progname;	/* the name of the running program */
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
void nsyslog(int pri, char *fmt, ...);

/*
 *	Malloc space, barf if out of memory.
 */
void *xmalloc(int bytes)
{
	void *p;

	if ((p = malloc(bytes)) == NULL) {
		if (stopped) kill(-1, SIGCONT);
		nsyslog(LOG_ERR, "out of memory");
		exit(1);
	}
	return p;
}

/*
 *	See if the proc filesystem is there. Mount if needed.
 */
int mount_proc(void)
{
	struct stat st;
	int pid, wst;
	char *args[] = { "mount", "-t", "proc", "none", "/proc", NULL };
	int did_mount = 0;

	/* Stat /proc/version to see if /proc is mounted. */
	if (stat("/proc/version", &st) < 0) {

		/* It's not there, so mount it. */
		if ((pid = fork()) < 0) {
			nsyslog(LOG_ERR, "cannot fork");
			exit(1);
		}
		if (pid == 0) {
			/* Try a few mount binaries. */
			execv("/sbin/mount", args);
			execv("/etc/mount", args);
			execv("/bin/mount", args);

			/* Okay, I give up. */
			nsyslog(LOG_ERR, "cannot execute mount");
			exit(1);
		}
		/* Wait for child. */
		while (wait(&wst) != pid)
			;
		if (WEXITSTATUS(wst) != 0)
			nsyslog(LOG_ERR, "mount returned non-zero exit status");

		did_mount = 1;
	}

	/* See if mount succeeded. */
	if (stat("/proc/version", &st) < 0) {
		nsyslog(LOG_ERR, "/proc not mounted, failed to mount.");
		exit(1);
	}

	return did_mount;
}

/*
 *	Read the proc filesystem.
 */
int readproc()
{
	DIR *dir;
	struct dirent *d;
	char path[256];
	char buf[256];
	char *s, *q;
	FILE *fp;
	int pid, f;
	PROC *p, *n;
	struct stat st;
	int c;

	/* Open the /proc directory. */
	if ((dir = opendir("/proc")) == NULL) {
		nsyslog(LOG_ERR, "cannot opendir(/proc)");
		return -1;
	}

	/* Free the already existing process list. */
	n = plist;
	for (p = plist; n; p = n) {
		n = p->next;
		if (p->fullname) free(p->fullname);
		free(p);
	}
	plist = NULL;

	/* Walk through the directory. */
	while ((d = readdir(dir)) != NULL) {

		/* See if this is a process */
		if ((pid = atoi(d->d_name)) == 0) continue;

		/* Get a PROC struct . */
		p = (PROC *)xmalloc(sizeof(PROC));
		memset(p, 0, sizeof(PROC));

		/* Open the status file. */
		snprintf(path, sizeof(path), "/proc/%s/stat", d->d_name);

		/* Read SID & statname from it. */
 		if ((fp = fopen(path, "r")) != NULL) {
			buf[0] = 0;
			fgets(buf, 256, fp);

			/* See if name starts with '(' */
			s = buf;
			while (*s != ' ') s++;
			s++;
			if (*s == '(') {
				/* Read program name. */
				q = strrchr(buf, ')');
				if (q == NULL) {
					p->sid = 0;
					nsyslog(LOG_ERR,
					"can't get program name from %s\n",
						path);
					free(p);
					continue;
				}
				s++;
			} else {
				q = s;
				while (*q != ' ') q++;
			}
			*q++ = 0;
			while (*q == ' ') q++;
			p->statname = (char *)xmalloc(strlen(s)+1);
			strcpy(p->statname, s);

			/* This could be replaced by getsid(pid) */
			if (sscanf(q, "%*c %*d %*d %d", &p->sid) != 1) {
				p->sid = 0;
				nsyslog(LOG_ERR, "can't read sid from %s\n",
					path);
				free(p);
				continue;
			}
			fclose(fp);
		} else {
			/* Process disappeared.. */
			free(p);
			continue;
		}

		/* Now read argv[0] */
		snprintf(path, sizeof(path), "/proc/%s/cmdline", d->d_name);
		if ((fp = fopen(path, "r")) != NULL) {
			f = 0;
			while(f < 127 && (c = fgetc(fp)) != EOF && c)
				buf[f++] = c;
			buf[f++] = 0;
			fclose(fp);

			/* Store the name into malloced memory. */
			p->fullname = (char *)xmalloc(f);
			strcpy(p->fullname, buf);

			/* Get a pointer to the basename. */
			if ((p->basename = strrchr(p->fullname, '/')) != NULL)
				p->basename++;
			else
				p->basename = p->fullname;
		} else {
			/* Process disappeared.. */
			free(p);
			continue;
		}

		/* Try to stat the executable. */
		snprintf(path, sizeof(path), "/proc/%s/exe", d->d_name);
		if (stat(path, &st) == 0) {
			p->dev = st.st_dev;
			p->ino = st.st_ino;
		}

		/* Link it into the list. */
		p->next = plist;
		plist = p;
		p->pid = pid;
	}
	closedir(dir);

	/* Done. */
	return 0;
}

PIDQ *init_pid_q(PIDQ *q)
{
	q->front =  q->next = q->rear = NULL;
	q->proc = NULL;
	return q;
}

int empty_q(PIDQ *q)
{
	return (q->front == NULL);
}

int add_pid_to_q(PIDQ *q, PROC *p)
{
	PIDQ *tmp;

	tmp = (PIDQ *)xmalloc(sizeof(PIDQ));

	tmp->proc = p;
	tmp->next = NULL;

	if (empty_q(q)) {
		q->front = tmp;
		q->rear  = tmp;
	} else {
		q->rear->next = tmp;
		q->rear = tmp;
	}
	return 0;
}

PROC *get_next_from_pid_q(PIDQ *q)
{
	PROC *p;
	PIDQ *tmp = q->front;

	if (!empty_q(q)) {
		p = q->front->proc;
		q->front = tmp->next;
		free(tmp);
		return p;
	}

	return NULL;
}

/* Try to get the process ID of a given process. */
PIDQ *pidof(char *prog)
{
	struct stat st;
	int dostat = 0;
	PROC *p;
	PIDQ *q;
	char *s;
	int foundone = 0;
	int ok = 0;

	/* Try to stat the executable. */
	if (prog[0] == '/' && stat(prog, &st) == 0) dostat++;

	/* Get basename of program. */
	if ((s = strrchr(prog, '/')) == NULL)
		s = prog;
	else
		s++;

	q = (PIDQ *)xmalloc(sizeof(PIDQ));
	q = init_pid_q(q);

	/* First try to find a match based on dev/ino pair. */
	if (dostat) {
		for (p = plist; p; p = p->next) {
			if (p->dev == st.st_dev && p->ino == st.st_ino) {
				add_pid_to_q(q, p);
				foundone++;
			}
		}
	}

	/* If we didn't find a match based on dev/ino, try the name. */
	if (!foundone) {
		for (p = plist; p; p = p->next) {
			ok = 0;

			ok += (strcmp(p->fullname, prog) == 0);
			ok += (strcmp(p->basename, s) == 0);

			if (p->fullname[0] == 0 ||
			    strchr(p->fullname, ' ') ||
			    scripts_too)
				ok += (strcmp(p->statname, s) == 0);

			if (ok) add_pid_to_q(q, p);
		}
	}

	 return q;
}

/* Give usage message and exit. */
void usage(void)
{
	nsyslog(LOG_ERR, "only one argument, a signal number, allowed");
	closelog();
	exit(1);
}

/* write to syslog file if not open terminal */
#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
void nsyslog(int pri, char *fmt, ...)
{
	va_list  args;

	va_start(args, fmt);

	if (ttyname(0) == NULL) {
		vsyslog(pri, fmt, args);
	} else {
		fprintf(stderr, "%s: ",progname);
		vfprintf(stderr, fmt, args);
		fprintf(stderr, "\n");
	}

	va_end(args);
}

#define PIDOF_SINGLE	0x01
#define PIDOF_OMIT	0x02

#define PIDOF_OMITSZ	5

/*
 *	Pidof functionality.
 */
int main_pidof(int argc, char **argv)
{
	PROC *p;
	PIDQ *q;
	int f;
	int first = 1;
	int i,oind, opt, flags = 0;
	pid_t opid[PIDOF_OMITSZ], spid;

	for (oind = PIDOF_OMITSZ-1; oind > 0; oind--)
		opid[oind] = 0;
	opterr = 0;

	while ((opt = getopt(argc,argv,"ho:sx")) != EOF) switch (opt) {
		case '?':
			nsyslog(LOG_ERR,"invalid options on command line!\n");
			closelog();
			exit(1);
		case 'o':
			if (oind >= PIDOF_OMITSZ -1) {
				nsyslog(LOG_ERR,"omit pid buffer size %d "
					"exceeded!\n", PIDOF_OMITSZ);
				closelog();
				exit(1);
			}
			if (strcmp("%PPID",optarg) == 0)
				opid[oind] = getppid();
			else if ((opid[oind] = atoi(optarg)) < 1) {
				nsyslog(LOG_ERR,
					"illegal omit pid value (%s)!\n",
					optarg);
				closelog();
				exit(1);
			}
			oind++;
			flags |= PIDOF_OMIT;
			break;
		case 's':
			flags |= PIDOF_SINGLE;
			break;
		case 'x':
			scripts_too++;
			break;
		default:
			/* Nothing */
			break;
	}
	argc -= optind;
	argv += optind;

	/* Print out process-ID's one by one. */
	readproc();
	for(f = 0; f < argc; f++) {
		if ((q = pidof(argv[f])) != NULL) {
			spid = 0;
			while ((p = get_next_from_pid_q(q))) {
				if (flags & PIDOF_OMIT) {
					for (i = 0; i < oind; i++)
						if (opid[i] == p->pid)
							break;
					/*
					 *	On a match, continue with
					 *	the for loop above.
					 */
					if (i < oind)
						continue;
				}
				if (flags & PIDOF_SINGLE) {
					if (spid)
						continue;
					else
						spid = 1;
				}
				if (!first)
					printf(" ");
				printf("%d", p->pid);
				first = 0;
			}
		}
	}
	printf("\n");
	closelog();
	return(first ? 1 : 0);
}



/* Main for either killall or pidof. */
int main(int argc, char **argv)
{
	PROC *p;
	int pid, sid = -1;
	int sig = SIGKILL;

	/* Get program name. */
	if ((progname = strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* Now connect to syslog. */
	openlog(progname, LOG_CONS|LOG_PID, LOG_DAEMON);

	/* First get the /proc filesystem online. */
	mount_proc();

	/* Were we called as 'pidof' ? */
	if (strcmp(progname, "pidof") == 0)
		return main_pidof(argc, argv);

	/* Right, so we are "killall". */
	if (argc > 1) {
		if (argc != 2) usage();
		if (argv[1][0] == '-') (argv[1])++;
		if ((sig = atoi(argv[1])) <= 0 || sig > 31) usage();
	}

	/*
	 *	Ignoring SIGKILL and SIGSTOP do not make sense, but
	 *	someday kill(-1, sig) might kill ourself if we don't
	 *	do this. This certainly is a valid concern for SIGTERM-
	 *	Linux 2.1 might send the calling process the signal too.
	 */
	signal(SIGTERM, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGKILL, SIG_IGN);

	/* Now stop all processes. */
	kill(-1, SIGSTOP);
	stopped = 1;

	/* Find out our own 'sid'. */
	if (readproc() < 0) {
		kill(-1, SIGCONT);
		exit(1);
	}

	pid = getpid();
	for (p = plist; p; p = p->next)
		if (p->pid == pid) {
			sid = p->sid;
			break;
		}

	/* Now kill all processes except our session. */
	for (p = plist; p; p = p->next)
		if (p->pid != pid && p->sid != sid)
			kill(p->pid, sig);

	/* And let them continue. */
	kill(-1, SIGCONT);

	/* Done. */
	closelog();

	return 0;
}
