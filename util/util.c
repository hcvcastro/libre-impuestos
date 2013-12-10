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
 * Written November 2013 by Henry Castro <hcvcastro@gmail.com>.  
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>

#include "util.h"


GtkAction *
libre_impuesto_lookup_action (GtkUIManager *ui_manager,
			      const gchar *action_name)
{
  GtkAction *action = NULL;
  GList *iter;

  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (action_name != NULL, NULL);

  iter = gtk_ui_manager_get_action_groups (ui_manager);

  while (iter != NULL) {
    GtkActionGroup *action_group = iter->data;

    action = gtk_action_group_get_action (
					  action_group, action_name);
    if (action != NULL)
      return action;

    iter = g_list_next (iter);
  }

  g_critical ("%s: action '%s' not found", G_STRFUNC, action_name);

  return NULL;
}

GtkActionGroup *
libre_impuesto_lookup_action_group (GtkUIManager *ui_manager,
				    const gchar *group_name)
{
  GList *iter;

  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (group_name != NULL, NULL);

  iter = gtk_ui_manager_get_action_groups (ui_manager);

  while (iter != NULL) {
    GtkActionGroup *action_group = iter->data;
    const gchar *name;

    name = gtk_action_group_get_name (action_group);
    if (strcmp (name, group_name) == 0)
      return action_group;

    iter = g_list_next (iter);
  }

  g_critical ("%s: action group '%s' not found", G_STRFUNC, group_name);

  return NULL;
}


GtkActionGroup *
libre_impuesto_add_action_group (GtkUIManager *ui_manager,
				const gchar *group_name)
{
  GtkActionGroup * action_group;
  const gchar *domain;

  g_return_val_if_fail (GTK_IS_UI_MANAGER (ui_manager), NULL);
  g_return_val_if_fail (group_name != NULL, NULL);

  domain = GETTEXT_PACKAGE;

  action_group = gtk_action_group_new (group_name);
  gtk_action_group_set_translation_domain (action_group, domain);
  gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);

  return action_group;

}
