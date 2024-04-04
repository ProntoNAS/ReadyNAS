/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering
  Copyright 2013 Thomas H.P. Andersen

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <sys/personality.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "alloc-util.h"
#include "architecture.h"
#include "log.h"
#include "macro.h"
#include "process-util.h"
#include "string-util.h"
#include "terminal-util.h"
#include "util.h"
#include "virt.h"

static void test_get_process_comm(void) {
        struct stat st;
        _cleanup_free_ char *a = NULL, *c = NULL, *d = NULL, *f = NULL, *i = NULL, *cwd = NULL, *root = NULL;
        _cleanup_free_ char *env = NULL;
        pid_t e;
        uid_t u;
        gid_t g;
        dev_t h;
        int r;
        pid_t me;

        if (stat("/proc/1/comm", &st) == 0) {
                assert_se(get_process_comm(1, &a) >= 0);
                log_info("pid1 comm: '%s'", a);
        } else
                log_warning("/proc/1/comm does not exist.");

        assert_se(get_process_cmdline(1, 0, true, &c) >= 0);
        log_info("pid1 cmdline: '%s'", c);

        assert_se(get_process_cmdline(1, 8, false, &d) >= 0);
        log_info("pid1 cmdline truncated: '%s'", d);

        assert_se(get_process_ppid(1, &e) >= 0);
        log_info("pid1 ppid: "PID_FMT, e);
        assert_se(e == 0);

        assert_se(is_kernel_thread(1) == 0);

        r = get_process_exe(1, &f);
        assert_se(r >= 0 || r == -EACCES);
        log_info("pid1 exe: '%s'", strna(f));

        assert_se(get_process_uid(1, &u) == 0);
        log_info("pid1 uid: "UID_FMT, u);
        assert_se(u == 0);

        assert_se(get_process_gid(1, &g) == 0);
        log_info("pid1 gid: "GID_FMT, g);
        assert_se(g == 0);

        me = getpid();

        r = get_process_cwd(me, &cwd);
        assert_se(r >= 0 || r == -EACCES);
        log_info("pid1 cwd: '%s'", cwd);

        r = get_process_root(me, &root);
        assert_se(r >= 0 || r == -EACCES);
        log_info("pid1 root: '%s'", root);

        r = get_process_environ(me, &env);
        assert_se(r >= 0 || r == -EACCES);
        log_info("self strlen(environ): '%zu'", strlen(env));

        if (!detect_container())
                assert_se(get_ctty_devnr(1, &h) == -ENXIO);

        getenv_for_pid(1, "PATH", &i);
        log_info("pid1 $PATH: '%s'", strna(i));
}

static void test_pid_is_unwaited(void) {
        pid_t pid;

        pid = fork();
        assert_se(pid >= 0);
        if (pid == 0) {
                _exit(EXIT_SUCCESS);
        } else {
                int status;

                waitpid(pid, &status, 0);
                assert_se(!pid_is_unwaited(pid));
        }
        assert_se(pid_is_unwaited(getpid()));
        assert_se(!pid_is_unwaited(-1));
}

static void test_pid_is_alive(void) {
        pid_t pid;

        pid = fork();
        assert_se(pid >= 0);
        if (pid == 0) {
                _exit(EXIT_SUCCESS);
        } else {
                int status;

                waitpid(pid, &status, 0);
                assert_se(!pid_is_alive(pid));
        }
        assert_se(pid_is_alive(getpid()));
        assert_se(!pid_is_alive(-1));
}

static void test_personality(void) {

        assert_se(personality_to_string(PER_LINUX));
        assert_se(!personality_to_string(PERSONALITY_INVALID));

        assert_se(streq(personality_to_string(PER_LINUX), architecture_to_string(native_architecture())));

        assert_se(personality_from_string(personality_to_string(PER_LINUX)) == PER_LINUX);
        assert_se(personality_from_string(architecture_to_string(native_architecture())) == PER_LINUX);

#ifdef __x86_64__
        assert_se(streq_ptr(personality_to_string(PER_LINUX), "x86-64"));
        assert_se(streq_ptr(personality_to_string(PER_LINUX32), "x86"));

        assert_se(personality_from_string("x86-64") == PER_LINUX);
        assert_se(personality_from_string("x86") == PER_LINUX32);
        assert_se(personality_from_string("ia64") == PERSONALITY_INVALID);
        assert_se(personality_from_string(NULL) == PERSONALITY_INVALID);

        assert_se(personality_from_string(personality_to_string(PER_LINUX32)) == PER_LINUX32);
#endif
}

int main(int argc, char *argv[]) {
        log_parse_environment();
        log_open();

        test_get_process_comm();
        test_pid_is_unwaited();
        test_pid_is_alive();
        test_personality();

        return 0;
}
