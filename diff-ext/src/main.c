/*
 * Copyright (c) 2008, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the COPYING file provided with this software.
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo.h>

#include <thunarx/thunarx.h>

#include <preferences.h>

static GType type_list[1];

static GString* saved = 0;

typedef struct _DIFF_EXT {
  GObject object;
} DIFF_EXT;

typedef struct _DIFF_EXT_CLASS {
  GObjectClass parent_class;
} DIFF_EXT_CLASS, DIFF_EXTClass;
  

static void menu_provider_init(ThunarxMenuProviderIface *iface);
static GList* get_file_actions(ThunarxMenuProvider* provider, GtkWidget* window, GList* files);
static GList *get_folder_actions(ThunarxMenuProvider* provider, GtkWidget* window, ThunarxFileInfo* folder);

THUNARX_DEFINE_TYPE_WITH_CODE(DIFF_EXT,
                               diff_ext,
                               G_TYPE_OBJECT,
                               THUNARX_IMPLEMENT_INTERFACE(THUNARX_TYPE_MENU_PROVIDER,
                                                            menu_provider_init));



void
diff(gchar* f1, gchar* f2) {
  gchar* argv[5];
  gchar* diff_tool = "";
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "compare-command", &diff_tool, NULL);
  
  g_object_unref(p);
  
  argv[0] = diff_tool;
  argv[1] = diff_tool;
  argv[2] = f1;
  argv[3] = f2;
  argv[4] = 0;
  
  g_spawn_async(0, argv, 0, G_SPAWN_FILE_AND_ARGV_ZERO | G_SPAWN_SEARCH_PATH, 0, 0, 0, 0);
}

void
diff3(gchar* f1, gchar* f2, gchar* f3) {
  gchar* argv[6];
  gchar* diff_tool;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "three-way-compare-command", &diff_tool, NULL);
  
  g_object_unref(p);

  argv[0] = diff_tool;
  argv[1] = diff_tool;
  argv[2] = f1;
  argv[3] = f2;
  argv[4] = f3;
  argv[5] = 0;
  
  g_spawn_async(0, argv, 0, G_SPAWN_FILE_AND_ARGV_ZERO | G_SPAWN_SEARCH_PATH, 0, 0, 0, 0);
}

void
compare_later(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::save"));
  gchar* uri;
  gchar* path;
    
  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  path = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);
  
  g_string_assign(saved, path);
  
  // g_free(path) ???
}

void
compare_to(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare_to"));
  gchar* f1 = 0;
  gchar* f2 = saved->str;
  gboolean keep_file = FALSE;
  gchar* uri;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "keep-files-in-list", &keep_file, NULL);
  
  g_object_unref(p);

  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f1 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);

  diff(f1, f2);

  if(!keep_file) {
    saved = g_string_assign(saved, "");
  }
}

void
compare3_to(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare3_to"));
  gchar* f1 = 0;
  gchar* f2 = 0;
  gchar* f3 = saved->str;
  gboolean keep_file = FALSE;
  gchar* uri;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "keep-files-in-list", &keep_file, NULL);
  
  g_object_unref(p);

  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f1 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);
  files = g_list_next(files);
  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f2 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);

  diff3(f1, f2, f3);

  if(!keep_file) {
    saved = g_string_assign(saved, "");
  }
}


void
compare(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare"));
  gchar* f1 = 0;
  gchar* f2 = 0;
  gchar* uri;

  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f1 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);
  files = g_list_next(files);
  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f2 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);

  diff(f1, f2);
}

void
compare3(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare3"));
  gchar* f1 = 0;
  gchar* f2 = 0;
  gchar* f3 = 0;
  gchar* uri;

  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f1 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);
  files = g_list_next(files);
  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f2 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);
  files = g_list_next(files);
  uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
  f3 = g_filename_from_uri(uri, NULL, NULL);
  g_free(uri);

  diff3(f1, f2, f3);
}



static void
diff_ext_class_init(DIFF_EXT_CLASS* klass)
{
  /* nothing to do here */
}



static void
diff_ext_init(DIFF_EXT* open_terminal) {
  /* nothing to do here */
}



static void
menu_provider_init(ThunarxMenuProviderIface* iface) {
  iface->get_file_actions = get_file_actions;
  iface->get_folder_actions = get_folder_actions;
  /* get a reference of iface here? */
  g_object_ref(iface);
}



static GList*
get_file_actions(ThunarxMenuProvider* provider, GtkWidget* window, GList* files) {
  GList* actions = 0;
	
  if(files != 0) {
    guint n = g_list_length(files);
    gchar* three_way_compare_command;
    xdiff_ext_preferences* p = xdiff_ext_preferences_instance();

    g_object_get(G_OBJECT(p), "three-way-compare-command", &three_way_compare_command, NULL);

    g_object_unref(p);
     
    if(n <= 3) {
      GList* scan = files;
      gboolean go = TRUE;
      
      while(scan && go) {
        gchar* scheme;

        scheme = thunarx_file_info_get_uri_scheme((ThunarxFileInfo*)(scan->data));
        go = strncmp(scheme, "file", 4) == 0;
        g_free(scheme);
        
        scan = g_list_next(scan);
      }
      
      if(go) {
        if(n == 1) {
          GtkAction* action = gtk_action_new("xdiff-ext::save", _("Compare later"), _("Select file for comparison"), NULL);
          g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_later), window);
          g_object_set_data_full(G_OBJECT(action), "xdiff-ext::save", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
          actions = g_list_append(actions, action);
          
          if(strcmp("", saved->str) != 0) {
            GString* caption = g_string_new("");
            GString* hint = g_string_new("");
            gchar* uri;
            gchar* path;

            uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
            path = g_filename_from_uri(uri, NULL, NULL);
            g_free(uri);
            
            g_string_printf(caption,_("Compare to '%s'"), saved->str);
            g_string_printf(hint, _("Compare '%s' and '%s'"), path, saved->str);
            
            action = gtk_action_new("xdiff-ext::compare_to", caption->str, hint->str, NULL);
            g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_to), window);
            g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
            actions = g_list_append(actions, action);
            
            g_string_free(caption, TRUE);
            g_string_free(hint, TRUE);
          }
        } else if(n == 2) {
          GtkAction* action = gtk_action_new("xdiff-ext::compare", _("Compare"), _("Compare selected files"), NULL);
          g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare), window);
          g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
          actions = g_list_append(actions, action);

          if(strcmp("", three_way_compare_command) != 0) {
            if(strcmp("", saved->str) != 0) {
              GString* caption = g_string_new("");
              GString* hint = g_string_new("");
              gchar* uri;
              gchar* path1;
              gchar* path2;

              uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
              path1 = g_filename_from_uri(uri, NULL, NULL);
              g_free(uri);
              files = g_list_next(files);
              uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
              path2 = g_filename_from_uri(uri, NULL, NULL);
              g_free(uri);
              
              g_string_printf(caption, _("3-way compare to '%s'"), saved->str);
              g_string_printf(hint, _("3-way compare '%s', '%s' and '%s'"), path1, path2, saved->str);
              
              action = gtk_action_new("xdiff-ext::compare3_to", caption->str, hint->str, NULL);
              g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare3_to), window);
              g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare3_to", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
              actions = g_list_append(actions, action);
              
              g_string_free(caption, TRUE);
              g_string_free(hint, TRUE);
            }
          }
        } else if(n == 3) {
          if(strcmp("", three_way_compare_command) != 0) {
            GtkAction* action = gtk_action_new("xdiff-ext::compare3", _("3-way Compare"), _("Compare selected files"), NULL);
            g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare3), window);
            g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare3", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
            actions = g_list_append(actions, action);
          }
        }
      }
    }
  }
          
  /* g_free(three_way_compare_command) ??? */
	
  return actions;
}



static GList*
get_folder_actions(ThunarxMenuProvider* provider, GtkWidget* window, ThunarxFileInfo* folder) {
  GList* actions = 0;
  gchar* scheme;

  scheme = thunarx_file_info_get_uri_scheme(folder);
  if(strncmp(scheme, "file", 4) == 0) {
    GList* files = g_list_append(0, folder);
    GtkAction* action = gtk_action_new("xdiff-ext::save", _("Compare later"), _("Select file for comparison"), NULL);
    g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_later), window);
    g_object_set_data_full(G_OBJECT(action), "xdiff-ext::save", thunarx_file_info_list_copy(files), (GDestroyNotify)thunarx_file_info_list_free);
    actions = g_list_append(actions, action);
    
    if(strcmp("", saved->str) != 0) {
      GString* caption = g_string_new("");
      GString* hint = g_string_new("");
      gchar* uri;
      gchar* path;

      uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
      path = g_filename_from_uri(uri, NULL, NULL);
      g_free(uri);
      
      g_string_printf(caption, _("Compare to '%s'"), saved->str);
      g_string_printf(hint, _("Compare '%s' and '%s'"), path, saved->str);
      
      action = gtk_action_new("xdiff-ext::compare_to", caption->str, hint->str, NULL);
      g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_to), window);
      g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files), (GDestroyNotify)thunarx_file_info_list_free);
      actions = g_list_append(actions, action);
      
      g_string_free(caption, TRUE);
      g_string_free(hint, TRUE);
    }
    
    g_list_free(files);
  }

  g_free(scheme);

  return actions;
}




G_MODULE_EXPORT void
thunar_extension_initialize(ThunarxProviderPlugin* plugin) {
  const gchar* mismatch;

  /* verify that the thunarx versions are compatible */
  mismatch = thunarx_check_version(THUNARX_MAJOR_VERSION, THUNARX_MINOR_VERSION, THUNARX_MICRO_VERSION);
  
  if(G_LIKELY(mismatch == NULL)) {
    g_message("Initializing xdiff-ext extension");
    
#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, DIFF_EXT_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
#endif /* ENABLE_NLS */

    /* register the types provided by this plugin */
    diff_ext_register_type(plugin);

    /* setup the plugin type list */
    type_list[0] = diff_ext_get_type();
    saved = g_string_new("");
    
    thunarx_provider_plugin_set_resident(plugin, TRUE);
  } else {
    g_warning("Version mismatch: %s", mismatch);
  }
}



G_MODULE_EXPORT void
thunar_extension_shutdown() {
  g_message("Shutting down xdiff-ext extension");
}



G_MODULE_EXPORT void
thunar_extension_list_types(const GType** types, gint* n_types) {
  *types = type_list;
  *n_types = G_N_ELEMENTS(type_list);
}
