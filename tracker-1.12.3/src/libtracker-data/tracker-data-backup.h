/*
 * Copyright (C) 2006, Jamie McCracken <jamiemcc@gnome.org>
 * Copyright (C) 2008, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __LIBTRACKER_DATA_BACKUP_H__
#define __LIBTRACKER_DATA_BACKUP_H__

#include <glib.h>
#include <gio/gio.h>

#include <libtracker-data/tracker-data-update.h>

G_BEGIN_DECLS

#if !defined (__LIBTRACKER_DATA_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "only <libtracker-data/tracker-data.h> must be included directly."
#endif

#define TRACKER_DATA_BACKUP_ERROR_DOMAIN "TrackerBackup"
#define TRACKER_DATA_BACKUP_ERROR        tracker_data_backup_error_quark()

typedef enum {
	TRACKER_DATA_BACKUP_ERROR_UNKNOWN = 0,
	TRACKER_DATA_BACKUP_ERROR_INVALID_URI
} TrackerDataBackupError;

typedef void (*TrackerDataBackupFinished) (GError *error, gpointer user_data);

GQuark tracker_data_backup_error_quark (void);
void   tracker_data_backup_save        (GFile                     *destination,
                                        TrackerDataBackupFinished  callback,
                                        gpointer                   user_data,
                                        GDestroyNotify             destroy);
void   tracker_data_backup_restore     (GFile                     *journal,
                                        const gchar              **test_schema,
                                        TrackerBusyCallback        busy_callback,
                                        gpointer                   busy_user_data,
                                        GError                   **error);

G_END_DECLS

#endif /* __LIBTRACKER_DATA_BACKUP_H__ */
