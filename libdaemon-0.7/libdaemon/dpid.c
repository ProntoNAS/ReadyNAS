/* $Id: dpid.c 71 2004-12-18 23:29:50Z lennart $ */

/*
 * This file is part of libdaemon.
 *
 * libdaemon is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libdaemon is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdaemon; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "dpid.h"
#include "dlog.h"

#define VARRUN "/var/run"

const char *daemon_pid_file_ident = 0;
daemon_pid_file_proc_t daemon_pid_file_proc = daemon_pid_file_proc_default;

const char *daemon_pid_file_proc_default(void) {
    static char fn[PATH_MAX];
    snprintf(fn, sizeof(fn), "%s/%s.pid", VARRUN, daemon_pid_file_ident ? daemon_pid_file_ident : "unknown");
    return fn;
}

pid_t daemon_pid_file_is_running(void) {
    const char *fn;
    static char txt[256];
    FILE *f;
    pid_t pid;


    if (!(fn = daemon_pid_file_proc()))
        return (pid_t) -1;

    if (!(f = fopen(fn, "r")))
        return (pid_t) -1;

        if (!(fgets(txt, sizeof(txt), f))) {
        daemon_log(LOG_WARNING, "PID file corrupt, removing. (%s)", fn);
        unlink(fn);
        fclose(f);
        return (pid_t) -1;
    }

    fclose(f);

    if ((pid = (pid_t) atoi(txt)) <= 0) {
        daemon_log(LOG_WARNING, "PID file corrupt, removing. (%s)", fn);
        unlink(fn);
        return (pid_t) -1;
    }

    if (kill(pid, 0) != 0 && errno != EPERM) {
        daemon_log(LOG_WARNING, "Daemon %u killed: %s; removing PID file. (%s)", pid, strerror(errno), fn);
        unlink(fn);
        return (pid_t) -1;
    }
    
    return pid;
}

int daemon_pid_file_kill(int s) {
    pid_t pid;
    
    if ((pid = daemon_pid_file_is_running()) < 0)
        return -1;

    if (kill(pid, s) < 0)
        return -1;

    return 0;
}

int daemon_pid_file_kill_wait(int s, int m) {
    pid_t pid;
    time_t t;
    
    if ((pid = daemon_pid_file_is_running()) < 0)
        return -1;

    if (kill(pid, s) < 0)
        return -1;

    t = time(NULL) + m;

    for (;;) {
        int r;
        struct timeval tv = { 0, 100000 };

        if (time(NULL) > t)
            return -1;
            
        if ((r = kill(pid, 0)) < 0 && errno != ESRCH)
            return -1;

        if (r)
            return 0;

        if (select(0, NULL, NULL, NULL, &tv) < 0)
            return -1;
    }
}

int daemon_pid_file_create(void) {
    const char *fn;
    FILE *f;
    mode_t save;

    if (!(fn = daemon_pid_file_proc()))
        return -1;

    save = umask(022);
    
    if (!(f = fopen(fn, "w")))
        return -1;

    fprintf(f, "%u\n", getpid());
    fclose(f);

    umask(save);

    return 0;
}

int daemon_pid_file_remove(void) {
    const char *fn;
    
    if (!(fn = daemon_pid_file_proc()))
        return -1;
    
    if (unlink(fn) < 0)
        return -1;

    return 0;
}

