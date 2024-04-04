/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#ifndef foocgroupshowhfoo
#define foocgroupshowhfoo

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

#include <stdbool.h>
#include <sys/types.h>

int show_cgroup_by_path(const char *path, const char *prefix, unsigned columns, bool kernel_threads, bool all);
int show_cgroup(const char *controller, const char *path, const char *prefix, unsigned columns, bool kernel_threads, bool all);

int show_cgroup_and_extra_by_spec(const char *spec, const char *prefix, unsigned n_columns, bool kernel_threads, bool all, const pid_t extra_pids[], unsigned n_extra_pids);
int show_cgroup_and_extra(const char *controller, const char *path, const char *prefix, unsigned n_columns, bool kernel_threads, bool all, const pid_t extra_pids[], unsigned n_extra_pids);

#endif
