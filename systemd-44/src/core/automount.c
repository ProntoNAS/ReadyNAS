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

#include <errno.h>
#include <limits.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <linux/auto_fs4.h>
#include <linux/auto_dev-ioctl.h>

#include "unit.h"
#include "automount.h"
#include "load-fragment.h"
#include "load-dropin.h"
#include "unit-name.h"
#include "dbus-automount.h"
#include "bus-errors.h"
#include "special.h"
#include "label.h"
#include "mkdir.h"
#include "path-util.h"

static const UnitActiveState state_translation_table[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD] = UNIT_INACTIVE,
        [AUTOMOUNT_WAITING] = UNIT_ACTIVE,
        [AUTOMOUNT_RUNNING] = UNIT_ACTIVE,
        [AUTOMOUNT_FAILED] = UNIT_FAILED
};

static int open_dev_autofs(Manager *m);

static void automount_init(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(u);
        assert(u->load_state == UNIT_STUB);

        a->pipe_watch.fd = a->pipe_fd = -1;
        a->pipe_watch.type = WATCH_INVALID;

        a->directory_mode = 0755;

        UNIT(a)->ignore_on_isolate = true;
}

static void repeat_unmout(const char *path) {
        assert(path);

        for (;;) {
                /* If there are multiple mounts on a mount point, this
                 * removes them all */

                if (umount2(path, MNT_DETACH) >= 0)
                        continue;

                if (errno != EINVAL)
                        log_error("Failed to unmount: %m");

                break;
        }
}

static void unmount_autofs(Automount *a) {
        assert(a);

        if (a->pipe_fd < 0)
                return;

        automount_send_ready(a, -EHOSTDOWN);

        unit_unwatch_fd(UNIT(a), &a->pipe_watch);
        close_nointr_nofail(a->pipe_fd);
        a->pipe_fd = -1;

        /* If we reload/reexecute things we keep the mount point
         * around */
        if (a->where &&
            (UNIT(a)->manager->exit_code != MANAGER_RELOAD &&
             UNIT(a)->manager->exit_code != MANAGER_REEXECUTE))
                repeat_unmout(a->where);
}

static void automount_done(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        unmount_autofs(a);
        unit_ref_unset(&a->mount);

        free(a->where);
        a->where = NULL;

        set_free(a->tokens);
        a->tokens = NULL;
}

int automount_add_one_mount_link(Automount *a, Mount *m) {
        int r;

        assert(a);
        assert(m);

        if (UNIT(a)->load_state != UNIT_LOADED ||
            UNIT(m)->load_state != UNIT_LOADED)
                return 0;

        if (!path_startswith(a->where, m->where))
                return 0;

        if (path_equal(a->where, m->where))
                return 0;

        if ((r = unit_add_two_dependencies(UNIT(a), UNIT_AFTER, UNIT_REQUIRES, UNIT(m), true)) < 0)
                return r;

        return 0;
}

static int automount_add_mount_links(Automount *a) {
        Unit *other;
        int r;

        assert(a);

        LIST_FOREACH(units_by_type, other, UNIT(a)->manager->units_by_type[UNIT_MOUNT])
                if ((r = automount_add_one_mount_link(a, MOUNT(other))) < 0)
                        return r;

        return 0;
}

static int automount_add_default_dependencies(Automount *a) {
        int r;

        assert(a);

        if (UNIT(a)->manager->running_as == MANAGER_SYSTEM) {

                if ((r = unit_add_dependency_by_name(UNIT(a), UNIT_BEFORE, SPECIAL_BASIC_TARGET, NULL, true)) < 0)
                        return r;

                if ((r = unit_add_two_dependencies_by_name(UNIT(a), UNIT_BEFORE, UNIT_CONFLICTS, SPECIAL_UMOUNT_TARGET, NULL, true)) < 0)
                        return r;
        }

        return 0;
}

static int automount_verify(Automount *a) {
        bool b;
        char *e;
        assert(a);

        if (UNIT(a)->load_state != UNIT_LOADED)
                return 0;

        if (path_equal(a->where, "/")) {
                log_error("Cannot have an automount unit for the root directory. Refusing.");
                return -EINVAL;
        }

        if (!(e = unit_name_from_path(a->where, ".automount")))
                return -ENOMEM;

        b = unit_has_name(UNIT(a), e);
        free(e);

        if (!b) {
                log_error("%s's Where setting doesn't match unit name. Refusing.", UNIT(a)->id);
                return -EINVAL;
        }

        return 0;
}

static int automount_load(Unit *u) {
        int r;
        Automount *a = AUTOMOUNT(u);

        assert(u);
        assert(u->load_state == UNIT_STUB);

        /* Load a .automount file */
        if ((r = unit_load_fragment_and_dropin_optional(u)) < 0)
                return r;

        if (u->load_state == UNIT_LOADED) {
                Unit *x;

                if (!a->where)
                        if (!(a->where = unit_name_to_path(u->id)))
                                return -ENOMEM;

                path_kill_slashes(a->where);

                if ((r = automount_add_mount_links(a)) < 0)
                        return r;

                r = unit_load_related_unit(u, ".mount", &x);
                if (r < 0)
                        return r;

                unit_ref_set(&a->mount, x);

                r = unit_add_two_dependencies(u, UNIT_BEFORE, UNIT_TRIGGERS, UNIT_DEREF(a->mount), true);
                if (r < 0)
                        return r;

                if (UNIT(a)->default_dependencies)
                        if ((r = automount_add_default_dependencies(a)) < 0)
                                return r;
        }

        return automount_verify(a);
}

static void automount_set_state(Automount *a, AutomountState state) {
        AutomountState old_state;
        assert(a);

        old_state = a->state;
        a->state = state;

        if (state != AUTOMOUNT_WAITING &&
            state != AUTOMOUNT_RUNNING)
                unmount_autofs(a);

        if (state != old_state)
                log_debug("%s changed %s -> %s",
                          UNIT(a)->id,
                          automount_state_to_string(old_state),
                          automount_state_to_string(state));

        unit_notify(UNIT(a), state_translation_table[old_state], state_translation_table[state], true);
}

static int automount_coldplug(Unit *u) {
        Automount *a = AUTOMOUNT(u);
        int r;

        assert(a);
        assert(a->state == AUTOMOUNT_DEAD);

        if (a->deserialized_state != a->state) {

                if ((r = open_dev_autofs(u->manager)) < 0)
                        return r;

                if (a->deserialized_state == AUTOMOUNT_WAITING ||
                    a->deserialized_state == AUTOMOUNT_RUNNING) {

                        assert(a->pipe_fd >= 0);

                        if ((r = unit_watch_fd(UNIT(a), a->pipe_fd, EPOLLIN, &a->pipe_watch)) < 0)
                                return r;
                }

                automount_set_state(a, a->deserialized_state);
        }

        return 0;
}

static void automount_dump(Unit *u, FILE *f, const char *prefix) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        fprintf(f,
                "%sAutomount State: %s\n"
                "%sResult: %s\n"
                "%sWhere: %s\n"
                "%sDirectoryMode: %04o\n",
                prefix, automount_state_to_string(a->state),
                prefix, automount_result_to_string(a->result),
                prefix, a->where,
                prefix, a->directory_mode);
}

static void automount_enter_dead(Automount *a, AutomountResult f) {
        assert(a);

        if (f != AUTOMOUNT_SUCCESS)
                a->result = f;

        automount_set_state(a, a->result != AUTOMOUNT_SUCCESS ? AUTOMOUNT_FAILED : AUTOMOUNT_DEAD);
}

static int open_dev_autofs(Manager *m) {
        struct autofs_dev_ioctl param;

        assert(m);

        if (m->dev_autofs_fd >= 0)
                return m->dev_autofs_fd;

        label_fix("/dev/autofs", false, false);

        if ((m->dev_autofs_fd = open("/dev/autofs", O_CLOEXEC|O_RDONLY)) < 0) {
                log_error("Failed to open /dev/autofs: %s", strerror(errno));
                return -errno;
        }

        init_autofs_dev_ioctl(&param);
        if (ioctl(m->dev_autofs_fd, AUTOFS_DEV_IOCTL_VERSION, &param) < 0) {
                close_nointr_nofail(m->dev_autofs_fd);
                m->dev_autofs_fd = -1;
                return -errno;
        }

        log_debug("Autofs kernel version %i.%i", param.ver_major, param.ver_minor);

        return m->dev_autofs_fd;
}

static int open_ioctl_fd(int dev_autofs_fd, const char *where, dev_t devid) {
        struct autofs_dev_ioctl *param;
        size_t l;
        int r;

        assert(dev_autofs_fd >= 0);
        assert(where);

        l = sizeof(struct autofs_dev_ioctl) + strlen(where) + 1;

        if (!(param = malloc(l)))
                return -ENOMEM;

        init_autofs_dev_ioctl(param);
        param->size = l;
        param->ioctlfd = -1;
        param->openmount.devid = devid;
        strcpy(param->path, where);

        if (ioctl(dev_autofs_fd, AUTOFS_DEV_IOCTL_OPENMOUNT, param) < 0) {
                r = -errno;
                goto finish;
        }

        if (param->ioctlfd < 0) {
                r = -EIO;
                goto finish;
        }

        fd_cloexec(param->ioctlfd, true);
        r = param->ioctlfd;

finish:
        free(param);
        return r;
}

static int autofs_protocol(int dev_autofs_fd, int ioctl_fd) {
        uint32_t major, minor;
        struct autofs_dev_ioctl param;

        assert(dev_autofs_fd >= 0);
        assert(ioctl_fd >= 0);

        init_autofs_dev_ioctl(&param);
        param.ioctlfd = ioctl_fd;

        if (ioctl(dev_autofs_fd, AUTOFS_DEV_IOCTL_PROTOVER, &param) < 0)
                return -errno;

        major = param.protover.version;

        init_autofs_dev_ioctl(&param);
        param.ioctlfd = ioctl_fd;

        if (ioctl(dev_autofs_fd, AUTOFS_DEV_IOCTL_PROTOSUBVER, &param) < 0)
                return -errno;

        minor = param.protosubver.sub_version;

        log_debug("Autofs protocol version %i.%i", major, minor);
        return 0;
}

static int autofs_set_timeout(int dev_autofs_fd, int ioctl_fd, time_t sec) {
        struct autofs_dev_ioctl param;

        assert(dev_autofs_fd >= 0);
        assert(ioctl_fd >= 0);

        init_autofs_dev_ioctl(&param);
        param.ioctlfd = ioctl_fd;
        param.timeout.timeout = sec;

        if (ioctl(dev_autofs_fd, AUTOFS_DEV_IOCTL_TIMEOUT, &param) < 0)
                return -errno;

        return 0;
}

static int autofs_send_ready(int dev_autofs_fd, int ioctl_fd, uint32_t token, int status) {
        struct autofs_dev_ioctl param;

        assert(dev_autofs_fd >= 0);
        assert(ioctl_fd >= 0);

        init_autofs_dev_ioctl(&param);
        param.ioctlfd = ioctl_fd;

        if (status) {
                param.fail.token = token;
                param.fail.status = status;
        } else
                param.ready.token = token;

        if (ioctl(dev_autofs_fd, status ? AUTOFS_DEV_IOCTL_FAIL : AUTOFS_DEV_IOCTL_READY, &param) < 0)
                return -errno;

        return 0;
}

int automount_send_ready(Automount *a, int status) {
        int ioctl_fd, r;
        unsigned token;

        assert(a);
        assert(status <= 0);

        if (set_isempty(a->tokens))
                return 0;

        if ((ioctl_fd = open_ioctl_fd(UNIT(a)->manager->dev_autofs_fd, a->where, a->dev_id)) < 0) {
                r = ioctl_fd;
                goto fail;
        }

        if (status)
                log_debug("Sending failure: %s", strerror(-status));
        else
                log_debug("Sending success.");

        r = 0;

        /* Autofs thankfully does not hand out 0 as a token */
        while ((token = PTR_TO_UINT(set_steal_first(a->tokens)))) {
                int k;

                /* Autofs fun fact II:
                 *
                 * if you pass a positive status code here, the kernel will
                 * freeze! Yay! */

                if ((k = autofs_send_ready(UNIT(a)->manager->dev_autofs_fd,
                                           ioctl_fd,
                                           token,
                                           status)) < 0)
                        r = k;
        }

fail:
        if (ioctl_fd >= 0)
                close_nointr_nofail(ioctl_fd);

        return r;
}

static void automount_enter_waiting(Automount *a) {
        int p[2] = { -1, -1 };
        char name[32], options[128];
        bool mounted = false;
        int r, ioctl_fd = -1, dev_autofs_fd;
        struct stat st;

        assert(a);
        assert(a->pipe_fd < 0);
        assert(a->where);

        if (a->tokens)
                set_clear(a->tokens);

        if ((dev_autofs_fd = open_dev_autofs(UNIT(a)->manager)) < 0) {
                r = dev_autofs_fd;
                goto fail;
        }

        /* We knowingly ignore the results of this call */
        mkdir_p_label(a->where, 0555);

        if (dir_is_empty(a->where) <= 0)
                log_notice("%s: Directory %s to mount over is not empty, mounting anyway. (To see the over-mounted files, please manually mount the underlying file system to a secondary location.)", a->meta.id, a->where);

        if (pipe2(p, O_NONBLOCK|O_CLOEXEC) < 0) {
                r = -errno;
                goto fail;
        }

        snprintf(options, sizeof(options), "fd=%i,pgrp=%u,minproto=5,maxproto=5,direct", p[1], (unsigned) getpgrp());
        char_array_0(options);

        snprintf(name, sizeof(name), "systemd-%u", (unsigned) getpid());
        char_array_0(name);

        if (mount(name, a->where, "autofs", 0, options) < 0) {
                r = -errno;
                goto fail;
        }

        mounted = true;

        close_nointr_nofail(p[1]);
        p[1] = -1;

        if (stat(a->where, &st) < 0) {
                r = -errno;
                goto fail;
        }

        if ((ioctl_fd = open_ioctl_fd(dev_autofs_fd, a->where, st.st_dev)) < 0) {
                r = ioctl_fd;
                goto fail;
        }

        if ((r = autofs_protocol(dev_autofs_fd, ioctl_fd)) < 0)
                goto fail;

        if ((r = autofs_set_timeout(dev_autofs_fd, ioctl_fd, 300)) < 0)
                goto fail;

        /* Autofs fun fact:
         *
         * Unless we close the ioctl fd here, for some weird reason
         * the direct mount will not receive events from the
         * kernel. */

        close_nointr_nofail(ioctl_fd);
        ioctl_fd = -1;

        if ((r = unit_watch_fd(UNIT(a), p[0], EPOLLIN, &a->pipe_watch)) < 0)
                goto fail;

        a->pipe_fd = p[0];
        a->dev_id = st.st_dev;

        automount_set_state(a, AUTOMOUNT_WAITING);

        return;

fail:
        assert_se(close_pipe(p) == 0);

        if (ioctl_fd >= 0)
                close_nointr_nofail(ioctl_fd);

        if (mounted)
                repeat_unmout(a->where);

        log_error("Failed to initialize automounter: %s", strerror(-r));
        automount_enter_dead(a, AUTOMOUNT_FAILURE_RESOURCES);
}

static void automount_enter_runnning(Automount *a) {
        int r;
        struct stat st;
        DBusError error;

        assert(a);
        assert(UNIT_DEREF(a->mount));

        dbus_error_init(&error);

        /* We don't take mount requests anymore if we are supposed to
         * shut down anyway */
        if (unit_pending_inactive(UNIT(a))) {
                log_debug("Suppressing automount request on %s since unit stop is scheduled.", UNIT(a)->id);
                automount_send_ready(a, -EHOSTDOWN);
                return;
        }

        mkdir_p_label(a->where, a->directory_mode);

        /* Before we do anything, let's see if somebody is playing games with us? */
        if (lstat(a->where, &st) < 0) {
                log_warning("%s failed to stat automount point: %m", UNIT(a)->id);
                goto fail;
        }

        if (!S_ISDIR(st.st_mode) || st.st_dev != a->dev_id)
                log_info("%s's automount point already active?", UNIT(a)->id);
        else if ((r = manager_add_job(UNIT(a)->manager, JOB_START, UNIT_DEREF(a->mount), JOB_REPLACE, true, &error, NULL)) < 0) {
                log_warning("%s failed to queue mount startup job: %s", UNIT(a)->id, bus_error(&error, r));
                goto fail;
        }

        automount_set_state(a, AUTOMOUNT_RUNNING);
        return;

fail:
        automount_enter_dead(a, AUTOMOUNT_FAILURE_RESOURCES);
        dbus_error_free(&error);
}

static int automount_start(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        assert(a->state == AUTOMOUNT_DEAD || a->state == AUTOMOUNT_FAILED);

        if (path_is_mount_point(a->where, false)) {
                log_error("Path %s is already a mount point, refusing start for %s", a->where, u->id);
                return -EEXIST;
        }

        if (UNIT_DEREF(a->mount)->load_state != UNIT_LOADED)
                return -ENOENT;

        a->result = AUTOMOUNT_SUCCESS;
        automount_enter_waiting(a);
        return 0;
}

static int automount_stop(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        assert(a->state == AUTOMOUNT_WAITING || a->state == AUTOMOUNT_RUNNING);

        automount_enter_dead(a, AUTOMOUNT_SUCCESS);
        return 0;
}

static int automount_serialize(Unit *u, FILE *f, FDSet *fds) {
        Automount *a = AUTOMOUNT(u);
        void *p;
        Iterator i;

        assert(a);
        assert(f);
        assert(fds);

        unit_serialize_item(u, f, "state", automount_state_to_string(a->state));
        unit_serialize_item(u, f, "result", automount_result_to_string(a->result));
        unit_serialize_item_format(u, f, "dev-id", "%u", (unsigned) a->dev_id);

        SET_FOREACH(p, a->tokens, i)
                unit_serialize_item_format(u, f, "token", "%u", PTR_TO_UINT(p));

        if (a->pipe_fd >= 0) {
                int copy;

                if ((copy = fdset_put_dup(fds, a->pipe_fd)) < 0)
                        return copy;

                unit_serialize_item_format(u, f, "pipe-fd", "%i", copy);
        }

        return 0;
}

static int automount_deserialize_item(Unit *u, const char *key, const char *value, FDSet *fds) {
        Automount *a = AUTOMOUNT(u);
        int r;

        assert(a);
        assert(fds);

        if (streq(key, "state")) {
                AutomountState state;

                if ((state = automount_state_from_string(value)) < 0)
                        log_debug("Failed to parse state value %s", value);
                else
                        a->deserialized_state = state;
        } else if (streq(key, "result")) {
                AutomountResult f;

                f = automount_result_from_string(value);
                if (f < 0)
                        log_debug("Failed to parse result value %s", value);
                else if (f != AUTOMOUNT_SUCCESS)
                        a->result = f;

        } else if (streq(key, "dev-id")) {
                unsigned d;

                if (safe_atou(value, &d) < 0)
                        log_debug("Failed to parse dev-id value %s", value);
                else
                        a->dev_id = (unsigned) d;
        } else if (streq(key, "token")) {
                unsigned token;

                if (safe_atou(value, &token) < 0)
                        log_debug("Failed to parse token value %s", value);
                else {
                        if (!a->tokens)
                                if (!(a->tokens = set_new(trivial_hash_func, trivial_compare_func)))
                                        return -ENOMEM;

                        if ((r = set_put(a->tokens, UINT_TO_PTR(token))) < 0)
                                return r;
                }
        } else if (streq(key, "pipe-fd")) {
                int fd;

                if (safe_atoi(value, &fd) < 0 || fd < 0 || !fdset_contains(fds, fd))
                        log_debug("Failed to parse pipe-fd value %s", value);
                else {
                        if (a->pipe_fd >= 0)
                                close_nointr_nofail(a->pipe_fd);

                        a->pipe_fd = fdset_remove(fds, fd);
                }
        } else
                log_debug("Unknown serialization key '%s'", key);

        return 0;
}

static UnitActiveState automount_active_state(Unit *u) {
        assert(u);

        return state_translation_table[AUTOMOUNT(u)->state];
}

static const char *automount_sub_state_to_string(Unit *u) {
        assert(u);

        return automount_state_to_string(AUTOMOUNT(u)->state);
}

static bool automount_check_gc(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        if (!UNIT_DEREF(a->mount))
                return false;

        return UNIT_VTABLE(UNIT_DEREF(a->mount))->check_gc(UNIT_DEREF(a->mount));
}

static void automount_fd_event(Unit *u, int fd, uint32_t events, Watch *w) {
        Automount *a = AUTOMOUNT(u);
        union autofs_v5_packet_union packet;
        ssize_t l;
        int r;

        assert(a);
        assert(fd == a->pipe_fd);

        if (events != EPOLLIN) {
                log_error("Got invalid poll event on pipe.");
                goto fail;
        }

        if ((l = loop_read(a->pipe_fd, &packet, sizeof(packet), true)) != sizeof(packet)) {
                log_error("Invalid read from pipe: %s", l < 0 ? strerror(-l) : "short read");
                goto fail;
        }

        switch (packet.hdr.type) {

        case autofs_ptype_missing_direct:

                if (packet.v5_packet.pid > 0) {
                        char *p = NULL;

                        get_process_comm(packet.v5_packet.pid, &p);
                        log_debug("Got direct mount request on %s, triggered by %lu (%s)",
                                  a->where, (unsigned long) packet.v5_packet.pid, strna(p));
                        free(p);

                } else
                        log_debug("Got direct mount request on %s", a->where);

                if (!a->tokens)
                        if (!(a->tokens = set_new(trivial_hash_func, trivial_compare_func))) {
                                log_error("Failed to allocate token set.");
                                goto fail;
                        }

                if ((r = set_put(a->tokens, UINT_TO_PTR(packet.v5_packet.wait_queue_token))) < 0) {
                        log_error("Failed to remember token: %s", strerror(-r));
                        goto fail;
                }

                automount_enter_runnning(a);
                break;

        default:
                log_error("Received unknown automount request %i", packet.hdr.type);
                break;
        }

        return;

fail:
        automount_enter_dead(a, AUTOMOUNT_FAILURE_RESOURCES);
}

static void automount_shutdown(Manager *m) {
        assert(m);

        if (m->dev_autofs_fd >= 0)
                close_nointr_nofail(m->dev_autofs_fd);
}

static void automount_reset_failed(Unit *u) {
        Automount *a = AUTOMOUNT(u);

        assert(a);

        if (a->state == AUTOMOUNT_FAILED)
                automount_set_state(a, AUTOMOUNT_DEAD);

        a->result = AUTOMOUNT_SUCCESS;
}

static const char* const automount_state_table[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD] = "dead",
        [AUTOMOUNT_WAITING] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED] = "failed"
};

DEFINE_STRING_TABLE_LOOKUP(automount_state, AutomountState);

static const char* const automount_result_table[_AUTOMOUNT_RESULT_MAX] = {
        [AUTOMOUNT_SUCCESS] = "success",
        [AUTOMOUNT_FAILURE_RESOURCES] = "resources"
};

DEFINE_STRING_TABLE_LOOKUP(automount_result, AutomountResult);

const UnitVTable automount_vtable = {
        .object_size = sizeof(Automount),
        .sections =
                "Unit\0"
                "Automount\0"
                "Install\0",

        .no_alias = true,
        .no_instances = true,

        .init = automount_init,
        .load = automount_load,
        .done = automount_done,

        .coldplug = automount_coldplug,

        .dump = automount_dump,

        .start = automount_start,
        .stop = automount_stop,

        .serialize = automount_serialize,
        .deserialize_item = automount_deserialize_item,

        .active_state = automount_active_state,
        .sub_state_to_string = automount_sub_state_to_string,

        .check_gc = automount_check_gc,

        .fd_event = automount_fd_event,

        .reset_failed = automount_reset_failed,

        .bus_interface = "org.freedesktop.systemd1.Automount",
        .bus_message_handler = bus_automount_message_handler,
        .bus_invalidating_properties = bus_automount_invalidating_properties,

        .shutdown = automount_shutdown,

        .status_message_formats = {
                .finished_start_job = {
                        [JOB_DONE]       = "Set up automount %s.",
                        [JOB_FAILED]     = "Failed to set up automount %s.",
                        [JOB_DEPENDENCY] = "Dependency failed for %s.",
                },
                .finished_stop_job = {
                        [JOB_DONE]       = "Unset automount %s.",
                        [JOB_FAILED]     = "Failed to unset automount %s.",
                },
        },
};
