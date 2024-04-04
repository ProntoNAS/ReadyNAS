/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#ifndef foojournalhfoo
#define foojournalhfoo

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

#include <inttypes.h>
#include <sys/types.h>
#include <stdarg.h>
#include <sys/uio.h>
#include <syslog.h>

#include <systemd/sd-id128.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Write to daemon */
int sd_journal_print(int piority, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
int sd_journal_printv(int priority, const char *format, va_list ap);
int sd_journal_send(const char *format, ...) __attribute__((sentinel));
int sd_journal_sendv(const struct iovec *iov, int n);

/* Used by the macros below */
int sd_journal_print_with_location(int priority, const char *file, const char *line, const char *func, const char *format, ...) __attribute__ ((format (printf, 5, 6)));
int sd_journal_printv_with_location(int priority, const char *file, const char *line, const char *func, const char *format, va_list ap);
int sd_journal_send_with_location(const char *file, const char *line, const char *func, const char *format, ...) __attribute__((sentinel));
int sd_journal_sendv_with_location(const char *file, const char *line, const char *func, const struct iovec *iov, int n);

/* implicitly add code location to messages sent, if this is enabled */
#ifndef SD_JOURNAL_SUPPRESS_LOCATION

#define _sd_XSTRINGIFY(x) #x
#define _sd_STRINGIFY(x) _sd_XSTRINGIFY(x)

#define sd_journal_print(priority, ...) sd_journal_print_with_location(priority, "CODE_FILE=" __FILE__, "CODE_LINE=" _sd_STRINGIFY(__LINE__), __func__, __VA_ARGS__)
#define sd_journal_printv(priority, format, ap) sd_journal_printv_with_location(priority, "CODE_FILE=" __FILE__, "CODE_LINE=" _sd_STRINGIFY(__LINE__), __func__, format, ap)
#define sd_journal_send(...) sd_journal_send_with_location("CODE_FILE=" __FILE__, "CODE_LINE=" _sd_STRINGIFY(__LINE__), __func__, __VA_ARGS__)
#define sd_journal_sendv(iovec, n) sd_journal_sendv_with_location("CODE_FILE=" __FILE__, "CODE_LINE=" _sd_STRINGIFY(__LINE__), __func__, iovec, n)

#endif

int sd_journal_stream_fd(const char *identifier, int priority, int level_prefix);

/* Browse journal stream */

typedef struct sd_journal sd_journal;

enum {
        SD_JOURNAL_LOCAL_ONLY = 1,
        SD_JOURNAL_RUNTIME_ONLY = 2,
        SD_JOURNAL_SYSTEM_ONLY = 4
};

int sd_journal_open(sd_journal **ret, int flags);
void sd_journal_close(sd_journal *j);

int sd_journal_previous(sd_journal *j);
int sd_journal_next(sd_journal *j);

int sd_journal_previous_skip(sd_journal *j, uint64_t skip);
int sd_journal_next_skip(sd_journal *j, uint64_t skip);

int sd_journal_get_realtime_usec(sd_journal *j, uint64_t *ret);
int sd_journal_get_monotonic_usec(sd_journal *j, uint64_t *ret, sd_id128_t *ret_boot_id);
int sd_journal_get_data(sd_journal *j, const char *field, const void **data, size_t *l);
int sd_journal_enumerate_data(sd_journal *j, const void **data, size_t *l);
void sd_journal_restart_data(sd_journal *j);

int sd_journal_add_match(sd_journal *j, const void *data, size_t size);
void sd_journal_flush_matches(sd_journal *j);

int sd_journal_seek_head(sd_journal *j);
int sd_journal_seek_tail(sd_journal *j);
int sd_journal_seek_monotonic_usec(sd_journal *j, sd_id128_t boot_id, uint64_t usec);
int sd_journal_seek_realtime_usec(sd_journal *j, uint64_t usec);
int sd_journal_seek_cursor(sd_journal *j, const char *cursor);

int sd_journal_get_cursor(sd_journal *j, char **cursor);

int sd_journal_get_cutoff_realtime_usec(sd_journal *j, uint64_t *from, uint64_t *to);
int sd_journal_get_cutoff_monotonic_usec(sd_journal *j, const sd_id128_t boot_id, uint64_t *from, uint64_t *to);

/* int sd_journal_query_unique(sd_journal *j, const char *field);      /\* missing *\/ */
/* int sd_journal_enumerate_unique(sd_journal *j, const void **data, size_t *l); /\* missing *\/ */
/* void sd_journal_restart_unique(sd_journal *j);                      /\* missing *\/ */

enum {
        SD_JOURNAL_NOP,
        SD_JOURNAL_APPEND,
        SD_JOURNAL_INVALIDATE_ADD,
        SD_JOURNAL_INVALIDATE_REMOVE
};

int sd_journal_get_fd(sd_journal *j);
int sd_journal_process(sd_journal *j);

#define SD_JOURNAL_FOREACH(j)                                           \
        if (sd_journal_seek_head(j) >= 0)                               \
                while (sd_journal_next(j) > 0)

#define SD_JOURNAL_FOREACH_BACKWARDS(j)                                 \
        if (sd_journal_seek_tail(j) >= 0)                               \
                while (sd_journal_previous(j) > 0)

#define SD_JOURNAL_FOREACH_DATA(j, data, l)                             \
        for (sd_journal_restart_data(j); sd_journal_enumerate_data((j), &(data), &(l)) > 0; )

#define SD_JOURNAL_FOREACH_UNIQUE(j, data, l)                           \
        for (sd_journal_restart_unique(j); sd_journal_enumerate_data((j), &(data), &(l)) > 0; )

#ifdef __cplusplus
}
#endif

#endif
