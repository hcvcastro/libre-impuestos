/*
  Widget Stack necesario para usarlo en el proyecto libre-impuestos
  como no esta disponible en la version estable de debian, es decir
  gtk-3.4.2, asi que tuve que extraer los archivos fuentes de la 
  version gtk 3.10.5, y hacer modificaciones para compilar los widgets
 */


/*
 * Copyright (c) 2013 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef __GTK_STACK_H__
#define __GTK_STACK_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define GTK_TYPE_STACK (gtk_stack_get_type ())
#define GTK_STACK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_STACK, GtkStack))
#define GTK_STACK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_STACK, GtkStackClass))
#define GTK_IS_STACK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_STACK))
#define GTK_IS_STACK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_STACK))
#define GTK_STACK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_STACK, GtkStackClass))

typedef struct _GtkStack GtkStack;
typedef struct _GtkStackPrivate GtkStackPrivate;
typedef struct _GtkStackClass GtkStackClass;

typedef enum {
  GTK_STACK_TRANSITION_TYPE_NONE,
  GTK_STACK_TRANSITION_TYPE_CROSSFADE,
  GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT,
  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT,
  GTK_STACK_TRANSITION_TYPE_SLIDE_UP,
  GTK_STACK_TRANSITION_TYPE_SLIDE_DOWN,
  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT,
  GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN
} GtkStackTransitionType;

struct _GtkStack {
  GtkContainer parent_instance;

  GtkStackPrivate *priv;
};

struct _GtkStackClass {
  GtkContainerClass parent_class;
};

GType                  gtk_stack_get_type                (void) G_GNUC_CONST;

GtkWidget *            gtk_stack_new                     (void);

void                   gtk_stack_add_named               (GtkStack               *stack,
                                                          GtkWidget              *child,
                                                          const gchar            *name);

void                   gtk_stack_add_titled              (GtkStack               *stack,
                                                          GtkWidget              *child,
                                                          const gchar            *name,
                                                          const gchar            *title);

void                   gtk_stack_set_visible_child       (GtkStack               *stack,
                                                          GtkWidget              *child);

GtkWidget *            gtk_stack_get_visible_child       (GtkStack               *stack);

void                   gtk_stack_set_visible_child_name  (GtkStack               *stack,
                                                          const gchar            *name);

const gchar *          gtk_stack_get_visible_child_name  (GtkStack               *stack);

void                   gtk_stack_set_visible_child_full  (GtkStack               *stack,
                                                          const gchar            *name,
                                                          GtkStackTransitionType  transition);

void                   gtk_stack_set_homogeneous         (GtkStack               *stack,
                                                          gboolean                homogeneous);

gboolean               gtk_stack_get_homogeneous         (GtkStack               *stack);

void                   gtk_stack_set_transition_duration (GtkStack               *stack,
                                                          guint                   duration);

guint                  gtk_stack_get_transition_duration (GtkStack               *stack);

void                   gtk_stack_set_transition_type     (GtkStack               *stack,
                                                          GtkStackTransitionType  transition);

GtkStackTransitionType gtk_stack_get_transition_type (GtkStack *stack);
GType gtk_stack_transition_get_type (void);
void gtk_widget_register_window (GtkWidget    *widget, GdkWindow    *window);
void gtk_widget_unregister_window (GtkWidget    *widget, GdkWindow    *window);



G_END_DECLS

#endif
