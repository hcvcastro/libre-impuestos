/*
 * libre-impuesto-window-private.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libre-impuesto-window-private.h"

void
libre_impuesto_window_private_dispose (LibreImpuestoWindow *impuesto_window)
{
  LibreImpuestoWindowPrivate *priv = impuesto_window->priv;
  DISPOSE (priv->ui_manager);
}

void
libre_impuesto_window_private_finalize (LibreImpuestoWindow *impuesto_window)
{
  LibreImpuestoWindowPrivate *priv = impuesto_window->priv;
  g_free (priv->geometry);
}


/* instance constructor */
void
libre_impuesto_window_private_init (LibreImpuestoWindow *impuesto_window)
{
  LibreImpuestoWindowPrivate *priv = impuesto_window->priv;

  priv->statusbar_visible = TRUE;
  priv->sidebar_visible = TRUE;
  priv->headerbar_visible = TRUE;
  priv->ui_manager = gtk_ui_manager_new ();

  gtk_window_set_title (GTK_WINDOW (impuesto_window), _("Libre Impuestos"));
  gtk_window_set_icon_name (GTK_WINDOW (impuesto_window), "libre-impuestos");

}

