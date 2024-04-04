
/* Generated data (by glib-mkenums) */

#include <config.h>

#include "tracker-enum-types.h"

/* enumerations from "tracker-date-time.h" */
#include "tracker-date-time.h"
GType
tracker_date_error_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_DATE_ERROR_OFFSET, "TRACKER_DATE_ERROR_OFFSET", "offset" },
      { TRACKER_DATE_ERROR_INVALID_ISO8601, "TRACKER_DATE_ERROR_INVALID_ISO8601", "invalid-iso8601" },
      { TRACKER_DATE_ERROR_EMPTY, "TRACKER_DATE_ERROR_EMPTY", "empty" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerDateError"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

/* enumerations from "tracker-dbus.h" */
#include "tracker-dbus.h"
GType
tracker_dbus_events_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_DBUS_EVENTS_TYPE_ADD, "TRACKER_DBUS_EVENTS_TYPE_ADD", "add" },
      { TRACKER_DBUS_EVENTS_TYPE_UPDATE, "TRACKER_DBUS_EVENTS_TYPE_UPDATE", "update" },
      { TRACKER_DBUS_EVENTS_TYPE_DELETE, "TRACKER_DBUS_EVENTS_TYPE_DELETE", "delete" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerDBusEventsType"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_dbus_error_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_DBUS_ERROR_ASSERTION_FAILED, "TRACKER_DBUS_ERROR_ASSERTION_FAILED", "assertion-failed" },
      { TRACKER_DBUS_ERROR_UNSUPPORTED, "TRACKER_DBUS_ERROR_UNSUPPORTED", "unsupported" },
      { TRACKER_DBUS_ERROR_BROKEN_PIPE, "TRACKER_DBUS_ERROR_BROKEN_PIPE", "broken-pipe" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerDBusError"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

/* enumerations from "tracker-enums.h" */
#include "tracker-enums.h"
GType
tracker_verbosity_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_VERBOSITY_ERRORS, "TRACKER_VERBOSITY_ERRORS", "errors" },
      { TRACKER_VERBOSITY_MINIMAL, "TRACKER_VERBOSITY_MINIMAL", "minimal" },
      { TRACKER_VERBOSITY_DETAILED, "TRACKER_VERBOSITY_DETAILED", "detailed" },
      { TRACKER_VERBOSITY_DEBUG, "TRACKER_VERBOSITY_DEBUG", "debug" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerVerbosity"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_sched_idle_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_SCHED_IDLE_ALWAYS, "TRACKER_SCHED_IDLE_ALWAYS", "always" },
      { TRACKER_SCHED_IDLE_FIRST_INDEX, "TRACKER_SCHED_IDLE_FIRST_INDEX", "first-index" },
      { TRACKER_SCHED_IDLE_NEVER, "TRACKER_SCHED_IDLE_NEVER", "never" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerSchedIdle"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_serialization_format_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_SERIALIZATION_FORMAT_SPARQL, "TRACKER_SERIALIZATION_FORMAT_SPARQL", "sparql" },
      { TRACKER_SERIALIZATION_FORMAT_TURTLE, "TRACKER_SERIALIZATION_FORMAT_TURTLE", "turtle" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerSerializationFormat"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

/* enumerations from "tracker-locale.h" */
#include "tracker-locale.h"
GType
tracker_locale_id_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_LOCALE_LANGUAGE, "TRACKER_LOCALE_LANGUAGE", "language" },
      { TRACKER_LOCALE_TIME, "TRACKER_LOCALE_TIME", "time" },
      { TRACKER_LOCALE_COLLATE, "TRACKER_LOCALE_COLLATE", "collate" },
      { TRACKER_LOCALE_NUMERIC, "TRACKER_LOCALE_NUMERIC", "numeric" },
      { TRACKER_LOCALE_MONETARY, "TRACKER_LOCALE_MONETARY", "monetary" },
      { TRACKER_LOCALE_LAST, "TRACKER_LOCALE_LAST", "last" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerLocaleID"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}



/* Generated data ends here */

