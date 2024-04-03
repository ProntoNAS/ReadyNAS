/*
 * <noflushd.c>
 *
 * noflushd is a daemon to spin down disks after a certain amount of idle
 * time. It's useful on Linux systems of version 2.2.11 and up that have a
 * kernel thread named kupdate. For earlier versions use bdflush 1.6.
 *
 * noflushd version 1.7 was heavily based on bdflush 1.6 by 
 * Eric Youngdale (<ericy@gnu.ai.mit.edu>, 
 * Phil Bostley <bostley@cs.colorado.edu>,
 * Daniel Quinlan <quinlan@yggdrasil.com>, and
 * Pavel Machek <pavel@atrey.karlin.mff.cuni.cz>. 
 * Pavel Machek added sleep support to bdflush 1.6. noflushd still uses his
 * basic idea, but was completely rewritten in version 1.8 so the code 
 * doesn't bear much (if any) resemblance to bdflush anymore.
 *
 * noflushd is currently maintained by Daniel Kobras <kobras@linux.de>
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
 * $Id: noflushd.c,v 1.2 2011-02-10 20:29:24 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disk_info.h"
#include "disk_stat.h"
#include "intr_stat.h"
#include "state.h"
#include "bug.h"
#include "pidfile.h"
#include "timeout.h"
#include "noflushd.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

kupdate_t kupdate;
volatile int advance_timeout;
static pidfile_t pid;

int verbose=0;
int released=0;

extern int nfd_do_scsi;

static void hup_handler(int i)
{
	DEBUG("Switching to next timeout set.");
	advance_timeout++;
}

static void term_handler(int i)
{
#ifdef __sparc__
	FILE *led;
	int dimmer=0xFFFFFF;
#endif
	
#ifdef __sparc__
	led=fopen("/proc/sys/dev/led/set_pwm","w");
	if(led) {
		fprintf(led,"%d", dimmer);
		fclose(led);
	}
	unlink(RPM_OVERRIDE_FILE);
#endif
	unlink("/ramfs/spindown");
	kupdate->start(kupdate);
	if (vm_setting) {
		vm_setting = 0;
		adjust_vm_settings();
	}
	pidfile_drop(pid);
	exit(0);
}

#ifdef __GNUC__
static void usage(char **, int) __attribute__ ((noreturn));
#endif

static void usage(char **argv, int err)
{
	printf("Usage: %s [ -d | -v | -h | -n <timeout> ] [ -r <disk> ]\n"
	       "          [ -t <timeout> ] [-i <intrs>] <disk> ...\n", argv[0]);
	printf("  -h  Give this help\n");
	printf("  -V  Show version\n");
	printf("  -v  Be verbose\n");
	printf("  -d  Show debugging information\n");
	printf("  -r  Mark disk as read-only\n");
	printf("  -n  Default timeout for spindown in minutes\n");
	printf("  -t  Spindown timeout in minutes for next disk\n");
	printf("  -i  Comma separated list of irqs to check interactivity\n");
	exit(err);
}

int main(int argc, char *argv[])
{
	unsigned int i;
	unsigned int ch;
	int ret;
	int debug=0;
	
	struct stat statbuf;
	
	disk_stat_t diskstat;
	disk_info_t diskinfo, di;
	tolist_t next_timeout = NULL;

	unsigned int monitor = 0;
	int default_timeout=0;

	extern char *optarg;
	extern int optopt;

	/* We absolutely depend on /proc - check if it is online. */
	ret = stat("/proc/version", &statbuf);
	if (ret == -1) {
		perror("Checking /proc/version failed");
		ERR("Cannot access /proc - giving up");
		exit(1);
	}
	
	if (geteuid() != 0) {
		ERR("%s: not run as superuser", argv[0]);
		exit(1);
	}

	/* disk_info_init must be called before the cmdline
	 * is parsed, so for sane debugging, we have to parse several
	 * options by hand. (Yes, ain't pretty, but...)
	 */
	for (i=1; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;
		if (argv[i][1] == 'd' && argv[i][2] == '\0') {
			debug++;
			verbose++;
			continue;
		}
		if (argv[i][1] == 'v' && argv[i][2] == '\0')
			verbose++;

		if (argv[i][1] == 'h' && argv[i][2] == '\0')
			usage(argv, 0);	/* usage() never returns */
	}
		
	devname_init();

	diskinfo = disk_info_init();
	if (!diskinfo)
		BUG("Disk info init failed.");

	if (!intr_stat_init())
		BUG("Interrupt stat init failed.");

	while ((ch = getopt(argc, argv, "-dvVhr:n:t:i:y:")) != EOF) {
		switch (ch) {
		case 'V':
			printf("%s - idle disk daemon. Version %s.\n",
			       PACKAGE, VERSION );
			break;
		case 'h':
		case 'd':
		case 'v':
			/* Help, debug and verbose were handled seperately. */
			break;
		case 'n':
		        default_timeout=atoi(optarg)*60;
			timeout_parse(NULL, optarg, 1);
			next_timeout = NULL;
			break;
		case 'r':
			di=disk_info_get_byname(diskinfo,optarg);
			disk_info_mark_ro(di);
			break;
		case 't':
			next_timeout=timeout_parse(NULL, optarg, 0);
			break;
		case 'i':
			intr_stat_register_byids(optarg);
			break;
		case 'y':
			if (!strcmp(optarg, "es-scsi-idle-patch-is-in-kernel"))
				nfd_do_scsi = 1;
			break;
		case 1:
			di=disk_info_get_byname(diskinfo,optarg);
			disk_info_setup(di, next_timeout);
			next_timeout = NULL;
			monitor++;
			break;
		case ':':
			ERR("Option -%c requires an argument", optopt);
			usage(argv, 1);	/* never returns */
		default:
			usage(argv, 1);	/* never returns */
		}
	}

	for(i=optind; i < argc; i++) {
		di=disk_info_get_byname(diskinfo,argv[i]);
		disk_info_setup(di, next_timeout);
		next_timeout = NULL;	
		monitor++;
	}
	
	/* If no devices to monitor are given on the command line, try
	 * to find a sensible default. We monitor all read-write devices
	 * found; read-only devices are disregarding because in most cases
	 * they're CD-ROMs or DVDs that are not meant to spindown via
	 * noflushd.
	 */
	if (!monitor) {
		INFO("No devices given - autoprobing.");
		disk_info_foreach(diskinfo, di) {
			if (!disk_info_is_rw(di))
				continue;
			disk_info_setup(di, NULL);
			monitor++;
		}
	}

	if (!monitor) {
		INFO("No devices found - giving up.");
		exit(1);
	}
	
	kupdate = kupdate_init();
	if (!kupdate) {
		/* This can happen on pre-2.2.11 kernels. (Or if someone
		 * yet again decides to rename kupdate[d].) 
		 */
		INFO("No kupdate found. Giving up.");
		exit(1);
	}
		
	diskstat = disk_stat_init();
	if (!diskstat) {
		ERR("Disk stat init failed. Does your kernel provide "
		    "statistics for your drives?");
		exit(1);
	}
		
	intr_set_timeout(default_timeout);
	
	if (debug||(!daemon(0, 0))) {
		if (debug) {
			signal(SIGINT, &term_handler);
		} else {
			released=1;
			openlog("noflushd", LOG_CONS | LOG_PID, LOG_DAEMON);
			signal(SIGINT, SIG_IGN);
		}
		signal(SIGHUP, &hup_handler);
		signal(SIGTERM, &term_handler);	/* must restart kupdate! */

		if (!(pid = pidfile_get(NOFLUSHD_PID_FILE))) {
			ERR("Error creating pidfile");
			exit(1);
		}
		nfd_daemon(diskinfo, diskstat);
		BUG("Huh!? Main daemon returned?");
	}
	ERR("Unable to fork off daemon: %s", strerror(errno));
	exit(1);
}
