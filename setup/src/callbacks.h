/*
 * Copyright (c) 2008, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the COPYING file provided with this software.
 *
 */
#include <gtk/gtk.h>

extern void apply();

void on_ok_clicked(GtkButton* button, gpointer user_data);
void on_apply_clicked(GtkButton* button, gpointer user_data);
void on_about_clicked(GtkButton* button, gpointer user_data);
void on_diff_tool_selection_changed(GtkFileChooser* filechooser, gpointer user_data);
void on_diff_tool_realize(GtkWidget* widget, gpointer user_data);
void on_diff3_tool_realize(GtkWidget* widget, gpointer user_data);
void on_diff3_tool_selection_changed(GtkFileChooser* filechooser, gpointer user_data);

void on_keep_files_toggled(GtkToggleButton *togglebutton, gpointer user_data);

void on_keep_files_realize(GtkWidget *widget, gpointer user_data);
