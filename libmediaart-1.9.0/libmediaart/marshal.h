
#ifndef __media_art_marshal_MARSHAL_H__
#define __media_art_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:STRING,STRING (marshal.list:1) */
extern void media_art_marshal_VOID__STRING_STRING (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);

/* VOID:STRING,STRING,STRING,BOOLEAN,BOOLEAN (marshal.list:2) */
extern void media_art_marshal_VOID__STRING_STRING_STRING_BOOLEAN_BOOLEAN (GClosure     *closure,
                                                                          GValue       *return_value,
                                                                          guint         n_param_values,
                                                                          const GValue *param_values,
                                                                          gpointer      invocation_hint,
                                                                          gpointer      marshal_data);

G_END_DECLS

#endif /* __media_art_marshal_MARSHAL_H__ */

