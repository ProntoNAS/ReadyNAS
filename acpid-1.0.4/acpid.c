/*
 *  acpid.c - ACPI daemon
 *
 *  Portions Copyright (C) 2000 Andrew Henroid
 *  Portions Copyright (C) 2001 Sun Microsystems (thockin@sun.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <sys/poll.h>
#include <grp.h>

#include "acpid.h"
#include "ud_socket.h"

static int handle_cmdline(int *argc, char ***argv);
static void close_fds(void);
static int daemonize(void);
static int open_logs(void);
static void clean_exit(int sig);
static void reopen_logs(int sig);
static void reload_conf(int sig);
static char *read_line(int fd);

/* global debug level */
int acpid_debug;

static const char *progname;
static const char *confdir = ACPI_CONFDIR;
static const char *logfile = ACPI_LOGFILE;
static const char *eventfile = ACPI_EVENTFILE;
static const char *socketfile = ACPI_SOCKETFILE;
static int nosocket;
static const char *socketgroup;
static mode_t socketmode = ACPI_SOCKETMODE;
static int foreground;

int
main(int argc, char **argv)
{
	int event_fd;
	int sock_fd;

	/* learn who we really are */
	progname = (const char *)strrchr(argv[0], '/');
	progname = progname ? (progname + 1) : argv[0];

	/* handle the commandline  */
	handle_cmdline(&argc, &argv);

	/* close any extra file descriptors */
	close_fds();

	/* actually open the event file */
	event_fd = open(eventfile, O_RDONLY);
	if (event_fd < 0) {
		fprintf(stderr, "%s: can't open %s: %s\n", progname, 
			eventfile, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fcntl(event_fd, F_SETFD, FD_CLOEXEC);

/*
 * if there is data, and the kernel is NOT broken, this eats 1 byte.  We
 * can't have that.  This is ifdef'ed out on the assumption that old kernels
 * are out of popular use, by now.
 */
#ifdef TEST_FOR_BAD_KERNELS
	/*
	 * Older kernels did not support read() properly or poll() at all
	 * Check that the kernel supports the proper semantics, or die.
	 *
	 * Good kernels will respect O_NONBLOCK and return -1.  Bad kernels
	 * will ignore O_NONBLOCK and return 0.  Really bad kernels will block
	 * and overflow the buffer.  Can't deal with the really bad ones.
	 */
	{
		int fl;
		char buf;

		fl = fcntl(event_fd, F_GETFL);
		fcntl(event_fd, F_SETFL, fl | O_NONBLOCK);
		if (read(event_fd, &buf, 1) == 0) {
			fprintf(stderr, 
				"%s: this kernel does not support proper "
				"event file handling.\n"
				"Please get the patch from "
				"http://acpid.sourceforge.net\n", 
				progname);
			exit(EXIT_FAILURE);
		}
		fcntl(event_fd, F_SETFL, fl);
	}
#endif

	/* open our socket */
	if (!nosocket) {
		sock_fd = ud_create_socket(socketfile);
		if (sock_fd < 0) {
			fprintf(stderr, "%s: can't open socket %s: %s\n",
				progname, socketfile, strerror(errno));
			exit(EXIT_FAILURE);
		}
		fcntl(sock_fd, F_SETFD, FD_CLOEXEC);
		chmod(socketfile, socketmode);
		if (socketgroup) {
			struct group *gr;
			struct stat buf;
			gr = getgrnam(socketgroup);
			if (!gr) {
				fprintf(stderr, "%s: group %s does not exist\n",
					progname, socketgroup);
				exit(EXIT_FAILURE);
			}
			if (stat(socketfile, &buf) < 0) {
				fprintf(stderr, "%s: can't stat %s\n",
					progname, socketfile);
				exit(EXIT_FAILURE);
			}
			if (chown(socketfile, buf.st_uid, gr->gr_gid) < 0) {
				fprintf(stderr, "%s: chown(): %s\n",
					progname, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	/* if we're running in foreground, we don't daemonize */
	if (!foreground) {
		if (daemonize() < 0)
			exit(EXIT_FAILURE);
	}

	/* normal logging unless debugging */
	if (!acpid_debug) {
		if (open_logs() < 0)
			exit(EXIT_FAILURE);
	}
	acpid_log("starting up\n");

	/* trap key signals */
	signal(SIGHUP, reload_conf);
	signal(SIGUSR1, reopen_logs);
	signal(SIGINT, clean_exit);
	signal(SIGQUIT, clean_exit);
	signal(SIGTERM, clean_exit);
	signal(SIGPIPE, SIG_IGN);

	/* read in our configuration */
	acpid_read_conf(confdir);

	/* main loop */
	while (1) {
		struct pollfd ar[2];
		int r;
		int fds = 0;
		
		/* poll for the socket and the event file */
		ar[0].fd = event_fd; ar[0].events = POLLIN; fds++;
		if (!nosocket) {
			ar[1].fd = sock_fd; ar[1].events = POLLIN; fds++;
		}
		r = poll(ar, fds, -1);

		if (r < 0 && errno == EINTR) {
			continue;
		} else if (r < 0) {
			acpid_log("ERR: poll(): %s\n", strerror(errno));
			continue;
		}

		/* was it an event? */
		if (ar[0].revents) {
			char *event;
			
			/* this shouldn't happen */
			if (!ar[0].revents & POLLIN) {
				acpid_log("odd, poll set flags 0x%x\n", 
					ar[0].revents);
				continue;
			}

			/* read and handle an event */
			event = read_line(event_fd);
			if (event) {
				acpid_log("received event \"%s\"\n", event);
				acpid_handle_event(event);
				acpid_log("completed event \"%s\"\n", event);
			} else if (errno == EPIPE) {
				acpid_log("events file connection closed\n");
				break;
			} else {
				static int nerrs;
				if (++nerrs >= ACPI_MAX_ERRS) {
					acpid_log("too many errors reading "
						"events file - aborting\n");
					break;
				}
			}
		} 

		/* was it a new connection? */
		if (!nosocket && ar[1].revents) {
			int cli_fd;
			struct ucred creds;
			char buf[32];

			/* this shouldn't happen */
			if (!ar[1].revents & POLLIN) {
				acpid_log("odd, poll set flags 0x%x\n", 
					ar[1].revents);
				continue;
			}

			/* accept and add to our lists */
			cli_fd = ud_accept(sock_fd, &creds);
			if (cli_fd < 0) {
				acpid_log("ERR: can't accept client: %s\n", 
					strerror(errno));
				continue;
			}
			snprintf(buf, sizeof(buf)-1, "%d[%d:%d]",
				creds.pid, creds.uid, creds.gid);
			acpid_add_client(cli_fd, buf);
		}
	}

	clean_exit(EXIT_SUCCESS);

	return 0;
}

/*
 * Parse command line arguments
 */
static int
handle_cmdline(int *argc, char ***argv)
{
	struct option opts[] = {
		{"confdir", 1, 0, 'c'},
		{"debug", 0, 0, 'd'},
		{"eventfile", 1, 0, 'e'},
		{"foreground", 0, 0, 'f'},
		{"socketgroup", 1, 0, 'g'},
		{"logfile", 1, 0, 'l'},
		{"socketmode", 1, 0, 'm'},
		{"socketfile", 1, 0, 's'},
		{"nosocket", 1, 0, 'S'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{NULL, 0, 0, 0},
	};
	const char *opts_help[] = {
		"Set the configuration directory.",	/* confdir */
		"Increase debugging level (implies -f).",/* debug */
		"Use the specified file for events.",	/* eventfile */
		"Run in the foreground.",		/* foreground */
		"Set the group on the socket file.",	/* socketgroup */
		"Use the specified log file.",		/* logfile */
		"Set the permissions on the socket file.",/* socketmode */
		"Use the specified socket file.",	/* socketfile */
		"Do not listen on a UNIX socket (overrides -s).",/* nosocket */
		"Print version information.",		/* version */
		"Print this message.",			/* help */
	};
	struct option *opt;
	const char **hlp;
	int max, size;

	for (;;) {
		int i;
		i = getopt_long(*argc, *argv, "c:de:fg:l:m:s:Svh", opts, NULL);
		if (i == -1) {
			break;
		}
		switch (i) {
		case 'c':
			confdir = optarg;
			break;
		case 'd':
			foreground = 1;
			acpid_debug++;
			break;
		case 'e':
			eventfile = optarg;
			break;
		case 'f':
			foreground = 1;
			break;
		case 'g':
			socketgroup = optarg;
			break;
		case 'l':
			logfile = optarg;
			break;
		case 'm':
			socketmode = strtol(optarg, NULL, 8);
			break;
		case 's':
			socketfile = optarg;
			break;
		case 'S':
			nosocket = 1;
			break;
		case 'v':
			printf(PACKAGE "-" VERSION "\n");
			exit(EXIT_SUCCESS);
		case 'h':
		default:
			fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
			max = 0;
			for (opt = opts; opt->name; opt++) {
				size = strlen(opt->name);
				if (size > max)
					max = size;
			}
			for (opt = opts, hlp = opts_help;
			     opt->name;
			     opt++, hlp++)
			{
				fprintf(stderr, "  -%c, --%s",
					opt->val, opt->name);
				size = strlen(opt->name);
				for (; size < max; size++)
					fprintf(stderr, " ");
				fprintf(stderr, "  %s\n", *hlp);
			}
			exit(EXIT_FAILURE);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;

	return 0;
}

static void
close_fds(void)
{
	int fd, max;
	max = sysconf(_SC_OPEN_MAX);
	for (fd = 3; fd < max; fd++)
		close(fd);
}

static int
daemonize(void)
{
	switch(fork()) {
	case -1:
		fprintf(stderr, "%s: fork: %s\n", progname, strerror(errno));
		return -1;
	case 0:
		/* child */
		break;
	default:
		/* parent */
		exit(EXIT_SUCCESS);
	}

	/* disconnect */
	setsid();
	umask(0);

	/* get outta the way */
	chdir("/");

	return 0;
}

static int
open_logs(void)
{
	int logfd;
	int nullfd;

	/* set up stdout, stderr to log and stdin to /dev/null */
	nullfd = open("/dev/null", O_RDONLY, 0640);
	if (nullfd < 0) {
		fprintf(stderr, "%s: can't open %s: %s\n", progname, 
			"/dev/null", strerror(errno));
		return -1;
	}
	logfd = open(logfile, O_WRONLY|O_CREAT|O_APPEND);
	if (logfd < 0) {
		fprintf(stderr, "%s: can't open %s: %s\n", progname, 
			logfile, strerror(errno));
		return -1;
	}

	if (dup2(nullfd, STDIN_FILENO) != STDIN_FILENO) {
		fprintf(stderr, "%s: dup2: %s\n", progname, strerror(errno));
		return -1;
	}
	if (dup2(logfd, STDOUT_FILENO) != STDOUT_FILENO) {
		fprintf(stderr, "%s: dup2: %s\n", progname, strerror(errno));
		return -1;
	}
	if (dup2(logfd, STDERR_FILENO) != STDERR_FILENO) {
		fprintf(stderr, "%s: dup2: %s\n", progname, strerror(errno));
		return -1;
	}

	close(nullfd);
	close(logfd);

	return 0;
}

static void
clean_exit(int sig)
{
	acpid_cleanup_rules();
	acpid_log("exiting\n");
	exit(EXIT_SUCCESS);
}

static void
reload_conf(int sig)
{
	acpid_log("reloading configuration\n");
	acpid_cleanup_rules();
	acpid_read_conf(confdir);
}

static void
reopen_logs(int sig)
{
	/* normal logging unless debugging */
	if (!acpid_debug) {
		if (open_logs() < 0)
			exit(EXIT_FAILURE);
	}

	acpid_log("logfile reopened\n");
}

int 
acpid_log(const char *fmt, ...)
{
	va_list args;
	int len = 0;
	time_t curtime;
	char *timestr;

	va_start(args, fmt);
	curtime = time(NULL);
	timestr = ctime(&curtime);
	timestr[strlen(timestr)-1] = '\0';
	len += fprintf(stderr, "[%s] ", timestr);
	len += vfprintf(stderr, fmt, args);
	va_end(args);

	return 0;
}

/* 
 * This depends on fixes in linux ACPI after 2.4.8
 */
#define MAX_BUFLEN	1024
static char *
read_line(int fd)
{
	static char *buf;
	int buflen = 64;
	int i = 0;
	int r;
	int searching = 1;

	while (searching) {
		buf = realloc(buf, buflen);
		if (!buf) {
			acpid_log("ERR: malloc(%d): %s\n",
				buflen, strerror(errno));
			return NULL;
		}
		memset(buf+i, 0, buflen-i);

		while (i < buflen) {
			r = read(fd, buf+i, 1);
			if (r < 0 && errno != EINTR) {
				/* we should do something with the data */
				acpid_log("ERR: read(): %s\n",
					strerror(errno));
				return NULL;
			} else if (r == 0) {
				/* signal this in an almost standard way */
				errno = EPIPE;
				return NULL;
			} else if (r == 1) {
				/* scan for a newline */
				if (buf[i] == '\n') {
					searching = 0;
					buf[i] = '\0';
					break;
				}
				i++;
			}
		}
		if (buflen >= MAX_BUFLEN) {
			break;
		} 
		buflen *= 2;
	}

	return buf;
}
