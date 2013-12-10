/* main.c - main application for libre-impuestos.
   Copyright (C) 1992, 1997-2002, 2004-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Written November 2013 by Henry Castro <hcvcastro@gmail.com>.  */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libre-impuesto.h"
#include <glib/gi18n.h>

#define APPLICATION_ID "www.softwarelibre.bo.libre-impuestos"

static gchar *geometry = NULL;
static gchar **remaining_args;

/* Adds a function to be called whenever there are no higher priority events
   pending to the default main loop */
static gboolean idle_cb (const gchar * const * uris)
{
  LibreImpuesto *impuesto;

  impuesto = libre_impuesto_get_default ();
  libre_impuesto_create_main_window (impuesto);

  return FALSE;
}

/* create default libre impuesto instance */
static LibreImpuesto * create_default_libre_impuesto(void)
{
  LibreImpuesto * impuesto;
  GApplicationFlags flags;
  GError *error = NULL;

  flags = G_APPLICATION_HANDLES_OPEN | G_APPLICATION_HANDLES_COMMAND_LINE;
  impuesto = g_initable_new (
			  TYPE_LIBRE_IMPUESTO, NULL, &error,
			  "application-id", APPLICATION_ID,
			  "flags", flags,
			  "geometry", geometry,
			  NULL);

  /* Failure to register is fatal. */
  if (error != NULL)
    g_error ("%s", error->message);

  return impuesto;
}


/* main function to run libre impuesto application */
gint main(int argc, char *argv[])
{
  GError *error = NULL;
  gchar * locale = NULL;

  g_type_init();
  
  /* this is to initialize threading for dbus-glib used by GConf */
  //dbus_g_thread_init ();

  bindtextdomain (GETTEXT_PACKAGE, LIBRE_IMPUESTO_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  gtk_init_with_args ( &argc, 
		       &argv,
		       _("Software Libre Bolivia - Libre Impuestos"),
		       NULL, 
		       (gchar *) GETTEXT_PACKAGE, 
		       &error);

  locale = g_strdup (setlocale (LC_MESSAGES, NULL));

  if (g_strcmp0 (locale, "es_BO") != 0) {
    g_message("Cambiando el locale de %s a es_BO", locale);
    setlocale(LC_MESSAGES, "es_BO");
  }

  g_free(locale);

  if (error != NULL) 
  {
    g_printerr ("%s\n", error->message);
    g_error_free (error);
    return 1;
  }

  gtk_window_set_default_icon_name ("libre-impuestos");

  create_default_libre_impuesto();

  g_idle_add((GSourceFunc) idle_cb, remaining_args);

  gtk_main();

  return 0;
}




