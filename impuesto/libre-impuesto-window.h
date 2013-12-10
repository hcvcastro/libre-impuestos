/*
 * libre-impuesto-window.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the program; if not, see <http://www.gnu.org/licenses/>
 *
 *
 * Written November 2013 by Henry Castro <hcvcastro@gmail.com>.  
 *
 */

#ifndef LIBRE_IMPUESTO_WINDOW_H
#define LIBRE_IMPUESTO_WINDOW_H

#include <impuesto/libre-impuesto.h>


/* Standard GObject macros */
#define TYPE_LIBRE_IMPUESTO_WINDOW \
	(libre_impuesto_window_get_type ())
#define LIBRE_IMPUESTO_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), TYPE_LIBRE_IMPUESTO_WINDOW, LibreImpuestoWindow))
#define LIBRE_IMPUESTO_WINDOW_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), TYPE_LIBRE_IMPUESTO_WINDOW, LibreImpuestoWindowClass))
#define IS_LIBRE_IMPUESTO_WINDOW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), TYPE_LIBRE_IMPUESTO_WINDOW))
#define IS_LIBRE_IMPUESTO_WINDOW_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((obj), TYPE_LIBRE_IMPUESTO_WINDOW))
#define LIBRE_IMPUESTO_WINDOW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), TYPE_LIBRE_IMPUESTO_WINDOW, LibreImpuestoWindowClass))

G_BEGIN_DECLS

typedef struct _LibreImpuestoWindow LibreImpuestoWindow;
typedef struct _LibreImpuestoWindowClass LibreImpuestoWindowClass;
typedef struct _LibreImpuestoWindowPrivate LibreImpuestoWindowPrivate;

struct _LibreImpuestoWindow {
	GtkWindow parent;
	LibreImpuestoWindowPrivate *priv;
};

struct _LibreImpuestoWindowClass {
	GtkWindowClass parent_class;

  /* These are all protected methods.  Not for public use. */
  GtkWidget *	(*construct_menubar)	(LibreImpuestoWindow *impuesto_window);
  GtkWidget *	(*construct_sidebar)	(LibreImpuesto * impuesto, 
					 LibreImpuestoWindow *impuesto_window);
  GtkWidget *	(*construct_headerbar)	(LibreImpuestoWindow *impuesto_window);
};

GType		libre_impuesto_window_get_type		(void);
LibreImpuesto *	libre_impuesto_window_get_impuesto (LibreImpuestoWindow *impuesto_window);
GtkAction *	libre_impuesto_window_get_action (LibreImpuestoWindow *impuesto_window,
						 const gchar *action_name);
GtkWidget *	libre_impuesto_window_get_managed_widget
						(LibreImpuestoWindow *impuesto_window,
						 const gchar *widget_path);
void		libre_impuesto_window_add_action_group (LibreImpuestoWindow *impuesto_window,
						 const gchar *group_name);
GtkWidget *	libre_impuesto_window_get_menu_bar_box (LibreImpuestoWindow *impuesto_window);

GtkUIManager * libre_impuesto_window_get_ui_manager (LibreImpuestoWindow *impuesto_window);

GtkActionGroup *
libre_impuesto_window_get_action_group (LibreImpuestoWindow *impuesto_window,
					const gchar *group_name);

gboolean libre_impuesto_window_get_sidebar_visible (LibreImpuestoWindow *impuesto_window);

void
libre_impuesto_window_set_sidebar_visible (LibreImpuestoWindow *impuesto_window,
					   gboolean sidebar_visible);

gboolean
libre_impuesto_window_get_switcher_visible (LibreImpuestoWindow *impuesto_window);


void
libre_impuesto_window_set_switcher_visible (LibreImpuestoWindow *impuesto_window,
					    gboolean switcher_visible);

gboolean
libre_impuesto_window_get_headerbar_visible (LibreImpuestoWindow *impuesto_window);

void
libre_impuesto_window_set_headerbar_visible (LibreImpuestoWindow *impuesto_window,
					   gboolean taskbar_visible);

gboolean
libre_impuesto_window_get_statusbar_visible (LibreImpuestoWindow *impuesto_window);

void
libre_impuesto_window_set_statusbar_visible (LibreImpuestoWindow *impuesto_window,
					   gboolean toolbar_visible);

GtkWidget *
libre_impuesto_window_new (LibreImpuesto *impuesto,  const gchar *geometry);


G_END_DECLS

#endif /* LIBRE_IMPUESTO_WINDOW_H */
