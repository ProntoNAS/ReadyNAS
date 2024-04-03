/* $Id: testd.c 72 2004-12-18 23:33:34Z lennart $ */

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

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>

#include <libdaemon/dfork.h>
#include <libdaemon/dsignal.h>
#include <libdaemon/dlog.h>
#include <libdaemon/dpid.h>
#include <libdaemon/dexec.h>

int main(int argc, char *argv[]) {
    pid_t pid;

    /* Set indetification string for the daemon for both syslog and PID file */
    daemon_pid_file_ident = daemon_log_ident = daemon_ident_from_argv0(argv[0]);

    /* Check if we are called with -k parameter */
    if (argc >= 2 && !strcmp(argv[1], "-k")) {
        int ret;

        /* Kill daemon with SIGINT */
        
        /* Check if the new function daemon_pid_file_kill_wait() is available, if it is, use it. */
#ifdef DAEMON_PID_FILE_KILL_WAIT_AVAILABLE        
        if ((ret = daemon_pid_file_kill_wait(SIGINT, 5)) < 0)
#else
        if ((ret = daemon_pid_file_kill(SIGINT)) < 0)
#endif
            daemon_log(LOG_WARNING, "Failed to kill daemon");
        
        return ret < 0 ? 1 : 0;
    }

    /* Check that the daemon is not rung twice a the same time */
    if ((pid = daemon_pid_file_is_running()) >= 0) {
        daemon_log(LOG_ERR, "Daemon already running on PID file %u", pid);
        return 1;
        
    }

    /* Prepare for return value passing from the initialization procedure of the daemon process */
    daemon_retval_init();

    /* Do the fork */
    if ((pid = daemon_fork()) < 0) {

        /* Exit on error */
        daemon_retval_done();
        return 1;
        
    } else if (pid) { /* The parent */
        int ret;

        /* Wait for 20 seconds for the return value passed from the daemon process */
        if ((ret = daemon_retval_wait(20)) < 0) {
            daemon_log(LOG_ERR, "Could not recieve return value from daemon process.");
            return 255;
        }

        daemon_log(ret != 0 ? LOG_ERR : LOG_INFO, "Daemon returned %i as return value.", ret);
        return ret;
        
    } else { /* The daemon */
        int fd, quit = 0;
        fd_set fds;

        /* Create the PID file */
        if (daemon_pid_file_create() < 0) {
            daemon_log(LOG_ERR, "Could not create PID file (%s).", strerror(errno));

            /* Send the error condition to the parent process */
            daemon_retval_send(1);
            goto finish;
        }

        /* Initialize signal handling */
        if (daemon_signal_init(SIGINT, SIGQUIT, SIGHUP, 0) < 0) {
            daemon_log(LOG_ERR, "Could not register signal handlers (%s).", strerror(errno));
            daemon_retval_send(2);
            goto finish;
        }
        
        /*... do some further init work here */


        /* Send OK to parent process */
        daemon_retval_send(0);

        daemon_log(LOG_INFO, "Sucessfully started");


        /* Prepare for select() on the signal fd */
        FD_ZERO(&fds);
        FD_SET(fd = daemon_signal_fd(), &fds);
        
        while (!quit) {
            fd_set fds2 = fds;

            /* Wait for an incoming signal */
            if (select(FD_SETSIZE, &fds2, 0, 0, 0) < 0) {

                /* If we've been interrupted by an incoming signal, continue */
                if (errno == EINTR)
                    continue;
                
                daemon_log(LOG_ERR, "select(): %s", strerror(errno));
                break;
            }

            /* Check if a signal has been recieved */
            if (FD_ISSET(fd, &fds)) {
                int sig;

                /* Get signal */
                if ((sig = daemon_signal_next()) <= 0) {
                    daemon_log(LOG_ERR, "daemon_signal_next() failed.");
                    break;
                }

                /* Dispatch signal */
                switch (sig) {

                    case SIGINT:
                    case SIGQUIT:
                        daemon_log(LOG_WARNING, "Got SIGINT or SIGQUIT");
                        quit = 1;
                        break;

                    case SIGHUP:
                        daemon_log(LOG_INFO, "Got a HUP");
                        daemon_exec("/", NULL, "/bin/ls", "ls", (char*) NULL);
                        break;

                }
            }
        }

        /* Do a cleanup */
finish:
        daemon_log(LOG_INFO, "Exiting...");

        daemon_signal_done();
        daemon_pid_file_remove();
        
        return 0;
    }
}
