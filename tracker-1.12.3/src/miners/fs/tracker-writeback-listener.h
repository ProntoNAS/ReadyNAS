/*
 * Copyright (C) 2010, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __TRACKER_MINER_FS_WRITEBACK_LISTENER_H__
#define __TRACKER_MINER_FS_WRITEBACK_LISTENER_H__

#include <glib-object.h>

#include "tracker-miner-files.h"

G_BEGIN_DECLS

#define TRACKER_TYPE_WRITEBACK_LISTENER            (tracker_writeback_listener_get_type ())
#define TRACKER_WRITEBACK_LISTENER(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), TRACKER_TYPE_WRITEBACK_LISTENER, TrackerWritebackListener))
#define TRACKER_WRITEBACK_LISTENER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_DBUS_WRITEBACK_LISTENER, TrackerWritebackListenerClass))
#define TRACKER_IS_WRITEBACK_LISTENER(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), TRACKER_TYPE_WRITEBACK_LISTENER))
#define TRACKER_IS_WRITEBACK_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_WRITEBACK_LISTENER))
#define TRACKER_WRITEBACK_LISTENER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_WRITEBACK_LISTENER, TrackerWritebackListenerClass))

typedef struct TrackerWritebackListener TrackerWritebackListener;
typedef struct TrackerWritebackListenerClass TrackerWritebackListenerClass;

struct TrackerWritebackListener {
	GObject parent;
};

struct TrackerWritebackListenerClass {
	GObjectClass parent;
};

GType                     tracker_writeback_listener_get_type (void);
TrackerWritebackListener *tracker_writeback_listener_new      (TrackerMinerFiles  *miner_files,
                                                               GError            **error);

G_END_DECLS

#endif /* __TRACKER_MINER_FS_WRITEBACK_LISTENER_H__ */
