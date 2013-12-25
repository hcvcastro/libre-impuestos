/*
 * libre-impuesto-window-private.h
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

#ifndef LIBRE_IMPUESTO_WINDOW_PRIVATE_H
#define LIBRE_IMPUESTO_WINDOW_PRIVATE_H

#include "libre-impuesto-window.h"
#include <string.h>
#include <glib/gi18n.h>
#include <util/util.h>
#include <libre-impuesto.h>
#include <libre-impuesto-window-actions.h>


#define LIBRE_IMPUESTO_WINDOW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE \
	((obj), TYPE_LIBRE_IMPUESTO_WINDOW, LibreImpuestoWindowPrivate))

/* Shorthand, requires a variable named "impuesto_window". */
#define ACTION(name) \
	(LIBRE_IMPUESTO_WINDOW_ACTION_##name (impuesto_window))
#define ACTION_GROUP(name) \
	(LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_##name (impuesto_window))

/* For use in dispose() methods. */
#define DISPOSE(obj) \
	G_STMT_START { \
	if ((obj) != NULL) { g_object_unref (obj); (obj) = NULL; } \
	} G_STMT_END

G_BEGIN_DECLS

struct _LibreImpuestoWindowPrivate {

  gpointer impuesto;  /* weak pointer */

  /*** UI Management ***/

  GtkUIManager *ui_manager;

  /* Miscellaneous */
  GtkWidget *menubar_box;
  GtkNotebook *notebook;
  GtkStack *stack;
  
  gchar *geometry;

  guint sidebar_visible  : 1;
  guint headerbar_visible  : 1;
  guint statusbar_visible  : 1;
};

void		libre_impuesto_window_private_init	(LibreImpuestoWindow *impuseto_window);
void		libre_impuesto_window_private_constructed
						(LibreImpuestoWindow *impuesto_window);
void		libre_impuesto_window_private_dispose	(LibreImpuestoWindow *impuesto_window);
void		libre_impuesto_window_private_finalize	(LibreImpuestoWindow *impuesto_window);

void
libre_impuesto_window_actions_init (LibreImpuestoWindow *impuesto_window);


G_END_DECLS

#endif /* LIBRE_IMPUESTO_WINDOW_PRIVATE_H */
