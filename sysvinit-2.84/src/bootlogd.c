/*
 * bootlogd.c	Store output from the console during bootup into a file.
 *		The file is usually located on the /var partition, and
 *		gets written (and fsynced) as soon as possible.
 *
 * Version:	@(#)bootlogd  2.79  11-Sep-2000  miquels@cistron.nl
 *
 * Bugs:	Uses openpty(), only available in glibc. Sorry.
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 1991-2000 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 *				*NOTE* *NOTE* *NOTE*
 *			This is a PROOF OF CONCEPT IMPLEMENTATION
 *		I do not recommend using this on production systems.
 *
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <dirent.h>
#include <fcntl.h>
#include <pty.h>

char *Version = "@(#) bootlogd 2.79 11-Sep-2000 MvS";

/*
 *	Until the kernel knows about TIOCGDEV, use a really ugly
 *	non-portable (not even between architectures) hack.
 */
#ifndef TIOCGDEV
#  define TIOCTTYGSTRUCT_HACK	1
#endif

#define LOGFILE	"/var/log/boot.log"

char ringbuf[32768];
char *endptr = ringbuf + sizeof(ringbuf);
char *inptr  = ringbuf;
char *outptr = ringbuf;

int got_signal = 0;
int didnl = 1;


#ifdef TIOCTTYGSTRUCT_HACK
struct tty_offsets {
	char	*kver;
	int	offset;
} tty_offsets[] = {
#if ((~0UL) == 0xffffffff)	/* 32 bits */
  { "2.0.",	236	},
  { "2.1.",	268	},
  { "2.2.", 	272	},
  { "2.3.",	272	},
  { "2.4.",	272	},
  { "2.5.",	272	},
#else				/* 64 bits */
  { "2.2.", 	480	},
  { "2.3.", 	480	},
  { "2.4.", 	480	},
  { "2.5.", 	480	},
#endif
  { NULL,	0	},
};
#endif


/*
 *	Catch signals.
 */
void handler(int sig)
{
	got_signal = sig;
}


/*
 *	Scan /dev and find the device name.
 *	Side-effect: directory is changed to /dev
 */
int findtty(char *res, int rlen, dev_t dev)
{
	DIR		*dir;
	struct dirent	*ent;
	struct stat	st;
	int		r = 0;

	if (chdir("/dev") < 0 || (dir = opendir(".")) == NULL) {
		perror("bootlogd: /dev");
		return -1;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (lstat(ent->d_name, &st) != 0)
			continue;
		if (!S_ISCHR(st.st_mode))
			continue;
		if (st.st_rdev == dev) {
			break;
		}
	}
	if (ent == NULL) {
		fprintf(stderr, "bootlogd: cannot find console device\n");
		r = -1;
	} else if (strlen(ent->d_name) >= rlen) {
		fprintf(stderr, "bootlogd: console device name too long\n");
		r = -1;
	} else
		strcpy(res, ent->d_name);
	closedir(dir);

	return r;
}


/*
 *	Find out the _real_ console. Assume that stdin is connected to
 *	the console device (/dev/console).
 */
int consolename(char *res, int rlen)
{
	struct stat	st;
#if TIOCTTYGSTRUCT_HACK
	struct utsname	uts;
	struct tty_offsets *tt;
	dev_t		dev;
	unsigned short	*kdev;
	char		buf[4096];
	int		offset = -1;
#endif
#ifdef TIOCGDEV
	kdev_t		kdev;
#endif

	fstat(0, &st);
	if (st.st_rdev != 0x0501) {
		/*
		 *	Old kernel, can find real device easily.
		 */
		return findtty(res, rlen, st.st_rdev);
	}

#ifdef TIOCGDEV
	if (ioctl(0, TIOCGDEV, &kdev) == 0)
		return findtty(res, rlen, (dev_t)kdev);
	return -1;
#endif

	/*
	 *	New kernel and new console device - hard to find
	 *	out what device the real console is ..
	 */
#if TIOCTTYGSTRUCT_HACK
	if (ioctl(0, TIOCTTYGSTRUCT, buf) != 0) {
		perror("bootlogd: TIOCTTYGSTRUCT");
		return -1;
	}
	uname(&uts);
	for (tt = tty_offsets; tt->kver; tt++)
		if (!strncmp(uts.release, tt->kver, strlen(tt->kver))) {
			offset = tt->offset;
			break;
		}
	if (offset < 0) {
		fprintf(stderr, "bootlogd: don't know offsetof"
		"(struct tty_struct, device) for kernel %s\n", uts.release);
		return -1;
	}
	kdev = (unsigned short *)(&buf[offset]);
	dev = (dev_t)(*kdev);
	return findtty(res, rlen, dev);
#endif
}


/*
 *	Write data and make sure it's on disk.
 */
void writelog(FILE *fp, unsigned char *ptr, int len)
{
	time_t		t;
	char		*s;
	int		olen = len;

	while (len > 0) {
		if (didnl) {
			time(&t);
			s = ctime(&t);
			fprintf(fp, "%.24s: ", s);
			didnl = 0;
		}
		switch (*ptr) {
			case '\r':
				break;
			case '\n':
				didnl = 1;
			case '\t':
			case  32 ... 127:
			case 161 ... 255:
				fputc(*ptr, fp);
				break;
			default:
				fprintf(fp, "\\%03o", *ptr);
				break;
		}
		ptr++;
		len--;
	}
	fflush(fp);
	fdatasync(fileno(fp));

	outptr += olen;
	if (outptr >= endptr)
		outptr = ringbuf;

}


/*
 *	Print usage message and exit.
 */
void usage(void)
{
	fprintf(stderr, "Usage: bootlogd [-v] [-r] [-d] [-p pidfile] [-l logfile]\n");
	exit(1);
}


int main(int argc, char **argv)
{
	FILE		*fp;
	struct timeval	tv;
	fd_set		fds;
	char		buf[1024];
	char		*p;
	char		*logfile;
	char		*pidfile;
	int		rotate;
	int		dontfork;
	int		ptm, pts;
	int		realfd;
	int		n, m, i;
	int		todo;

	fp = NULL;
	logfile = LOGFILE;
	pidfile = NULL;
	rotate = 0;
	dontfork = 0;

	while ((i = getopt(argc, argv, "dl:p:rv")) != EOF) switch(i) {
		case 'l':
			logfile = optarg;
			break;
		case 'r':
			rotate = 1;
			break;
		case 'v':
			printf("%s\n", Version);
			exit(0);
			break;
		case 'p':
			pidfile = optarg;
			break;
		case 'd':
			dontfork = 1;
			break;
		default:
			usage();
			break;
	}
	if (optind < argc) usage();

	signal(SIGTERM, handler);
	signal(SIGQUIT, handler);
	signal(SIGINT,  handler);
	signal(SIGTTIN,  SIG_IGN);
	signal(SIGTTOU,  SIG_IGN);
	signal(SIGTSTP,  SIG_IGN);

	/*
	 *	Open console device directly.
	 */
	if (consolename(buf, sizeof(buf)) < 0)
		return 1;
	if ((realfd = open(buf, O_WRONLY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "bootlogd: %s: %s\n", buf, strerror(errno));
		return 1;
	}
	n = fcntl(realfd, F_GETFL);
	n &= ~(O_NONBLOCK);
	fcntl(realfd, F_SETFL, n);

	/*
	 *	Grab a pty, and redirect console messages to it.
	 */
	if (openpty(&ptm, &pts, buf, NULL, NULL) < 0) {
		fprintf(stderr, "bootlogd: cannot allocate pseudo tty\n");
		return 1;
	}
	(void)ioctl(0, TIOCCONS, NULL);
#if 1
	/* Work around bug in 2.1/2.2 kernels. Fixed in 2.2.13 and 2.3.18 */
	if ((n = open("/dev/tty0", O_RDWR)) >= 0) {
		(void)ioctl(n, TIOCCONS, NULL);
		close(n);
	}
#endif
	if (ioctl(pts, TIOCCONS, NULL) < 0) {
		fprintf(stderr, "bootlogd: ioctl(%s, TIOCCONS): %s\n",
			buf, strerror(errno));
		return 1;
	}

	/*
	 *	Fork and write pidfile if needed.
	 */
	if (!dontfork) {
		if (fork())
			exit(1);
		setsid();
	}
	if (pidfile) {
		unlink(pidfile);
		if ((fp = fopen(pidfile, "w")) != NULL) {
			fprintf(fp, "%d\n", (int)getpid());
			fclose(fp);
		}
		fp = NULL;
	}

	/*
	 *	Read the console messages from the pty, and write
	 *	to the real console and the logfile.
	 */
	while (!got_signal) {

		/*
		 *	We timeout after 5 seconds if we still need to
		 *	open the logfile. There might be buffered messages
		 *	we want to write.
		 */
		tv.tv_sec = fp ? 86400 : 5;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(ptm, &fds);
		if (select(ptm + 1, &fds, NULL, NULL, &tv) == 1) {
			/*
			 *	See how much space there is left, read.
			 */
			if ((n = read(ptm, inptr, endptr - inptr)) >= 0) {
				/*
				 *	Write data (in chunks if needed)
				 *	to the real output device.
				 */
				m = n;
				p = inptr;
				while (m > 0) {
					i = write(realfd, p, m);
					if (i <= 0) break;
					m -= i;
					p += i;
				}
				/*
				 *	Increment buffer position. Handle
				 *	wraps, and also drag output pointer
				 *	along if we cross it.
				 */
				inptr += n;
				if (inptr - n < outptr && inptr > outptr)
					outptr = inptr;
				if (inptr >= endptr)
					inptr = ringbuf;
				if (outptr >= endptr)
					outptr = ringbuf;
			}
		}

		/*
		 *	Perhaps we need to open the logfile.
		 */
		if (fp == NULL && rotate && access(logfile, F_OK) == 0) {
			snprintf(buf, sizeof(buf), "%s~", logfile);
			rename(logfile, buf);
		}
		if (fp == NULL)
			fp = fopen(logfile, "a");
		if (inptr >= outptr)
			todo = inptr - outptr;
		else
			todo = endptr - outptr;
		if (fp && todo)
			writelog(fp, outptr, todo);
	}

	if (fp && !didnl) {
		fputc('\n', fp);
		fclose(fp);
	}

	close(pts);
	close(ptm);
	close(realfd);

	return 0;
}

