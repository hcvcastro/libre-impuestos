/*
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

#ifndef LIBRE_IMPUESTO_UTIL_H
#define LIBRE_IMPUESTO_UTIL_H

#include <sys/types.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <libxml/tree.h>

G_BEGIN_DECLS

GtkAction *	libre_impuesto_lookup_action	(GtkUIManager *ui_manager,
						 const gchar *action_name);
GtkActionGroup *libre_impuesto_lookup_action_group (GtkUIManager *ui_manager,
						    const gchar *action_group);
GtkActionGroup *
libre_impuesto_add_action_group (GtkUIManager *ui_manager,
				 const gchar *group_name);

G_END_DECLS

#endif /* LIBRE_IMPUESTO_UTIL_H */
