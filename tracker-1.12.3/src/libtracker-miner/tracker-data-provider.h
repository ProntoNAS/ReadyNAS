/*
 * Copyright (C) 2014, Softathome <contact@softathome.com>
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
 *
 * Author: Martyn Russell <martyn@lanedo.com>
 */

#ifndef __LIBTRACKER_MINER_DATA_PROVIDER_H__
#define __LIBTRACKER_MINER_DATA_PROVIDER_H__

#if !defined (__LIBTRACKER_MINER_H_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "Only <libtracker-miner/tracker-miner.h> can be included directly."
#endif

#include <gio/gio.h>

#include "tracker-enumerator.h"
#include "tracker-miner-enums.h"

G_BEGIN_DECLS

#define TRACKER_TYPE_DATA_PROVIDER           (tracker_data_provider_get_type ())
#define TRACKER_DATA_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_DATA_PROVIDER, TrackerDataProvider))
#define TRACKER_IS_DATA_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_DATA_PROVIDER))
#define TRACKER_DATA_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), TRACKER_TYPE_DATA_PROVIDER, TrackerDataProviderIface))

/**
 * TrackerDataProvider:
 *
 * An interface to enumerate URIs and feed the data to Tracker.
 **/
typedef struct _TrackerDataProvider TrackerDataProvider;
typedef struct _TrackerDataProviderIface TrackerDataProviderIface;

/**
 * TrackerDataProviderIface:
 * @g_iface: Parent interface type.
 * @begin: Called when the data_provider is synchronously
 * opening and starting the iteration of a given location.
 * @begin_async: Called when the data_provider is synchronously
 * opening and starting the iteration of a given location. Completed
 * using @begin_finish.
 * @begin_finish: Called when the data_provider is completing the
 * asynchronous operation provided by @begin_async.
 * @end: Called when the data_provider is synchronously
 * closing and cleaning up the iteration of a given location.
 * @end_async: Called when the data_provider is asynchronously
 * closing and cleaning up the iteration of a given location.
 * Completed using @end_finish.
 * @end_finish: Called when the data_provider is completing the
 * asynchronous operation provided by @end_async.
 *
 * Virtual methods left to implement.
 **/
struct _TrackerDataProviderIface {
	GTypeInterface g_iface;

	/* Virtual Table */

	/* Start the data_provider for a given location, attributes and flags */
	TrackerEnumerator *   (* begin)              (TrackerDataProvider    *data_provider,
	                                              GFile                  *url,
	                                              const gchar            *attributes,
	                                              TrackerDirectoryFlags   flags,
	                                              GCancellable           *cancellable,
	                                              GError                **error);
	void                  (* begin_async)        (TrackerDataProvider    *data_provider,
	                                              GFile                  *url,
	                                              const gchar            *attributes,
	                                              TrackerDirectoryFlags   flags,
	                                              gint                    io_priority,
	                                              GCancellable           *cancellable,
	                                              GAsyncReadyCallback     callback,
	                                              gpointer                user_data);
	TrackerEnumerator *   (* begin_finish)       (TrackerDataProvider    *data_provider,
	                                              GAsyncResult           *result,
	                                              GError                **error);

	/* Close the given location */
	gboolean              (* end)                (TrackerDataProvider    *data_provider,
	                                              TrackerEnumerator      *enumerator,
	                                              GCancellable           *cancellable,
	                                              GError                **error);
	void                  (* end_async)          (TrackerDataProvider    *data_provider,
	                                              TrackerEnumerator      *enumerator,
	                                              gint                    io_priority,
	                                              GCancellable           *cancellable,
	                                              GAsyncReadyCallback     callback,
	                                              gpointer                user_data);
	gboolean              (* end_finish)         (TrackerDataProvider    *data_provider,
	                                              GAsyncResult           *result,
	                                              GError                **error);

	/*< private >*/
	/* Padding for future expansion */
	void (*_tracker_reserved1) (void);
	void (*_tracker_reserved2) (void);
	void (*_tracker_reserved3) (void);
	void (*_tracker_reserved4) (void);
	void (*_tracker_reserved5) (void);
	void (*_tracker_reserved6) (void);
	void (*_tracker_reserved7) (void);
	void (*_tracker_reserved8) (void);
};

GType              tracker_data_provider_get_type        (void) G_GNUC_CONST;
TrackerEnumerator *tracker_data_provider_begin           (TrackerDataProvider   *data_provider,
                                                          GFile                 *url,
                                                          const gchar           *attributes,
                                                          TrackerDirectoryFlags  flags,
                                                          GCancellable          *cancellable,
                                                          GError               **error);
void               tracker_data_provider_begin_async     (TrackerDataProvider   *data_provider,
                                                          GFile                 *url,
                                                          const gchar           *attributes,
                                                          TrackerDirectoryFlags  flags,
                                                          gint                   io_priority,
                                                          GCancellable          *cancellable,
                                                          GAsyncReadyCallback    callback,
                                                          gpointer               user_data);
TrackerEnumerator *tracker_data_provider_begin_finish    (TrackerDataProvider   *data_provider,
                                                          GAsyncResult          *result,
                                                          GError               **error);
gboolean           tracker_data_provider_end             (TrackerDataProvider   *data_provider,
                                                          TrackerEnumerator     *enumerator,
                                                          GCancellable          *cancellable,
                                                          GError               **error);
void               tracker_data_provider_end_async       (TrackerDataProvider   *data_provider,
                                                          TrackerEnumerator     *enumerator,
                                                          gint                   io_priority,
                                                          GCancellable          *cancellable,
                                                          GAsyncReadyCallback    callback,
                                                          gpointer               user_data);
gboolean           tracker_data_provider_end_finish      (TrackerDataProvider   *data_provider,
                                                          GAsyncResult          *result,
                                                          GError               **error);

G_END_DECLS

#endif /* __LIBTRACKER_MINER_DATA_PROVIDER_H__ */
