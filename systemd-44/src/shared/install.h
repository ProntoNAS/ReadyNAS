/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#ifndef fooinstallhfoo
#define fooinstallhfoo

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

#include "hashmap.h"

typedef enum UnitFileScope {
        UNIT_FILE_SYSTEM,
        UNIT_FILE_GLOBAL,
        UNIT_FILE_USER,
        _UNIT_FILE_SCOPE_MAX,
        _UNIT_FILE_SCOPE_INVALID = -1
} UnitFileScope;

typedef enum UnitFileState {
        UNIT_FILE_ENABLED,
        UNIT_FILE_ENABLED_RUNTIME,
        UNIT_FILE_LINKED,
        UNIT_FILE_LINKED_RUNTIME,
        UNIT_FILE_MASKED,
        UNIT_FILE_MASKED_RUNTIME,
        UNIT_FILE_STATIC,
        UNIT_FILE_DISABLED,
        _UNIT_FILE_STATE_MAX,
        _UNIT_FILE_STATE_INVALID = -1
} UnitFileState;

typedef enum UnitFileChangeType {
        UNIT_FILE_SYMLINK,
        UNIT_FILE_UNLINK,
        _UNIT_FILE_CHANGE_TYPE_MAX,
        _UNIT_FILE_CHANGE_TYPE_INVALID = -1
} UnitFileChangeType;

typedef struct UnitFileChange {
        UnitFileChangeType type;
        char *path;
        char *source;
} UnitFileChange;

typedef struct UnitFileList {
        char *path;
        UnitFileState state;
} UnitFileList;

int unit_file_enable(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], bool force, UnitFileChange **changes, unsigned *n_changes);
int unit_file_disable(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], UnitFileChange **changes, unsigned *n_changes);
int unit_file_reenable(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], bool force, UnitFileChange **changes, unsigned *n_changes);
int unit_file_link(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], bool force, UnitFileChange **changes, unsigned *n_changes);
int unit_file_preset(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], bool force, UnitFileChange **changes, unsigned *n_changes);
int unit_file_mask(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], bool force, UnitFileChange **changes, unsigned *n_changes);
int unit_file_unmask(UnitFileScope scope, bool runtime, const char *root_dir, char *files[], UnitFileChange **changes, unsigned *n_changes);

UnitFileState unit_file_get_state(UnitFileScope scope, const char *root_dir, const char *filename);

int unit_file_get_list(UnitFileScope scope, const char *root_dir, Hashmap *h);

void unit_file_list_free(Hashmap *h);
void unit_file_changes_free(UnitFileChange *changes, unsigned n_changes);

int unit_file_query_preset(UnitFileScope scope, const char *name);

const char *unit_file_state_to_string(UnitFileState s);
UnitFileState unit_file_state_from_string(const char *s);

const char *unit_file_change_type_to_string(UnitFileChangeType s);
UnitFileChangeType unit_file_change_type_from_string(const char *s);

#endif
