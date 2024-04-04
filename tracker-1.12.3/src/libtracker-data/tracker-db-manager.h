/*
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

#ifndef __LIBTRACKER_DB_MANAGER_H__
#define __LIBTRACKER_DB_MANAGER_H__

#include <glib-object.h>

#include "tracker-db-interface.h"

G_BEGIN_DECLS

#if !defined (__LIBTRACKER_DATA_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "only <libtracker-data/tracker-data.h> must be included directly."
#endif

#define TRACKER_DB_CACHE_SIZE_DEFAULT 250
#define TRACKER_DB_CACHE_SIZE_UPDATE 2000

#define TRACKER_TYPE_DB (tracker_db_get_type ())

typedef enum {
	TRACKER_DB_UNKNOWN,
	TRACKER_DB_METADATA,
	TRACKER_DB_FULLTEXT,
} TrackerDB;

typedef enum {
	TRACKER_DB_MANAGER_FORCE_REINDEX         = 1 << 1,
	TRACKER_DB_MANAGER_REMOVE_CACHE          = 1 << 2,
	/* 1 << 3 Was low mem mode */
	TRACKER_DB_MANAGER_REMOVE_ALL            = 1 << 4,
	TRACKER_DB_MANAGER_READONLY              = 1 << 5,
	TRACKER_DB_MANAGER_DO_NOT_CHECK_ONTOLOGY = 1 << 6,
	TRACKER_DB_MANAGER_ENABLE_MUTEXES        = 1 << 7,
} TrackerDBManagerFlags;

GType               tracker_db_get_type                       (void) G_GNUC_CONST;
gboolean            tracker_db_manager_init                   (TrackerDBManagerFlags   flags,
                                                               gboolean               *first_time,
                                                               gboolean                restoring_backup,
                                                               gboolean                shared_cache,
                                                               guint                   select_cache_size,
                                                               guint                   update_cache_size,
                                                               TrackerBusyCallback     busy_callback,
                                                               gpointer                busy_user_data,
                                                               const gchar            *busy_operation,
                                                               GError                **error);
void                tracker_db_manager_shutdown               (void);
void                tracker_db_manager_remove_all             (gboolean               rm_journal);
void                tracker_db_manager_optimize               (void);
const gchar *       tracker_db_manager_get_file               (TrackerDB              db);
TrackerDBInterface *tracker_db_manager_get_db_interface       (void);
void                tracker_db_manager_init_locations         (void);
gboolean            tracker_db_manager_has_enough_space       (void);
void                tracker_db_manager_create_version_file    (void);
void                tracker_db_manager_remove_version_file    (void);

TrackerDBManagerFlags
                    tracker_db_manager_get_flags              (guint *select_cache_size,
                                                               guint *update_cache_size);

gboolean            tracker_db_manager_get_first_index_done   (void);
guint64             tracker_db_manager_get_last_crawl_done    (void);
gboolean            tracker_db_manager_get_need_mtime_check   (void);

void                tracker_db_manager_set_first_index_done   (gboolean done);
void                tracker_db_manager_set_last_crawl_done    (gboolean done);
void                tracker_db_manager_set_need_mtime_check   (gboolean needed);

gboolean            tracker_db_manager_locale_changed         (GError **error);
void                tracker_db_manager_set_current_locale     (void);

gboolean            tracker_db_manager_get_tokenizer_changed  (void);
void                tracker_db_manager_tokenizer_update       (void);

G_END_DECLS

#endif /* __LIBTRACKER_DB_MANAGER_H__ */
