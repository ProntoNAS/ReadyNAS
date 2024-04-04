/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

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

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "util.h"
#include "unit-name.h"
#include "mkdir.h"

#define TIMEOUT_SEC 600

const char *arg_dest = "/tmp";

static bool has_option(const char *haystack, const char *needle) {
        const char *f = haystack;
        size_t l;

        assert(needle);

        if (!haystack)
                return false;

        l = strlen(needle);

        while ((f = strstr(f, needle))) {

                if (f > haystack && f[-1] != ',') {
                        f++;
                        continue;
                }

                if (f[l] != 0 && f[l] != ',') {
                        f++;
                        continue;
                }

                return true;
        }

        return false;
}

static int create_keysearch(const char *name, const char *volname, const char *keyfile) {
        char *p, *n = NULL, *to = NULL, *from = NULL;
        FILE *f;
        int r = 0;

        assert(name);
        assert(volname);

        mkdir_parents_label(keyfile, 0755);

        p = join(arg_dest, "/", volname, "-key.service", NULL);
        if (!p) {
                log_error("Failed to allocate unit file name.");
                return -ENOMEM;
        }

        f = fopen(p, "wxe");
        if (!f) {
		if (errno == EEXIST)
			goto make_symlinks;
                log_error("Failed to create unit file: %m");
                r = -errno;
                goto fail2;
        }

        fprintf(f,
                "[Unit]\n"
                "Description=Cryptography Key Search for %s\n"
                "DefaultDependencies=no\n"
                "Before=local-fs-pre.target\n",
                volname);

        fprintf(f,
                "\n[Service]\n"
                "Type=oneshot\n"
                "ExecStart=/usr/bin/rnutil search_for_key '%s' '%s' %d\n",
                volname, keyfile, TIMEOUT_SEC);

        fflush(f);

        if (ferror(f)) {
                log_error("Failed to write file: %m");
                r = -errno;
                fclose(f);
                goto fail2;
        }

        fclose(f);

make_symlinks:
        if (asprintf(&from, "../%s-key.service", volname) < 0) {
                r = -ENOMEM;
                goto fail2;
        }

        n = unit_name_from_path_instance("cryptsetup", name, ".service");
        if (!n) {
                r = -ENOMEM;
                log_error("Failed to allocate unit name.");
                goto fail2;
        }

        to = join(arg_dest, "/", n, ".requires/", volname, "-key.service", NULL);
        if (!to) {
                r = -ENOMEM;
                goto fail2;
        }

        mkdir_parents_label(to, 0755);
        if (symlink(from, to) < 0) {
                log_error("Failed to create symlink '%s' to '%s': %m", from, to);
                r = -errno;
                goto fail2;
        }
fail2:
	free(p);
	free(n);
	free(to);
	free(from);

	return r;
}

static int create_disk(
                const char *name,
                const char *device,
                const char *password,
                const char *options,
                const char *volname) {

        char *p = NULL, *n = NULL, *d = NULL, *u = NULL, *from = NULL, *to = NULL, *e = NULL;
        int r;
        FILE *f = NULL;
        bool noauto, nofail;

        assert(name);
        assert(device);

        noauto = has_option(options, "noauto");
        nofail = has_option(options, "nofail");

        n = unit_name_from_path_instance("cryptsetup", name, ".service");
        if (!n) {
                r = -ENOMEM;
                log_error("Failed to allocate unit name.");
                goto fail;
        }

        p = join(arg_dest, "/", n, NULL);
        if (!p) {
                r = -ENOMEM;
                log_error("Failed to allocate unit file name.");
                goto fail;
        }

        u = fstab_node_to_udev_node(device);
        if (!u) {
                r = -ENOMEM;
                log_error("Failed to allocate device node.");
                goto fail;
        }

        d = unit_name_from_path(u, ".device");
        if (!d) {
                r = -ENOMEM;
                log_error("Failed to allocate device name.");
                goto fail;
        }

        f = fopen(p, "wxe");
        if (!f) {
                r = -errno;
                log_error("Failed to create unit file: %m");
                goto fail;
        }

        fprintf(f,
                "[Unit]\n"
                "Description=Cryptography Setup for %%I\n"
                "Conflicts=umount.target\n"
                "DefaultDependencies=no\n"
                "BindsTo=%s dev-mapper-%%i.device\n"
                "After=systemd-readahead-collect.service systemd-readahead-replay.service %s %s-key.service\n"
                "Before=umount.target local-fs-pre.target\n"
                "JobTimeoutSec=%d\n",
                d, d, volname, TIMEOUT_SEC);

        if (!nofail)
                fprintf(f,
                        "Before=cryptsetup.target\n");

        if (password && (streq(password, "/dev/urandom") ||
                         streq(password, "/dev/random") ||
                         streq(password, "/dev/hw_random")))
                fprintf(f,
                        "After=systemd-random-seed-load.service\n");
        else
                fprintf(f,
                        "Before=local-fs.target\n");

        fprintf(f,
                "\n[Service]\n"
                "Type=oneshot\n"
                "RemainAfterExit=yes\n"
                "TimeoutSec=0\n" /* the binary handles timeouts anyway */
                "ExecStart=" SYSTEMD_CRYPTSETUP_PATH " attach '%s' '%s' '%s' '%s' ; /sbin/btrfs device scan '/dev/mapper/%s'\n"
                "ExecStop=" SYSTEMD_CRYPTSETUP_PATH " detach '%s'\n",
                name, u, strempty(password), strempty(options),
                name, name);

        if (has_option(options, "tmp"))
                fprintf(f,
                        "ExecStartPost=/sbin/mke2fs '/dev/mapper/%s'\n",
                        name);

        if (has_option(options, "swap"))
                fprintf(f,
                        "ExecStartPost=/sbin/mkswap '/dev/mapper/%s'\n",
                        name);

        fflush(f);

        if (ferror(f)) {
                r = -errno;
                log_error("Failed to write file: %m");
                goto fail;
        }

        if (asprintf(&from, "../%s", n) < 0) {
                r = -ENOMEM;
                goto fail;
        }

        if (!noauto) {

                to = join(arg_dest, "/", d, ".wants/", n, NULL);
                if (!to) {
                        r = -ENOMEM;
                        goto fail;
                }

                mkdir_parents_label(to, 0755);
                if (symlink(from, to) < 0) {
                        log_error("Failed to create symlink '%s' to '%s': %m", from, to);
                        r = -errno;
                        goto fail;
                }

                free(to);

                if (!nofail)
                        to = join(arg_dest, "/cryptsetup.target.requires/", n, NULL);
                else
                        to = join(arg_dest, "/cryptsetup.target.wants/", n, NULL);
                if (!to) {
                        r = -ENOMEM;
                        goto fail;
                }

                mkdir_parents_label(to, 0755);
                if (symlink(from, to) < 0) {
                        log_error("Failed to create symlink '%s' to '%s': %m", from, to);
                        r = -errno;
                        goto fail;
                }

                free(to);
                to = NULL;
        }

        e = unit_name_escape(name);
        to = join(arg_dest, "/dev-mapper-", e, ".device.requires/", n, NULL);
        if (!to) {
                r = -ENOMEM;
                goto fail;
        }

        mkdir_parents_label(to, 0755);
        if (symlink(from, to) < 0) {
                log_error("Failed to create symlink '%s' to '%s': %m", from, to);
                r = -errno;
                goto fail;
        }

        r = 0;

fail:
        free(p);
        free(n);
        free(d);
        free(e);

        free(from);
        free(to);

        if (f)
                fclose(f);

        return r;
}

int main(int argc, char *argv[]) {
        FILE *f;
        int r = EXIT_SUCCESS;
        unsigned n = 0;

        if (argc > 2) {
                log_error("This program takes one or no arguments.");
                return EXIT_FAILURE;
        }

        if (argc > 1)
                arg_dest = argv[1];

        log_set_target(LOG_TARGET_SAFE);
        log_parse_environment();
        log_open();

        umask(0022);

        f = fopen("/etc/crypttab", "re");
        if (!f) {

                if (errno == ENOENT)
                        r = EXIT_SUCCESS;
                else {
                        r = EXIT_FAILURE;
                        log_error("Failed to open /etc/crypttab: %m");
                }

                goto finish;
        }

        for (;;) {
                char line[LINE_MAX], *l;
                char *name = NULL, *device = NULL, *password = NULL, *options = NULL;
                char *volname;
                int k;

                if (!fgets(line, sizeof(line), f))
                        break;

                n++;

                l = strstrip(line);
                if (*l == '#' || *l == 0)
                        continue;

                k = sscanf(l, "%ms %ms %ms %ms", &name, &device, &password, &options);
                if (k < 2 || k > 4) {
                        log_error("Failed to parse /etc/crypttab:%u, ignoring.", n);
                        r = EXIT_FAILURE;
                        goto next;
                }

                volname = strdup(name);
                if (!volname)
                        goto next;
                l = strrchr(volname, '-');
                if (l)
                        *l = '\0';

                if (strcmp(password, "search") == 0)
                {
                        free(password);
                        if (asprintf(&password, "/run/systemd/cryptsetup/%s.key", volname) < 0) {
                                r = -ENOMEM;
                                log_error("Failed to allocate key file path.");
                                goto next;
                        }
                        if (create_keysearch(name, volname, password) < 0)
                                r = EXIT_FAILURE;
                }

                if (create_disk(name, device, password, options, volname) < 0)
                        r = EXIT_FAILURE;

		free(volname);
        next:
                free(name);
                free(device);
                free(password);
                free(options);
        }

finish:
        return r;
}
