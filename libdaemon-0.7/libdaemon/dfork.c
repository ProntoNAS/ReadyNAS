/* $Id: dfork.c 71 2004-12-18 23:29:50Z lennart $ */

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

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/time.h>

#include "dfork.h"
#include "dnonblock.h"
#include "dlog.h"

static int _daemon_retval_pipe[2] = { -1, -1 };

static int _null_open(int f, int fd) {
    int fd2;

    if ((fd2 = open("/dev/null", f)) < 0)
        return -1;
    
    if (fd2 == fd)
        return fd;

    if (dup2(fd2, fd) < 0)
        return -1;

    close(fd2);
    return fd;
}

static ssize_t atomic_read(int fd, void *d, size_t l) {
    ssize_t t = 0;
    
    while (l > 0) {
        ssize_t r;
        
        if ((r = read(fd, d, l)) <= 0) {

            if (r < 0)
                return t > 0 ? t : -1;
            else
                return t;
        }

        t += r;
        d = (char*) d + r;
        l -= r;
    }

    return t;
}

static ssize_t atomic_write(int fd, const void *d, size_t l) {
    ssize_t t = 0;
    
    while (l > 0) {
        ssize_t r;
        
        if ((r = write(fd, d, l)) <= 0) {

            if (r < 0)
                return t > 0 ? t : -1;
            else
                return t;
        }

        t += r;
        d = (char*) d + r;
        l -= r;
    }

    return t;
}

static int move_fd_up(int *fd) {
    assert(fd);
    
    while (*fd <= 2) {
        if ((*fd = dup(*fd)) < 0) {
            daemon_log(LOG_ERR, "dup(): %s", strerror(errno));
            return -1;
        }
    }

    return 0;
}

static void sigchld(int s) {
}

pid_t daemon_fork(void) {
    pid_t pid;
    int pipe_fds[2] = {-1, -1};
    struct sigaction sa_old, sa_new;
    sigset_t ss_old, ss_new;

    memset(&sa_new, 0, sizeof(sa_new));
    sa_new.sa_handler = sigchld;
    sa_new.sa_flags = SA_RESTART;
    
    if (sigaction(SIGCHLD, &sa_new, &sa_old) < 0) {
        daemon_log(LOG_ERR, "sigaction() failed: %s", strerror(errno));
        return (pid_t) -1;
    }

    sigemptyset(&ss_new);
    sigaddset(&ss_new, SIGCHLD);
    
    if (sigprocmask(SIG_UNBLOCK, &ss_new, &ss_old) < 0) {
        daemon_log(LOG_ERR, "sigprocmask() failed: %s", strerror(errno));
        sigaction(SIGCHLD, &sa_old, NULL);
        return (pid_t) -1;
    }
    
    if (pipe(pipe_fds) < 0) {
        daemon_log(LOG_ERR, "pipe() failed: %s", strerror(errno));
        sigaction(SIGCHLD, &sa_old, NULL);
        sigprocmask(SIG_SETMASK, &ss_old, NULL);
        return (pid_t) -1;
    }

    if ((pid = fork()) < 0) { // First fork
        daemon_log(LOG_ERR, "First fork() failed: %s\n", strerror(errno));
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        sigaction(SIGCHLD, &sa_old, NULL);
        sigprocmask(SIG_SETMASK, &ss_old, NULL);
        return (pid_t) -1;

    } else if (pid == 0) {
        pid_t dpid;
        
        /* First child */

        sigaction(SIGCHLD, &sa_old, NULL);
        sigprocmask(SIG_SETMASK, &ss_old, NULL);
        close(pipe_fds[0]);

        /* Move file descriptors up*/
        if (move_fd_up(&pipe_fds[1]) < 0)
            goto fail;
        if (_daemon_retval_pipe[0] >= 0 && move_fd_up(&_daemon_retval_pipe[0]) < 0)
            goto fail;
        if (_daemon_retval_pipe[1] >= 0 && move_fd_up(&_daemon_retval_pipe[1]) < 0)
            goto fail;
            
        if (_null_open(O_RDONLY, 0) < 0) {
            daemon_log(LOG_ERR, "Failed to open /dev/null for STDIN: %s", strerror(errno));
            goto fail;
        }
        
        if (_null_open(O_WRONLY, 1) < 0) {
            daemon_log(LOG_ERR, "Failed to open /dev/null for STDOUT: %s", strerror(errno));
            goto fail;
        }
        
        if (_null_open(O_WRONLY, 2) < 0) {
            daemon_log(LOG_ERR, "Failed to open /dev/null for STDERR: %s", strerror(errno));
            goto fail;
        }

        setsid();
        umask(0777);
        chdir("/");

        if ((pid = fork()) < 0) { // Second fork
            daemon_log(LOG_ERR, "Second fork() failed: %s", strerror(errno));
            goto fail;

        } else if (pid == 0) {
		int tty_fd;
            /* Second child */
            
            if (daemon_log_use & DAEMON_LOG_AUTO)
                daemon_log_use = DAEMON_LOG_SYSLOG;
        
	    signal(SIGTTOU, SIG_IGN);
	    signal(SIGTTIN, SIG_IGN);
	    signal(SIGTSTP, SIG_IGN);
	    
	    setsid();
	    setpgrp();
            
	    if ((tty_fd = open("/dev/tty", O_RDWR)) >= 0) {
		ioctl(tty_fd, TIOCNOTTY, NULL);
		close(tty_fd);
	    }
        
            dpid = getpid();
            if (atomic_write(pipe_fds[1], &dpid, sizeof(dpid)) != sizeof(dpid))
                goto fail;
            close(pipe_fds[1]);


            return 0;

        } else {
            /* Second father */
            close(pipe_fds[1]);
            exit(0);
        }
            
    fail:
        dpid = (pid_t) -1;
        if (atomic_write(pipe_fds[1], &dpid, sizeof(dpid)) != sizeof(dpid))
            daemon_log(LOG_ERR, "Failed to write error PID.");
        close(pipe_fds[1]);
        exit(0);

    } else {
        /* First father */
        pid_t dpid;

        close(pipe_fds[1]);
        waitpid(pid, NULL, WUNTRACED);
        
        sigprocmask(SIG_SETMASK, &ss_old, NULL);
        sigaction(SIGCHLD, &sa_old, NULL);

        if (atomic_read(pipe_fds[0], &dpid, sizeof(dpid)) != sizeof(dpid)) {
            daemon_log(LOG_ERR, "Failed to read daemon PID.");
            dpid = (pid_t) -1;
        }

        close(pipe_fds[0]);
        return dpid;
    }

}

int daemon_retval_init(void) {
    if (pipe(_daemon_retval_pipe) < 0)
        return -1;

    return 0;
}

void daemon_retval_done(void) {
    if (_daemon_retval_pipe[0] >= 0)
        close(_daemon_retval_pipe[0]);
    
    if (_daemon_retval_pipe[1] >= 0)
        close(_daemon_retval_pipe[1]);

    _daemon_retval_pipe[0] = _daemon_retval_pipe[1] = -1;
}

int daemon_retval_send(int i) {
    ssize_t r;

    r = atomic_write(_daemon_retval_pipe[1], &i, sizeof(i));

    daemon_retval_done();

    if (r != sizeof(i)) {

        if (r < 0)
            daemon_log(LOG_ERR, "read() failed while writing return value to pipe: %s", strerror(errno));
        else
            daemon_log(LOG_ERR, "write() too short while writing return value from pipe");
        
        return -1;
    }

    return 0;
}

int daemon_retval_wait(int timeout) {
    ssize_t r;
    int i;

    if (timeout > 0) {
        struct timeval tv;
        int s;
        fd_set fds;

        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        
        FD_ZERO(&fds);
        FD_SET(_daemon_retval_pipe[0], &fds);

        if ((s = select(FD_SETSIZE, &fds, 0, 0, &tv)) != 1) {
            
            if (s < 0)
                daemon_log(LOG_ERR, "select() failed while waiting for return value: %s", strerror(errno));
            else
                daemon_log(LOG_ERR, "Timeout reached while wating for return value");
        
            return -1;
        }
    }

    if ((r = atomic_read(_daemon_retval_pipe[0], &i, sizeof(i))) != sizeof(i)) {

        if (r < 0)
            daemon_log(LOG_ERR, "read() failed while reading return value from pipe: %s", strerror(errno));
        else if (r == 0)
            daemon_log(LOG_ERR, "read() failed with EOF while reading return value from pipe.");
        else if (r > 0)
            daemon_log(LOG_ERR, "read() too short while reading return value from pipe.");
        
        return -1;
    }

    daemon_retval_done();
    
    return i;
}

