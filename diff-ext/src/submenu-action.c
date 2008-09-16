#include <config.h>

#include "submenu-action.h"

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkradiomenuitem.h>
#include <gtk/gtkseparatormenuitem.h>
#include <gtk/gtkimagemenuitem.h>


struct _xdiff_ext_submenu_action {
  GtkAction parent;
  
  GList* actions;
};

struct _xdiff_ext_submenu_action_class {
  GtkActionClass parent_class;
};

static void submenu_action_init(xdiff_ext_submenu_action* action);
static void submenu_action_class_init(xdiff_ext_submenu_action_class* klass);

static GObjectClass* parent_class = NULL;

GType
xdiff_ext_submenu_action_get_type () {
  static GType type = 0;

  if(G_UNLIKELY(type == 0)) {
    const GTypeInfo our_info = {
      sizeof (xdiff_ext_submenu_action_class),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) submenu_action_class_init,
      NULL,
      NULL, /* class_data */
      sizeof (xdiff_ext_submenu_action),
      0, /* n_preallocs */
      (GInstanceInitFunc)submenu_action_init,
      NULL,
    };

    type = g_type_register_static (GTK_TYPE_ACTION, "xdiff_ext_submenu_action", &our_info, 0);
  }

  return type;
}

xdiff_ext_submenu_action* 
xdiff_ext_submenu_action_new(const gchar *name, const gchar *label, const gchar *tooltip, const gchar *stock_id) {
  xdiff_ext_submenu_action* action;
  
  action = g_object_new (XDIFF_EXT_TYPE_SUBMENU_ACTION,
                         "name", name,
                         "label", label,
                         "tooltip", tooltip,
                         "stock_id", stock_id,
                         NULL);
  
  return action;
}

void 
xdiff_ext_submenu_action_add(xdiff_ext_submenu_action* action, GtkAction* subaction) {
  action->actions = g_list_append(action->actions, subaction);
}

static GtkWidget *
create_menu_item (GtkAction* action) {
  GtkWidget* menu;
  GtkWidget* menu_item;
  GList* actions = XDIFF_EXT_SUBMENU_ACTION(action)->actions;

  menu = gtk_menu_new ();
  
  while(actions) {
    GtkAction* a = GTK_ACTION(actions->data);
    
    if(a != NULL) {
      menu_item = gtk_action_create_menu_item(a);
      gtk_widget_show(menu_item);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    } else {
      GtkWidget* s = gtk_separator_menu_item_new();
      gtk_widget_show(s);
      gtk_menu_shell_append(GTK_MENU_SHELL(menu), s);
    }
    
    actions = g_list_next(actions);
  }

  gtk_widget_show(menu);

  menu_item = GTK_ACTION_CLASS(parent_class)->create_menu_item(action);

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);

  gtk_widget_show(menu_item);

  return menu_item;
}

static void
submenu_action_class_init (xdiff_ext_submenu_action_class* klass) {
  GtkActionClass* action_class = GTK_ACTION_CLASS (klass);
  GObjectClass* gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  action_class->menu_item_type = GTK_TYPE_IMAGE_MENU_ITEM;
  action_class->create_menu_item = create_menu_item;
}

static void 
submenu_action_init(xdiff_ext_submenu_action* action) {
}
