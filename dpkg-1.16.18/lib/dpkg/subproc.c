/*
 * libdpkg - Debian packaging suite library routines
 * subproc.c - subprocess helper routines
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/subproc.h>

static int signo_ignores[] = { SIGQUIT, SIGINT };
static struct sigaction sa_save[array_count(signo_ignores)];

void
subproc_signals_setup(const char *name)
{
	struct sigaction sa;
	size_t i;

	onerr_abort++;
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;

	for (i = 0; i < array_count(signo_ignores); i++)
		if (sigaction(signo_ignores[i], &sa, &sa_save[i]))
			ohshite(_("unable to ignore signal %s before running %.250s"),
			        strsignal(signo_ignores[i]), name);
	push_cleanup(subproc_signals_cleanup, ~0, NULL, 0, 0);
	onerr_abort--;
}

void
subproc_signals_cleanup(int argc, void **argv)
{
	size_t i;

	for (i = 0; i < array_count(signo_ignores); i++) {
		if (sigaction(signo_ignores[i], &sa_save[i], NULL)) {
			fprintf(stderr, _("error un-catching signal %s: %s\n"),
			        strsignal(signo_ignores[i]), strerror(errno));
			onerr_abort++;
		}
	}
}

static void
print_subproc_error(const char *emsg, const char *contextstring)
{
	fprintf(stderr, _("%s (subprocess): %s\n"), dpkg_get_progname(), emsg);
}

pid_t
subproc_fork(void)
{
	pid_t pid;

	pid = fork();
	if (pid == -1) {
		onerr_abort++;
		ohshite(_("fork failed"));
	}
	if (pid > 0)
		return pid;

	/* Push a new error context, so that we don't do the other cleanups,
	 * because they'll be done by/in the parent process. */
	push_error_context_func(catch_fatal_error, print_subproc_error, NULL);

	return pid;
}

int
subproc_check(int status, const char *desc, int flags)
{
	void (*out)(const char *fmt, ...) DPKG_ATTR_PRINTF(1);
	int n;

	if (flags & PROCWARN)
		out = warning;
	else
		out = ohshit;

	if (WIFEXITED(status)) {
		n = WEXITSTATUS(status);
		if (!n)
			return 0;
		if (flags & PROCNOERR)
			return n;

		out(_("subprocess %s returned error exit status %d"), desc, n);
	} else if (WIFSIGNALED(status)) {
		n = WTERMSIG(status);
		if (!n)
			return 0;
		if ((flags & PROCPIPE) && n == SIGPIPE)
			return 0;

		if (n == SIGINT)
			out(_("subprocess %s was interrupted"), desc);
		else
			out(_("subprocess %s was killed by signal (%s)%s"),
			    desc, strsignal(n),
			    WCOREDUMP(status) ? _(", core dumped") : "");
	} else {
		out(_("subprocess %s failed with wait status code %d"), desc,
		    status);
	}

	return -1;
}

int
subproc_wait(pid_t pid, const char *desc)
{
	pid_t r;
	int status;

	while ((r = waitpid(pid, &status, 0)) == -1 && errno == EINTR) ;

	if (r != pid) {
		onerr_abort++;
		ohshite(_("wait for subprocess %s failed"), desc);
	}

	return status;
}

int
subproc_wait_check(pid_t pid, const char *desc, int flags)
{
	int status;

	status = subproc_wait(pid, desc);

	return subproc_check(status, desc, flags);
}
