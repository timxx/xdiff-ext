#ifndef __preferences_h__
#define __preferences_h__

#include <glib-object.h>

G_BEGIN_DECLS;

typedef struct _xdiff_ext_preferences_class xdiff_ext_preferences_class;
typedef struct _xdiff_ext_preferences xdiff_ext_preferences;

#define XDIFF_EXT_TYPE_PREFERENCES             (xdiff_ext_preferences_get_type ())
#define XDIFF_EXT_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), XDIFF_EXT_TYPE_PREFERENCES, xdiff_ext_preferences))
#define XDIFF_EXT_IS_PREFERENCES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XDIFF_EXT_TYPE_PREFERENCES))
#define XDIFF_EXT_PREFERENCES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), XDIFF_EXT_TYPE_PREFERENCES, xdiff_ext_preferences_class))
#define XDIFF_EXT_IS_PREFERENCES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), XDIFF_EXT_TYPE_PREFERENCES))
#define XDIFF_EXT_PREFERENCES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), XDIFF_EXT_TYPE_PREFERENCES, xdiff_ext_preferences_class))

GType xdiff_ext_preferences_get_type () G_GNUC_CONST;

xdiff_ext_preferences* xdiff_ext_preferences_instance();

gboolean xdiff_ext_preferences_load(gpointer user_data);
gboolean xdiff_ext_preferences_store(gpointer user_data);


G_END_DECLS;

#endif /* __preferences_h__ */
