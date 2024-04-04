/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2011 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>

#include <systemd/sd-journal.h>

#include "log.h"
#include "util.h"
#include "path-util.h"
#include "build.h"
#include "pager.h"
#include "logs-show.h"

static OutputMode arg_output = OUTPUT_SHORT;
static bool arg_follow = false;
static bool arg_show_all = false;
static bool arg_no_pager = false;
static int arg_lines = -1;
static bool arg_no_tail = false;
static bool arg_new_id128 = false;
static bool arg_quiet = false;
static bool arg_local = false;

static int help(void) {

        printf("%s [OPTIONS...] [MATCH]\n\n"
               "Send control commands to or query the journal.\n\n"
               "  -h --help           Show this help\n"
               "     --version        Show package version\n"
               "     --no-pager       Do not pipe output into a pager\n"
               "  -a --all            Show all fields, including long and unprintable\n"
               "  -f --follow         Follow journal\n"
               "  -n --lines=INTEGER  Journal entries to show\n"
               "     --no-tail        Show all lines, even in follow mode\n"
               "  -o --output=STRING  Change journal output mode (short, short-monotonic,\n"
               "                      verbose, export, json, cat)\n"
               "  -q --quiet          Don't show privilege warning\n"
               "     --new-id128      Generate a new 128 Bit id\n"
               "  -l --local          Only local entries\n",
               program_invocation_short_name);

        return 0;
}

static int parse_argv(int argc, char *argv[]) {

        enum {
                ARG_VERSION = 0x100,
                ARG_NO_PAGER,
                ARG_NO_TAIL,
                ARG_NEW_ID128
        };

        static const struct option options[] = {
                { "help",      no_argument,       NULL, 'h'           },
                { "version" ,  no_argument,       NULL, ARG_VERSION   },
                { "no-pager",  no_argument,       NULL, ARG_NO_PAGER  },
                { "follow",    no_argument,       NULL, 'f'           },
                { "output",    required_argument, NULL, 'o'           },
                { "all",       no_argument,       NULL, 'a'           },
                { "lines",     required_argument, NULL, 'n'           },
                { "no-tail",   no_argument,       NULL, ARG_NO_TAIL   },
                { "new-id128", no_argument,       NULL, ARG_NEW_ID128 },
                { "quiet",     no_argument,       NULL, 'q'           },
                { "local",     no_argument,       NULL, 'l'           },
                { NULL,        0,                 NULL, 0             }
        };

        int c, r;

        assert(argc >= 0);
        assert(argv);

        while ((c = getopt_long(argc, argv, "hfo:an:ql", options, NULL)) >= 0) {

                switch (c) {

                case 'h':
                        help();
                        return 0;

                case ARG_VERSION:
                        puts(PACKAGE_STRING);
                        puts(DISTRIBUTION);
                        puts(SYSTEMD_FEATURES);
                        return 0;

                case ARG_NO_PAGER:
                        arg_no_pager = true;
                        break;

                case 'f':
                        arg_follow = true;
                        break;

                case 'o':
                        arg_output =  output_mode_from_string(optarg);
                        if (arg_output < 0) {
                                log_error("Unknown output '%s'.", optarg);
                                return -EINVAL;
                        }

                        break;

                case 'a':
                        arg_show_all = true;
                        break;

                case 'n':
                        r = safe_atoi(optarg, &arg_lines);
                        if (r < 0 || arg_lines < 0) {
                                log_error("Failed to parse lines '%s'", optarg);
                                return -EINVAL;
                        }
                        break;

                case ARG_NO_TAIL:
                        arg_no_tail = true;
                        break;

                case ARG_NEW_ID128:
                        arg_new_id128 = true;
                        break;

                case 'q':
                        arg_quiet = true;
                        break;

                case 'l':
                        arg_local = true;
                        break;

                case '?':
                        return -EINVAL;

                default:
                        log_error("Unknown option code %c", c);
                        return -EINVAL;
                }
        }

        if (arg_follow && !arg_no_tail && arg_lines < 0)
                arg_lines = 10;

        return 1;
}

static int generate_new_id128(void) {
        sd_id128_t id;
        int r;
        unsigned i;

        r = sd_id128_randomize(&id);
        if (r < 0) {
                log_error("Failed to generate ID: %s", strerror(-r));
                return r;
        }

        printf("As string:\n"
               SD_ID128_FORMAT_STR "\n\n"
               "As UUID:\n"
               "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\n"
               "As macro:\n"
              "#define MESSAGE_XYZ SD_ID128_MAKE(",
               SD_ID128_FORMAT_VAL(id),
               SD_ID128_FORMAT_VAL(id));

        for (i = 0; i < 16; i++)
                printf("%02x%s", id.bytes[i], i != 15 ? "," : "");

        fputs(")\n", stdout);

        return 0;
}

int main(int argc, char *argv[]) {
        int r, i, fd;
        sd_journal *j = NULL;
        unsigned line = 0;
        bool need_seek = false;
        struct stat st;

        log_parse_environment();
        log_open();

        r = parse_argv(argc, argv);
        if (r <= 0)
                goto finish;

        if (arg_new_id128) {
                r = generate_new_id128();
                goto finish;
        }

#ifdef HAVE_ACL
        if (!arg_quiet && geteuid() != 0 && in_group("adm") <= 0)
                log_warning("Showing user generated messages only. Users in the group 'adm' can see all messages. Pass -q to turn this message off.");
#endif

        r = sd_journal_open(&j, arg_local ? SD_JOURNAL_LOCAL_ONLY : 0);
        if (r < 0) {
                log_error("Failed to open journal: %s", strerror(-r));
                goto finish;
        }

        for (i = optind; i < argc; i++) {
                if (path_is_absolute(argv[i])) {
                        char *p = NULL;
                        const char *path;

                        p = canonicalize_file_name(argv[i]);
                        path = p ? p : argv[i];

                        if (stat(path, &st) < 0)  {
                                free(p);
                                log_error("Couldn't stat file: %m");
                                r = -errno;
                                goto finish;
                        }

                        if (S_ISREG(st.st_mode) && (0111 & st.st_mode)) {
                                char *t;

                                t = strappend("_EXE=", path);
                                if (!t) {
                                        free(p);
                                        log_error("Out of memory");
                                        goto finish;
                                }

                                r = sd_journal_add_match(j, t, strlen(t));
                                free(t);
                        } else {
                                free(p);
                                log_error("File is not a regular file or is not executable: %s", argv[i]);
                                goto finish;
                        }

                        free(p);
                } else
                        r = sd_journal_add_match(j, argv[i], strlen(argv[i]));

                if (r < 0) {
                        log_error("Failed to add match: %s", strerror(-r));
                        goto finish;
                }
        }

        fd = sd_journal_get_fd(j);
        if (fd < 0) {
                log_error("Failed to get wakeup fd: %s", strerror(-fd));
                goto finish;
        }

        if (!arg_quiet) {
                usec_t start, end;
                char start_buf[FORMAT_TIMESTAMP_MAX], end_buf[FORMAT_TIMESTAMP_MAX];

                r = sd_journal_get_cutoff_realtime_usec(j, &start, &end);
                if (r < 0) {
                        log_error("Failed to get cutoff: %s", strerror(-r));
                        goto finish;
                }

                if (r > 0) {
                        if (arg_follow)
                                printf("Logs begin at %s.\n", format_timestamp(start_buf, sizeof(start_buf), start));
                        else
                                printf("Logs begin at %s, end at %s.\n",
                                       format_timestamp(start_buf, sizeof(start_buf), start),
                                       format_timestamp(end_buf, sizeof(end_buf), end));
                }
        }

        if (arg_lines >= 0) {
                r = sd_journal_seek_tail(j);
                if (r < 0) {
                        log_error("Failed to seek to tail: %s", strerror(-r));
                        goto finish;
                }

                r = sd_journal_previous_skip(j, arg_lines);
        } else {
                r = sd_journal_seek_head(j);
                if (r < 0) {
                        log_error("Failed to seek to head: %s", strerror(-r));
                        goto finish;
                }

                r = sd_journal_next(j);
        }

        if (r < 0) {
                log_error("Failed to iterate through journal: %s", strerror(-r));
                goto finish;
        }

        if (!arg_no_pager && !arg_follow) {
                columns();
                pager_open();
        }

        if (arg_output == OUTPUT_JSON) {
                fputc('[', stdout);
                fflush(stdout);
        }

        for (;;) {
                for (;;) {
                        if (need_seek) {
                                r = sd_journal_next(j);
                                if (r < 0) {
                                        log_error("Failed to iterate through journal: %s", strerror(-r));
                                        goto finish;
                                }
                        }

                        if (r == 0)
                                break;

                        line ++;

                        r = output_journal(j, arg_output, line, 0, arg_show_all);
                        if (r < 0)
                                goto finish;

                        need_seek = true;
                }

                if (!arg_follow)
                        break;

                r = fd_wait_for_event(fd, POLLIN, (usec_t) -1);
                if (r < 0) {
                        log_error("Couldn't wait for event: %s", strerror(-r));
                        goto finish;
                }

                r = sd_journal_process(j);
                if (r < 0) {
                        log_error("Failed to process: %s", strerror(-r));
                        goto finish;
                }
        }

        if (arg_output == OUTPUT_JSON)
                fputs("\n]\n", stdout);

finish:
        if (j)
                sd_journal_close(j);

        pager_close();

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
