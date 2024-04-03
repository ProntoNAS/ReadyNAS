/*
 * <kupdate.c>
 *
 * Handle interaction with kupdate(d)/pdflush kernel daemon.
 *
 * Copyright (C) 2000-2003 Daniel Kobras
 *           (C) 2003 Michael Buesch (parts of pdflush support)
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
 * $Id: kupdate.c,v 1.4 2010-05-17 19:17:58 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kupdate.h"
#include "bug.h"
#include "util.h"
#include "noflushd.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>		/* for HZ. (Yeah, I know...) */
#include <sys/types.h>

/*
 * Here's a simple yet effective fallback in case everything goes haywire
 * and kupdate(d) is left messed up. In fact, this is bdflush's/update's
 * fallback behaviour as well.
 */

static void last_resort(long interval)
{
	if (interval <= 0)
		interval = 1;

	ERR("Kernel background updates messed up");
	ERR("Falling back to brute-force emergency syncing every %ld secs",
	    interval); 

	while (1==1) {
		sync();
		sleep(interval);
	}
}

	
/*
 * Ugly fumbling through /proc to determine the PID of the kupdate kernel
 * thread. My, do I hate writing parsers! I wonder how you do it without
 * /proc or /proc mounted on a different node. Humm... [peeking through
 * pidof source] looks like you're simply screwed. Weird. But then thank
 * god the days of messing with /dev/kmem have gone.
 */

static int get_kupdate_pid(kupdate_t k)
{
	DIR	*proc, *pid_dir;
	FILE	*f=NULL;
	char	*line, *s_pid, *name;
	char	namebuf[267] = "/proc/";	/* d_name[] length of 256 bytes
						   is hardcoded in <direntry.h>.
						   We need to store at max 
						   /proc/<d_name[]>/stat. */
	int	pid, kpid=-1;
	struct dirent	*procent;

	proc = opendir("/proc");
	if (!proc) {
		ERR("Unable to open /proc");
		return 0;
	}

	/* More space efficient than using scandir()... */ 
	while ((procent = readdir(proc))) {
		if (!isdigit((int)(procent->d_name[0])))
			continue;
		strcpy(namebuf+6, procent->d_name);
		if (!(pid_dir = opendir(namebuf)))
			continue;
		closedir(pid_dir);
		strcat(namebuf, "/stat");
		if (f)
			fclose(f);
		f = fopen(namebuf, "r");
		line=get_line(f);
		if (!line)
			continue;
		name=_get_entry(line, 1);
		if (!name) {
			release_line(line);
			continue;
		}
		/* An ordinary user might have started a process named kupdate
		 * but it will never get a lower pid than the kupdate kernel
		 * thread. Uh, versions later than 2.4.2 seem to call it
		 * kupdated. Alan's valentine? Lovely...
		 */
		if (!strcmp(name, "(kupdate)") || !strcmp(name, "(kupdated)")) {
			s_pid=_get_entry(line, 0);
			if (!s_pid)
				BUG("No pid info on kupdate");
			pid=atoi(s_pid);
			if((pid < kpid) || (kpid == -1)) {
				kpid=pid;
				if (k->stat)
					fclose(k->stat);
				k->stat=f;
				f=NULL;
			}
			DEBUG("Detected process %s at pid %d", name, pid);
		}
		release_line(line);
		
	}
	closedir(proc);
	if (kpid == -1) {
		ERR("Could not determine PID of kupdate");
		return 0;
	}
	k->pid=(pid_t) kpid;
	return 1;
}

/* Check if kupdate is really stopped. */
static int kupdate_check_stop(kupdate_t k)
{
	char *line;
	char *state;
	int ret;
	
	fflush(k->stat);
	rewind(k->stat);
	line=get_line(k->stat);
	if (!line)
		BUG("Could not read kupdate stat");
	
	state=_get_entry(line, 2);
	if (!state)
		BUG("Could not read kupdate state");
	
	ret=(state[0]=='T');
	
	release_line(line);
	
	return ret;
}

/*
 * get/set_interval are simple helper functions to wrap [gs]etting the
 * basic flush interval of kupdate.
 */

static int _kupdate_get_interval(kupdate_t k, long *interval)
{
	long args[9];
	int i;

	rewind(k->f);
	for (i=0; i < 9; i++) {
		if (fscanf(k->f, "%ld", &args[i]) != 1) {
			ERR("Error reading kupdate(d) control file");
			return 0;
		}
	}
	*interval = args[4];
	if (k->interval <= 0) {
		INFO("Bogus kupdate interval %ld, setting to %d",
		       *interval, 5*HZ);
		*interval = 5*HZ;
	}
	return 1;
}

static int _kupdate_set_interval(kupdate_t k, long interval)
{
	long args[9];
	int i;
	
	rewind(k->f);
	for (i=0; i < 9; i++) {
		if (fscanf(k->f, "%ld", &args[i]) != 1) {
			ERR("Error reading kupdate(d) control file");
			return 0;
		}
	}
	
	args[4]=interval;
	
	rewind(k->f);
	for (i=0; i < 9; i++) {
		if (fprintf(k->f, "%ld", args[i]) <= 0) {
			ERR("Error writing to kupdate(d) control file");
			return 0;
		}
	}
	fprintf(k->f, "\n");
	fflush(k->f);

	return 1;
}

static long kupdate_fixup_interval(long interval)
{
	return (interval+HZ-1)/HZ;
}

/*
 * Handle background flushing of dirty buffers to disk via starting or 
 * stopping the kupdate kernel thread. kupdate is either (re)started or
 * stopped, depending on current setting in struct *k.
 * There's one ugly thing: kupdate expects its params scaled by HZ. Which
 * is quite funny as HZ is a kernel thingie and nowhere exported to userspace
 * (and rightfully so). 
 */

static void __kupdate_stop(kupdate_t k)
{
	long interval, left;

	if (!k)
		return;

	_kupdate_get_interval(k, &interval);
	if (!_kupdate_set_interval(k, 0))
		ERR("kupdate interval still set. Expect frequent spinups");

	kill(k->pid, SIGSTOP);	/* The signal itself is ignored, but it will
				 * cause kupdate to wakeup now and detect 
				 * interval=0. */

	/* Make sure kupdate is really asleep. We poll 5 times per second and
	 * wait at most 10 kupdate wakeup cycles. (During heavy system load,
	 * kupdate wakeup might be delayed.) */
	left = (10*k->interval*5+HZ-1)/HZ;
	while (!kupdate_check_stop(k) && left > 0) {
		usleep(200000);
		left--;
	}

	if (!left) {
		ERR("Could not stop kupdate. Expect lousy spindown times");
		return;
	}
		
	k->stopped=1;
	DEBUG("kupdate stopped");

}
	
static void __kupdate_start(kupdate_t k)
{
	if (!k)
		return;

	if (!_kupdate_set_interval(k, k->interval) ||
	    (-1 == kill(k->pid, SIGCONT))) {
		ERR("Could not restart kupdate");
		ERR("Falling back to sync()");
		last_resort(k->interval);
	}

	k->stopped=0;
	
	DEBUG("kupdate restarted with interval %ld.", k->interval);
}


/* 
 * Set up kupdate struct. Determines pid of kupdate kernel daemon.
 */

static int _kupdate_init(kupdate_t k)
{
	k->start=__kupdate_start;
	k->stop =__kupdate_stop;
	k->fixup_interval=kupdate_fixup_interval;
	k->stat=NULL;
	if (!get_kupdate_pid(k)) {
		DEBUG("No kupdate(d) found");
		return 0;
	}
	k->f=fopen("/proc/sys/vm/bdflush","r+");
	if (!k->f) {
		ERR("Unable to open kupdate(d) control file");
		fclose(k->stat);
		return 0;
	}
	if (!_kupdate_get_interval(k, &k->interval)) {
		ERR("Could not get valid kupdate interval");
		fclose(k->f);
		fclose(k->stat);
		return 0;
	}
	return 1;
}

/*
 * Linux kernels 2.5 and up handle background flushing via the pdflush
 * daemon.
 */

static int pdflush_get_interval(kupdate_t k, long *interval)
{
	char *buf;

	rewind(k->f);
	if (!(buf = get_line(k->f))) {
		ERR("Failed to get pdflush update interval");
		return 0;
	}
	*interval = strtol(_get_entry(buf, 0), NULL, 0);
	if (*interval < 0) {
		ERR("Bogus pdflush interval");
		return 0;
	}
	release_line(buf);
	return 1;
}

static int pdflush_set_interval(kupdate_t k, long interval)
{
	rewind(k->f);
	if (fprintf(k->f, "%ld\n", interval) < 0) {
		ERR("Failed to set net update interval");
		return 0;
	}
	fflush(k->f);
	return 1;
}
	
static int check_pdflush(kupdate_t k)
{
	long interval;

	if (!pdflush_get_interval(k, &k->interval)) {
		DEBUG("Failed to get pdflush interval");
		return 0;
	}
	
	if (k->interval <= 100) {
		INFO("pdflush wakeup interval %ld suspiciously low, "
		     "overriding to 5 seconds\n", k->interval);
		k->interval = 500;
	}

	if (!pdflush_set_interval(k, 0)) {
		DEBUG("Failed to set pdflush interval");
		return 0;
	}
	if (!pdflush_get_interval(k, &interval)) {
		DEBUG("Failed to get 2nd pdflush interval");
		return 0;
	}
	if (interval != 0) {
		DEBUG("pdflush interval inconsistent - giving up");
		return 0;
	}
	
	if (!pdflush_set_interval(k, k->interval)) {
		DEBUG("Failed to set 2nd pdflush interval");
		return 0;
	}

	return 1;
}

static void pdflush_start(kupdate_t k)
{
	if (!k)
		return;

	if (!pdflush_set_interval(k, k->interval)) {
		ERR("Failed to set pdflush interval");
		last_resort(k->interval);
	}
	
	k->stopped = 0;

	DEBUG("pdflush restarted with interval %ld.", k->interval);
}

static void pdflush_stop(kupdate_t k)
{
	if (!k)
		return;

	if (!pdflush_get_interval(k, &k->interval)) {
		ERR("Failed to get pdflush interval");
		return;
	}
	
	if (!pdflush_set_interval(k, 0))
		ERR("Failed to stop pdflush");
	else {
		/* Need to wait until all pending pdflush timers have
		 * expired. Unfortunately there's no way to kick them. */
		sleep(k->fixup_interval(k->interval));
		
		DEBUG("pdflush stopped");
		k->stopped = 1;
	}
}

static long pdflush_fixup_interval(long interval)
{
	return (interval+100-1)/100;
}

static int pdflush_init(kupdate_t k)
{
	FILE *f = NULL;

	f = fopen("/proc/sys/vm/dirty_writeback_centisecs", "w+");
	if (!f) {
		DEBUG("Could not open pdflush control interface");
		return 0;
	}

	k->f = f;

	/* On success, check_pdflush() initializes k->interval as a
	 * side-effect.
	 */
	if (!check_pdflush(k)) {
		ERR("Cannot control pdflush writeback");
		return 0;
	}
	
	k->start = pdflush_start;
	k->stop = pdflush_stop;
	k->fixup_interval = pdflush_fixup_interval;
	
	return 1;
}

/* The kupdate kernel interface scales with HZ whereas the pdflush interface
 * exports centisecs. Need to take care when converting into secs.
 */

long kupdate_get_interval(kupdate_t k)
{
	return k->fixup_interval(k->interval);
}

/* This is the main init function for both kupdate(d) and pdflush support.
 * The kupdate prefix for all generic methods is both unfortunate and
 * historic. Tough...
 */

kupdate_t kupdate_init(void)
{
	kupdate_t k;

	k=malloc(sizeof(struct kupdate_s));
	if (!k)
		return NULL;

	if (pdflush_init(k))
		DEBUG("Controlling pdflush daemon");
	else if (_kupdate_init(k))
		DEBUG("Controlling kupdate(d) daemon");
	else {
		ERR("No kernel support for flush control found");
		free(k);
		return NULL;
	}

	k->stopped=k->interval ? 0 : 1;

	return k;
}

