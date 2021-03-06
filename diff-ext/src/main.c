/*
 * Copyright (c) 2008-2009, Sergey Zorin. All rights reserved.
 * Copyright (c) 2016, Weitian Leung. All rights reserved.
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
#include <glib.h>
#include <glib/gi18n-lib.h>

#include <thunarx/thunarx.h>

#include <preferences.h>

#include "submenu-action.h"

static GType type_list[1];

static GQueue* _saved = 0;

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
  // handle spawn errors
  g_free(diff_tool);
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
// handle spawn errors
  g_free(diff_tool);
}

void
clear_queue(GQueue* queue) {
  while(!g_queue_is_empty(queue)) {
    g_queue_pop_head(queue);
  }
}

gint
path_compare(ThunarxFileInfo* a, ThunarxFileInfo* b) {
  gchar* p1 = thunarx_file_info_get_name(a);
  gchar* p2 = thunarx_file_info_get_name(b);

  gint result = -1;

  if (p1 == NULL && p2 == NULL)
    result = 0;
  else if (p1 == NULL)
    result = -1;
  else if (p2 == NULL)
    result = 1;
  else
    result = strcmp(p1, p2);

  g_free(p1);
  g_free(p2);

  return result;
}

void
compare_later(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::save"));
    
  while(files) {
    GList* link;

    link = g_queue_find_custom(_saved, files->data, (GCompareFunc)path_compare);
    if(link == NULL) {
      g_queue_push_head(_saved, files->data);
    } else {
      g_queue_unlink(_saved, link);
      g_queue_push_head_link(_saved, link);
    }
    // g_free(path) ???
    files = g_list_next(files);
  }
}

void
compare_to(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare_to"));
  GList* saved = (GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::saved");
  GFile* f;
  gchar* f1;
  gchar* f2;
  gboolean keep_file = FALSE;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "keep-files-in-list", &keep_file, NULL);
  
  g_object_unref(p);

  f = thunarx_file_info_get_location(files->data);
  f1 = g_file_get_path(f);
  g_object_unref(f);

  f = thunarx_file_info_get_location(saved->data);
  f2 = g_file_get_path(f);
  g_object_unref(f);

  diff(f1, f2);

  g_free(f1);
  g_free(f2);
  
  if(!keep_file) {
    clear_queue(_saved);
  }
}

void
compare3_to(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare_to"));
  GList* saved = (GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::saved");
  GFile* f;
  gchar* f1 = 0;
  gchar* f2 = 0;
  gchar* f3 = (gchar*)saved->data;
  gboolean keep_file = FALSE;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  
  g_object_get(G_OBJECT(p), "keep-files-in-list", &keep_file, NULL);
  
  g_object_unref(p);

  f = thunarx_file_info_get_location(files->data);
  f1 = g_file_get_path(f);
  g_object_unref(f);

  files = g_list_next(files);
  f = thunarx_file_info_get_location(files->data);
  f2 = g_file_get_path(f);
  g_object_unref(f);

  f = thunarx_file_info_get_location(saved->data);
  f3 = g_file_get_path(f);
  g_object_unref(f);

  diff3(f1, f2, f3);

  g_free(f1);
  g_free(f2);
  g_free(f3);
  
  if(!keep_file) {
    clear_queue(_saved);
  }
}

void
compare(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare"));
  GFile* f;
  gchar* f1 = 0;
  gchar* f2 = 0;

  f = thunarx_file_info_get_location(files->data);
  f1 = g_file_get_path(f);
  g_object_unref(f);

  files = g_list_next(files);
  f = thunarx_file_info_get_location(files->data);
  f2 = g_file_get_path(f);
  g_object_unref(f);

  diff(f1, f2);
  
  g_free(f1);
  g_free(f2);
}

void
compare3(GtkAction* action, GtkWidget* window) {
  GList* files = g_list_first((GList*)g_object_get_data(G_OBJECT(action), "xdiff-ext::compare3"));
  GFile* f;
  gchar* f1 = 0;
  gchar* f2 = 0;
  gchar* f3 = 0;

  f = thunarx_file_info_get_location(files->data);
  f1 = g_file_get_path(f);
  g_object_unref(f);

  files = g_list_next(files);
  f = thunarx_file_info_get_location(files->data);
  f2 = g_file_get_path(f);
  g_object_unref(f);

  files = g_list_next(files);
  f = thunarx_file_info_get_location(files->data);
  f3 = g_file_get_path(f);
  g_object_unref(f);

  diff3(f1, f2, f3);
  g_free(f1);
  g_free(f2);
  g_free(f3);
}

void
clear(GtkAction* action, GtkWidget* window) {
  clear_queue(_saved);
}

static void
diff_ext_class_init(DIFF_EXT_CLASS* klass) {
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

static void
add_compare_to_menu(GList* actions, GtkWidget* window, GList* files, char* menu_name, const gchar* icon_name, GString* (*make_hint)(va_list, char*), GCallback callback, ...) {
  GtkAction* action;    
  GList* head = g_queue_peek_head_link(_saved);
  xdiff_ext_submenu_action* submenu;
  GString* name = g_string_new("");
  int n = 1;
  
  submenu = xdiff_ext_submenu_action_new("xdiff-ext::compare_to_menu", menu_name, "", NULL);
  xdiff_ext_submenu_action_set_icon_name(submenu, icon_name);

  actions = g_list_append(actions, submenu);

  while(head) {
    va_list args;
    GIcon* icon;
    GFile* f;
    GFileInfo* fi;
    GString* hint;
    gchar* head_file;

    f = thunarx_file_info_get_location(head->data);
    fi = thunarx_file_info_get_file_info(head->data);
    head_file = g_file_get_path(f);
    icon = g_file_info_get_icon(fi);

    g_object_unref(f);
    g_object_unref(fi);

    va_start(args, callback);

    hint = make_hint(args, head_file);

    g_string_printf(name, "xdiff-ext::compare_to_%d", n);
    
    action = gtk_action_new(name->str, head_file, hint->str, NULL);
    gtk_action_set_gicon(action, icon);
    g_signal_connect(G_OBJECT(action), "activate", callback, window);
    g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
    g_object_set_data(G_OBJECT(action), "xdiff-ext::saved", head);
    xdiff_ext_submenu_action_add(submenu, action);
    
    g_string_free(hint, TRUE);
    g_free(head_file);

    head = g_list_next(head);
    n++;
    va_end(args);
  }
  
  xdiff_ext_submenu_action_add(submenu, NULL);
  
  action = gtk_action_new("xdiff-ext::clear", _("Clear"), _("Clear selected files list"), GTK_STOCK_CLEAR);
  g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(clear), window);
  xdiff_ext_submenu_action_add(submenu, action);
  
  g_string_free(name, TRUE);
}

static GString* 
make_hint(va_list args, char* path2) {
  char* path = va_arg(args, char*);
  GString* hint = g_string_new("");
  
  g_string_printf(hint, _("Compare '%s' and '%s'"), path, path2);
  
  return hint;
}

static GString* 
make_hint3(va_list args, char* path3) {
  char* path1 = va_arg(args, char*);
  char* path2 = va_arg(args, char*);
  GString* hint = g_string_new("");
  
  g_string_printf(hint, _("3-way compare '%s', '%s' and '%s'"), path1, path2, path3);
  
  return hint;
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
      GtkAction* action = gtk_action_new("xdiff-ext::save", _("Compare later"), _("Select file for comparison"), NULL);
      gtk_action_set_icon_name(action, "diff_later");
      g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_later), window);
      g_object_set_data_full(G_OBJECT(action), "xdiff-ext::save", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
      actions = g_list_append(actions, action);
      
      if(n == 1) {
        if(!g_queue_is_empty(_saved)) {
          GString* caption = g_string_new("");
          GString* hint = g_string_new("");
          GList* head = g_queue_peek_head_link(_saved);
          gchar* head_file;
          gchar* path;
          GFile* f;

          f = thunarx_file_info_get_location(head->data);
          head_file = g_file_get_path(f);
          g_object_unref(f);
          f = thunarx_file_info_get_location(files->data);
          path = g_file_get_path(f);
          g_object_unref(f);

          g_string_printf(caption,_("Compare to '%s'"), head_file);
          g_string_printf(hint, _("Compare '%s' and '%s'"), path, head_file);

          action = gtk_action_new("xdiff-ext::compare_to", caption->str, hint->str, NULL);
          gtk_action_set_icon_name(action, "diff_with");
          g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_to), window);
          g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
          g_object_set_data(G_OBJECT(action), "xdiff-ext::saved", head);
          actions = g_list_append(actions, action);
          
          if(_saved->length > 1) {
            add_compare_to_menu(actions, window, files, _("Compare to"), "diff_with", make_hint, G_CALLBACK(compare_to), path);
          }
          
          g_free(head_file);
          g_free(path);
          g_string_free(caption, TRUE);
          g_string_free(hint, TRUE);
        }
      } else if(n == 2) {
        GtkAction* action = gtk_action_new("xdiff-ext::compare", _("Compare"), _("Compare selected files"), NULL);
        gtk_action_set_icon_name(action, "diff");
        g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare), window);
        g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
        actions = g_list_append(actions, action);

        if(strcmp("", three_way_compare_command) != 0) {
          if(!g_queue_is_empty(_saved)) {
            GString* caption = g_string_new("");
            GString* hint = g_string_new("");
            GList* head = g_queue_peek_head_link(_saved);
            gchar* head_file = (gchar*)head->data;
            gchar* path1;
            gchar* path2;
            GFile* f;

            f = thunarx_file_info_get_location(head->data);
            head_file = g_file_get_path(f);
            g_object_unref(f);
            f = thunarx_file_info_get_location(files->data);
            path1 = g_file_get_path(f);
            g_object_unref(f);

            files = g_list_next(files);

            f = thunarx_file_info_get_location(files->data);
            path2 = g_file_get_path(f);
            g_object_unref(f);

            g_string_printf(caption, _("3-way compare to '%s'"), head_file);
            g_string_printf(hint, _("3-way compare '%s', '%s' and '%s'"), path1, path2, head_file);
            
            action = gtk_action_new("xdiff-ext::compare_to", caption->str, hint->str, NULL);
            gtk_action_set_icon_name(action, "diff3_with");
            g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare3_to), window);
            g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
            g_object_set_data(G_OBJECT(action), "xdiff-ext::saved", head);
            actions = g_list_append(actions, action);

            if(_saved->length > 1) {
              add_compare_to_menu(actions, window, files, _("3-way compare to"), "diff3_with", make_hint3, G_CALLBACK(compare3_to), path1, path2);
            }
            
            g_free(head_file);
            g_free(path1);
            g_free(path2);
            g_string_free(caption, TRUE);
            g_string_free(hint, TRUE);
          }
        }
      } else if(n == 3) {
        if(strcmp("", three_way_compare_command) != 0) {
          GtkAction* action = gtk_action_new("xdiff-ext::compare3", _("3-way Compare"), _("Compare selected files"), NULL);
          gtk_action_set_icon_name(action, "diff3");
          g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare3), window);
          g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare3", thunarx_file_info_list_copy(files),(GDestroyNotify)thunarx_file_info_list_free);
          actions = g_list_append(actions, action);
        }
      }
    }

    g_free(three_way_compare_command);
  }

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
    gtk_action_set_icon_name(action, "diff_later");
    g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_later), window);
    g_object_set_data_full(G_OBJECT(action), "xdiff-ext::save", thunarx_file_info_list_copy(files), (GDestroyNotify)thunarx_file_info_list_free);
    actions = g_list_append(actions, action);
    
    if(!g_queue_is_empty(_saved)) {
      GString* caption = g_string_new("");
      GString* hint = g_string_new("");
      GList* head = g_queue_peek_head_link(_saved);

      gchar* uri = thunarx_file_info_get_uri((ThunarxFileInfo*)head->data);
      gchar* path = g_filename_from_uri(uri, NULL, NULL);
      gchar* head_file = path;
      g_free(uri);

      uri = thunarx_file_info_get_uri((ThunarxFileInfo*)files->data);
      path = g_filename_from_uri(uri, NULL, NULL);
      g_free(uri);
      
      g_string_printf(caption, _("Compare to '%s'"), head_file);
      g_string_printf(hint, _("Compare '%s' and '%s'"), path, head_file);

      g_free(head_file);
      g_free(path);
      
      action = gtk_action_new("xdiff-ext::compare_to", caption->str, hint->str, NULL);
      gtk_action_set_icon_name(action, "diff_with");
      g_signal_connect(G_OBJECT(action), "activate", G_CALLBACK(compare_to), window);
      g_object_set_data_full(G_OBJECT(action), "xdiff-ext::compare_to", thunarx_file_info_list_copy(files), (GDestroyNotify)thunarx_file_info_list_free);
      g_object_set_data(G_OBJECT(action), "xdiff-ext::saved", head);
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
#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, DIFF_EXT_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
#endif /* ENABLE_NLS */

    /* register the types provided by this plugin */
    diff_ext_register_type(plugin);

    /* setup the plugin type list */
    type_list[0] = diff_ext_get_type();
    _saved = g_queue_new();

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
