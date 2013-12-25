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

#include "gtkstack.h"
#include "gtkstackswitcher.h"
#include "libre-impuesto-window-private.h"
#include "impuesto-command.h"
#include "impuesto-notebook.h"
#include "libre-impuesto-web-view.h"
#include "libre-impuesto-embed.h"

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

typedef struct
{
	ImpuestoCommand command;
	gchar * arg;
} Command;



static void 
action_message_cb (GtkAction *action,
		   GtkWindow *window)
{
  GtkMessageDialog *dialog = NULL;
  //gboolean ret_val = FALSE;
  gchar *message = NULL;

  message = g_strdup_printf (_("The function is not implemente yet. Help?"));
  
  dialog = (GtkMessageDialog *) gtk_message_dialog_new_with_markup 
    (
     (GtkWindow *) gtk_widget_get_toplevel ( GTK_WIDGET(window)), 
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
                 GtkWindow *window)
{
	gchar *translator_credits;

	translator_credits = _("translator-credits");
	if (strcmp (translator_credits, "translator-credits") == 0)
		translator_credits = NULL;

	gtk_show_about_dialog (
		window,
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


static gchar* 
filenameToURL(const char* filename)
{
  gchar *fileURL = NULL;
  gchar *full_filename;
  //We load the data tree
  full_filename = g_build_filename (LIBRE_IMPUESTO_BASE_URL, 
				    filename, NULL);

  if (g_file_test(full_filename, G_FILE_TEST_EXISTS)) {
    GFile *gfile = g_file_new_for_path(full_filename);
    fileURL = g_file_get_uri(gfile);
    g_object_unref(gfile);
  }

  g_free (full_filename);

  return fileURL;
}


static gint
libre_impuesto_find_page_data(GtkNotebook *notebook, const gchar *form)
{
  gint ret_val = -1;
  int page_num = 0;
  GtkNotebook *nb = GTK_NOTEBOOK (notebook);
  GtkWidget *page;
  const gchar *data_form;

  while ((page = gtk_notebook_get_nth_page (nb, page_num))) {
    data_form = g_object_get_data (G_OBJECT (page), "form");

    if (g_strcmp0 (form, data_form) == 0 ) {
      ret_val = page_num;
      break;
    }

    page_num++;
  }

  return ret_val;
}


static void
libre_impuesto_open_form(LibreImpuesto * impuesto, const gchar * args)
{
  GtkNotebook * notebook;
  GtkWindow * impuesto_window = libre_impuesto_get_active_window(impuesto);
  GtkWidget * widget, * switcher, * tree_view;
  LibreImpuestoEmbed * embed;
  WebKitWebView *webview;
  GtkTreeSelection * selection;
  GtkTreeIter selected;
  GtkTreePath * path;
  gchar *fileURL;
  gint current;

  if (args == NULL) {
    action_message_cb (NULL, impuesto_window);    
    return;
  }

  notebook = libre_impuesto_window_get_notebook(LIBRE_IMPUESTO_WINDOW(impuesto_window));

  current = libre_impuesto_find_page_data(notebook, args);
  if (current == -1) {
    embed = LIBRE_IMPUESTO_EMBED (g_object_new (LIBRE_IMPUESTO_TYPE_EMBED, NULL));
    g_assert (embed != NULL);

    switcher = g_object_get_data ( G_OBJECT(impuesto_window), "switcher");
    tree_view = gtk_stack_get_visible_child(gtk_stack_switcher_get_stack (GTK_STACK_SWITCHER(switcher)));
    tree_view = gtk_bin_get_child(GTK_BIN(tree_view));
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW(tree_view) );
    gtk_tree_selection_get_selected (selection, NULL, &selected);
    path = gtk_tree_model_get_path (gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)), &selected);

    g_object_set_data(G_OBJECT(embed), "impuesto-window", impuesto_window);
    g_object_set_data(G_OBJECT(embed), "tree-view", tree_view);
    g_object_set_data(G_OBJECT(embed), "path", path);
    g_object_set_data(G_OBJECT(embed), "form", g_strdup(args));

    webview = libre_impuesto_embed_get_web_view (LIBRE_IMPUESTO_EMBED(embed));
    widget = gtk_label_new(NULL);
    current = gtk_notebook_append_page (notebook, GTK_WIDGET(embed), widget );
    gtk_widget_show (GTK_WIDGET (embed));

    fileURL = filenameToURL(args);
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), fileURL ? fileURL : args);
    g_free(fileURL);

    gtk_notebook_set_current_page(notebook, current);
  }
  else
    gtk_notebook_set_current_page(notebook, current);

  if ( gtk_notebook_get_n_pages (libre_impuesto_window_get_notebook(LIBRE_IMPUESTO_WINDOW(impuesto_window))) > 0 )
    gtk_stack_set_visible_child_name(libre_impuesto_window_get_stack(LIBRE_IMPUESTO_WINDOW(impuesto_window)), "notebook");
  else
    gtk_stack_set_visible_child_name(libre_impuesto_window_get_stack(LIBRE_IMPUESTO_WINDOW(impuesto_window)), "background");
    
}

static void 
libre_impuesto_sync_selection(LibreImpuesto * impuesto, const gchar * args)
{
  GtkWindow * impuesto_window = libre_impuesto_get_active_window(impuesto);
  GtkWidget * notebook, * switcher;
  GtkWidget * page_child, * tree_view;
  GtkTreeSelection * selection;
  GtkTreeIter selected;
  GtkTreePath * page_path, * tree_path;
  GtkWidget * page_tree_view;

  notebook = g_object_get_data ( G_OBJECT(impuesto_window), "notebook");
  switcher = g_object_get_data ( G_OBJECT(impuesto_window), "switcher");

  page_child = gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), 
					  gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook) ));

  page_path = g_object_get_data ( G_OBJECT(page_child), "path" );
  page_tree_view = g_object_get_data ( G_OBJECT(page_child), "tree-view" );
  
  tree_view = gtk_stack_get_visible_child(gtk_stack_switcher_get_stack (GTK_STACK_SWITCHER(switcher)));
  tree_view = gtk_bin_get_child(GTK_BIN(tree_view));

  selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW(tree_view) );
  gtk_tree_selection_get_selected (selection, NULL, &selected);

  if (page_path == NULL || page_tree_view == NULL ) {
    gtk_tree_selection_unselect_all( selection );
    return;
  }

  if ( gtk_tree_selection_count_selected_rows(selection) > 0 ) {
    tree_path = gtk_tree_model_get_path (gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)), &selected);
    if (  gtk_tree_path_compare( page_path, tree_path ) != 0 ) {
      tree_view = g_object_get_data ( G_OBJECT(page_child), "tree-view" );
      selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW(tree_view) );
      gtk_tree_selection_select_path ( selection, page_path );    
      tree_view = gtk_widget_get_parent ( GTK_WIDGET(tree_view) );
      gtk_stack_set_visible_child (gtk_stack_switcher_get_stack (GTK_STACK_SWITCHER(switcher)), tree_view );
    }
    gtk_tree_path_free(tree_path);
  }
  else {
    tree_view = g_object_get_data ( G_OBJECT(page_child), "tree-view" );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW(tree_view) );
    gtk_tree_selection_select_path ( selection, page_path );    
    tree_view = gtk_widget_get_parent ( GTK_WIDGET(tree_view) );
    gtk_stack_set_visible_child (gtk_stack_switcher_get_stack (GTK_STACK_SWITCHER(switcher)), tree_view );
  }
}

void
libre_impuesto_command_process(ImpuestoCommand command, const gchar * args)
{
  LibreImpuesto * impuesto = libre_impuesto_get_default ();

  switch (command) {
  case IMPUESTO_COMMAND_START:
  case IMPUESTO_COMMAND_LAST:
    break;
  case IMPUESTO_COMMAND_NOT_IMPLEMENTED:
    action_message_cb (NULL, libre_impuesto_get_active_window(impuesto));
    break;
  case IMPUESTO_COMMAND_OPEN_FORM:
    libre_impuesto_open_form (impuesto, args);
    break;
  case IMPUESTO_COMMAND_SYNC_SELECTION:
    libre_impuesto_sync_selection (impuesto, args);
    break;
  case IMPUESTO_COMMAND_MAIN_WINDOW:
    libre_impuesto_create_main_window (impuesto);
    break;
  }
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
