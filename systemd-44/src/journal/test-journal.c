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
#include <unistd.h>

#include <systemd/sd-journal.h>

#include "journal-file.h"
#include "log.h"

int main(int argc, char *argv[]) {
        dual_timestamp ts;
        JournalFile *f;
        struct iovec iovec;
        static const char test[] = "test", test2[] = "test2";
        Object *o;
        uint64_t p;

        log_set_max_level(LOG_DEBUG);

        unlink("test.journal");

        assert_se(journal_file_open("test.journal", O_RDWR|O_CREAT, 0666, NULL, &f) == 0);

        dual_timestamp_get(&ts);

        iovec.iov_base = (void*) test;
        iovec.iov_len = strlen(test);
        assert_se(journal_file_append_entry(f, &ts, &iovec, 1, NULL, NULL, NULL) == 0);

        iovec.iov_base = (void*) test2;
        iovec.iov_len = strlen(test2);
        assert_se(journal_file_append_entry(f, &ts, &iovec, 1, NULL, NULL, NULL) == 0);

        iovec.iov_base = (void*) test;
        iovec.iov_len = strlen(test);
        assert_se(journal_file_append_entry(f, &ts, &iovec, 1, NULL, NULL, NULL) == 0);

        journal_file_dump(f);

        assert(journal_file_next_entry(f, NULL, 0, DIRECTION_DOWN, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_next_entry(f, o, p, DIRECTION_DOWN, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 2);

        assert(journal_file_next_entry(f, o, p, DIRECTION_DOWN, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 3);

        assert(journal_file_next_entry(f, o, p, DIRECTION_DOWN, &o, &p) == 0);

        assert(journal_file_next_entry(f, NULL, 0, DIRECTION_DOWN, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_skip_entry(f, o, p, 2, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 3);

        assert(journal_file_skip_entry(f, o, p, -2, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_skip_entry(f, o, p, -2, &o, &p) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_find_data_object(f, test, strlen(test), NULL, &p) == 1);
        assert(journal_file_next_entry_for_data(f, NULL, 0, p, DIRECTION_DOWN, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_next_entry_for_data(f, NULL, 0, p, DIRECTION_UP, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 3);

        assert(journal_file_find_data_object(f, test2, strlen(test2), NULL, &p) == 1);
        assert(journal_file_next_entry_for_data(f, NULL, 0, p, DIRECTION_UP, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 2);

        assert(journal_file_next_entry_for_data(f, NULL, 0, p, DIRECTION_DOWN, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 2);

        assert(journal_file_find_data_object(f, "quux", 4, NULL, &p) == 0);

        assert(journal_file_move_to_entry_by_seqnum(f, 1, DIRECTION_DOWN, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 1);

        assert(journal_file_move_to_entry_by_seqnum(f, 3, DIRECTION_DOWN, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 3);

        assert(journal_file_move_to_entry_by_seqnum(f, 2, DIRECTION_DOWN, &o, NULL) == 1);
        assert(le64toh(o->entry.seqnum) == 2);

        assert(journal_file_move_to_entry_by_seqnum(f, 10, DIRECTION_DOWN, &o, NULL) == 0);

        journal_file_rotate(&f);
        journal_file_rotate(&f);

        journal_file_close(f);

        journal_directory_vacuum(".", 3000000, 0);

        log_error("Exiting...");

        return 0;
}
