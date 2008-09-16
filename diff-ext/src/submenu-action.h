#ifndef __submenu_action__
#define __submenu_action__

#include <gtk/gtkaction.h>

G_BEGIN_DECLS

#define XDIFF_EXT_TYPE_SUBMENU_ACTION            (xdiff_ext_submenu_action_get_type ())
#define XDIFF_EXT_SUBMENU_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XDIFF_EXT_TYPE_SUBMENU_ACTION, xdiff_ext_submenu_action))
#define XDIFF_EXT_SUBMENU_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XDIFF_EXT_TYPE_SUBMENU_ACTION, xdiff_ext_submenu_action_class))
#define XDIFF_EXT_IS_SUBMENU_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XDIFF_EXT_TYPE_SUBMENU_ACTION))
#define XDIFF_EXT_IS_SUBMENU_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), XDIFF_EXT_TYPE_SUBMENU_ACTION))
#define XDIFF_EXT_SUBMENU_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), XDIFF_EXT_TYPE_SUBMENU_ACTION, xdiff_ext_submenu_action_class))

typedef struct _xdiff_ext_submenu_action		xdiff_ext_submenu_action;
typedef struct _xdiff_ext_submenu_action_class	xdiff_ext_submenu_action_class;

xdiff_ext_submenu_action* xdiff_ext_submenu_action_new(const gchar* name, const gchar* label, const gchar* tooltip, const gchar* stock_id);
void xdiff_ext_submenu_action_add(xdiff_ext_submenu_action* action, GtkAction* sub_action);

GType	xdiff_ext_submenu_action_get_type();

G_END_DECLS

#endif /* submenu_action */
