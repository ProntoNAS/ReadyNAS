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

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <printf.h>

#define SD_JOURNAL_SUPPRESS_LOCATION

#include "sd-journal.h"
#include "util.h"
#include "socket-util.h"

#define SNDBUF_SIZE (8*1024*1024)

/* We open a single fd, and we'll share it with the current process,
 * all its threads, and all its subprocesses. This means we need to
 * initialize it atomically, and need to operate on it atomically
 * never assuming we are the only user */

static int journal_fd(void) {
        int fd;
        static int fd_plus_one = 0;

retry:
        if (fd_plus_one > 0)
                return fd_plus_one - 1;

        fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        if (fd < 0)
                return -errno;

        fd_inc_sndbuf(fd, SNDBUF_SIZE);

        if (!__sync_bool_compare_and_swap(&fd_plus_one, 0, fd+1)) {
                close_nointr_nofail(fd);
                goto retry;
        }

        return fd;
}

_public_ int sd_journal_print(int priority, const char *format, ...) {
        int r;
        va_list ap;

        va_start(ap, format);
        r = sd_journal_printv(priority, format, ap);
        va_end(ap);

        return r;
}

_public_ int sd_journal_printv(int priority, const char *format, va_list ap) {
        char buffer[8 + LINE_MAX], p[11];
        struct iovec iov[2];

        if (priority < 0 || priority > 7)
                return -EINVAL;

        if (!format)
                return -EINVAL;

        snprintf(p, sizeof(p), "PRIORITY=%i", priority & LOG_PRIMASK);
        char_array_0(p);

        memcpy(buffer, "MESSAGE=", 8);
        vsnprintf(buffer+8, sizeof(buffer) - 8, format, ap);
        char_array_0(buffer);

        zero(iov);
        IOVEC_SET_STRING(iov[0], buffer);
        IOVEC_SET_STRING(iov[1], p);

        return sd_journal_sendv(iov, 2);
}

static int fill_iovec_sprintf(const char *format, va_list ap, int extra, struct iovec **_iov) {
        int r, n = 0, i = 0, j;
        struct iovec *iov = NULL;
        int saved_errno;

        assert(_iov);
        saved_errno = errno;

        if (extra > 0) {
                n = MAX(extra * 2, extra + 4);
                iov = malloc0(n * sizeof(struct iovec));
                if (!iov) {
                        r = -ENOMEM;
                        goto fail;
                }

                i = extra;
        }

        while (format) {
                struct iovec *c;
                char *buffer;
                va_list aq;

                if (i >= n) {
                        n = MAX(i*2, 4);
                        c = realloc(iov, n * sizeof(struct iovec));
                        if (!c) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        iov = c;
                }

                va_copy(aq, ap);
                if (vasprintf(&buffer, format, aq) < 0) {
                        va_end(aq);
                        r = -ENOMEM;
                        goto fail;
                }
                va_end(aq);

                VA_FORMAT_ADVANCE(format, ap);

                IOVEC_SET_STRING(iov[i++], buffer);

                format = va_arg(ap, char *);
        }

        *_iov = iov;

        errno = saved_errno;
        return i;

fail:
        for (j = 0; j < i; j++)
                free(iov[j].iov_base);

        free(iov);

        errno = saved_errno;
        return r;
}

_public_ int sd_journal_send(const char *format, ...) {
        int r, i, j;
        va_list ap;
        struct iovec *iov = NULL;

        va_start(ap, format);
        i = fill_iovec_sprintf(format, ap, 0, &iov);
        va_end(ap);

        if (_unlikely_(i < 0)) {
                r = i;
                goto finish;
        }

        r = sd_journal_sendv(iov, i);

finish:
        for (j = 0; j < i; j++)
                free(iov[j].iov_base);

        free(iov);

        return r;
}

_public_ int sd_journal_sendv(const struct iovec *iov, int n) {
        int fd, buffer_fd;
        struct iovec *w;
        uint64_t *l;
        int r, i, j = 0;
        struct msghdr mh;
        struct sockaddr_un sa;
        ssize_t k;
        int saved_errno;
        union {
                struct cmsghdr cmsghdr;
                uint8_t buf[CMSG_SPACE(sizeof(int))];
        } control;
        struct cmsghdr *cmsg;
        /* We use /dev/shm instead of /tmp here, since we want this to
         * be a tmpfs, and one that is available from early boot on
         * and where unprivileged users can create files. */
        char path[] = "/dev/shm/journal.XXXXXX";

        if (_unlikely_(!iov))
                return -EINVAL;

        if (_unlikely_(n <= 0))
                return -EINVAL;

        saved_errno = errno;

        w = alloca(sizeof(struct iovec) * n * 5);
        l = alloca(sizeof(uint64_t) * n);

        for (i = 0; i < n; i++) {
                char *c, *nl;

                if (_unlikely_(!iov[i].iov_base || iov[i].iov_len <= 1)) {
                        r = -EINVAL;
                        goto finish;
                }

                c = memchr(iov[i].iov_base, '=', iov[i].iov_len);
                if (_unlikely_(!c || c == iov[i].iov_base)) {
                        r = -EINVAL;
                        goto finish;
                }

                nl = memchr(iov[i].iov_base, '\n', iov[i].iov_len);
                if (nl) {
                        if (_unlikely_(nl < c)) {
                                r = -EINVAL;
                                goto finish;
                        }

                        /* Already includes a newline? Bummer, then
                         * let's write the variable name, then a
                         * newline, then the size (64bit LE), followed
                         * by the data and a final newline */

                        w[j].iov_base = iov[i].iov_base;
                        w[j].iov_len = c - (char*) iov[i].iov_base;
                        j++;

                        IOVEC_SET_STRING(w[j++], "\n");

                        l[i] = htole64(iov[i].iov_len - (c - (char*) iov[i].iov_base) - 1);
                        w[j].iov_base = &l[i];
                        w[j].iov_len = sizeof(uint64_t);
                        j++;

                        w[j].iov_base = c + 1;
                        w[j].iov_len = iov[i].iov_len - (c - (char*) iov[i].iov_base) - 1;
                        j++;

                } else
                        /* Nothing special? Then just add the line and
                         * append a newline */
                        w[j++] = iov[i];

                IOVEC_SET_STRING(w[j++], "\n");
        }

        fd = journal_fd();
        if (_unlikely_(fd < 0)) {
                r = fd;
                goto finish;
        }

        zero(sa);
        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, "/run/systemd/journal/socket", sizeof(sa.sun_path));

        zero(mh);
        mh.msg_name = &sa;
        mh.msg_namelen = offsetof(struct sockaddr_un, sun_path) + strlen(sa.sun_path);
        mh.msg_iov = w;
        mh.msg_iovlen = j;

        k = sendmsg(fd, &mh, MSG_NOSIGNAL);
        if (k >= 0) {
                r = 0;
                goto finish;
        }

        if (errno != EMSGSIZE && errno != ENOBUFS) {
                r = -errno;
                goto finish;
        }

        /* Message doesn't fit... Let's dump the data in a temporary
         * file and just pass a file descriptor of it to the other
         * side */

        buffer_fd = mkostemp(path, O_CLOEXEC|O_RDWR);
        if (buffer_fd < 0) {
                r = -errno;
                goto finish;
        }

        if (unlink(path) < 0) {
                close_nointr_nofail(buffer_fd);
                r = -errno;
                goto finish;
        }

        n = writev(buffer_fd, w, j);
        if (n < 0) {
                close_nointr_nofail(buffer_fd);
                r = -errno;
                goto finish;
        }

        mh.msg_iov = NULL;
        mh.msg_iovlen = 0;

        zero(control);
        mh.msg_control = &control;
        mh.msg_controllen = sizeof(control);

        cmsg = CMSG_FIRSTHDR(&mh);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        memcpy(CMSG_DATA(cmsg), &buffer_fd, sizeof(int));

        mh.msg_controllen = cmsg->cmsg_len;

        k = sendmsg(fd, &mh, MSG_NOSIGNAL);
        close_nointr_nofail(buffer_fd);

        if (k < 0) {
                r = -errno;
                goto finish;
        }

        r = 0;

finish:
        errno = saved_errno;

        return r;
}

_public_ int sd_journal_stream_fd(const char *identifier, int priority, int level_prefix) {
        union sockaddr_union sa;
        int fd;
        char *header;
        size_t l;
        ssize_t r;

        if (priority < 0 || priority > 7)
                return -EINVAL;

        fd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        if (fd < 0)
                return -errno;

        zero(sa);
        sa.un.sun_family = AF_UNIX;
        strncpy(sa.un.sun_path, "/run/systemd/journal/stdout", sizeof(sa.un.sun_path));

        r = connect(fd, &sa.sa, offsetof(union sockaddr_union, un.sun_path) + strlen(sa.un.sun_path));
        if (r < 0) {
                close_nointr_nofail(fd);
                return -errno;
        }

        if (shutdown(fd, SHUT_RD) < 0) {
                close_nointr_nofail(fd);
                return -errno;
        }

        fd_inc_sndbuf(fd, SNDBUF_SIZE);

        if (!identifier)
                identifier = "";

        l = strlen(identifier);
        header = alloca(l + 1 + 2 + 2 + 2 + 2 + 2);

        memcpy(header, identifier, l);
        header[l++] = '\n';
        header[l++] = '0' + priority;
        header[l++] = '\n';
        header[l++] = '0' + !!level_prefix;
        header[l++] = '\n';
        header[l++] = '0';
        header[l++] = '\n';
        header[l++] = '0';
        header[l++] = '\n';
        header[l++] = '0';
        header[l++] = '\n';

        r = loop_write(fd, header, l, false);
        if (r < 0) {
                close_nointr_nofail(fd);
                return (int) r;
        }

        if ((size_t) r != l) {
                close_nointr_nofail(fd);
                return -errno;
        }

        return fd;
}

_public_ int sd_journal_print_with_location(int priority, const char *file, const char *line, const char *func, const char *format, ...) {
        int r;
        va_list ap;

        va_start(ap, format);
        r = sd_journal_printv_with_location(priority, file, line, func, format, ap);
        va_end(ap);

        return r;
}

_public_ int sd_journal_printv_with_location(int priority, const char *file, const char *line, const char *func, const char *format, va_list ap) {
        char buffer[8 + LINE_MAX], p[11];
        struct iovec iov[5];
        char *f;
        size_t fl;

        if (priority < 0 || priority > 7)
                return -EINVAL;

        if (_unlikely_(!format))
                return -EINVAL;

        snprintf(p, sizeof(p), "PRIORITY=%i", priority & LOG_PRIMASK);
        char_array_0(p);

        memcpy(buffer, "MESSAGE=", 8);
        vsnprintf(buffer+8, sizeof(buffer) - 8, format, ap);
        char_array_0(buffer);

        /* func is initialized from __func__ which is not a macro, but
         * a static const char[], hence cannot easily be prefixed with
         * CODE_FUNC=, hence let's do it manually here. */
        fl = strlen(func);
        f = alloca(fl + 10);
        memcpy(f, "CODE_FUNC=", 10);
        memcpy(f + 10, func, fl + 1);

        zero(iov);
        IOVEC_SET_STRING(iov[0], buffer);
        IOVEC_SET_STRING(iov[1], p);
        IOVEC_SET_STRING(iov[2], file);
        IOVEC_SET_STRING(iov[3], line);
        IOVEC_SET_STRING(iov[4], f);

        return sd_journal_sendv(iov, ELEMENTSOF(iov));
}

_public_ int sd_journal_send_with_location(const char *file, const char *line, const char *func, const char *format, ...) {
        int r, i, j;
        va_list ap;
        struct iovec *iov = NULL;
        char *f;
        size_t fl;

        va_start(ap, format);
        i = fill_iovec_sprintf(format, ap, 3, &iov);
        va_end(ap);

        if (_unlikely_(i < 0)) {
                r = i;
                goto finish;
        }

        fl = strlen(func);
        f = alloca(fl + 10);
        memcpy(f, "CODE_FUNC=", 10);
        memcpy(f + 10, func, fl + 1);

        IOVEC_SET_STRING(iov[0], file);
        IOVEC_SET_STRING(iov[1], line);
        IOVEC_SET_STRING(iov[2], f);

        r = sd_journal_sendv(iov, i);

finish:
        for (j = 3; j < i; j++)
                free(iov[j].iov_base);

        free(iov);

        return r;
}

_public_ int sd_journal_sendv_with_location(const char *file, const char *line, const char *func, const struct iovec *iov, int n) {
        struct iovec *niov;
        char *f;
        size_t fl;

        if (_unlikely_(!iov))
                return -EINVAL;

        if (_unlikely_(n <= 0))
                return -EINVAL;

        niov = alloca(sizeof(struct iovec) * (n + 3));
        memcpy(niov, iov, sizeof(struct iovec) * n);

        fl = strlen(func);
        f = alloca(fl + 10);
        memcpy(f, "CODE_FUNC=", 10);
        memcpy(f + 10, func, fl + 1);

        IOVEC_SET_STRING(niov[n++], file);
        IOVEC_SET_STRING(niov[n++], line);
        IOVEC_SET_STRING(niov[n++], f);

        return sd_journal_sendv(niov, n);
}
