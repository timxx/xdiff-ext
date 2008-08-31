#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <thunar-vfs/thunar-vfs.h>

#include <preferences.h>

#define RC_PATH "xdiff-ext/xdiff-extrc"

/* Property identifiers */
enum {
  PROP_0 = 0,
  PROP_COMPARE_COMMAND, /* string */
  PROP_3WAY_COMPARE_COMMAND, /* string */
  PROP_COMPARE_DIRECTORIES, /* boolean */
  PROP_KEEP_FILES_IN_LIST, /* boolean */
  N_PROPERTIES
};



static void xdiff_ext_preferences_class_init(xdiff_ext_preferences_class* klass);
static void xdiff_ext_preferences_init(xdiff_ext_preferences* preferences);
static void xdiff_ext_preferences_finalize(GObject* object);

static void xdiff_ext_preferences_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
static void xdiff_ext_preferences_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);

static void xdiff_ext_preferences_resume_monitor(xdiff_ext_preferences* preferences);
static void xdiff_ext_preferences_suspend_monitor(xdiff_ext_preferences* preferences);
static void xdiff_ext_preferences_monitor(ThunarVfsMonitor* monitor, 
                                          ThunarVfsMonitorHandle* handle,
                                          ThunarVfsMonitorEvent event,
                                          ThunarVfsPath* handle_path,
                                          ThunarVfsPath* event_path,
                                          gpointer user_data);
static void xdiff_ext_preferences_queue_load(xdiff_ext_preferences* preferences);
static void xdiff_ext_preferences_queue_store(xdiff_ext_preferences* preferences);
static void xdiff_ext_preferences_load_idle_destroy(gpointer user_data);
static void xdiff_ext_preferences_store_idle_destroy(gpointer user_data);


struct _xdiff_ext_preferences_class {
  GObjectClass __parent__;
};

struct _xdiff_ext_preferences {
  GObject __parent__;

  ThunarVfsMonitorHandle* handle;
  ThunarVfsMonitor* monitor;

  GValue values[N_PROPERTIES];

  gboolean loading_in_progress;

  gint load_idle_id;
  gint store_idle_id;
};



static GObjectClass* xdiff_ext_preferences_parent_class;


static void
transform_string_to_boolean (const GValue* src, GValue* dst) {
  g_value_set_boolean(dst, strcmp(g_value_get_string(src), "FALSE") != 0);
}


GType
xdiff_ext_preferences_get_type() {
  static GType type = G_TYPE_INVALID;

  if(G_UNLIKELY(type == G_TYPE_INVALID)) {
    static const GTypeInfo info = {
      sizeof(xdiff_ext_preferences_class),
      NULL,
      NULL,
      (GClassInitFunc)xdiff_ext_preferences_class_init,
      NULL,
      NULL,
      sizeof(xdiff_ext_preferences),
      0,
      (GInstanceInitFunc)xdiff_ext_preferences_init,
      NULL,
    };

    type = g_type_register_static(G_TYPE_OBJECT, I_("xdiff_ext_preferences"), &info, 0);
  }

  return type;
}



static void
xdiff_ext_preferences_class_init(xdiff_ext_preferences_class* klass) {
  GObjectClass* gobject_class;

  if (!g_value_type_transformable (G_TYPE_STRING, G_TYPE_BOOLEAN)) {
    g_value_register_transform_func (G_TYPE_STRING, G_TYPE_BOOLEAN, transform_string_to_boolean);
  }
  
  /* determine the parent type class */
  xdiff_ext_preferences_parent_class = g_type_class_peek_parent(klass);

  gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = xdiff_ext_preferences_finalize;
  gobject_class->get_property = xdiff_ext_preferences_get_property;
  gobject_class->set_property = xdiff_ext_preferences_set_property;

  g_object_class_install_property(gobject_class,
                                   PROP_COMPARE_COMMAND,
                                   g_param_spec_string("compare-command",
                                                        "compare-command",
                                                        "compare-command",
                                                        "",
                                                        EXO_PARAM_READWRITE));

  g_object_class_install_property(gobject_class,
                                   PROP_3WAY_COMPARE_COMMAND,
                                   g_param_spec_string("three-way-compare-command",
                                                        "three-way-compare-command",
                                                        "three-way-compare-command",
                                                        "",
                                                        EXO_PARAM_READWRITE));

  g_object_class_install_property(gobject_class,
                                   PROP_COMPARE_DIRECTORIES,
                                   g_param_spec_boolean("compare-directories",
                                                         "compare-directories",
                                                         "compare-directories",
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));

  g_object_class_install_property(gobject_class,
                                   PROP_KEEP_FILES_IN_LIST,
                                   g_param_spec_boolean("keep-files-in-list",
                                                         "keep-files-in-list",
                                                         "keep-files-in-list",
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));
}



static void
xdiff_ext_preferences_init(xdiff_ext_preferences* preferences) {
  /* grab a reference on the VFS monitor */
  preferences->monitor = thunar_vfs_monitor_get_default();

  /* load the settings */
  xdiff_ext_preferences_load(preferences);

  /* launch the file monitor */
  xdiff_ext_preferences_resume_monitor(preferences);
}



static void
xdiff_ext_preferences_finalize(GObject* object) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(object);
  guint n;

  /* flush preferences */
  if(G_UNLIKELY(preferences->store_idle_id != 0)) {
    xdiff_ext_preferences_store(preferences);
    g_source_remove(preferences->store_idle_id);
  }

  /* stop any pending load idle source */
  if(G_UNLIKELY(preferences->load_idle_id != 0)) {
    g_source_remove(preferences->load_idle_id);
  }

  /* stop the file monitor */
  if(G_LIKELY(preferences->monitor != NULL)) {
    xdiff_ext_preferences_suspend_monitor(preferences);
    g_object_unref(G_OBJECT(preferences->monitor));
  }

  /* release the property values */
  for(n = 1; n < N_PROPERTIES; ++n) {
    if(G_IS_VALUE(preferences->values + n)) {
      g_value_unset(preferences->values + n);
    }
  }

 (*G_OBJECT_CLASS(xdiff_ext_preferences_parent_class)->finalize)(object);
}



static void
xdiff_ext_preferences_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(object);
  GValue* src;

  src = preferences->values + prop_id;
  if(G_IS_VALUE(src)) {
    g_value_copy(src, value);
  } else {
    g_param_value_set_default(pspec, value);
  }
}



static void
xdiff_ext_preferences_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(object);
  GValue* dst;

  dst = preferences->values + prop_id;
  if(G_UNLIKELY(!G_IS_VALUE(dst))) {
    g_value_init(dst, pspec->value_type);
    g_param_value_set_default(pspec, dst);
  }

  if(g_param_values_cmp(pspec, value, dst) != 0) {
    g_value_copy(value, dst);
/*    xdiff_ext_preferences_queue_store(preferences); /* use explicit store of prorepties */
  }
}



static void
xdiff_ext_preferences_resume_monitor(xdiff_ext_preferences* preferences) {
  ThunarVfsPath* path;
  gchar* filename;

  /* verify that the monitor is suspended */
  if(G_LIKELY(preferences->handle == NULL)) {
    /* determine the save location for thunarrc to monitor */
    filename = xfce_resource_save_location(XFCE_RESOURCE_CONFIG, RC_PATH, TRUE);
    if(G_LIKELY(filename != NULL)) {
      /* determine the VFS path for the filename */
      path = thunar_vfs_path_new(filename, NULL);
      if(G_LIKELY(path != NULL)) {
        /* add the monitor handle for the file */
        preferences->handle = thunar_vfs_monitor_add_file(preferences->monitor, path, xdiff_ext_preferences_monitor, preferences);
        thunar_vfs_path_unref(path);
      }

      /* release the filename */
      g_free(filename);
    }
  }
}



static void
xdiff_ext_preferences_suspend_monitor(xdiff_ext_preferences* preferences) {
  /* verify that the monitor is active */
  if(G_LIKELY(preferences->handle != NULL)) {
    /* disconnect the handle from the monitor */
    thunar_vfs_monitor_remove(preferences->monitor, preferences->handle);
    preferences->handle = NULL;
  }
}



static void
xdiff_ext_preferences_monitor(ThunarVfsMonitor* monitor,
                                                ThunarVfsMonitorHandle* handle,
                                                ThunarVfsMonitorEvent event,
                                                ThunarVfsPath* handle_path,
                                                ThunarVfsPath* event_path,
                                                gpointer user_data) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(user_data);

  g_return_if_fail(XDIFF_EXT_IS_PREFERENCES(preferences));
  g_return_if_fail(THUNAR_VFS_IS_MONITOR(monitor));
  g_return_if_fail(preferences->monitor == monitor);
  g_return_if_fail(preferences->handle == handle);

  /* schedule a reload whenever the file is created/changed */
  if(event == THUNAR_VFS_MONITOR_EVENT_CHANGED || event == THUNAR_VFS_MONITOR_EVENT_CREATED) {
    xdiff_ext_preferences_queue_load(preferences);
  }
}



static void
xdiff_ext_preferences_queue_load(xdiff_ext_preferences* preferences) {
  if(preferences->load_idle_id == 0 && preferences->store_idle_id == 0) {
    preferences->load_idle_id = g_idle_add_full(G_PRIORITY_LOW, xdiff_ext_preferences_load,
                                                 preferences, xdiff_ext_preferences_load_idle_destroy);
  }
}



static void
xdiff_ext_preferences_queue_store(xdiff_ext_preferences* preferences) {
  if(preferences->store_idle_id == 0 && !preferences->loading_in_progress) {
    preferences->store_idle_id = g_idle_add_full(G_PRIORITY_LOW, xdiff_ext_preferences_store,
                                                  preferences, xdiff_ext_preferences_store_idle_destroy);
  }
}



static gchar*
property_name_to_option_name(const gchar* property_name) {
  const gchar* s;
  gboolean upper = TRUE;
  gchar* option;
  gchar* t;

  option = g_new(gchar, strlen(property_name) + 1);
  for(s = property_name, t = option; *s != '\0'; ++s) {
    if(*s == '-') {
      upper = TRUE;
    } else if(upper) {
      *t++ = g_ascii_toupper(*s);
      upper = FALSE;
    } else {
      *t++ = *s;
    }
  }
  *t = '\0';

  return option;
}



gboolean
xdiff_ext_preferences_load(gpointer user_data) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(user_data);
  const gchar* string;
  GParamSpec** specs;
  GParamSpec* spec;
  XfceRc* rc;
  GValue dst = { 0, };
  GValue src = { 0, };
  gchar* option;
  guint nspecs;
  guint n;

  rc = xfce_rc_config_open(XFCE_RESOURCE_CONFIG, RC_PATH, TRUE);
  if(G_LIKELY(rc != NULL)) {
    g_object_freeze_notify(G_OBJECT(preferences));

    xfce_rc_set_group(rc, "Configuration");

    preferences->loading_in_progress = TRUE;

    specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(preferences), &nspecs);
    
    for(n = 0; n < nspecs; ++n) {
      spec = specs[n];

      option = property_name_to_option_name(spec->name);
      string = xfce_rc_read_entry(rc, option, NULL);
      g_free(option);

      if(G_LIKELY(string != NULL)) {
        g_value_init(&src, G_TYPE_STRING);
        g_value_set_static_string(&src, string);

        if(spec->value_type == G_TYPE_STRING) {
          g_object_set_property(G_OBJECT(preferences), spec->name, &src);
        } else if(g_value_type_transformable(G_TYPE_STRING, spec->value_type)) {
          g_value_init(&dst, spec->value_type);
          if(g_value_transform(&src, &dst)) {
            g_object_set_property(G_OBJECT(preferences), spec->name, &dst);
          }
          g_value_unset(&dst);
        } else {
          g_warning("Failed to load property \"%s\"", spec->name);
        }

        g_value_unset(&src);
      }
    }
    
    g_free(specs);

    preferences->loading_in_progress = FALSE;

    xfce_rc_close(rc);

    g_object_thaw_notify(G_OBJECT(preferences));
  } else {
    g_warning("Failed to load thunar preferences.");
  }

  return FALSE;
}



static void
xdiff_ext_preferences_load_idle_destroy(gpointer user_data) {
  XDIFF_EXT_PREFERENCES(user_data)->load_idle_id = 0;
}



gboolean
xdiff_ext_preferences_store(gpointer user_data) {
  xdiff_ext_preferences* preferences = XDIFF_EXT_PREFERENCES(user_data);
  const gchar* string;
  GParamSpec** specs;
  GParamSpec* spec;
  XfceRc* rc;
  GValue dst = { 0, };
  GValue src = { 0, };
  gchar* option;
  guint nspecs;
  guint n;

  rc = xfce_rc_config_open(XFCE_RESOURCE_CONFIG, RC_PATH, FALSE);
  
  if(G_LIKELY(rc != NULL)) {
    /* suspend the monitor(hopefully tricking FAM to avoid unnecessary reloads) */
    xdiff_ext_preferences_suspend_monitor(preferences);
    g_object_unref(G_OBJECT(preferences->monitor));

    xfce_rc_set_group(rc, "Configuration");

    specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(preferences), &nspecs);
    
    for(n = 0; n < nspecs; ++n) {
      spec = specs[n];

      g_value_init(&dst, G_TYPE_STRING);

      if(spec->value_type == G_TYPE_STRING) {
        g_object_get_property(G_OBJECT(preferences), spec->name, &dst);
      } else {
        g_value_init(&src, spec->value_type);
        g_object_get_property(G_OBJECT(preferences), spec->name, &src);
        g_value_transform(&src, &dst);
        g_value_unset(&src);
      }

      /* determine the option name for the spec */
      option = property_name_to_option_name(spec->name);

      /* store the setting */
      string = g_value_get_string(&dst);
      if(G_LIKELY(string != NULL)) {
        xfce_rc_write_entry(rc, option, string);
      }

      /* cleanup */
      g_value_unset(&dst);
      g_free(option);
    }

    /* cleanup */
    xfce_rc_close(rc);
    g_free(specs);

    /* restart the monitor */
    xdiff_ext_preferences_resume_monitor(preferences);

  } else {
    g_warning("Failed to store thunar preferences.");
  }
  
  return FALSE;
}



static void
xdiff_ext_preferences_store_idle_destroy(gpointer user_data) {
  XDIFF_EXT_PREFERENCES(user_data)->store_idle_id = 0;
}



/**
 * xdiff_ext_preferences_get:
 *
 * Queries the global #xdiff_ext_preferences instance, which is shared
 * by all modules. The function automatically takes a reference
 * for the caller, so you'll need to call g_object_unref() when
 * you're done with it.
 *
 * Return value: the global #xdiff_ext_preferences instance.
 **/
xdiff_ext_preferences*
xdiff_ext_preferences_instance() {
  static xdiff_ext_preferences* preferences = NULL;

  if(G_UNLIKELY(preferences == NULL)) {
    preferences = g_object_new(XDIFF_EXT_TYPE_PREFERENCES, NULL);
    g_object_add_weak_pointer(G_OBJECT(preferences),
                              (gpointer)&preferences);
  } else {
    g_object_ref(G_OBJECT(preferences));
  }

  return preferences;
}
