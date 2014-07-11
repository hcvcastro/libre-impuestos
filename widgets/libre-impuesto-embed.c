/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/* vim: set sw=2 ts=2 sts=2 et: */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA *
 *  Written December 2013 by Henry Castro <hcvcastro@gmail.com>.  
 */

#include "gtkstack.h"
#include "libre-impuesto-window-private.h"
#include "libre-impuesto-web-view.h"
#include "libre-impuesto-embed.h"
#include "impuesto-notebook.h"
#include "impuesto-command.h"
#include "nautilus-floating-bar.h"
#include <webkit/webkit.h>
#include <glib/gi18n.h>


#define LIBRE_IMPUESTO_EMBED_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LIBRE_IMPUESTO_TYPE_EMBED, LibreImpuestoEmbedPrivate))

#define LIBRE_IMPUESTO_EMBED_STATUSBAR_TAB_MESSAGE_CONTEXT_DESCRIPTION "tab_message"

typedef struct {
  gchar *text;
  guint context_id;
  guint message_id;
} LibreImpuestoEmbedStatusbarMsg;



struct _LibreImpuestoEmbedPrivate
{
  GtkBox *top_widgets_vbox;
  GtkScrolledWindow *scrolled_window;
  GtkPaned *paned;
  WebKitWebView *web_view;
  gboolean inspector_attached;
  GSList *destroy_on_transition_list;
  GtkWidget *floating_bar;
  GtkWidget *progress;

  GSList *messages;
  GSList *keys;

  guint seq_context_id;
  guint seq_message_id;

  guint tab_message_id;
  guint clear_statusbar_source_id;
  guint clear_progress_source_id;

  gulong status_handler_id;
  gulong progress_update_handler_id;

};

G_DEFINE_TYPE (LibreImpuestoEmbed, libre_impuesto_embed, GTK_TYPE_BOX)


static void
on_button_open_clicked (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  LibreImpuestoWindow *libre_impuesto_window = g_object_get_data ( 
                                                   G_OBJECT(embed), 
                                                   "impuesto-window");
  LibreImpuestoCommand *impuesto_command;

  impuesto_command = libre_impuesto_get_command(libre_impuesto_window_get_impuesto(libre_impuesto_window));    
  libre_impuesto_queue_command (impuesto_command,
                                IMPUESTO_COMMAND_NOT_IMPLEMENTED,
                                NULL,
                                FALSE);
}

static void
on_button_save_clicked (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  LibreImpuestoWindow *libre_impuesto_window = g_object_get_data ( 
                                                   G_OBJECT(embed), 
                                                   "impuesto-window");
  LibreImpuestoCommand *impuesto_command;

  impuesto_command = libre_impuesto_get_command(libre_impuesto_window_get_impuesto(libre_impuesto_window));    
  libre_impuesto_queue_command (impuesto_command,
                                IMPUESTO_COMMAND_NOT_IMPLEMENTED,
                                NULL,
                                FALSE);
}


static void
print_load_status_changed_cb (WebKitWebView *clone_web_view,
                              GParamSpec *spec,
                              GMainLoop *loop)
{
  WebKitLoadStatus status = webkit_web_view_get_load_status (clone_web_view);

  if (status == WEBKIT_LOAD_FINISHED) {
    g_main_loop_quit(loop);
  }
}

static void
on_button_print_clicked (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  WebKitDOMDocument* document;
  WebKitDOMNodeList* list;
  WebKitDOMNode* parent = NULL;
  WebKitDOMNode* header = NULL;
  WebKitDOMNode* footer = NULL;
  WebKitDOMNode* node = NULL;
  WebKitDOMHTMLElement* body;
  gulong length; //, iterator;
  WebKitWebView* web_view = embed->priv->web_view;
  WebKitWebFrame* main_frame, * frame_content, * frame_header, * frame_footer;
  GtkWidget* content_web_view,* header_web_view,* footer_web_view;
  GtkPrintOperation* operation;
  GtkPageSetup* page_setup;
  GError* error = NULL;
  WebKitWebDataSource *data_source;
  const GString *data;
  GMainLoop* loop;
  //gchar* text_header, * text_footer;
  const gchar *baseURL;


  // Clone Document
  main_frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(web_view));
  data_source = webkit_web_frame_get_data_source (main_frame);
  data = webkit_web_data_source_get_data (data_source);

  baseURL = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(web_view));

  content_web_view = webkit_web_view_new();
  header_web_view = webkit_web_view_new();
  footer_web_view = webkit_web_view_new();
  g_object_ref_sink(footer_web_view);
  g_object_ref_sink(header_web_view);
  g_object_ref_sink(content_web_view);

  loop = g_main_loop_new(NULL, FALSE);
  g_object_connect (content_web_view,
                    "signal::notify::load-status", 
                    G_CALLBACK (print_load_status_changed_cb), loop, NULL);

  g_object_connect (header_web_view,
                    "signal::notify::load-status", 
                    G_CALLBACK (print_load_status_changed_cb), loop, NULL);

  g_object_connect (footer_web_view,
                    "signal::notify::load-status", 
                    G_CALLBACK (print_load_status_changed_cb), loop, NULL);

  // Clone Document for Content
  webkit_web_view_load_string(WEBKIT_WEB_VIEW (content_web_view), data->str, NULL, NULL, baseURL);

  g_main_loop_run(loop);

  // Clone Document for Header
  webkit_web_view_load_string(WEBKIT_WEB_VIEW (header_web_view), data->str, NULL, NULL, baseURL);

  g_main_loop_run(loop);

  // Clone Document for footer
  webkit_web_view_load_string(WEBKIT_WEB_VIEW (footer_web_view), data->str, NULL, NULL, baseURL);

  g_main_loop_run(loop);

  document = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW (content_web_view));

  // Get Header Tag
  list = webkit_dom_document_get_elements_by_tag_name(document, "header");
  length = webkit_dom_node_list_get_length(list);  
  if ( length > 0 ) 
    header = webkit_dom_node_list_item(list, 0);
  g_object_unref(list);

  if ( header != NULL ) {
    parent = webkit_dom_node_get_parent_node(header);
    webkit_dom_node_remove_child(parent, header, NULL);
  }

  // Get Footer Tag
  list = webkit_dom_document_get_elements_by_tag_name(document, "footer");
  length = webkit_dom_node_list_get_length(list);  
  if ( length > 0 ) 
    footer = webkit_dom_node_list_item(list, length - 1);
  g_object_unref(list);
 
  if ( footer != NULL ) {
    parent = webkit_dom_node_get_parent_node(footer);
    webkit_dom_node_remove_child(parent, footer, NULL);
  }

  document = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW (header_web_view));
  body = webkit_dom_document_get_body(document); 
  list = webkit_dom_node_get_child_nodes(WEBKIT_DOM_NODE(body));
  while (webkit_dom_node_list_get_length(list) > 0) {
    node = webkit_dom_node_list_item(list, 0);
    webkit_dom_node_remove_child(WEBKIT_DOM_NODE(body), node, NULL);    
    g_object_unref(node);
  }
  g_object_unref(list);
  webkit_dom_node_append_child(WEBKIT_DOM_NODE(body), header, NULL);
 
  document = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW (footer_web_view));
  body = webkit_dom_document_get_body(document); 
  list = webkit_dom_node_get_child_nodes(WEBKIT_DOM_NODE(body));
  while (webkit_dom_node_list_get_length(list) > 0) {
    node = webkit_dom_node_list_item(list, 0);
    webkit_dom_node_remove_child(WEBKIT_DOM_NODE(body), node, NULL);    
    g_object_unref(node);
  }
  g_object_unref(list);
  webkit_dom_node_append_child(WEBKIT_DOM_NODE(body), footer, NULL);

  // get text header
  /*  if ( header != NULL ) {
    g_object_get ((WebKitDOMHTMLElement *)header, "outer-html", &text_header, NULL);
    webkit_web_view_load_string(WEBKIT_WEB_VIEW (header_web_view), text_header, NULL, NULL, NULL);
    g_main_loop_run(loop);    
    g_free(text_header);
  }

  // get text footer
  if ( footer != NULL ) {
    g_object_get ((WebKitDOMHTMLElement *)footer, "outer-html", &text_footer, NULL);
    webkit_web_view_load_string(WEBKIT_WEB_VIEW (footer_web_view), text_footer, NULL, NULL, NULL);
    g_main_loop_run(loop);    
    g_free(text_footer);
    }*/

  g_main_loop_unref(loop);

  frame_content = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (content_web_view));
  frame_header = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (header_web_view));
  frame_footer = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (footer_web_view));

  operation = gtk_print_operation_new ();
  gtk_print_operation_set_embed_page_setup (operation, TRUE);
  page_setup = gtk_page_setup_new();
  gtk_print_operation_set_default_page_setup (operation, page_setup);

  webkit_web_frame_libre_impuesto_print_full (frame_header, 
                                              frame_content, 
                                              frame_footer,
                                              operation, 
                                              GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, 
                                              &error);

  if (error) {
    g_error_free (error);
  }

  g_object_unref (page_setup);
  g_object_unref (operation);
  g_object_unref (footer_web_view);
  g_object_unref (header_web_view);
  g_object_unref (content_web_view);
}

static void
on_button_export_clicked (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  LibreImpuestoWindow *libre_impuesto_window = g_object_get_data ( 
                                                   G_OBJECT(embed), 
                                                   "impuesto-window");
  LibreImpuestoCommand *impuesto_command;

  impuesto_command = libre_impuesto_get_command(libre_impuesto_window_get_impuesto(libre_impuesto_window));    
  libre_impuesto_queue_command (impuesto_command,
                                IMPUESTO_COMMAND_NOT_IMPLEMENTED,
                                NULL,
                                FALSE);
}

static void
on_button_verify_clicked (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  LibreImpuestoWindow *libre_impuesto_window = g_object_get_data ( 
                                                   G_OBJECT(embed), 
                                                   "impuesto-window");
  LibreImpuestoCommand *impuesto_command;

  impuesto_command = libre_impuesto_get_command(libre_impuesto_window_get_impuesto(libre_impuesto_window));    
  libre_impuesto_queue_command (impuesto_command,
                                IMPUESTO_COMMAND_NOT_IMPLEMENTED,
                                NULL,
                                FALSE);
}


static void
libre_impuesto_embed_set_statusbar_label (LibreImpuestoEmbed *embed, 
                                          const gchar *label)
{
  LibreImpuestoEmbedPrivate *priv = embed->priv;

  nautilus_floating_bar_set_label (NAUTILUS_FLOATING_BAR (priv->floating_bar), 
                                   label);

  if (label == NULL || label[0] == '\0') {
    gtk_widget_hide (priv->floating_bar);
    gtk_widget_set_halign (priv->floating_bar, GTK_ALIGN_START);
  } else
    gtk_widget_show (priv->floating_bar);
}

static gboolean
pop_statusbar_later_cb (gpointer data)
{
  LibreImpuestoEmbed *embed = LIBRE_IMPUESTO_EMBED (data);

  libre_impuesto_embed_set_statusbar_label (embed, NULL);
  return FALSE;
}


static void 
resource_request_starting_cb(WebKitWebView* web_view, 
                             WebKitWebFrame* web_frame, 
                             WebKitWebResource* web_resource, 
                             WebKitNetworkRequest* request, 
                             WebKitNetworkResponse* response, 
                             LibreImpuestoEmbed *embed)
{
  LibreImpuestoEmbedPrivate *priv = embed->priv;
  const gchar *uri = webkit_network_request_get_uri (request);

  if (uri) {
    if (priv->clear_statusbar_source_id) {
      g_source_remove (priv->clear_statusbar_source_id);
      priv->clear_statusbar_source_id = 0;
    }

    libre_impuesto_embed_set_statusbar_label(embed, uri);
  }
}





static void
libre_impuesto_embed_destroy_top_widgets (LibreImpuestoEmbed *embed)
{
  GSList *iter;

  for (iter = embed->priv->destroy_on_transition_list; iter; iter = iter->next)
    gtk_widget_destroy (GTK_WIDGET (iter->data));
}


static void
remove_from_destroy_list_cb (GtkWidget *widget, LibreImpuestoEmbed *embed)
{
  GSList *list;

  list = embed->priv->destroy_on_transition_list;
  list = g_slist_remove (list, widget);
  embed->priv->destroy_on_transition_list = list;
}


static gboolean
clear_progress_cb (LibreImpuestoEmbed *embed)
{
  gtk_widget_hide (embed->priv->progress);
  embed->priv->clear_progress_source_id = 0;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (embed->priv->progress), 0.0);

  return FALSE;
}



static void
progress_update_cb (WebKitWebView *view, 
                    GParamSpec *pspec, 
                    LibreImpuestoEmbed *embed)
{
  gdouble progress;
  //  gboolean loading;
  const char *uri;

  LibreImpuestoEmbedPrivate *priv = embed->priv;

  if (priv->clear_progress_source_id) {
    g_source_remove (priv->clear_progress_source_id);
    priv->clear_progress_source_id = 0;
  }

  uri = webkit_web_view_get_uri (priv->web_view);
  if (!uri || g_str_equal (uri, "about:blank"))
    return;

  progress = webkit_web_view_get_progress (priv->web_view);

  if (progress == 1.0)
    priv->clear_progress_source_id = g_timeout_add (500,
                                                    (GSourceFunc)clear_progress_cb, embed);
  else
    gtk_widget_show (priv->progress);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress), progress);
}

static void web_view_title_changed_cb(WebKitWebView* view, 
                                      WebKitWebFrame* frame, 
                                      const gchar* title, 
                                      LibreImpuestoEmbed * embed)
{
  GtkWindow * impuesto_window = libre_impuesto_get_active_window(libre_impuesto_get_default());  
  GtkNotebook * notebook;
  GtkWidget * widget;
  GtkWidget * label;

  notebook = libre_impuesto_window_get_notebook(LIBRE_IMPUESTO_WINDOW(impuesto_window));
  widget = gtk_notebook_get_tab_label(notebook, GTK_WIDGET(embed));
  label = g_object_get_data (G_OBJECT(widget), "label");
  gtk_label_set_text (GTK_LABEL(label), title);
}

static void
load_status_changed_cb (WebKitWebView *web_view,
                        GParamSpec *spec,
                        LibreImpuestoEmbed *embed)
{
  WebKitLoadStatus status = webkit_web_view_get_load_status (web_view);

  if (status == WEBKIT_LOAD_FINISHED)
    if (embed->priv->clear_statusbar_source_id == 0 )
      embed->priv->clear_statusbar_source_id = g_timeout_add (250, 
                                               pop_statusbar_later_cb, 
                                                              embed);  
  if (status == WEBKIT_LOAD_COMMITTED) 
    libre_impuesto_embed_destroy_top_widgets (embed);

  if (status == WEBKIT_LOAD_FAILED)
    web_view_title_changed_cb (web_view, NULL, "Error", embed);

  if (status == WEBKIT_LOAD_PROVISIONAL)
    web_view_title_changed_cb (web_view, NULL, "Loading", embed);
}


static gboolean
web_view_ready_cb (WebKitWebView *web_view,
                   LibreImpuestoEmbed *embed)
{
	gtk_widget_show (GTK_WIDGET (embed));
	return TRUE;
}


static WebKitWebView*
create_web_view_cb (WebKitWebView *web_view,
                    WebKitWebFrame *frame,
                    LibreImpuestoEmbed *embed)
{
  GtkWindow * impuesto_window = libre_impuesto_get_active_window(libre_impuesto_get_default());  
  GtkNotebook * notebook;
	WebKitWebView *new_web_view;
  GtkWidget * widget;
  const gchar * form;

  notebook = libre_impuesto_window_get_notebook(LIBRE_IMPUESTO_WINDOW(impuesto_window));

  form = webkit_web_view_get_uri(web_view);

  embed = LIBRE_IMPUESTO_EMBED (g_object_new (LIBRE_IMPUESTO_TYPE_EMBED, NULL));
  g_assert (embed != NULL);

  g_object_set_data(G_OBJECT(embed), "impuesto-window", impuesto_window);
  g_object_set_data(G_OBJECT(embed), "tree-view", NULL);
  g_object_set_data(G_OBJECT(embed), "path", NULL);
  g_object_set_data(G_OBJECT(embed), "form", g_strdup(form));

  new_web_view = libre_impuesto_embed_get_web_view (LIBRE_IMPUESTO_EMBED(embed));
  widget = gtk_label_new(NULL);
  gtk_notebook_append_page (notebook, GTK_WIDGET(embed), widget );
	return new_web_view;
}




static guint
libre_impuesto_embed_statusbar_get_context_id (LibreImpuestoEmbed *embed, const char  *context_description)
{
  char *string;
  guint id;

  g_return_val_if_fail (LIBRE_IMPUESTO_IS_EMBED (embed), 0);
  g_return_val_if_fail (context_description != NULL, 0);

  /* we need to preserve namespaces on object datas */
  string = g_strconcat ("libre-impuesto-embed-status-bar-context:", context_description, NULL);

  id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (embed), string));
  if (id == 0) {
    LibreImpuestoEmbedPrivate *priv = embed->priv;

    id = priv->seq_context_id++;
    g_object_set_data_full (G_OBJECT (embed), string, GUINT_TO_POINTER (id), NULL);
    priv->keys = g_slist_prepend (priv->keys, string);
  } else
    g_free (string);

  return id;
}

static void
window_resize_requested (WebKitWebWindowFeatures *features, GParamSpec *pspec, LibreImpuestoEmbed *embed)
{
  GtkWidget *window;
  gboolean is_popup;
  const char *property_name;
  int width, height;

  window = gtk_widget_get_toplevel (GTK_WIDGET (embed));
  if (!window || !gtk_widget_is_toplevel (window))
    return;

  g_object_get (window, "is-popup", &is_popup, NULL);
  if (!is_popup)
    return;

  property_name = g_param_spec_get_name (pspec);

  if (g_str_equal (property_name, "x") || g_str_equal (property_name, "y")) {
    int x, y;
    g_object_get (features, "x", &x, "y", &y, NULL);
    gtk_window_move (GTK_WINDOW (window), x, y);
    return;
  }

  g_object_get (features, "width", &width, "height", &height, NULL);
  gtk_window_resize (GTK_WINDOW (window), width, height);
}



static void
libre_impuesto_embed_constructed (GObject *object)
{
  LibreImpuestoEmbed *embed = (LibreImpuestoEmbed*)object;
  LibreImpuestoEmbedPrivate *priv = embed->priv;
  GtkWidget *scrolled_window;
  GtkWidget *paned;
  WebKitWebView *web_view;
  WebKitWebWindowFeatures *window_features;
  GtkWidget *overlay;
  GtkWidget *hbbox;
  GtkWidget *vbox;
  GtkWidget *button;

  // Skeleton
  //web_view = WEBKIT_WEB_VIEW (libre_impuesto_web_view_new ());
  web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
  scrolled_window = GTK_WIDGET (priv->scrolled_window);
  
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  overlay = gtk_overlay_new ();
  gtk_widget_add_events (overlay, 
                         GDK_ENTER_NOTIFY_MASK |
                         GDK_LEAVE_NOTIFY_MASK);
  gtk_widget_set_name (overlay, "libre-impuesto-overlay");
  gtk_container_add (GTK_CONTAINER (overlay), scrolled_window);

  hbbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout (GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_CENTER);
  button = gtk_button_new_with_label(_("Abrir"));
  gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_open_clicked), 
                    embed);
  gtk_container_add (GTK_CONTAINER (hbbox), button);
  button = gtk_button_new_with_label(_("Guardar"));
  gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_save_clicked), 
                    embed);
  gtk_container_add (GTK_CONTAINER (hbbox), button);
  button = gtk_button_new_with_label(_("Imprimir"));
  gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock(GTK_STOCK_PRINT, GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_print_clicked), 
                    embed);
  gtk_container_add (GTK_CONTAINER (hbbox), button);
  button = gtk_button_new_with_label(_("Exportar"));
  gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock(GTK_STOCK_DND_MULTIPLE, GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_export_clicked), 
                    embed);
  gtk_container_add (GTK_CONTAINER (hbbox), button);
  button = gtk_button_new_with_label(_("Verificar"));
  gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_verify_clicked), 
                    embed);
  gtk_container_add (GTK_CONTAINER (hbbox), button);


  gtk_box_pack_end (GTK_BOX (vbox), hbbox, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (vbox), overlay, TRUE, TRUE, 0);
  //gtk_container_add (GTK_CONTAINER (vbox), overlay);
  //gtk_container_add (GTK_CONTAINER (vbox), hbbox);

  // statusbar is hidden by default 
  priv->floating_bar = nautilus_floating_bar_new (NULL, FALSE);
  gtk_widget_set_halign (priv->floating_bar, GTK_ALIGN_START);
  gtk_widget_set_valign (priv->floating_bar, GTK_ALIGN_END);
  gtk_widget_set_no_show_all (priv->floating_bar, TRUE);

  gtk_overlay_add_overlay (GTK_OVERLAY (overlay), priv->floating_bar);

  priv->progress = gtk_progress_bar_new ();
  gtk_widget_set_name (priv->progress, "libre-impuesto-progress-bar");
  gtk_widget_set_halign (priv->progress, GTK_ALIGN_FILL);
  gtk_widget_set_valign (priv->progress, GTK_ALIGN_START);
  gtk_overlay_add_overlay (GTK_OVERLAY (overlay), priv->progress);

  paned = GTK_WIDGET (priv->paned);

  priv->web_view = web_view;
  priv->progress_update_handler_id =  g_signal_connect (
          web_view, "notify::progress",
          G_CALLBACK (progress_update_cb), object);

  gtk_container_add (GTK_CONTAINER (scrolled_window),
                     GTK_WIDGET (web_view));
  /*gtk_paned_pack1 (GTK_PANED (paned), GTK_WIDGET (overlay),
    TRUE, FALSE);*/

  gtk_paned_pack1 (GTK_PANED (paned), GTK_WIDGET (vbox), TRUE, FALSE);

  gtk_box_pack_start (GTK_BOX (embed),
                      GTK_WIDGET (priv->top_widgets_vbox),
                      FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (embed), paned, TRUE, TRUE, 0);

  gtk_widget_show (GTK_WIDGET (priv->top_widgets_vbox));
  gtk_widget_show (GTK_WIDGET (web_view));
  gtk_widget_show_all (paned);

  g_object_connect (web_view,
                    "signal::notify::load-status", 
                    G_CALLBACK (load_status_changed_cb), embed,
                    "signal::resource-request-starting", 
                    G_CALLBACK (resource_request_starting_cb), embed,
                    "signal::create-web-view",
                    G_CALLBACK (create_web_view_cb), embed,
                    "signal::title-changed",
                    G_CALLBACK (web_view_title_changed_cb), embed,               
                    "signal::web-view-ready",
                    G_CALLBACK (web_view_ready_cb), embed,
                    NULL);

  // Window features 
  window_features = webkit_web_view_get_window_features (web_view);
  g_object_connect (window_features,
                    "signal::notify::x", G_CALLBACK (window_resize_requested), embed,
                    "signal::notify::y", G_CALLBACK (window_resize_requested), embed,
                    "signal::notify::width", G_CALLBACK (window_resize_requested), embed,
                    "signal::notify::height", G_CALLBACK (window_resize_requested), embed,
                    NULL);

}


static void
libre_impuesto_embed_finalize (GObject *object)
{
  LibreImpuestoEmbed *embed = LIBRE_IMPUESTO_EMBED (object);
  LibreImpuestoEmbedPrivate *priv = embed->priv;
  GSList *list;
  gchar * form;
  GtkTreePath * path;

  list = priv->destroy_on_transition_list;
  for (; list; list = list->next) {
    GtkWidget *widget = GTK_WIDGET (list->data);
    g_signal_handlers_disconnect_by_func (widget, remove_from_destroy_list_cb, embed);
  }
  g_slist_free (priv->destroy_on_transition_list);

  for (list = priv->messages; list; list = list->next) {
    LibreImpuestoEmbedStatusbarMsg *msg;

    msg = list->data;
    g_free (msg->text);
    g_slice_free (LibreImpuestoEmbedStatusbarMsg, msg);
  }

  g_slist_free (priv->messages);
  priv->messages = NULL;

  for (list = priv->keys; list; list = list->next)
    g_free (list->data);

  g_slist_free (priv->keys);
  priv->keys = NULL;
  
  form = g_object_get_data (object, "form");
  if (form != NULL) {
    g_free (form);
    g_object_set_data (object, "form", NULL);
  }

  path = g_object_get_data (object, "path");
  if (path != NULL) {
    gtk_tree_path_free (path);
    g_object_set_data (object, "path", NULL);
  }

  G_OBJECT_CLASS (libre_impuesto_embed_parent_class)->finalize (object);
}


static void
libre_impuesto_embed_dispose (GObject *object)
{
  LibreImpuestoEmbed *embed = LIBRE_IMPUESTO_EMBED (object);
  LibreImpuestoEmbedPrivate *priv = embed->priv;

  if (priv->clear_statusbar_source_id) {
    g_source_remove (priv->clear_statusbar_source_id);
    priv->clear_statusbar_source_id = 0;
  }

  if (priv->clear_progress_source_id) {
    g_source_remove (priv->clear_progress_source_id);
    priv->clear_progress_source_id = 0;
  }

  if (priv->status_handler_id) {
    g_signal_handler_disconnect (priv->web_view, priv->status_handler_id);
    priv->status_handler_id = 0;
  }

  if (priv->progress_update_handler_id) {
    g_signal_handler_disconnect (priv->web_view, priv->progress_update_handler_id);
    priv->progress_update_handler_id = 0;
  }

  G_OBJECT_CLASS (libre_impuesto_embed_parent_class)->dispose (object);
}


static void
libre_impuesto_embed_init (LibreImpuestoEmbed *embed)
{
  embed->priv = LIBRE_IMPUESTO_EMBED_GET_PRIVATE (embed);

  gtk_orientable_set_orientation (GTK_ORIENTABLE (embed),
                                  GTK_ORIENTATION_VERTICAL);

  embed->priv->scrolled_window = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new (NULL, NULL));
  embed->priv->paned = GTK_PANED (gtk_paned_new (GTK_ORIENTATION_VERTICAL));
  embed->priv->top_widgets_vbox = GTK_BOX (gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));
  embed->priv->seq_context_id = 1;
  embed->priv->seq_message_id = 1;
  embed->priv->tab_message_id = libre_impuesto_embed_statusbar_get_context_id (embed, LIBRE_IMPUESTO_EMBED_STATUSBAR_TAB_MESSAGE_CONTEXT_DESCRIPTION);

  gtk_scrolled_window_set_policy (embed->priv->scrolled_window,
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
}


static void
libre_impuesto_embed_class_init (LibreImpuestoEmbedClass *klass)
{
  GObjectClass *object_class = (GObjectClass *)klass;

  object_class->constructed = libre_impuesto_embed_constructed;
  object_class->finalize = libre_impuesto_embed_finalize;
  object_class->dispose = libre_impuesto_embed_dispose;

  g_type_class_add_private (G_OBJECT_CLASS (klass), sizeof(LibreImpuestoEmbedPrivate));
}


WebKitWebView*
libre_impuesto_embed_get_web_view (LibreImpuestoEmbed *embed)
{
  g_return_val_if_fail (LIBRE_IMPUESTO_IS_EMBED (embed), NULL);

  return embed->priv->web_view;
}
