
/* Generated data (by glib-mkenums) */

#include "config.h"

#include "tracker-sparql-enum-types.h"

/* enumerations from "tracker-notifier.h" */
#include "tracker-notifier.h"
GType
tracker_notifier_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GFlagsValue values[] = {
      { TRACKER_NOTIFIER_FLAG_NONE, "TRACKER_NOTIFIER_FLAG_NONE", "none" },
      { TRACKER_NOTIFIER_FLAG_QUERY_URN, "TRACKER_NOTIFIER_FLAG_QUERY_URN", "query-urn" },
      { TRACKER_NOTIFIER_FLAG_QUERY_LOCATION, "TRACKER_NOTIFIER_FLAG_QUERY_LOCATION", "query-location" },
      { TRACKER_NOTIFIER_FLAG_NOTIFY_UNEXTRACTED, "TRACKER_NOTIFIER_FLAG_NOTIFY_UNEXTRACTED", "notify-unextracted" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id =
       g_flags_register_static (g_intern_static_string ("TrackerNotifierFlags"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
tracker_notifier_event_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_NOTIFIER_EVENT_CREATE, "TRACKER_NOTIFIER_EVENT_CREATE", "create" },
      { TRACKER_NOTIFIER_EVENT_DELETE, "TRACKER_NOTIFIER_EVENT_DELETE", "delete" },
      { TRACKER_NOTIFIER_EVENT_UPDATE, "TRACKER_NOTIFIER_EVENT_UPDATE", "update" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id =
       g_enum_register_static (g_intern_static_string ("TrackerNotifierEventType"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}



/* Generated data ends here */

