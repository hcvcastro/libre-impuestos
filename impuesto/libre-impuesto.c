/*
 * libre-impuesto.c
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

#include "libre-impuesto.h"
#include "gtkstack.h"
#include "libre-impuesto-window.h"
#include "impuesto-command.h"

#include <glib/gi18n.h>
#include "util/util.h"

#define APPLICATION_ID "www.softwarelibre.bo.libre-impuestos"

#define LIBRE_IMPUESTO_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE \
	((obj), TYPE_LIBRE_IMPUESTO, LibreImpuestoPrivate))

#define LIBRE_IMPUESTO_PARENT_CLASS libre_impuesto_parent_class

struct _LibreImpuestoPrivate {
  
  LibreImpuestoCommand * command;

  GtkTreeStore *tree_data_general;
  GtkTreeStore *tree_data_consultor;
  GtkTreeStore *tree_data_formulario;

  GActionGroup *action_group;
  GtkCssProvider *css_provider;

  gchar *geometry;

  
};

enum {
	PROP_0,
	PROP_GEOMETRY,
};


enum {
	EVENT,
	LAST_SIGNAL
};


typedef enum
{
  LIBRE_IMPUESTO_ERROR_GENERAL,
} LibreImpuestoError;


enum {
	DEBUG_KEY_SETTINGS = 1 << 0
};


static gpointer default_impuesto;


/* Forward Declarations */
static void libre_impuesto_initable_init (GInitableIface *interface);

G_DEFINE_TYPE_WITH_CODE (
			 LibreImpuesto,
			 libre_impuesto,
			 GTK_TYPE_APPLICATION,
			 G_IMPLEMENT_INTERFACE (
				G_TYPE_INITABLE, libre_impuesto_initable_init)
			 )



static void
libre_impuesto_queue_commands (LibreImpuesto *impuesto)
{
  LibreImpuestoCommand *impuesto_command;

  impuesto_command = impuesto->priv->command;
  g_assert (impuesto_command != NULL);

  libre_impuesto_queue_command (impuesto_command,
                                IMPUESTO_COMMAND_MAIN_WINDOW,
				NULL,
                                TRUE);
}


static GQuark
libre_impuesto_error_quark (void)
{
  return g_quark_from_static_string ("libre-impuesto-error-quark");
}



static gboolean 
libre_impuesto_load_node (GtkTreeStore *tree_store,
			  GtkIconTheme *theme,
			  xmlNodePtr node, 
			  GtkTreeIter *tree_parent,
			  GError **error)
{
  GdkPixbuf *pixbuf;
  GtkTreeIter tree_node;
  xmlChar *node_icon, *node_name, *node_form;
  gboolean ret_val = TRUE;

  while(node) {
    // check for the proper element name 
    if(!xmlStrcmp(node->name, (guchar*)"node")) {
       // get the saved properties 
       node_icon = xmlGetProp(node, (guchar*)"icon");
       node_name = xmlGetProp(node, (guchar*)"name");
       node_form = xmlGetProp(node, (guchar*)"form");

       pixbuf = gtk_icon_theme_load_icon (theme, (gchar *)node_icon, 16, 0, NULL);

       if (pixbuf == NULL) {
	 pixbuf = gtk_icon_theme_load_icon (theme, "error", 16, 0, NULL);	 
       }

       gtk_tree_store_append (tree_store, &tree_node, tree_parent);

       // set the data
       gtk_tree_store_set ( tree_store, 
			    &tree_node, 
			    0, pixbuf, 
			    1, node_name, 
			    2, node_form,
			    -1);

       // free the data 
       xmlFree(node_icon);
       xmlFree(node_name);

       ret_val = libre_impuesto_load_node (tree_store, theme, 
					   node->xmlChildrenNode, &tree_node, 
					   error);
    }
    node = node->next;
  }

  return ret_val;
}


static gboolean
libre_impuesto_load_root_node(GtkTreeStore * tree_store, 
			      GFile *file, GError ** error)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr node = NULL;
  gchar *contents = NULL;
  gboolean ret_val = FALSE;
  GtkIconTheme *theme = NULL;

  /* load xml file */
  ret_val = g_file_load_contents (file, NULL, &contents, NULL, NULL, error);
  if (!ret_val) 
    goto out;

  doc = xmlParseDoc((guchar *)contents);
  if (doc == NULL) {
    *error = g_error_new_literal (libre_impuesto_error_quark (),
				  LIBRE_IMPUESTO_ERROR_GENERAL,
				  _("Could not parse the contents\n"));
    goto out;
  }

  /* get the root item */
   node = xmlDocGetRootElement(doc);
   if(node == NULL) {
    *error = g_error_new_literal (libre_impuesto_error_quark (),
				  LIBRE_IMPUESTO_ERROR_GENERAL,
				  _("Could not get root item\n"));
     goto out;
   }

  theme = gtk_icon_theme_get_default ();

   // iterate through the root's children items 
   node = node->xmlChildrenNode;

   ret_val = libre_impuesto_load_node (tree_store, theme, node, NULL, error);

   return ret_val;

out:
   g_free (contents);
   xmlFreeDoc(doc);

   return ret_val;
}


static gboolean
libre_impuesto_load_from_file (GtkTreeStore  *tree_store,
                                 GFile           *file,
                                 GError         **error)
{
  g_return_val_if_fail (GTK_IS_TREE_STORE (tree_store), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);

  return libre_impuesto_load_root_node (tree_store, file, error);
}


static gboolean
libre_impuesto_load_from_path (GtkTreeStore  *tree_store,
			       const gchar     *path,
			       GError         **error)
{
  GFile *file;
  gboolean result;

  g_return_val_if_fail (GTK_IS_TREE_STORE (tree_store), FALSE);
  g_return_val_if_fail (path != NULL, FALSE);

  file = g_file_new_for_path (path);
  
  result = libre_impuesto_load_from_file (tree_store, file, error);

  g_object_unref (file);

  return result;
}




/* Quit libre impuesto application */
gboolean
libre_impuesto_quit (LibreImpuesto *impuesto,
		     LibreImpuestoQuitReason reason)
{
  //GtkApplication *application;
  //GList *list;

  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), FALSE);

  //application = GTK_APPLICATION (impuesto);

  /* Destroy all watched windows.  Note, we iterate over a -copy-
   * of the watched windows list because the act of destroying a
   * watched window will modify the watched windows list, which
   * would derail the iteration. */
  /*list = g_list_copy (gtk_application_get_windows (application));
  g_list_foreach (list, (GFunc) gtk_widget_destroy, NULL);
  g_list_free (list);*/

  if (gtk_main_level () > 0)
    gtk_main_quit ();

  return TRUE;
}


/* get deafault libre impuesto application */
LibreImpuesto *
libre_impuesto_get_default (void)
{
  return default_impuesto;
}


/* Create the main window libre impuesto application */
GtkWidget *
libre_impuesto_create_main_window (LibreImpuesto *impuesto)
{
  GtkWidget *impuesto_window;

  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);


  /* create the main window */
  impuesto_window = libre_impuesto_window_new (
					       impuesto,
					       impuesto->priv->geometry);

  /* Clear the first-time-only options. */
  g_free (impuesto->priv->geometry);
  impuesto->priv->geometry = NULL;

  gtk_widget_show (impuesto_window);

  return impuesto_window;
}


/* Get current active window */
GtkWindow *
libre_impuesto_get_active_window (LibreImpuesto *impuesto)
{
  GtkApplication *application;
  GList *list;

  if (impuesto == NULL)
    impuesto = libre_impuesto_get_default ();

  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);

  application = GTK_APPLICATION (impuesto);
  list = gtk_application_get_windows (application);

  if (list == NULL)
    return NULL;

  /* Sanity check */
  g_return_val_if_fail (GTK_IS_WINDOW (list->data), NULL);

  return GTK_WINDOW (list->data);
}


GtkStyleProvider*
libre_impuesto_get_provider (LibreImpuesto *impuesto)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);
  g_return_val_if_fail (impuesto->priv->css_provider, NULL);

  return GTK_STYLE_PROVIDER(impuesto->priv->css_provider);
}

GtkTreeStore*
libre_impuesto_get_tree_data_general (LibreImpuesto *impuesto)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);
  g_return_val_if_fail (impuesto->priv->tree_data_general, NULL);

  return impuesto->priv->tree_data_general;
}

GtkTreeStore*
libre_impuesto_get_tree_data_formulario (LibreImpuesto *impuesto)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);
  g_return_val_if_fail (impuesto->priv->tree_data_formulario, NULL);

  return impuesto->priv->tree_data_formulario;
}

GtkTreeStore*
libre_impuesto_get_tree_data_consultor (LibreImpuesto *impuesto)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);
  g_return_val_if_fail (impuesto->priv->tree_data_consultor, NULL);

  return impuesto->priv->tree_data_consultor;
}

LibreImpuestoCommand *
libre_impuesto_get_command(LibreImpuesto *impuesto)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO (impuesto), NULL);
  g_return_val_if_fail (impuesto->priv->command, NULL);

  return impuesto->priv->command;
}


/* Maximize main window */
void
libre_impuesto_maximize_window (LibreImpuesto *impuesto,
				GtkWindow *window)
{
  GdkScreen *screen;
  GdkRectangle rect;
  GdkWindow *gdk_window;
  gint monitor;


  if (gtk_widget_get_realized (GTK_WIDGET (window))) {
    screen = gdk_screen_get_default ();
    gdk_window = gtk_widget_get_window (GTK_WIDGET (window));
    monitor = gdk_screen_get_monitor_at_window (screen, gdk_window);
    gdk_screen_get_monitor_geometry (screen, monitor, &rect);

    gtk_window_set_default_size (window, rect.width, rect.height);
    gtk_window_set_decorated (window, TRUE);
    gtk_window_maximize (window);
  }
}


/*  Inicializacion de la clase libre_impuesto */

/* Gets a property of an libre impuesto object. */
static void
libre_impuesto_get_property (GObject *object,
			     guint property_id,
			     GValue *value,
			     GParamSpec *pspec)
{
  switch (property_id) {
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}



static void
libre_impuesto_set_geometry (LibreImpuesto *impuesto,
			     const gchar *geometry)
{
  g_return_if_fail (impuesto->priv->geometry == NULL);
  impuesto->priv->geometry = g_strdup (geometry);
}


/* Sets a property on an libre_impuesto object. */
static void
libre_impuesto_set_property (GObject *object,
			     guint property_id,
			     const GValue *value,
			     GParamSpec *pspec)
{
	switch (property_id) {
	
		case PROP_GEOMETRY:
			libre_impuesto_set_geometry (
				LIBRE_IMPUESTO (object),
				g_value_get_string (value));
			return;
	}

	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

/* Releases all references to other objects. */
static void
libre_impuesto_dispose (GObject *object)
{
  LibreImpuestoPrivate *priv;

  priv = LIBRE_IMPUESTO_GET_PRIVATE (object);

  if (priv->action_group != NULL) {
    g_object_unref (priv->action_group);
    priv->action_group = NULL;
  }

  if (priv->tree_data_general != NULL) {
    g_object_unref (priv->tree_data_general);
    priv->tree_data_general = NULL;
  }

  if (priv->tree_data_formulario != NULL) {
    g_object_unref (priv->tree_data_formulario);
    priv->tree_data_formulario = NULL;
  }

  if (priv->tree_data_consultor != NULL) {
    g_object_unref (priv->tree_data_consultor);
    priv->tree_data_consultor = NULL;
  }

  if (priv->css_provider != NULL) {
    g_object_unref (priv->css_provider);
    priv->css_provider = NULL;
  }

  /* Chain up to parent's dispose() method. */
  G_OBJECT_CLASS (LIBRE_IMPUESTO_PARENT_CLASS)->dispose (object);
}

/*Shouldd finish the finalization of the instance begun in dispose. */
static void
libre_impuesto_finalize (GObject *object)
{
  LibreImpuestoPrivate *priv;

  priv = LIBRE_IMPUESTO_GET_PRIVATE (object);

  if (priv->geometry != NULL) {
    g_free (priv->geometry);
    priv->geometry = NULL;
  }

  if (priv->command != NULL ) {
    g_object_unref (priv->command);
    priv->command = NULL;
  }

  /* Chain up to parent's finalize() method. */
  G_OBJECT_CLASS (LIBRE_IMPUESTO_PARENT_CLASS)->finalize (object);
}

/* the final step of the object creation process */
static void
libre_impuesto_constructed (GObject *object)
{
  /* The first libre impuesto instance is the default. */
  if (default_impuesto == NULL) {
    default_impuesto = object;
    g_object_add_weak_pointer (object, &default_impuesto);
  }

  /* Chain up to parent's constructed() method. */
  G_OBJECT_CLASS (LIBRE_IMPUESTO_PARENT_CLASS)->constructed (object);
}

/* The startup signal is emitted on the primary instance immediately after 
  registration. */
static void
libre_impuesto_on_startup (GApplication *application)
{
  /* Chain up to parent's startup() method. */
  G_APPLICATION_CLASS (LIBRE_IMPUESTO_PARENT_CLASS)->startup (application);

  libre_impuesto_queue_commands (LIBRE_IMPUESTO (application));
}

/* The activate signal is emitted on the primary instance when an
 activation occurs. See g_application_activate(). */
static void
libre_impuesto_on_activate (GApplication *application)
{
  GList *list;

  /* Do not chain up.  Default method just emits a warning. */
  list = gtk_application_get_windows (GTK_APPLICATION (application));

  /* Present the first libre impuesto window, if found. */
  while (list != NULL) {
    GtkWindow *window = GTK_WINDOW (list->data);
    
    if (IS_LIBRE_IMPUESTO_WINDOW (window)) {
      gtk_window_present (window);
      return;
    }

    list = g_list_next (list);
  }
}


/* The delete-event signal is emitted if a user requests that a toplevel window
   is closed. */
static gboolean
libre_impuesto_delete_event_cb (GtkWindow *window,
				GdkEvent *event,
				GtkApplication *application)
{
  /* If other windows are open we can safely close this one. */
  if (g_list_length (gtk_application_get_windows (application)) > 1)
    return FALSE;

  /* Otherwise we initiate application quit. */
  libre_impuesto_quit (LIBRE_IMPUESTO (application), 
		       LIBRE_IMPUESTO_QUIT_LAST_WINDOW);

	return TRUE;
}

/* Emitted when a GtkWindow is added to application through
   gtk_application_add_window(). */
static void
libre_impuesto_on_window_added (GtkApplication *application,
				GtkWindow *window)
{
  gchar *role;

  /* Chain up to parent's window_added() method. */
  GTK_APPLICATION_CLASS (LIBRE_IMPUESTO_PARENT_CLASS)->
    window_added (application, window);

  g_signal_connect (
		    window, "delete-event",
		    G_CALLBACK (libre_impuesto_delete_event_cb), application);

  /* We use the window's own type name and memory
   * address to form a unique window role for X11. */
  role = g_strdup_printf (
			  "%s-%" G_GINTPTR_FORMAT,
			  G_OBJECT_TYPE_NAME (window),
			  (gintptr) window);
  gtk_window_set_role (window, role);
  g_free (role);
}



static void
libre_impuesto_class_init (LibreImpuestoClass *class)
{
  GObjectClass *object_class;
  GApplicationClass *application_class;
  GtkApplicationClass *gtk_application_class;

  g_type_class_add_private (class, sizeof (LibreImpuestoPrivate));

  object_class = G_OBJECT_CLASS (class);
  object_class->set_property = libre_impuesto_set_property;
  object_class->get_property = libre_impuesto_get_property;
  object_class->dispose = libre_impuesto_dispose;
  object_class->finalize = libre_impuesto_finalize;
  object_class->constructed = libre_impuesto_constructed;

  application_class = G_APPLICATION_CLASS (class);
  application_class->startup = libre_impuesto_on_startup;
  application_class->activate = libre_impuesto_on_activate;

  gtk_application_class = GTK_APPLICATION_CLASS (class);
  gtk_application_class->window_added = libre_impuesto_on_window_added;

  g_object_class_install_property (
		object_class,
		PROP_GEOMETRY,
		g_param_spec_string (
			"geometry",
			"Geometry",
			"Initial window geometry string",
			NULL,
			G_PARAM_WRITABLE |
			G_PARAM_CONSTRUCT_ONLY));

}


/* libre impuesto class constructor */

static void
libre_impuesto_init (LibreImpuesto *impuesto)
{
  impuesto->priv = LIBRE_IMPUESTO_GET_PRIVATE (impuesto);
  impuesto->priv->command = g_object_new (LIBRE_IMPUESTO_TYPE_COMMAND, NULL);
}


/* Interface libre impuesto inicializacion */


/* call back when quit action is invoked */
static void
libre_impuesto_action_quit_cb (GSimpleAction *action,
			       GVariant *parameter,
			       LibreImpuesto *impuesto)
{
  libre_impuesto_quit (impuesto, LIBRE_IMPUESTO_QUIT_LAST_WINDOW);
}


static void
libre_impuesto_add_actions (GApplication *application)
{
  LibreImpuesto *impuesto;
  GSimpleActionGroup *action_group;
  GSimpleAction *action;

  /* Add actions that remote instances can invoke. */

  action_group = g_simple_action_group_new ();

  action = g_simple_action_new ("quit", NULL);
  g_signal_connect (
		    action, 
		    "activate",
		    G_CALLBACK (libre_impuesto_action_quit_cb), 
		    application);

  g_simple_action_group_insert (action_group, G_ACTION (action));
  g_object_unref (action);

  impuesto = LIBRE_IMPUESTO (application);
  impuesto->priv->action_group = G_ACTION_GROUP (action_group);

  g_application_set_action_group (
		application, impuesto->priv->action_group);
}

static gboolean
impuesto_initable_init (GInitable *initable,
			GCancellable *cancellable,
			GError **error)
{
  gchar* filename;
  GdkScreen* screen;
  GValue bTrue = G_VALUE_INIT;
  GtkSettings* settings;
  GtkSettingsValue value;
  gboolean ret_val = FALSE;
  GtkIconTheme *icon_theme;
  GApplication *application = G_APPLICATION (initable);
  LibreImpuestoPrivate *priv = LIBRE_IMPUESTO(initable)->priv;

  g_value_init(&bTrue, G_TYPE_LONG);
  g_value_set_long(&bTrue, 1);
  value.value = bTrue;
  settings = gtk_settings_get_default();
  gtk_settings_set_property_value(settings, "gtk-button-images", &value);

  // Append icon search path
  icon_theme = gtk_icon_theme_get_default ();
  gtk_icon_theme_append_search_path (icon_theme, LIBRE_IMPUESTO_ICONDIR);


  //We load the data tree
  filename = g_build_filename (LIBRE_IMPUESTO_XML, 
			       "tree-data-general.xml", NULL);
 
  priv->tree_data_general = gtk_tree_store_new (3, GDK_TYPE_PIXBUF, 
						G_TYPE_STRING,
						G_TYPE_STRING);
  ret_val = libre_impuesto_load_from_path(priv->tree_data_general,
					  filename, error);
  g_free (filename);
  if (!ret_val) 
    return ret_val;

  filename = g_build_filename (LIBRE_IMPUESTO_XML, 
			       "tree-data-formulario.xml", NULL);

  priv->tree_data_formulario = gtk_tree_store_new (3, GDK_TYPE_PIXBUF, 
						   G_TYPE_STRING,
						   G_TYPE_STRING);
  ret_val = libre_impuesto_load_from_path(priv->tree_data_formulario,
					  filename, error);
  g_free (filename);
  if (!ret_val) 
    return ret_val;


  filename = g_build_filename (LIBRE_IMPUESTO_XML, 
			       "tree-data-consultor.xml", NULL);

  priv->tree_data_consultor = gtk_tree_store_new (3, GDK_TYPE_PIXBUF, 
						  G_TYPE_STRING,
						  G_TYPE_STRING);
  ret_val = libre_impuesto_load_from_path(priv->tree_data_consultor, 
					  filename, error);
  g_free (filename);
  if (!ret_val) 
    return ret_val;


  filename = g_build_filename (LIBRE_IMPUESTO_CSS, 
			       "libre-impuestos.css", NULL);

  priv->css_provider = gtk_css_provider_new();
  ret_val = gtk_css_provider_load_from_path (priv->css_provider,
					     filename, error);
  g_free(filename);
  if (!ret_val) 
    return ret_val;


  screen = gdk_display_get_default_screen (gdk_display_get_default ());
  gtk_style_context_add_provider_for_screen (screen, 
			     GTK_STYLE_PROVIDER (priv->css_provider),
			     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  libre_impuesto_add_actions (application);

  return g_application_register (application, cancellable, error);
}


static void
libre_impuesto_initable_init (GInitableIface *interface)
{
  interface->init = impuesto_initable_init;
}


/* create default libre impuesto instance */
LibreImpuesto * 
libre_impuesto_create_instance(void)
{
  LibreImpuesto * impuesto;
  GError *error = NULL;

  impuesto = g_initable_new (
			  TYPE_LIBRE_IMPUESTO, NULL, &error,
			  "application-id", APPLICATION_ID,
			  NULL);

  /* Failure to register is fatal. */
  if (error != NULL)
    g_error ("%s", error->message);

  return impuesto;
}

