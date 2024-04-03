/*
 * last.c	Re-implementation of the 'last' command, this time
 *		for Linux. Yes I know there is BSD last, but I
 *		just felt like writing this. No thanks :-).
 *		Also, this version gives lots more info (especially with -x)
 *
 * Author:	Miquel van Smoorenburg, miquels@cistron.nl
 *
 * Version:	@(#)last  2.79  13-Jun-2001  miquels@cistron.nl
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
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <utmp.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "oldutmp.h"

#ifndef SHUTDOWN_TIME
#  define SHUTDOWN_TIME 254
#endif

char *Version = "@(#) last 2.79 11-Sep-2000 miquels";

#define CHOP_DOMAIN	0	/* Define to chop off local domainname. */
#define NEW_UTMP	1	/* Fancy & fast utmp read code. */
#define UCHUNKSIZE	16384	/* How much we read at once. */

/* Double linked list of struct utmp's */
struct utmplist {
  struct utmp ut;
  struct utmplist *next;
  struct utmplist *prev;
};
struct utmplist *utmplist = NULL;

/* Types of listing */
#define R_CRASH		1 /* No logout record, system boot in between */
#define R_DOWN		2 /* System brought down in decent way */
#define R_NORMAL	3 /* Normal */
#define R_NOW		4 /* Still logged in */
#define R_REBOOT	5 /* Reboot record. */
#define R_PHANTOM	6 /* No logout record but session is stale. */
#define R_TIMECHANGE	7 /* NEW_TIME or OLD_TIME */

/* Global variables */
int maxrecs = 0;	/* Maximum number of records to list. */
int recsdone = 0;	/* Number of records listed */
int showhost = 1;	/* Show hostname too? */
int altlist = 0;	/* Show hostname at the end. */
int usedns = 0;		/* Use DNS to lookup the hostname. */
int useip = 0;		/* Print IP address in number format */
int oldfmt = 0;		/* Use old libc5 format? */
char **show = NULL;	/* What do they want us to show */
char *ufile;		/* Filename of this file */
time_t lastdate;	/* Last date we've seen */
char *progname;		/* Name of this program */
#if CHOP_DOMAIN
char hostname[256];	/* For gethostbyname() */
char *domainname;	/* Our domainname. */
#endif

/*
 *	Convert old utmp format to new.
 */
void uconv(struct oldutmp *oldut, struct utmp *utn)
{
	memset(utn, 0, sizeof(struct utmp));
	utn->ut_type = oldut->ut_type;
	utn->ut_pid  = oldut->ut_pid;
	utn->ut_time = oldut->ut_oldtime;
	utn->ut_addr = oldut->ut_oldaddr;
	strncpy(utn->ut_line, oldut->ut_line, OLD_LINESIZE);
	strncpy(utn->ut_user, oldut->ut_user, OLD_NAMESIZE);
	strncpy(utn->ut_host, oldut->ut_host, OLD_HOSTSIZE);
}

#if NEW_UTMP
/*
 *	Read one utmp entry, return in new format.
 *	Automatically reposition file pointer.
 */
int uread(FILE *fp, struct utmp *u, int *quit)
{
	static int utsize;
	static char buf[UCHUNKSIZE];
	char tmp[1024];
	static off_t fpos;
	static int bpos;
	struct oldutmp uto;
	int r;
	off_t o;

	if (quit == NULL && u != NULL) {
		/*
		 *	Normal read.
		 */
		if (oldfmt) {
			r = fread(&uto, sizeof(uto), 1, fp);
			uconv(&uto, u);
		} else
			r = fread(u, sizeof(struct utmp), 1, fp);
		return r;
	}

	if (u == NULL) {
		/*
		 *	Initialize and position.
		 */
		utsize = oldfmt ? sizeof(uto) : sizeof(struct utmp);
		fseek(fp, 0L, SEEK_END);
		fpos = ftell(fp);
		if (fpos == 0)
			return 0;
		o = ((fpos - 1) / UCHUNKSIZE) * UCHUNKSIZE;
		if (fseek(fp, o, SEEK_SET) < 0) {
			fprintf(stderr, "%s: seek failed!\n", progname);
			return 0;
		}
		bpos = fpos - o;
		if (fread(buf, bpos, 1, fp) != 1) {
			fprintf(stderr, "%s: read failed!\n", progname);
			return 0;
		}
		fpos = o;
		return 1;
	}

	/*
	 *	Read one struct. From the buffer if possible.
	 */
	bpos -= utsize;
	if (bpos >= 0) {
		if (oldfmt)
			uconv((struct oldutmp *)(buf + bpos), u);
		else
			memcpy(u, buf + bpos, sizeof(struct utmp));
		return 1;
	}

	/*
	 *	Oops we went "below" the buffer. We should be able to
	 *	seek back UCHUNKSIZE bytes.
	 */
	fpos -= UCHUNKSIZE;
	if (fpos < 0)
		return 0;

	/*
	 *	Copy whatever is left in the buffer.
	 */
	memcpy(tmp + (-bpos), buf, utsize + bpos);
	if (fseek(fp, fpos, SEEK_SET) < 0) {
		perror("fseek");
		return 0;
	}

	/*
	 *	Read another UCHUNKSIZE bytes.
	 */
	if (fread(buf, UCHUNKSIZE, 1, fp) != 1) {
		perror("fread");
		return 0;
	}

	/*
	 *	The end of the UCHUNKSIZE byte buffer should be the first
	 *	few bytes of the current struct utmp.
	 */
	memcpy(tmp, buf + UCHUNKSIZE + bpos, -bpos);
	bpos += UCHUNKSIZE;

	if (oldfmt)
		uconv((struct oldutmp *)tmp, u);
	else
		memcpy(u, tmp, sizeof(struct utmp));

	return 1;
}

#else /* NEW_UTMP */

/*
 *	Read one utmp entry, return in new format.
 *	Automatically reposition file pointer.
 */
int uread(FILE *fp, struct utmp *u, int *quit)
{
	struct oldutmp uto;
	int r;

	if (u == NULL) {
		r = oldfmt ? sizeof(struct oldutmp) : sizeof(struct utmp);
		fseek(fp, -1L * r, SEEK_END);
		return 1;
	}

	if (!oldfmt) {
		r = fread(u, sizeof(struct utmp), 1, fp);
		if (r == 1) {
			if (fseek(fp, -2L * sizeof(struct utmp), SEEK_CUR) < 0)
				if (quit) *quit = 1;
		}
		return r;
	}
	r = fread(&uto, sizeof(struct oldutmp), 1, fp);
	if (r == 1) {
		if (fseek(fp, -2L * sizeof(struct oldutmp), SEEK_CUR) < 0)
			if (quit) *quit = 1;
		uconv(&uto, u);
	}

	return r;
}
#endif

/*
 *	Try to be smart about the location of the BTMP file
 */
#ifndef BTMP_FILE
#define BTMP_FILE getbtmp()
char *getbtmp()
{
	static char btmp[128];
	char *p;

	strcpy(btmp, WTMP_FILE);
	if ((p = strrchr(btmp, '/')) == NULL)
		p = btmp;
	else
		p++;
	*p = 0;
	strcat(btmp, "btmp");
	return btmp;
}
#endif

/*
 *	Print a short date.
 */
char *showdate()
{
	char *s = ctime(&lastdate);
	s[16] = 0;
	return s;
}

/*
 *	SIGINT handler
 */
void int_handler()
{
	printf("Interrupted %s\n", showdate());
	exit(1);
}

/*
 *	SIGQUIT handler
 */
void quit_handler()
{
	printf("Interrupted %s\n", showdate());
	signal(SIGQUIT, quit_handler);
}

/*
 *	Get the basename of a filename
 */
char *mybasename(char *s)
{
	char *p;

	if ((p = strrchr(s, '/')) != NULL)
		p++;
	else
		p = s;
	return p;
}

/*
 *	Lookup a host with DNS.
 */
int dns_lookup(char *result, int size, char *org, unsigned int ip)
{
	struct hostent *h;

	/*
	 *	Try to catch illegal IP numbers
	 */
	if (ip == 0 || (int)ip == -1 || (ip >> 24) == 0 || (ip & 255) == 0) {
		if (size > UT_HOSTSIZE) size = UT_HOSTSIZE+1;
		strncpy(result, org, size - 1);
		result[size-1] = 0;
		return 0;
	}

	if ((h = gethostbyaddr((char *)&ip, 4, AF_INET)) == NULL) {
		strncpy(result, inet_ntoa(*(struct in_addr *)&ip), size);
		result[size-1] = 0;
		return 0;
	}
	strncpy(result, h->h_name, size-1);
	result[size-1] = 0;

	return 0;
}

/*
 *	Show one line of information on screen
 */
int list(struct utmp *p, time_t t, int what)
{
	struct in_addr in;
	char logintime[32];
	char logouttime[32];
	char length[32];
	char final[128];
	char utline[UT_LINESIZE+1];
	char domain[256];
	time_t secs, tmp;
	int mins, hours, days;
	char *s, **walk;

	/*
	 *	uucp and ftp have special-type entries
	 */
	strncpy(utline, p->ut_line, UT_LINESIZE);
	utline[UT_LINESIZE - 1] = 0;
	if (strncmp(utline, "ftp", 3) == 0 && isdigit(utline[3]))
		utline[3] = 0;
	if (strncmp(utline, "uucp", 4) == 0 && isdigit(utline[4]))
		utline[4] = 0;

	/*
	 *	Is this something we wanna show?
	 */
	if (show) {
		for (walk = show; *walk; walk++) {
			if (strncmp(p->ut_name, *walk, UT_NAMESIZE) == 0 ||
			    strcmp(utline, *walk) == 0 ||
			    (strncmp(utline, "tty", 3) == 0 &&
			     strcmp(utline + 3, *walk) == 0)) break;
		}
		if (*walk == NULL) return 0;
	}

	/*
	 *	Calculate times
	 */
	tmp = (time_t)p->ut_time;
	strcpy(logintime, ctime(&tmp));
	logintime[16] = 0;
	sprintf(logouttime, "- %s", ctime(&t) + 11);
	logouttime[7] = 0;
	secs = t - p->ut_time;
	mins  = (secs / 60) % 60;
	hours = (secs / 3600) % 24;
	days  = secs / 86400;
	if (days)
		sprintf(length, "(%d+%02d:%02d)", days, hours, mins);
	else
		sprintf(length, " (%02d:%02d)", hours, mins);

	switch(what) {
		case R_CRASH:
			sprintf(logouttime, "- crash");
			break;
		case R_DOWN:
			sprintf(logouttime, "- down ");
			break;
		case R_NOW:
			length[0] = 0;
			sprintf(logouttime, "  still");
			sprintf(length, "logged in");
			break;
		case R_PHANTOM:
			length[0] = 0;
			sprintf(logouttime, "   gone");
			sprintf(length, "- no logout");
			break;
		case R_REBOOT:
			logouttime[0] = 0;      /* Print machine uptime */
			break;
		case R_TIMECHANGE:
			logouttime[0] = 0;
			length[0] = 0;
			break;
		case R_NORMAL:
			break;
 	}

	/*
	 *	Look up host with DNS if needed.
	 */
	if (usedns)
		dns_lookup(domain, sizeof(domain), p->ut_host, p->ut_addr);
	if (useip) {
		in.s_addr = p->ut_addr;
		strcpy(domain, inet_ntoa(in));
	} else {
		strncpy(domain, p->ut_host, UT_HOSTSIZE);
		domain[UT_HOSTSIZE-1] = 0;
	}

	if (showhost) {
#if CHOP_DOMAIN
		/*
		 *	See if this is in our domain.
		 */
		if (!usedns && (s = strchr(p->ut_host, '.')) != NULL &&
		     strcmp(s + 1, domainname) == 0) *s = 0;
#endif
		if (!altlist) {
			snprintf(final, sizeof(final),
				"%-8.8s %-12.12s %-16.16s "
				"%-16.16s %-7.7s %-12.12s\n",
				p->ut_name, utline,
				domain, logintime, logouttime, length);
		} else {
			snprintf(final, sizeof(final), 
				"%-8.8s %-12.12s %-16.16s %-7.7s %-12.12s %s\n",
				p->ut_name, utline,
				logintime, logouttime, length, domain);
		}
	} else
		snprintf(final, sizeof(final),
			"%-8.8s %-12.12s %-16.16s %-7.7s %-12.12s\n",
			p->ut_name, utline,
			logintime, logouttime, length);

	/*
	 *	Print out "final" string safely.
	 */
	for (s = final; *s; s++) {
		if (*s == '\n' || (*s >= 32 && (unsigned char)*s <= 126))
			putchar(*s);
		else
			putchar('*');
	}

	recsdone++;
	if (maxrecs && recsdone >= maxrecs)
		return 1;

	return 0;
}


/*
 *	show usage
 */
void usage(char *s)
{
	fprintf(stderr, "Usage: %s [-num | -n num] [-f file] "
			"[-R] [-x] [-o] [username..] [tty..]\n", s);
	exit(1);
}

int main(int argc, char **argv)
{
  FILE *fp;		/* Filepointer of wtmp file */

  struct utmp ut;	/* Current utmp entry */
  struct utmp oldut;	/* Old utmp entry to check for duplicates */
  struct utmplist *p;	/* Pointer into utmplist */
  struct utmplist *next;/* Pointer into utmplist */

  time_t lastboot = 0;  /* Last boottime */
  time_t lastrch = 0;	/* Last run level change */
  time_t lastdown;	/* Last downtime */
  time_t begintime;	/* When wtmp begins */
  int whydown = 0;	/* Why we went down: crash or shutdown */

  int c, x;		/* Scratch */
  struct stat st;	/* To stat the [uw]tmp file */
  int quit = 0;		/* Flag */
  int down = 0;		/* Down flag */
  int lastb = 0;	/* Is this 'lastb' ? */
  int extended = 0;	/* Lots of info. */
  char *altufile = NULL;/* Alternate wtmp */

  progname = mybasename(argv[0]);

  /* Process the arguments. */
  while((c = getopt(argc, argv, "f:n:Rxadio0123456789")) != EOF)
    switch(c) {
	case 'R':
		showhost = 0;
		break;
	case 'x':
		extended = 1;
		break;
	case 'n':
		maxrecs = atoi(optarg);
		break;
	case 'o':
		oldfmt = 1;
		break;
	case 'f':
		if((altufile = malloc(strlen(optarg)+1)) == NULL) {
			fprintf(stderr, "%s: out of memory\n",
				progname);
			exit(1);
		}
		strcpy(altufile, optarg);
		break;
	case 'd':
		usedns++;
		break;
	case 'i':
		useip++;
		break;
	case 'a':
		altlist++;
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		maxrecs = 10*maxrecs + c - '0';
		break;
	default:
		usage(progname);
		break;
    }
  if (optind < argc) show = argv + optind;

  /*
   *	Which file do we want to read?
   */
  if (strcmp(progname, "lastb") == 0) {
	ufile = BTMP_FILE;
	lastb = 1;
  } else
	ufile = WTMP_FILE;
  if (altufile)
	ufile = altufile;
  time(&lastdown);
  lastrch = lastdown;

  /*
   *	Fill in 'lastdate'
   */
  lastdate = lastdown;

#if CHOP_DOMAIN
  /*
   *	Find out domainname.
   *
   *	This doesn't work on modern systems, where only a DNS
   *	lookup of the result from hostname() will get you the domainname.
   *	Remember that domainname() is the NIS domainname, not DNS.
   *	So basically this whole piece of code is bullshit.
   */
  hostname[0] = 0;
  (void) gethostname(hostname, sizeof(hostname));
  if ((domainname = strchr(hostname, '.')) != NULL) domainname++;
  if (domainname == NULL || domainname[0] == 0) {
	hostname[0] = 0;
	(void) getdomainname(hostname, sizeof(hostname));
	hostname[sizeof(hostname) - 1] = 0;
	domainname = hostname;
	if (strcmp(domainname, "(none)") == 0 || domainname[0] == 0)
		domainname = NULL;
  }
#endif

  /*
   *	Install signal handlers
   */
  signal(SIGINT, int_handler);
  signal(SIGQUIT, quit_handler);

  /*
   *	Open the utmp file
   */
  if ((fp = fopen(ufile, "r")) == NULL) {
	x = errno;
	fprintf(stderr, "%s: %s: %s\n", progname, ufile, strerror(errno));
	if (altufile == NULL && x == ENOENT)
		fprintf(stderr, "Perhaps this file was removed by the "
			"operator to prevent logging %s info.\n", progname);
	exit(1);
  }

  /*
   *	Optimize the buffer size.
   */
  setvbuf(fp, NULL, _IOFBF, UCHUNKSIZE);

  /*
   *	Read first structure to capture the time field
   */
  if (uread(fp, &ut, NULL) == 1)
	begintime = ut.ut_time;
  else {
  	fstat(fileno(fp), &st);
	begintime = st.st_ctime;
	quit = 1;
  }

  /*
   *	Go to end of file minus one structure
   *	and/or initialize utmp reading code.
   */
  uread(fp, NULL, NULL);

  /*
   *	Read struct after struct backwards from the file.
   */
  while(!quit) {

	if (uread(fp, &ut, &quit) != 1)
		break;

	if (memcmp(&ut, &oldut, sizeof(struct utmp)) == 0) continue;
	memcpy(&oldut, &ut, sizeof(struct utmp));
	lastdate = ut.ut_time;

  	if (lastb) {
  		quit = list(&ut, ut.ut_time, R_NORMAL);
  		continue;
  	}

	/*
	 *	Set ut_type to the correct type.
	 */
	if (strncmp(ut.ut_line, "~", 1) == 0) {
		if (strncmp(ut.ut_user, "shutdown", 8) == 0)
			ut.ut_type = SHUTDOWN_TIME;
		else if (strncmp(ut.ut_user, "reboot", 6) == 0)
			ut.ut_type = BOOT_TIME;
		else if (strncmp(ut.ut_user, "runlevel", 7) == 0)
			ut.ut_type = RUN_LVL;
	}
#if 1 /*def COMPAT*/
	/*
	 *	For stupid old applications that don't fill in
	 *	ut_type correctly.
	 */
	else {
		if (ut.ut_type != DEAD_PROCESS &&
		    ut.ut_name[0] && ut.ut_line[0] &&
		    strcmp(ut.ut_name, "LOGIN") != 0)
			ut.ut_type = USER_PROCESS;
		/*
		 *	Even worse, applications that write ghost
		 *	entries: ut_type set to USER_PROCESS but
		 *	empty ut_name...
		 */
		if (ut.ut_name[0] == 0)
			ut.ut_type = DEAD_PROCESS;

		/*
		 *	Clock changes.
		 */
		if (strcmp(ut.ut_name, "date") == 0) {
			if (ut.ut_line[0] == '|') ut.ut_type = OLD_TIME;
			if (ut.ut_line[0] == '{') ut.ut_type = NEW_TIME;
		}
	}
#endif

	switch (ut.ut_type) {
		case SHUTDOWN_TIME:
			if (extended) {
				strcpy(ut.ut_line, "system down");
				quit = list(&ut, lastdown, R_NORMAL);
			}
			lastdown = lastrch = ut.ut_time;
			down = 1;
			break;
		case OLD_TIME:
		case NEW_TIME:
			if (extended) {
				strcpy(ut.ut_line,
				ut.ut_type == NEW_TIME ? "new time" :
					"old time");
				quit = list(&ut, lastdown, R_TIMECHANGE);
			}
			break;
		case BOOT_TIME:
			strcpy(ut.ut_line, "system boot");
			quit = list(&ut, lastdown, R_REBOOT);
			down = 1;
			break;
		case RUN_LVL:
			x = ut.ut_pid & 255;
			if (extended) {
				sprintf(ut.ut_line, "(to lvl %c)", x);
				quit = list(&ut, lastrch, R_NORMAL);
			}
			if (x == '0' || x == '6') {
				lastdown = ut.ut_time;
				down = 1;
				ut.ut_type = SHUTDOWN_TIME;
			}
			lastrch = ut.ut_time;
			break;

		case USER_PROCESS:
			/*
			 *	This was a login - show the first matching
			 *	logout record and delete all records with
			 *	the same ut_line.
			 */
			c = 0;
			for (p = utmplist; p; p = next) {
				next = p->next;
				if (strncmp(p->ut.ut_line, ut.ut_line,
				    UT_LINESIZE) == 0) {
					/* Show it */
					if (c == 0) {
						quit = list(&ut, p->ut.ut_time,
							R_NORMAL);
						c = 1;
					}
					if (p->next) p->next->prev = p->prev;
					if (p->prev)
						p->prev->next = p->next;
					else
						utmplist = p->next;
					free(p);
				}
			}
			/*
			 *	Not found? Then crashed, down, still
			 *	logged in, or missing logout record.
			 */
			if (c == 0) {
				if (lastboot == 0) {
					c = R_NOW;
					/* Is process still alive? */
					if (ut.ut_pid > 0 &&
					    kill(ut.ut_pid, 0) != 0 &&
					    errno == ESRCH)
						c = R_PHANTOM;
				} else
					c = whydown;
				quit = list(&ut, lastboot, c);
			}
			/* FALLTHRU */

		case DEAD_PROCESS:
			/*
			 *	Just store the data if it is
			 *	interesting enough.
			 */
			if (ut.ut_line[0] == 0)
				break;
			if ((p = malloc(sizeof(struct utmplist))) == NULL) {
				fprintf(stderr, "%s: out of memory\n",
					progname);
				exit(1);
			}
			memcpy(&p->ut, &ut, sizeof(struct utmp));
			p->next  = utmplist;
			p->prev  = NULL;
			if (utmplist) utmplist->prev = p;
			utmplist = p;
			break;

	}
	/*
	 *	If we saw a shutdown/reboot record we can remove
	 *	the entire current utmplist.
	 */
	if (down) {
		lastboot = ut.ut_time;
		whydown = (ut.ut_type == SHUTDOWN_TIME) ? R_DOWN : R_CRASH;
		for (p = utmplist; p; p = next) {
			next = p->next;
			free(p);
		}
		utmplist = NULL;
		down = 0;
	}
  }
  printf("\n%s begins %s", mybasename(ufile), ctime(&begintime));

  fclose(fp);

  /*
   *	Should we free memory here? Nah. This is not NT :)
   */
  return 0;
}
