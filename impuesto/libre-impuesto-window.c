/*
 * libre-impuesto-window.c
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
 *
 * Written November 2013 by Henry Castro <hcvcastro@gmail.com>.  */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtkstack.h"
#include "gtkstackswitcher.h"

#include <time.h>
#include "libre-impuesto-window-private.h"

enum {
	PROP_0,
	PROP_GEOMETRY,
	PROP_IMPUESTO,
	PROP_SIDEBAR_VISIBLE,
	PROP_HEADERBAR_VISIBLE,
	PROP_STATUSBAR_VISIBLE,
	PROP_UI_MANAGER
};

enum {
	IMPUESTO_VIEW_CREATED,
	LAST_SIGNAL
};



G_DEFINE_TYPE ( LibreImpuestoWindow,
		libre_impuesto_window,
		GTK_TYPE_WINDOW)


static void
libre_impuesto_window_update_close_action_cb (LibreImpuestoWindow *impuesto_window)
{
  LibreImpuesto *impuesto;
  GtkApplication *application;
  GList *list;
  gint n_impuesto_windows = 0;

  impuesto = libre_impuesto_window_get_impuesto (impuesto_window);

  if (impuesto == NULL)
    return;

  application = GTK_APPLICATION (impuesto);
  list = gtk_application_get_windows (application);

  while (list != NULL) {
    if (IS_LIBRE_IMPUESTO_WINDOW (list->data))
      n_impuesto_windows++;
    list = g_list_next (list);
  }
}

static void
libre_impuesto_window_set_geometry (LibreImpuestoWindow *impuesto_window,
				    const gchar *geometry)
{
  g_return_if_fail (impuesto_window->priv->geometry == NULL);
  impuesto_window->priv->geometry = g_strdup (geometry);
}

static void
libre_impuesto_window_set_impuesto (LibreImpuestoWindow *impuesto_window,
				    LibreImpuesto *impuesto)
{

  g_return_if_fail (impuesto_window->priv->impuesto == NULL);
  impuesto_window->priv->impuesto = impuesto;
  g_object_add_weak_pointer (G_OBJECT (impuesto), 
			     &impuesto_window->priv->impuesto);

  g_signal_connect_swapped (
		    impuesto, "window-added",
		    G_CALLBACK (libre_impuesto_window_update_close_action_cb),
		    impuesto_window);

  g_signal_connect_swapped (
		    impuesto, "window-removed",
		    G_CALLBACK (libre_impuesto_window_update_close_action_cb),
		    impuesto_window);

}

LibreImpuesto *
libre_impuesto_window_get_impuesto (LibreImpuestoWindow *impuesto_window)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), NULL);

  return LIBRE_IMPUESTO (impuesto_window->priv->impuesto);
}


GtkUIManager *
libre_impuesto_window_get_ui_manager (LibreImpuestoWindow *impuesto_window)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), NULL);
  return impuesto_window->priv->ui_manager;
}

GtkAction *
libre_impuesto_window_get_action (LibreImpuestoWindow *impuesto_window,
				  const gchar *action_name)
{
  GtkUIManager *ui_manager;

  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), NULL);
  g_return_val_if_fail (action_name != NULL, NULL);

  ui_manager = libre_impuesto_window_get_ui_manager (impuesto_window);
  return libre_impuesto_lookup_action (ui_manager, action_name);
}

GtkActionGroup *
libre_impuesto_window_get_action_group (LibreImpuestoWindow *impuesto_window,
					const gchar *group_name)
{
  GtkUIManager *ui_manager;

  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), NULL);
  g_return_val_if_fail (group_name != NULL, NULL);

  ui_manager = libre_impuesto_window_get_ui_manager (impuesto_window);
  return libre_impuesto_lookup_action_group (ui_manager, group_name);
}



GtkWidget *
libre_impuesto_window_get_managed_widget (LibreImpuestoWindow *impuesto_window,
					  const gchar *widget_path)
{
  GtkUIManager *ui_manager;
  GtkWidget *widget;

  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), NULL);
  g_return_val_if_fail (widget_path != NULL, NULL);

  ui_manager = libre_impuesto_window_get_ui_manager (impuesto_window);
  widget = gtk_ui_manager_get_widget (ui_manager, widget_path);

  g_return_val_if_fail (widget != NULL, NULL);

  return widget;
}


gboolean
libre_impuesto_window_get_sidebar_visible (LibreImpuestoWindow *impuesto_window)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), FALSE);
  return impuesto_window->priv->sidebar_visible;
}


void
libre_impuesto_window_set_sidebar_visible (LibreImpuestoWindow *impuesto_window,
					   gboolean sidebar_visible)
{
  g_return_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window));
  impuesto_window->priv->sidebar_visible = sidebar_visible;
  g_object_notify (G_OBJECT (impuesto_window), "sidebar-visible");
}


gboolean
libre_impuesto_window_get_headerbar_visible (LibreImpuestoWindow *impuesto_window)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), FALSE);
  return impuesto_window->priv->headerbar_visible;
}

void
libre_impuesto_window_set_headerbar_visible (LibreImpuestoWindow *impuesto_window,
					   gboolean headerbar_visible)
{
  g_return_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window));
  impuesto_window->priv->headerbar_visible = headerbar_visible;
  g_object_notify (G_OBJECT (impuesto_window), "headerbar-visible");
}

gboolean
libre_impuesto_window_get_statusbar_visible (LibreImpuestoWindow *impuesto_window)
{
  g_return_val_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window), FALSE);
  return impuesto_window->priv->statusbar_visible;
}

void
libre_impuesto_window_set_statusbar_visible (LibreImpuestoWindow *impuesto_window,					   
					     gboolean statusbar_visible)
{
  g_return_if_fail (IS_LIBRE_IMPUESTO_WINDOW (impuesto_window));
  impuesto_window->priv->statusbar_visible = statusbar_visible;
  g_object_notify (G_OBJECT (impuesto_window), "statusbar-visible");
}


/* Core functions */

/* Construct the status bar widget. */
static GtkWidget *
libre_impuesto_window_construct_headerbar (LibreImpuestoWindow *impuesto_window)
{
  gint size;
  time_t secs;
  GDate *today;
  struct tm *tm;
  GtkStateFlags state;
  GtkWidget *headerbar;
  gchar buffer[100] = "";
  GtkWidget *header_area;
  GtkWidget *grid_layout;
  GtkStyleContext *context;
  PangoFontDescription *font_desc;
  GtkWidget *label_libre_impuesto, *label_software_libre, *label_date;


  secs = time (NULL);
  tm = localtime (&secs);

  header_area = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
  g_object_bind_property (
		impuesto_window, "headerbar-visible",
		header_area, "visible",
		G_BINDING_SYNC_CREATE);

  headerbar = gtk_event_box_new();
  gtk_box_pack_start (GTK_BOX (header_area), 
		      headerbar, FALSE, FALSE, 0);

  gtk_widget_set_name (headerbar, "libre-impuesto-header");
  gtk_widget_set_size_request (headerbar, -1, 56);

  grid_layout = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (headerbar), grid_layout);
  gtk_grid_set_column_homogeneous (GTK_GRID(grid_layout), TRUE);
  gtk_grid_set_row_homogeneous (GTK_GRID (grid_layout), TRUE);

  label_libre_impuesto = gtk_label_new (_("Libre Impuestos"));
  gtk_grid_attach (GTK_GRID (grid_layout), label_libre_impuesto, 0, 0, 1, 1);  

  context = gtk_widget_get_style_context (label_libre_impuesto);
  state = gtk_widget_get_state_flags (label_libre_impuesto);
  size = pango_font_description_get_size (gtk_style_context_get_font (
								      context, 
								      state));
  font_desc = pango_font_description_new ();
  pango_font_description_set_weight (font_desc, PANGO_WEIGHT_BOLD);
  pango_font_description_set_size (font_desc, size * PANGO_SCALE_X_LARGE);
  gtk_widget_override_font (label_libre_impuesto, font_desc);
  pango_font_description_free (font_desc);

  gtk_misc_set_alignment (GTK_MISC (label_libre_impuesto), 0.2, 0.4);
  gtk_widget_show (label_libre_impuesto);

  label_software_libre = gtk_label_new (_("www.softwarelibre.org.bo"));
  gtk_grid_attach (GTK_GRID (grid_layout), label_software_libre, 1, 0, 1, 1);  
  gtk_misc_set_alignment (GTK_MISC (label_software_libre), 0.5, 0.8);
  gtk_widget_show (label_software_libre);

  today = g_date_new_dmy((gint)tm->tm_mday, 
			 (gint)tm->tm_mon + 1, 
			 1900 + tm->tm_year);

  g_date_strftime (buffer, 100-1, "%A, %d de %B de %Y", today);
  label_date = gtk_label_new (g_ascii_strup (buffer,100-1));
  g_date_free(today);

  gtk_grid_attach (GTK_GRID (grid_layout), label_date, 2, 0, 1, 1);  
  gtk_misc_set_alignment (GTK_MISC (label_date), 0.9, 0.5);

  return header_area;
}


static gboolean
draw_area_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (widget);

  gtk_render_background (context, cr, 0, 0,
			 gtk_widget_get_allocated_width (widget),
                         gtk_widget_get_allocated_height (widget));

  return TRUE;
}



static void
node_tree_selection_changed_cb ( LibreImpuestoWindow *libre_impuesto_window,
				 GtkTreeSelection *selection)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GList *list;

  list = gtk_tree_selection_get_selected_rows (selection, &model);

  if (list == NULL)
    goto exit;

  gtk_tree_model_get_iter (model, &iter, list->data);

  if (!gtk_tree_model_iter_has_child (model, &iter)) {
    action_message_cb (NULL, libre_impuesto_window);
  }

exit:
  g_list_free (list);
}



/* Constrcut side bar widget. */
static GtkWidget *
libre_impuesto_window_construct_sidebar (LibreImpuesto *impuesto,
					 LibreImpuestoWindow *impuesto_window)
{
  GtkStack * stack;
  GtkWidget *switcher;
  GtkTreePath *tree_path;
  GtkTreeStore * tree_store;
  GtkTreeViewColumn *column;
  GtkTreeSelection *selection;
  GtkCellRenderer *cell_renderer;
  GtkWidget *paned, *box_switcher, *widget, *frame, *scroll;


  paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

  g_object_bind_property (impuesto_window, "sidebar-visible",
			  paned, "visible",
			  G_BINDING_SYNC_CREATE);

  box_switcher = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

  switcher = gtk_stack_switcher_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (switcher), 
				  GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_end (GTK_BOX (box_switcher), switcher, FALSE, FALSE, 0);

  stack = GTK_STACK (gtk_stack_new ());
  gtk_stack_set_homogeneous ( stack, TRUE);
  gtk_box_pack_end (GTK_BOX (box_switcher), GTK_WIDGET(stack), TRUE, TRUE, 0);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  
  tree_store = libre_impuesto_get_tree_data_general( impuesto );
  widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL(tree_store)); 

  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title (column,_( "Libre Impuestos"));

  cell_renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, FALSE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "pixbuf", 0);

  cell_renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, TRUE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "text", 1);

  gtk_tree_view_append_column (GTK_TREE_VIEW(widget), column);

  gtk_tree_view_expand_all(GTK_TREE_VIEW(widget));

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
  g_signal_connect_swapped ( selection, "changed",
			     G_CALLBACK (node_tree_selection_changed_cb), 
			     G_OBJECT (impuesto_window));


  gtk_container_add (GTK_CONTAINER (scroll), widget);

  gtk_container_add_with_properties (GTK_CONTAINER (stack), scroll, 
				     "name", _("Generales"),
				     "title",_("General Options"),
				     "icon-name", GTK_STOCK_HOME,
				     NULL);


  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  tree_store = libre_impuesto_get_tree_data_formulario( impuesto );
  widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL(tree_store)); 

  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title (column, _("Electronic Forms"));

  cell_renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, FALSE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "pixbuf", 0);

  cell_renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, TRUE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "text", 1);

  gtk_tree_view_append_column (GTK_TREE_VIEW(widget), column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
  g_signal_connect_swapped ( selection, "changed",
			     G_CALLBACK (node_tree_selection_changed_cb), 
			     G_OBJECT (impuesto_window));

  gtk_container_add (GTK_CONTAINER (scroll), widget);

  gtk_container_add_with_properties (GTK_CONTAINER (stack), scroll, 
				     "name", _("Modulos"),
				     "title",_( "Electronic Forms"),
				     "icon-name", GTK_STOCK_HOME,
				     NULL);

  tree_path = gtk_tree_path_new_from_string ("0");
  gtk_tree_view_expand_row (GTK_TREE_VIEW(widget), tree_path, FALSE);


  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  tree_store = libre_impuesto_get_tree_data_consultor( impuesto );
  widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL(tree_store)); 

  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title (column,_("Consultores"));

  cell_renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, FALSE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "pixbuf", 0);

  cell_renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, cell_renderer, TRUE);
  gtk_tree_view_column_add_attribute (column, cell_renderer, "text", 1);

  gtk_tree_view_append_column (GTK_TREE_VIEW(widget), column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
  g_signal_connect_swapped ( selection, "changed",
			     G_CALLBACK (node_tree_selection_changed_cb), 
			     G_OBJECT (impuesto_window));

  gtk_container_add (GTK_CONTAINER (scroll), widget);

  gtk_container_add_with_properties (GTK_CONTAINER (stack), scroll, 
				     "name", _("Consultores"),
				     "title",_( "Consultores"),
				     "icon-name", GTK_STOCK_HOME,
				     NULL);

  tree_path = gtk_tree_path_new_from_string ("0");
  gtk_tree_view_expand_row (GTK_TREE_VIEW(widget), tree_path, FALSE);

  gtk_stack_switcher_set_stack (GTK_STACK_SWITCHER (switcher), GTK_STACK (stack));

  gtk_paned_pack1 ( GTK_PANED(paned), box_switcher, TRUE, FALSE );

  gtk_widget_show_all (box_switcher);

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

  widget = gtk_drawing_area_new ();
  gtk_widget_set_name (widget, "logo");

  /*gtk_widget_set_size_request (widget, 
			       gdk_pixbuf_get_width (logo), 
			       gdk_pixbuf_get_height (logo));*/

  gtk_widget_set_size_request (widget, -1, 50);

  g_signal_connect (widget, "draw",
		    G_CALLBACK (draw_area_cb), NULL);

  gtk_container_add (GTK_CONTAINER (frame), widget);

  gtk_paned_pack2 (GTK_PANED(paned), frame, TRUE, FALSE);

  return paned;

}


/* Construct menu bar */
static GtkWidget *
libre_impuesto_window_construct_menubar (LibreImpuestoWindow *impuesto_window)
{
  GtkWidget *main_menu;

  main_menu = libre_impuesto_window_get_managed_widget (impuesto_window, "/main-menu");

  return main_menu;
}



/*  The realize signal gets emitted when the widget its created. */
static void
libre_impuesto_window_on_realize (GtkWidget *widget)
{
  LibreImpuestoWindow *impuesto_window;

  /* Chain up to parent's dispose() method. */
  GTK_WIDGET_CLASS (libre_impuesto_window_parent_class)->realize (widget);

  impuesto_window = LIBRE_IMPUESTO_WINDOW (widget);
  libre_impuesto_maximize_window (impuesto_window->priv->impuesto,
				  GTK_WINDOW (widget));
}


/* set property function. */
static void
libre_impuesto_window_set_property (GObject *object,
				    guint property_id,
				    const GValue *value,
				    GParamSpec *pspec)
{
  switch (property_id) {

  case PROP_GEOMETRY:
    libre_impuesto_window_set_geometry (
	LIBRE_IMPUESTO_WINDOW (object),
	g_value_get_string (value));
    return;


  case PROP_IMPUESTO:
    libre_impuesto_window_set_impuesto (
	LIBRE_IMPUESTO_WINDOW (object),
	g_value_get_object (value));
    return;

  case PROP_SIDEBAR_VISIBLE:
    libre_impuesto_window_set_sidebar_visible (
	LIBRE_IMPUESTO_WINDOW (object),
	g_value_get_boolean (value));
    return;

  case PROP_HEADERBAR_VISIBLE:
    libre_impuesto_window_set_headerbar_visible (
	LIBRE_IMPUESTO_WINDOW (object),
	g_value_get_boolean (value));
    return;

  case PROP_STATUSBAR_VISIBLE:
    libre_impuesto_window_set_statusbar_visible (
       LIBRE_IMPUESTO_WINDOW (object),
       g_value_get_boolean (value));
    return;
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}


/* get property function. */
static void
libre_impuesto_window_get_property (GObject *object,
				    guint property_id,
				    GValue *value,
				    GParamSpec *pspec)
{
  switch (property_id) {

  case PROP_IMPUESTO:
    g_value_set_object (
			value, 
			libre_impuesto_window_get_impuesto (
			   LIBRE_IMPUESTO_WINDOW (object)));
    return;

  case PROP_SIDEBAR_VISIBLE:
    g_value_set_boolean (
			 value, 
			 libre_impuesto_window_get_sidebar_visible (
			    LIBRE_IMPUESTO_WINDOW (object)));
			return;

  case PROP_HEADERBAR_VISIBLE:
    g_value_set_boolean (
			 value, libre_impuesto_window_get_headerbar_visible (
			   LIBRE_IMPUESTO_WINDOW (object)));
    return;

  case PROP_STATUSBAR_VISIBLE:
    g_value_set_boolean (
			 value, libre_impuesto_window_get_statusbar_visible (
			   LIBRE_IMPUESTO_WINDOW (object)));
    return;

  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}


/* the dispose function is supposed to drop all references to other objects. */
static void
libre_impuesto_window_dispose (GObject *object)
{
  libre_impuesto_window_private_dispose (LIBRE_IMPUESTO_WINDOW (object));

  /* Chain up to parent's dispose() method. */
  G_OBJECT_CLASS (libre_impuesto_window_parent_class)->dispose (object);
}


/* instance finalization function.*/
static void
libre_impuesto_window_finalize (GObject *object)
{
  libre_impuesto_window_private_finalize (LIBRE_IMPUESTO_WINDOW (object));

  /* Chain up to parent's finalize() method. */
  G_OBJECT_CLASS (libre_impuesto_window_parent_class)->finalize (object);
}



/* object libre impuesto window constructed */
static void
libre_impuesto_window_constructed (GObject *object)
{
  GtkBox *box;
  GtkPaned *paned;
  GtkWidget *widget;
  LibreImpuesto *impuesto;
  LibreImpuestoWindow *impuesto_window = LIBRE_IMPUESTO_WINDOW (object);

  impuesto = libre_impuesto_window_get_impuesto (impuesto_window);
  libre_impuesto_window_actions_init(impuesto_window);

  // Create a GtkBox container
  widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (impuesto_window), widget);

  box = GTK_BOX (widget);

  widget = libre_impuesto_window_construct_menubar (impuesto_window);
  if (widget != NULL)
    gtk_box_pack_start (box, widget, FALSE, FALSE, 0);

  widget = libre_impuesto_window_construct_headerbar (impuesto_window);
  if (widget != NULL)
    gtk_box_pack_start (box, widget, FALSE, FALSE, 0);

  widget = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start (box, widget, TRUE, TRUE, 0);

  paned = GTK_PANED (widget);

  widget = gtk_statusbar_new();
  g_object_bind_property ( impuesto_window, "statusbar-visible",
			   widget, "visible",
			   G_BINDING_SYNC_CREATE);
  gtk_statusbar_push (GTK_STATUSBAR (widget), 
		      1,_("Libre Impuesto Version 0.0.1"));

  gtk_box_pack_start (box, widget, FALSE, FALSE, 0);

  widget = gtk_statusbar_get_message_area (GTK_STATUSBAR (widget));
  gtk_widget_set_halign (widget, GTK_ALIGN_END);
    
  widget = libre_impuesto_window_construct_sidebar (impuesto, impuesto_window);
  if (widget != NULL)
    gtk_paned_pack1 (paned, widget, FALSE, FALSE);

  widget = gtk_drawing_area_new ();
  gtk_widget_set_name (widget, "libre-impuesto-background");
  g_signal_connect (widget, "draw",
		    G_CALLBACK (draw_area_cb), NULL);

  if (widget != NULL)
    gtk_paned_pack2 (paned, widget, TRUE, FALSE);

  gtk_widget_show_all (GTK_WIDGET(box));

  gtk_application_add_window (GTK_APPLICATION (impuesto), 
			      GTK_WINDOW(impuesto_window));

  /* Chain up to parent's constructed() method. */
  G_OBJECT_CLASS (libre_impuesto_window_parent_class)->constructed (object);
}



/* class constructor */
static void
libre_impuesto_window_class_init (LibreImpuestoWindowClass *class)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;

  g_type_class_add_private (class, sizeof (LibreImpuestoWindowPrivate));

  object_class = G_OBJECT_CLASS (class);
  object_class->set_property = libre_impuesto_window_set_property;
  object_class->get_property = libre_impuesto_window_get_property;
  object_class->dispose = libre_impuesto_window_dispose;
  object_class->finalize = libre_impuesto_window_finalize;
  object_class->constructed = libre_impuesto_window_constructed;

  widget_class = GTK_WIDGET_CLASS (class);
  widget_class->realize = libre_impuesto_window_on_realize;

  class->construct_menubar = libre_impuesto_window_construct_menubar;
  class->construct_sidebar = libre_impuesto_window_construct_sidebar;
  class->construct_headerbar = libre_impuesto_window_construct_headerbar;

  g_object_class_install_property (
	   object_class,
	   PROP_IMPUESTO,
	   g_param_spec_object (
				"libre-impuesto",
				"Libre Impuesto",
				_("The Libre Impuesto singleton"),
				TYPE_LIBRE_IMPUESTO,
				G_PARAM_READWRITE |
				G_PARAM_CONSTRUCT_ONLY));


  g_object_class_install_property (
	   object_class,
	   PROP_GEOMETRY,
	   g_param_spec_string (
				"geometry",
				"Geometry",
				_("Initial window geometry string"),
				NULL,
				G_PARAM_WRITABLE |
				G_PARAM_CONSTRUCT_ONLY));


  g_object_class_install_property (
	   object_class,
	   PROP_HEADERBAR_VISIBLE,
	   g_param_spec_boolean (
				 "headerbar-visible",
				 "Headerbar Visible",
				 _("Whether the libre impuesto window's header bar is visible"),
				 TRUE,
				 G_PARAM_READWRITE));



  g_object_class_install_property (
           object_class,
	   PROP_SIDEBAR_VISIBLE,
	   g_param_spec_boolean (
				 "sidebar-visible",
				 "Sidebar Visible",
				 _("Whether the libre impuesto window's side bar is visible"),
				 TRUE,
				 G_PARAM_READWRITE));
	

  g_object_class_install_property (
            object_class,
	    PROP_STATUSBAR_VISIBLE,
	    g_param_spec_boolean (
				  "statusbar-visible",
				  "Statusbar Visible",
				  _("Whether the libre impuesto window's status bar is visible"),
				  TRUE,
				  G_PARAM_READWRITE));
}


/* Intance constructor */
static void
libre_impuesto_window_init (LibreImpuestoWindow *impuesto_window)
{
  impuesto_window->priv = LIBRE_IMPUESTO_WINDOW_GET_PRIVATE (impuesto_window);
  libre_impuesto_window_private_init (impuesto_window);
}

/* New libre_impuesto_window class. */
GtkWidget *
libre_impuesto_window_new (LibreImpuesto *impuesto,
			   const gchar *geometry)
{
  return g_object_new ( TYPE_LIBRE_IMPUESTO_WINDOW,
			"libre-impuesto", impuesto, 
			"geometry", geometry,
			NULL );
}

