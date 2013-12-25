/*
 * impuesto.h
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
 * Copyright (C) 2013 Libre Software Bolivia. Henry Castro <hcvcastro@gmail.com>
 *
 */

#ifndef LIBRE_IMPUESTO_H
#define LIBRE_IMPUESTO_H

#include <gtk/gtk.h>
#include <impuesto/libre-impuesto-common.h>

/* Standard GObject macros */
#define TYPE_LIBRE_IMPUESTO \
	(libre_impuesto_get_type ())
#define LIBRE_IMPUESTO(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), TYPE_LIBRE_IMPUESTO, LibreImpuesto))
#define LIBRE_IMPUESTO_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), TYPE_LIBRE_IMPUESTO, LibreImpuestoClass))
#define IS_LIBRE_IMPUESTO(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), TYPE_LIBRE_IMPUESTO))
#define IS_LIBRE_IMPUESTO_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), TYPE_LIBRE_IMPUESTO))
#define LIBRE_IMPUESTO_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), TYPE_LIBRE_IMPUESTO, LibreImpuestoClass))

G_BEGIN_DECLS

GType libre_impuesto_quit_reason_get_type (void);
#define TYPE_LIBRE_IMPUESTO_QUIT_REASON (libre_impuesto_quit_reason_get_type())


typedef struct _LibreImpuesto LibreImpuesto;
typedef struct _LibreImpuestoClass LibreImpuestoClass;
typedef struct _LibreImpuestoPrivate LibreImpuestoPrivate;

typedef enum {
	LIBRE_IMPUESTO_QUIT_ACTION,
	LIBRE_IMPUESTO_QUIT_LAST_WINDOW,
	LIBRE_IMPUESTO_QUIT_OPTION,
	LIBRE_IMPUESTO_QUIT_REMOTE_REQUEST,
	LIBRE_IMPUESTO_QUIT_SESSION_REQUEST
} LibreImpuestoQuitReason;

struct _LibreImpuesto {
	GtkApplication parent;
	LibreImpuestoPrivate *priv;
};

struct _LibreImpuestoClass {
	GtkApplicationClass parent_class;
};

GType	libre_impuesto_get_type	(void);
gboolean libre_impuesto_quit (LibreImpuesto *impuesto, 
			      LibreImpuestoQuitReason reason);
LibreImpuesto * libre_impuesto_get_default (void);
GtkWidget * libre_impuesto_create_main_window (LibreImpuesto *impuesto);
GtkWindow * libre_impuesto_get_active_window (LibreImpuesto *impuesto);
void libre_impuesto_maximize_window (LibreImpuesto *impuesto,
				     GtkWindow *window);

GtkStyleProvider * libre_impuesto_get_provider (LibreImpuesto *impuesto);
GtkTreeStore * libre_impuesto_get_tree_data_general (LibreImpuesto *impuesto);
GtkTreeStore * libre_impuesto_get_tree_data_formulario (LibreImpuesto *impuesto);
GtkTreeStore * libre_impuesto_get_tree_data_consultor (LibreImpuesto *impuesto);
LibreImpuesto * libre_impuesto_create_instance();


G_END_DECLS

#endif /* LIBRE_IMPUESTO_H */
