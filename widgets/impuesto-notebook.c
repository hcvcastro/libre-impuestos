/*
 * impuesto-notebook.c
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
 * Written December 2013 by Henry Castro <hcvcastro@gmail.com>.  
 *
 */

#include "gtkstack.h"
#include "libre-impuesto-window.h"
#include "libre-impuesto-web-view.h"
#include "libre-impuesto-embed.h"
#include "impuesto-notebook.h"
#include <glib/gi18n.h>

#define AFTER_ALL_TABS -1
#define TAB_WIDTH_N_CHARS 15
#define NOT_IN_APP_WINDOWS -2
#define IMPUESTO_NOTEBOOK_TAB_GROUP_ID "0x42"

#define IMPUESTO_NOTEBOOK_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ((object), IMPUESTO_TYPE_NOTEBOOK, ImpuestoNotebookPrivate))

struct _ImpuestoNotebookPrivate
{
  guint show_tabs : 1;
};

static const GtkTargetEntry url_drag_types [] = 
{
  { (gchar *)"GTK_NOTEBOOK_TAB", GTK_TARGET_SAME_APP, 0 },
  { (gchar *)IMPUESTO_DND_URI_LIST_TYPE, 0, 0 },
  { (gchar *)IMPUESTO_DND_URL_TYPE, 0, 1 },
};

enum
{
	PROP_0,
	PROP_SHOW_TABS
};

enum
{
	TAB_CLOSE_REQUEST,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

G_DEFINE_TYPE (ImpuestoNotebook, impuesto_notebook, GTK_TYPE_NOTEBOOK)


static void
update_tabs_visibility (ImpuestoNotebook *nb,
			gboolean before_inserting)
{
  ImpuestoNotebookPrivate *priv = nb->priv;
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (nb), priv->show_tabs);
}



void
impuesto_notebook_set_show_tabs (ImpuestoNotebook *nb,
				 gboolean show_tabs)
{
  ImpuestoNotebookPrivate *priv = nb->priv;

  priv->show_tabs = show_tabs != FALSE;
  update_tabs_visibility (nb, FALSE);
}



static void
impuesto_notebook_finalize (GObject *object)
{
  //ImpuestoNotebook *notebook = IMPUESTO_NOTEBOOK (object);
  //ImpuestoNotebookPrivate *priv = notebook->priv;

  G_OBJECT_CLASS (impuesto_notebook_parent_class)->finalize (object);
}

static void
impuesto_notebook_get_property (GObject *object,
				guint prop_id,
				GValue *value,
				GParamSpec *pspec)
{
  ImpuestoNotebook *notebook = IMPUESTO_NOTEBOOK (object);
  ImpuestoNotebookPrivate *priv = notebook->priv;

  switch (prop_id) {
  case PROP_SHOW_TABS:
    g_value_set_boolean (value, priv->show_tabs);
    break;
  }
}

static void
impuesto_notebook_set_property (GObject *object,
				guint prop_id,
				const GValue *value,
				GParamSpec *pspec)
{
  ImpuestoNotebook *notebook = IMPUESTO_NOTEBOOK (object);

  switch (prop_id) {
  case PROP_SHOW_TABS:
    impuesto_notebook_set_show_tabs (notebook, g_value_get_boolean (value));
    break;
  }
}


static void
close_button_clicked_cb (GtkWidget *widget, GtkWidget *tab)
{
  GtkWidget *notebook;

  notebook = gtk_widget_get_parent (tab);
  g_signal_emit (notebook, signals[TAB_CLOSE_REQUEST], 0, tab);
}

static void
tab_label_style_set_cb (GtkWidget *hbox,
			GtkStyle *previous_style,
			gpointer user_data)
{
  PangoFontMetrics *metrics;
  PangoContext *context;
  GtkStyleContext *style;
  GtkWidget *button;
  int char_width, h, w;

  context = gtk_widget_get_pango_context (hbox);
  style = gtk_widget_get_style_context (hbox);
  metrics = pango_context_get_metrics (context,
				       gtk_style_context_get_font (style, GTK_STATE_FLAG_NORMAL),
				       pango_context_get_language (context));
  char_width = pango_font_metrics_get_approximate_digit_width (metrics);
  pango_font_metrics_unref (metrics);

  gtk_icon_size_lookup_for_settings (gtk_widget_get_settings (hbox),
				     GTK_ICON_SIZE_MENU, &w, &h);

  gtk_widget_set_size_request
    (hbox, TAB_WIDTH_N_CHARS * PANGO_PIXELS(char_width) + 2 * w, -1);

  button = g_object_get_data (G_OBJECT (hbox), "close-button");
  gtk_widget_set_size_request (button, w + 2, h + 2);
}



static GtkWidget *
build_tab_label (ImpuestoNotebook *nb, GtkWidget *embed, gchar *text)
{
  GtkWidget *hbox, *label, *close_button, *image, *spinner, *icon;
  //WebKitWebView *view;

  /* set hbox spacing and label padding (see below) so that there's an
   * equal amount of space around the label */
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_widget_show (hbox);

  /* setup load feedback */
  spinner = gtk_spinner_new ();
  gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);

  /* setup site icon, empty by default */
  icon = gtk_image_new ();
  gtk_box_pack_start (GTK_BOX (hbox), icon, FALSE, FALSE, 0);
  /* don't show the icon */

  /* setup label */
  label = gtk_label_new (text);
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);
  gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label), 0, 0);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
  gtk_widget_show (label);

  /* setup close button */
  close_button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
  /* don't allow focus on the close button */
  gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);

  gtk_widget_set_name (close_button, "libre-impuesto-tab-close-button");

  image = gtk_image_new_from_icon_name ("window-close-symbolic",
					      GTK_ICON_SIZE_MENU);
  gtk_widget_set_tooltip_text (close_button, _("Close tab"));
  g_signal_connect (close_button, "clicked",
		    G_CALLBACK (close_button_clicked_cb), embed);

  gtk_container_add (GTK_CONTAINER (close_button), image);
  gtk_widget_show (image);

  gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);
  gtk_widget_show (close_button);

  /* Set minimal size */
  g_signal_connect (hbox, "style-set",
		    G_CALLBACK (tab_label_style_set_cb), NULL);

  g_object_set_data (G_OBJECT (hbox), "label", label);
  g_object_set_data (G_OBJECT (hbox), "spinner", spinner);
  g_object_set_data (G_OBJECT (hbox), "icon", icon);
  g_object_set_data (G_OBJECT (hbox), "close-button", close_button);

  return hbox;
}


static int
impuesto_notebook_insert_page (GtkNotebook *gnotebook,
			       GtkWidget *tab_widget,
			       GtkWidget *tab_label,
			       GtkWidget *menu_label,
			       int position)
{
  ImpuestoNotebook *notebook = IMPUESTO_NOTEBOOK (gnotebook);
  gchar *text;

  /* Destroy passed-in tab label */
  if (tab_label != NULL) {
    g_object_get (tab_label, "label", &text, NULL);
    g_object_ref_sink (tab_label);
    g_object_unref (tab_label);
  }

  g_assert (GTK_IS_WIDGET (tab_widget));

  tab_label = build_tab_label (notebook, tab_widget, text);
  g_free(text);

  update_tabs_visibility (notebook, TRUE);

  position = GTK_NOTEBOOK_CLASS (impuesto_notebook_parent_class)->insert_page 
    (gnotebook, tab_widget, tab_label, menu_label, position);

  gtk_notebook_set_tab_reorderable (gnotebook, tab_widget, TRUE);
  gtk_notebook_set_tab_detachable (gnotebook, tab_widget, TRUE);

  return position;
}


static ImpuestoNotebook *
find_notebook_at_pointer (GdkDisplay *display, gint abs_x, gint abs_y)
{
  GdkWindow *win_at_pointer, *toplevel_win;
  gpointer toplevel = NULL;
  gint x, y;

  win_at_pointer = gdk_device_get_window_at_position (
		    gdk_device_manager_get_client_pointer (
		    gdk_display_get_device_manager (display)),
		&x, &y);
  
  if (win_at_pointer == NULL) {
    // We are outside all windows containing a notebook 
    return NULL;
  }

  toplevel_win = gdk_window_get_toplevel (win_at_pointer);

  // get the GtkWidget which owns the toplevel GdkWindow 
  gdk_window_get_user_data (toplevel_win, &toplevel);

  // toplevel should be an ImpuestoWindow 
  if (toplevel != NULL && IS_LIBRE_IMPUESTO_WINDOW (toplevel)) {
    return IMPUESTO_NOTEBOOK  (libre_impuesto_window_get_notebook
			      (LIBRE_IMPUESTO_WINDOW (toplevel)));
  }

  return NULL;
}

static gboolean
is_in_notebook_window (ImpuestoNotebook *notebook,
		       gint abs_x, gint abs_y)
{
  ImpuestoNotebook *nb_at_pointer;

  nb_at_pointer = find_notebook_at_pointer (gtk_widget_get_display (GTK_WIDGET (notebook)), abs_x, abs_y);

  return nb_at_pointer == notebook;
}


static gint
find_tab_num_at_pos (ImpuestoNotebook *notebook, gint abs_x, gint abs_y)
{
  GtkPositionType tab_pos;
  int page_num = 0;
  GtkNotebook *nb = GTK_NOTEBOOK (notebook);
  GtkWidget *page;

  /* For some reason unfullscreen + quick click can
     cause a wrong click event to be reported to the tab */
  if (!is_in_notebook_window (notebook, abs_x, abs_y)) {
    return NOT_IN_APP_WINDOWS;
  }

  tab_pos = gtk_notebook_get_tab_pos (nb);

  while ((page = gtk_notebook_get_nth_page (nb, page_num))) {
    GtkWidget *tab;
    GtkAllocation allocation;
    gint max_x, max_y;
    gint x_root, y_root;

    tab = gtk_notebook_get_tab_label (nb, page);
    g_return_val_if_fail (tab != NULL, -1);

    if (!gtk_widget_get_mapped (GTK_WIDGET (tab))) {
      page_num++;
      continue;
    }

    gdk_window_get_origin (gtk_widget_get_window (tab),
			   &x_root, &y_root);

    gtk_widget_get_allocation (tab, &allocation);
    max_x = x_root + allocation.x + allocation.width;
    max_y = y_root + allocation.y + allocation.height;

    if (((tab_pos == GTK_POS_TOP)
	 || (tab_pos == GTK_POS_BOTTOM))
	&&(abs_x<=max_x)) {
      return page_num;
    }
    else if (((tab_pos == GTK_POS_LEFT)
	      || (tab_pos == GTK_POS_RIGHT))
	     && (abs_y<=max_y)) {
      return page_num;
    }

    page_num++;
  }
  return AFTER_ALL_TABS;
}

static gboolean
button_press_cb (ImpuestoNotebook *notebook,
		 GdkEventButton *event,
		 gpointer data)
{
  int tab_clicked;

  tab_clicked = find_tab_num_at_pos (notebook, event->x_root, event->y_root);

  if (event->type == GDK_BUTTON_PRESS &&
      event->button == 3 &&
      (event->state & gtk_accelerator_get_default_mod_mask ()) == 0) {
    if (tab_clicked == -1) {
      /* Consume event so that we don't pop up the context
       * menu when the mouse is not over a tab label.
       */
      return TRUE;
    }

    /* Switch to the page where the mouse is over, but don't consume the
     * event. */
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), tab_clicked);
  }

  return FALSE;
}

static void
impuesto_notebook_init (ImpuestoNotebook *notebook)
{
  ImpuestoNotebookPrivate *priv;
  //GtkWidget *widget = GTK_WIDGET (notebook);
  GtkNotebook *gnotebook = GTK_NOTEBOOK (notebook);

  priv = notebook->priv = IMPUESTO_NOTEBOOK_GET_PRIVATE (notebook);

  gtk_notebook_set_scrollable (gnotebook, TRUE);
  gtk_notebook_set_show_border (gnotebook, FALSE);
  gtk_notebook_set_show_tabs (gnotebook, FALSE);
  gtk_notebook_set_group_name (gnotebook, IMPUESTO_NOTEBOOK_TAB_GROUP_ID);

  priv->show_tabs = TRUE;

  g_signal_connect (notebook, "button-press-event",
		    (GCallback)button_press_cb, NULL);
}

static void
impuesto_notebook_class_init (ImpuestoNotebookClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  //GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  GtkNotebookClass *notebook_class = GTK_NOTEBOOK_CLASS (klass);

  object_class->finalize = impuesto_notebook_finalize;
  object_class->get_property = impuesto_notebook_get_property;
  object_class->set_property = impuesto_notebook_set_property;

  //container_class->remove = impuesto_notebook_remove;

  notebook_class->insert_page = impuesto_notebook_insert_page;

  signals[TAB_CLOSE_REQUEST] =
    g_signal_new ("tab-close-request",
		  G_OBJECT_CLASS_TYPE (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ImpuestoNotebookClass, tab_close_req),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__OBJECT,
		  G_TYPE_NONE,
		  1,
		  GTK_TYPE_WIDGET /* Can't use an interface type here */);

  g_object_class_install_property (object_class,
				   PROP_SHOW_TABS,
				   g_param_spec_boolean ("show-tabs", NULL, NULL,
							 TRUE,
							 G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_type_class_add_private (object_class, sizeof (ImpuestoNotebookPrivate));
}

GtkWidget *
impuesto_notebook_new ()
{
  return g_object_new ( IMPUESTO_TYPE_NOTEBOOK, NULL);
}


gint 
libre_impuesto_notebook_find_tab_page (ImpuestoNotebook *notebook, 
				       const gchar * name )
{
  int page_num = 0;
  GtkNotebook *nb = GTK_NOTEBOOK (notebook);
  GtkWidget *page;
  GtkWidget *tab;
  const gchar *label;

  while ((page = gtk_notebook_get_nth_page (nb, page_num))) {
    tab = gtk_notebook_get_tab_label (nb, page);
    g_return_val_if_fail (tab != NULL, -1);
    tab = g_object_get_data (G_OBJECT (tab), "label");
    label = gtk_label_get_text (GTK_LABEL(tab));

    if (g_strcmp0(label,name)==0)
      break;

    page_num++;
  }

  if (!page) page_num = -1;

  return page_num;
}
