
/* Generated data (by glib-mkenums) */

#include <config.h>

#include "tracker-miner-enum-types.h"

/* enumerations from "tracker-miner-enums.h" */
#include "tracker-miner-enums.h"
GType
tracker_directory_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GFlagsValue values[] = {
      { TRACKER_DIRECTORY_FLAG_NONE, "TRACKER_DIRECTORY_FLAG_NONE", "none" },
      { TRACKER_DIRECTORY_FLAG_RECURSE, "TRACKER_DIRECTORY_FLAG_RECURSE", "recurse" },
      { TRACKER_DIRECTORY_FLAG_CHECK_MTIME, "TRACKER_DIRECTORY_FLAG_CHECK_MTIME", "check-mtime" },
      { TRACKER_DIRECTORY_FLAG_MONITOR, "TRACKER_DIRECTORY_FLAG_MONITOR", "monitor" },
      { TRACKER_DIRECTORY_FLAG_IGNORE, "TRACKER_DIRECTORY_FLAG_IGNORE", "ignore" },
      { TRACKER_DIRECTORY_FLAG_PRESERVE, "TRACKER_DIRECTORY_FLAG_PRESERVE", "preserve" },
      { TRACKER_DIRECTORY_FLAG_PRIORITY, "TRACKER_DIRECTORY_FLAG_PRIORITY", "priority" },
      { TRACKER_DIRECTORY_FLAG_NO_STAT, "TRACKER_DIRECTORY_FLAG_NO_STAT", "no-stat" },
      { TRACKER_DIRECTORY_FLAG_CHECK_DELETED, "TRACKER_DIRECTORY_FLAG_CHECK_DELETED", "check-deleted" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_flags_register_static (g_intern_static_string ("TrackerDirectoryFlags"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_filter_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_FILTER_FILE, "TRACKER_FILTER_FILE", "file" },
      { TRACKER_FILTER_DIRECTORY, "TRACKER_FILTER_DIRECTORY", "directory" },
      { TRACKER_FILTER_PARENT_DIRECTORY, "TRACKER_FILTER_PARENT_DIRECTORY", "parent-directory" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerFilterType"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_filter_policy_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_FILTER_POLICY_DENY, "TRACKER_FILTER_POLICY_DENY", "deny" },
      { TRACKER_FILTER_POLICY_ACCEPT, "TRACKER_FILTER_POLICY_ACCEPT", "accept" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerFilterPolicy"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}

GType
tracker_network_type_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { TRACKER_NETWORK_TYPE_NONE, "TRACKER_NETWORK_TYPE_NONE", "none" },
      { TRACKER_NETWORK_TYPE_UNKNOWN, "TRACKER_NETWORK_TYPE_UNKNOWN", "unknown" },
      { TRACKER_NETWORK_TYPE_GPRS, "TRACKER_NETWORK_TYPE_GPRS", "gprs" },
      { TRACKER_NETWORK_TYPE_EDGE, "TRACKER_NETWORK_TYPE_EDGE", "edge" },
      { TRACKER_NETWORK_TYPE_3G, "TRACKER_NETWORK_TYPE_3G", "3g" },
      { TRACKER_NETWORK_TYPE_LAN, "TRACKER_NETWORK_TYPE_LAN", "lan" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("TrackerNetworkType"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}



/* Generated data ends here */

