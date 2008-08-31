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

#include "interface.h"
#include "support.h"

void
apply() {
  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();

  xdiff_ext_preferences_store(p);

  g_object_unref(p);
}

int
main(int argc, char *argv[]) {
  GtkWidget *main;

#ifdef ENABLE_NLS
  bindtextdomain(GETTEXT_PACKAGE, SETUP_LOCALE_DIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */

  gtk_set_locale();
  gtk_init(&argc, &argv);
  thunar_vfs_init();
  g_type_init();
  if (!g_thread_supported ()) {
    g_thread_init (NULL);
  }

  add_pixmap_directory(SETUP_DATA_DIR "/" PACKAGE_NAME "/pixmaps");
  add_pixmap_directory(SETUP_DATA_DIR "/" PACKAGE_NAME);

  xdiff_ext_preferences* p = xdiff_ext_preferences_instance();
  xdiff_ext_preferences_load(p);
  
  main = create_main ();
  gtk_widget_show (main);

  gtk_main ();
  
  g_object_unref(p);

  return 0;
}
