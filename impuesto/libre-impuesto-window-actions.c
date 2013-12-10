/*
 * libre-impuesto-window-actions.c
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
 * Copyright (C) 2013 Libre Software Bolivia. Henry Castro <hcvcastro@gmail.com>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libre-impuesto-window-private.h"


#define LIBRE_IMPUESTO_COPYRIGHT \
	"Copyright \xC2\xA9 2013 Software Libre Bolivia, Henry Castro <hcvcasro@gmail.com>"

#define LIBRE_IMPUESTO_WEBSITE \
	"http://www.softwarelibre.gob.bo/"

/* Authors and Documenters
 *
 * The names below must be in UTF-8.  The breaking of escaped strings
 * is so the hexadecimal sequences don't swallow too many characters.
 *
 * SO THAT MEANS, FOR 8-BIT CHARACTERS USE \xXX HEX ENCODING ONLY!
 *
 * Not all environments are UTF-8 and not all editors can handle it.
 */
static const gchar *authors[] = {
	"Henry Castro <hcvcastro@gmail.com>",
	"Ayudame! ...",
	"Hay mas espacio para mas autores",
	NULL
};

static const gchar *documenters[] = {
	"Henry Castro <hcvcastro@gmail.com>",
	"Ayudame! ...",
	"Hay mas espacio para mas autores",
	NULL
};


void 
action_message_cb (GtkAction *action,
		   LibreImpuestoWindow *libre_impuesto_window)
{
  GtkMessageDialog *dialog = NULL;
  //gboolean ret_val = FALSE;
  gchar *message = NULL;

  message = g_strdup_printf (_("The function is not implemente yet. Help?"));
  
  dialog = (GtkMessageDialog *) gtk_message_dialog_new_with_markup 
    (
     (GtkWindow *) gtk_widget_get_toplevel ( GTK_WIDGET(libre_impuesto_window)), 
     GTK_DIALOG_MODAL,
     GTK_MESSAGE_QUESTION, 
     GTK_BUTTONS_OK_CANCEL, "<b>%s</b>", 
     message);
  
  g_free (message);

  //ret_val = (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK);
  gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_destroy ((GtkWidget *) dialog);
}

static void
action_about_cb (GtkAction *action,
                 LibreImpuestoWindow *libre_impuesto_window)
{
	gchar *translator_credits;

	translator_credits = _("translator-credits");
	if (strcmp (translator_credits, "translator-credits") == 0)
		translator_credits = NULL;

	gtk_show_about_dialog (
		GTK_WINDOW (libre_impuesto_window),
		"program-name", "Libre Impuestos",
		"version", VERSION,
		"copyright", LIBRE_IMPUESTO_COPYRIGHT,
		"comments", _("Desktop application for filling forms Impuestos Nacionales Bolivia"),
		"website", LIBRE_IMPUESTO_WEBSITE,
		"website-label", _("Libre Impuesto Website"),
		"authors", authors,
		"documenters", documenters,
		"translator-credits", translator_credits,
		"logo-icon-name", "libre-impuesto",
		NULL);
}

static void
action_close_cb (GtkAction *action,
                 LibreImpuestoWindow *libre_impuesto_window)
{
  //Ayudame!
}



static void
action_quit_cb (GtkAction *action,
                LibreImpuestoWindow *libre_impuesto_window)
{
  libre_impuesto_quit (libre_impuesto_window_get_impuesto (libre_impuesto_window), 0 );
}



static GtkActionEntry impuesto_entries[] = {

	{ "about",
	  GTK_STOCK_ABOUT,
	  NULL,
	  NULL,
	  N_("Show information about Evolution"),
	  G_CALLBACK (action_about_cb) },

	{ "close",
	  GTK_STOCK_CLOSE,
	  N_("_Close Window"),
	  "<Control>w",
	  N_("Close this window"),
	  G_CALLBACK (action_close_cb) },

	{ "close-window-menu",
	  GTK_STOCK_CLOSE,
	  NULL,
	  "<Control>w",
	  N_("Close this window"),
	  G_CALLBACK (action_close_cb) },

	{ "close-window",
	  GTK_STOCK_CLOSE,
	  N_("_Close Window"),
	  "<Control>w",
	  N_("Close this window"),
	  G_CALLBACK (action_close_cb) },

	{ "contents",
	  GTK_STOCK_HELP,
	  N_("_Contents"),
	  "F1",
	  N_("Open the Evolution User Guide"),
	  G_CALLBACK (action_message_cb) },

	{ "quit",
	  GTK_STOCK_QUIT,
	  NULL,
	  NULL,
	  N_("Exit the program"),
	  G_CALLBACK (action_quit_cb) },


	/*** Menus ***/

	{ "file-menu",
	  NULL,
	  N_("_File"),
	  NULL,
	  NULL,
	  NULL },

	{ "layout-menu",
	  NULL,
	  N_("Lay_out"),
	  NULL,
	  NULL,
	  NULL },

	{ "window-menu",
	  NULL,
	  N_("_Window"),
	  NULL,
	  NULL,
	  NULL },

	{ "help-menu",
	  NULL,
	  N_("_Help"),
	  NULL,
	  NULL,
	  NULL },
};

static GtkToggleActionEntry impuesto_toggle_entries[] = {

	{ "show-sidebar",
	  NULL,
	  N_("Show Side _Bar"),
	  NULL,
	  N_("Show the side bar"),
	  NULL,
	  TRUE },

	{ "show-headerbar",
	  NULL,
	  N_("Show _Header Bar"),
	  NULL,
	  N_("Show the header bar"),
	  NULL,
	  TRUE },

	{ "show-statusbar",
	  NULL,
	  N_("Show _Status Bar"),
	  NULL,
	  N_("Show the status bar"),
	  NULL,
	  TRUE }
};



void
libre_impuesto_window_actions_init (LibreImpuestoWindow *impuesto_window)
{
  GtkActionGroup *action_group;
  GtkUIManager *ui_manager;
  gchar *filename;
  GError *error;
  gboolean ret_val;

  g_return_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window));

  ui_manager = libre_impuesto_window_get_ui_manager (impuesto_window);

  action_group  = libre_impuesto_add_action_group(ui_manager, "impuesto");

  gtk_action_group_add_actions ( action_group, impuesto_entries,
				 G_N_ELEMENTS (impuesto_entries), 
				 impuesto_window);

  gtk_action_group_add_toggle_actions (	action_group, impuesto_toggle_entries,
					G_N_ELEMENTS (impuesto_toggle_entries),
					impuesto_window);
  error = NULL;  
  filename = g_build_filename (LIBRE_IMPUESTO_UIDIR, 
			       "libre-impuestos-menu.ui", NULL);
  ret_val = gtk_ui_manager_add_ui_from_file (ui_manager, filename, &error);
  g_free (filename);
  if (!ret_val) {
    g_critical("%s", error->message); 
  }

  g_object_bind_property (impuesto_window, "headerbar-visible",
			  libre_impuesto_window_get_action (impuesto_window, 
							    "show-headerbar"), 
			  "active", G_BINDING_BIDIRECTIONAL | 
			  G_BINDING_SYNC_CREATE);

  g_object_bind_property (impuesto_window, "sidebar-visible",
			  libre_impuesto_window_get_action( impuesto_window,
							    "show-sidebar"), 
			  "active", G_BINDING_BIDIRECTIONAL | 
			  G_BINDING_SYNC_CREATE);

  g_object_bind_property (impuesto_window, "statusbar-visible",
			  libre_impuesto_window_get_action (impuesto_window, 
							    "show-statusbar"), 
			  "active", G_BINDING_BIDIRECTIONAL | 
			  G_BINDING_SYNC_CREATE);

}
