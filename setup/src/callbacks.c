/*
 * Copyright (c) 2008, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the COPYING file provided with this software.
 *
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <preferences.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

void
on_ok_clicked(GtkButton* button, gpointer user_data) {
  apply();
  gtk_main_quit();
}

void
on_apply_clicked(GtkButton* button, gpointer user_data) {
  apply();
}

void
on_about_clicked(GtkButton* button, gpointer user_data) {
  GtkWidget *about;
  const gchar *authors[] = {
    "Sergey Zorin <szorin@comcast.net>",
    NULL
  };
  
  /* TRANSLATORS: Replace this string with your names, one name per line. */
  gchar *translators = _("Sergey Zorin <szorin@comcast.net>");
  GdkPixbuf *about_logo_pixbuf;

  about = gtk_about_dialog_new();
  gtk_widget_set_name(about, "about");
  gtk_window_set_destroy_with_parent(GTK_WINDOW(about), TRUE);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), VERSION);
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about), _("xdiff-ext setup"));
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), _("(c) 2008 Sergey Zorin. All rights reserved."));
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), _("Copyright (c) 2008, Sergey Zorin\nAll rights reserved.\n\nRedistribution and use in source and binary forms, with or without\nmodification, are permitted provided that the following conditions\nare met:\n   1. Redistributions  of  source  code   must  retain  the  above\n      copyright notice,  this list of conditions and the following\n      disclaimer.\n   2. Redistributions in binary  form  must  reproduce  the  above\n      copyright notice,  this list of conditions and the following\n      disclaimer  in  the  documentation  and/or  other  materials\n      provided with the distribution.\n\nTHIS SOFTWARE  IS  PROVIDED  BY  THE  COPYRIGHT  HOLDERS  AND  CONTRIBUTORS\n\"AS IS\" AND ANY EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT LIMITED\nTO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\nPURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  COPYRIGHT   OWNER  OR\nCONTRIBUTORS  BE  LIABLE  FOR  ANY DIRECT,  INDIRECT, INCIDENTAL,  SPECIAL,\nEXEMPLARY,  OR  CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,\nPROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;\nOR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY THEORY  OF  LIABILITY,\nWHETHER IN CONTRACT,  STRICT LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE OR\nOTHERWISE) ARISING  IN  ANY WAY OUT OF THE USE  OF THIS  SOFTWARE,  EVEN IF\nADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"));
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "http://diff-ext.sourceforge.net");
  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), _("xdiff-ext"));
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), authors);
  gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about), translators);
  about_logo_pixbuf = create_pixbuf("de.png");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), about_logo_pixbuf);

  g_object_set_data(G_OBJECT(about), "about", about);
  
  gtk_widget_show(about); 
}


void
on_diff_tool_selection_changed(GtkFileChooser* filechooser, gpointer user_data) {
  gchar* name = gtk_file_chooser_get_filename(filechooser);
  if(name != NULL) {
    xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
    GValue value = {0,};
    
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_static_string(&value, name);
    
    g_object_set_property(G_OBJECT(p), "compare-command", &value);

    g_object_unref(p);
  }
}


void
on_diff3_tool_selection_changed(GtkFileChooser* filechooser, gpointer user_data) {
  gchar* name = gtk_file_chooser_get_filename(filechooser);
  if(name != NULL) {
    xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
    GValue value = {0,};
    
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_static_string(&value, name);
    
    g_object_set_property(G_OBJECT(p), "three-way-compare-command", &value);

    g_object_unref(p);
  }
}

static void
set_filter(GtkFileChooser* filechooser) {
  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_add_mime_type(filter, "application/x-executable");
  gtk_file_chooser_set_filter(filechooser, filter);
}

void
on_diff_tool_realize(GtkWidget* widget, gpointer user_data) {
  gchar* cmd;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
 
  g_object_get(G_OBJECT(xdiff_ext_preferences_instance()), "compare-command", &cmd, NULL);

  g_object_unref(p);

  set_filter((GtkFileChooser*)widget);

  if(!g_path_is_absolute(cmd)) {
    cmd = g_find_program_in_path(cmd);
  }

  gtk_file_chooser_set_filename((GtkFileChooser*)widget, cmd);
}

void
on_diff3_tool_realize(GtkWidget* widget, gpointer user_data) {
  gchar* cmd;
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
 
  g_object_get(G_OBJECT(xdiff_ext_preferences_instance()), "three-way-compare-command", &cmd, NULL);

  g_object_unref(p);

  set_filter((GtkFileChooser*)widget);

  if(!g_path_is_absolute(cmd)) {
    cmd = g_find_program_in_path(cmd);
  }

  gtk_file_chooser_set_filename((GtkFileChooser*)widget, cmd);
}

void
on_keep_files_toggled(GtkToggleButton* togglebutton, gpointer user_data) {
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  GValue value = {0,};
  
  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, gtk_toggle_button_get_active(togglebutton));
  
  g_object_set_property(G_OBJECT(p), "keep-files-in-list", &value);

  g_object_unref(p);
}


void
on_keep_files_realize(GtkWidget* widget, gpointer user_data) {
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  gboolean value;
  
  g_object_get(G_OBJECT(xdiff_ext_preferences_instance()), "keep-files-in-list", &value, NULL);

  g_object_unref(p);

  gtk_toggle_button_set_active((GtkToggleButton*)widget, value);
}

