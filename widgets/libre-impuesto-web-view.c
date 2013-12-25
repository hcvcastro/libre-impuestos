/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=2 sts=2 et: */
/*
 *  Copyright © 2008, 2009 Gustavo Noronha Silva
 *  Copyright © 2009, 2010 Igalia S.L.
 *
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
#include "libre-impuesto-web-view.h"
#include "libre-impuesto-embed.h"
#include "libre-impuesto-embed-type-builtins.h"

/*#include <gio/gio.h>*/
#include <glib/gi18n.h>
/*#include <glib/gstdio.h>*/
#include <gnome-keyring.h>
/*#include <gtk/gtk.h>
#include <libsoup/soup-gnome.h>
#include <string.h>
#include <webkit/webkit.h>*/
#include "util/profile.h"
#include "util/libre-impuesto-string.h"
#include "util/file-helpers.h"


#define MAX_HIDDEN_POPUPS       5
#define MAX_TITLE_LENGTH        512 /* characters */
#define RELOAD_DELAY            250 /* ms */
#define RELOAD_DELAY_MAX_TICKS  40  /* RELOAD_DELAY * RELOAD_DELAY_MAX_TICKS = 10 s */
#define EMPTY_PAGE              _("Blank page") /* Title for the empty page */
#define LIBRE_IMPUESTO_ABOUT_SCHEME "libre-impuesto-about"
#define FAVICON_SIZE 16
#define LIBRE_IMPUESTO_ABOUT_SCHEME_LEN 10
#define LIBRE_IMPUESTO_WEBKIT_BACK_FORWARD_LIMIT 100

#define LIBRE_IMPUESTO_WEB_VIEW_NON_SEARCH_REGEX  "(" \
                                        "^localhost(\\.[^[:space:]]+)?(:\\d+)?(/.*)?$|" \
                                        "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]$|" \
                                        "^::[0-9a-f:]*$|" \
                                        "^[0-9a-f:]+:[0-9a-f:]*$|" \
                                        "^[^\\.[:space:]]+\\.[^\\.[:space:]]+.*$|" \
                                        "^https?://[^/\\.[:space:]]+.*$|" \
                                        "^about:.*$|" \
                                        "^data:.*$|" \
                                        "^file:.*$" \
                                        ")"


struct _LibreImpuestoWebViewPrivate {
  LibreImpuestoWebViewSecurityLevel security_level;
  LibreImpuestoWebViewDocumentType document_type;
  LibreImpuestoWebViewNavigationFlags nav_flags;
  WebKitLoadStatus load_status;

  /* Flags */
  guint is_blank : 1;
  guint visibility : 1;
  guint loading_homepage : 1;
  guint is_setting_zoom : 1;

  char *address;
  char *typed_address;
  char *title;
  char *loading_title;
  char *status_message;
  char *link_message;
  GdkPixbuf *icon;
  gboolean expire_address_now;

  /* File watch */
  GFileMonitor *monitor;
  gboolean monitor_directory;
  guint reload_scheduled_id;
  guint reload_delay_ticks;

  /* Regex to figure out if we're dealing with a wanna-be URI */
  GRegex *non_search_regex;

  GSList *hidden_popups;
  GSList *shown_popups;

  GtkWidget *password_info_bar;

  GCancellable *history_service_cancellable;

};

typedef struct {
  char *url;
  char *name;
  char *features;
} PopupInfo;

enum {
  PROP_0,
  PROP_ADDRESS,
  PROP_DOCUMENT_TYPE,
  PROP_HIDDEN_POPUP_COUNT,
  PROP_ICON,
  PROP_LINK_MESSAGE,
  PROP_NAVIGATION,
  PROP_POPUPS_ALLOWED,
  PROP_SECURITY,
  PROP_STATUS_MESSAGE,
  PROP_EMBED_TITLE,
  PROP_TYPED_ADDRESS,
  PROP_VISIBLE,
  PROP_IS_BLANK,
};

#define LIBRE_IMPUESTO_WEB_VIEW_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ((object), LIBRE_IMPUESTO_TYPE_WEB_VIEW, LibreImpuestoWebViewPrivate))

#define LIBRE_IMPUESTO_GET_EMBED_FROM_LIBRE_IMPUESTO_WEB_VIEW(view) (LIBRE_IMPUESTO_EMBED (gtk_widget_get_parent (gtk_widget_get_parent (gtk_widget_get_parent (gtk_widget_get_parent ((GTK_WIDGET (view))))))))


G_DEFINE_TYPE (LibreImpuestoWebView, libre_impuesto_web_view, WEBKIT_TYPE_WEB_VIEW)

static guint
popup_blocker_n_hidden (LibreImpuestoWebView *view)
{
  return g_slist_length (view->priv->hidden_popups);
}

static void
popups_manager_free_info (PopupInfo *popup)
{
  g_free (popup->url);
  g_free (popup->name);
  g_free (popup->features);
  g_slice_free (PopupInfo, popup);
}

static void
popups_manager_add (LibreImpuestoWebView *view,
                    const char *url,
                    const char *name,
                    const char *features)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  PopupInfo *popup;

  popup = g_slice_new (PopupInfo);

  popup->url = g_strdup (url);
  popup->name = g_strdup (name);
  popup->features = g_strdup (features);

  priv->hidden_popups = g_slist_prepend (priv->hidden_popups, popup);

  if (popup_blocker_n_hidden (view) > MAX_HIDDEN_POPUPS) {/* bug #160863 */
    /* Remove the oldest popup */
    GSList *l = view->priv->hidden_popups;

    while (l->next->next != NULL) {
      l = l->next;
    }

    popup = (PopupInfo *)l->next->data;
    popups_manager_free_info (popup);

    l->next = NULL;
  } else {
    g_object_notify (G_OBJECT (view), "hidden-popup-count");
  }
}

static void
libre_impuesto_web_view_set_popups_allowed (LibreImpuestoWebView *view,
                                            gboolean allowed)
{
}

static gboolean
libre_impuesto_web_view_get_popups_allowed (LibreImpuestoWebView *view)
{
  return FALSE;
}


void
libre_impuesto_web_view_popups_manager_reset (LibreImpuestoWebView *view)
{
  g_slist_foreach (view->priv->hidden_popups,
                   (GFunc)popups_manager_free_info, NULL);
  g_slist_free (view->priv->hidden_popups);
  view->priv->hidden_popups = NULL;

  /*g_slist_foreach (view->priv->shown_popups,
    (GFunc)disconnect_popup, view);*/
  g_slist_free (view->priv->shown_popups);
  view->priv->shown_popups = NULL;

  g_object_notify (G_OBJECT (view), "hidden-popup-count");
  g_object_notify (G_OBJECT (view), "popups-allowed");
}



static void
libre_impuesto_web_view_file_monitor_cancel (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->monitor != NULL) {

    g_file_monitor_cancel (G_FILE_MONITOR (priv->monitor));
    priv->monitor = NULL;
  }

  if (priv->reload_scheduled_id != 0) {

    g_source_remove (priv->reload_scheduled_id);
    priv->reload_scheduled_id = 0;
  }

  priv->reload_delay_ticks = 0;
}




typedef struct {
  WebKitDOMNode *username_node;
  WebKitDOMNode *password_node;
} FillData;

/*static void
fill_data_free (gpointer data)
{
  FillData *fill_data = (FillData*)data;

  g_object_unref (fill_data->username_node);
  g_object_unref (fill_data->password_node);

  g_slice_free (FillData, fill_data);
  }*/

 /*static void
fill_form_cb (GnomeKeyringResult retval,
              GList *results,
              gpointer user_data)
{
  FillData *fill_data = (FillData*)user_data;
  GnomeKeyringNetworkPasswordData* keyring_data;

  if (!results) {
    return;
  }

  // FIXME: We use only the first result, for now; We need to do
  // * something smarter here 
  keyring_data = (GnomeKeyringNetworkPasswordData*)results->data;

  if (retval != GNOME_KEYRING_RESULT_OK) {
    return;
  }

  g_object_set (fill_data->username_node,
                "value", keyring_data->user, NULL);
  g_object_set (fill_data->password_node,
                "value", keyring_data->password, NULL);
                }*/

  /*static void
find_username_and_password_elements (WebKitDOMNode *form_node,
                                     WebKitDOMNode **username_node,
                                     WebKitDOMNode **password_node)
{
  WebKitDOMHTMLCollection *elements;
  WebKitDOMHTMLFormElement *form = WEBKIT_DOM_HTML_FORM_ELEMENT (form_node);
  gulong elements_n;
  int j;

  elements = webkit_dom_html_form_element_get_elements (form);
  elements_n = webkit_dom_html_collection_get_length (elements);

  if (elements_n == 0) {
    return;
  }

  for (j = 0; j < elements_n; j++) {
    WebKitDOMNode *element;

    element = webkit_dom_html_collection_item (elements, j);

    if (WEBKIT_DOM_IS_HTML_INPUT_ELEMENT (element)) {
      char *element_type;

      g_object_get (element, "type", &element_type, NULL);

      if (g_str_equal ("text", element_type) || g_str_equal ("email", element_type)) {
        // We found more than one inputs of type text; we won't be
        // saving here 
        if (*username_node) {
          g_object_unref (*username_node);
          *username_node = NULL;
          g_free (element_type);

          break;
        }

        *username_node = g_object_ref (element);
      }
      else if (g_str_equal ("password", element_type)) {
        if (*password_node) {
          g_object_unref (*password_node);
          *password_node = NULL;
          g_free (element_type);

          break;
        }

        *password_node = g_object_ref (element);
      }

      g_free (element_type);
    }
  }

  g_object_unref(elements);
}*/

typedef struct {
  LibreImpuestoEmbed *embed;
  char *uri;
  char *name_field;
  char *password_field;
  char *name_value;
  char *password_value;
} StorePasswordData;

/*static void
store_password_data_free (gpointer data)
{
  StorePasswordData *store_data = (StorePasswordData*)data;

  g_free (store_data->uri);
  g_free (store_data->name_field);
  g_free (store_data->name_value);
  g_free (store_data->password_field);
  g_free (store_data->password_value);

  g_slice_free (StorePasswordData, store_data);
  }*/


static void
_libre_impuesto_web_view_hook_into_links (LibreImpuestoWebView *web_view)
{
  WebKitDOMNodeList *links = NULL;
  WebKitDOMDocument *document = NULL;
  gulong links_n;
  int i;

  document = webkit_web_view_get_dom_document (WEBKIT_WEB_VIEW (web_view));
  links = webkit_dom_document_get_elements_by_tag_name (document, "link");
  links_n = webkit_dom_node_list_get_length (links);

  if (links_n == 0) {
    g_object_unref(links);
    return;
  }

  for (i = 0; i < links_n; i++) {
    WebKitDOMNode *link;
    char *rel = NULL;

    link = webkit_dom_node_list_item (links, i);
    rel = webkit_dom_html_link_element_get_rel (WEBKIT_DOM_HTML_LINK_ELEMENT (link));

    if (g_strcmp0 (rel, "alternate") == 0) {
      char *type = NULL;
      char *title = NULL;
      char *address = NULL;
      SoupURI *feed_uri;
      SoupURI *current_uri;

      g_object_get (link,
                    "type", &type,
                    "title", &title,
                    "href", &address,
                    NULL);

      feed_uri = soup_uri_new (address);
      if (!feed_uri) {
        current_uri = soup_uri_new (libre_impuesto_web_view_get_address (web_view));
        feed_uri = soup_uri_new_with_base (current_uri, address);
        soup_uri_free (current_uri);
      }

      if (feed_uri) {
        g_free (address);
        address = soup_uri_to_string (feed_uri, FALSE);
        g_signal_emit_by_name (web_view, "ge-feed-link", type, title, address);
        soup_uri_free (feed_uri);
      }

      g_free (type);
      g_free (title);
      g_free (address);
    }

    g_free (rel);
  }

  g_object_unref(links);
}

static void
update_navigation_flags (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  guint flags = 0;
  WebKitWebView *web_view = WEBKIT_WEB_VIEW (view);

  if (webkit_web_view_can_go_back (web_view))
    flags |= LIBRE_IMPUESTO_WEB_VIEW_NAV_BACK;

  if (webkit_web_view_can_go_forward (web_view))
    flags |= LIBRE_IMPUESTO_WEB_VIEW_NAV_FORWARD;

  if (priv->nav_flags != (LibreImpuestoWebViewNavigationFlags)flags) {
    priv->nav_flags = (LibreImpuestoWebViewNavigationFlags)flags;

    g_object_notify (G_OBJECT (view), "navigation");
  }
}

/*static void
libre_impuesto_web_view_clear_history (LibreImpuestoWebView *view)
{
  WebKitWebBackForwardList *history_list;

  g_return_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view));

  history_list = webkit_web_view_get_back_forward_list (WEBKIT_WEB_VIEW (view));
  if (history_list != NULL) {
    WebKitWebHistoryItem *current_item;

    // Save a ref to the first element to add it later 
    current_item = webkit_web_back_forward_list_get_current_item (history_list);
    g_object_ref (current_item);

    // Clear the history and add the first element once again 
    webkit_web_back_forward_list_clear (history_list);
    webkit_web_back_forward_list_add_item (history_list, current_item);
    g_object_unref (current_item);

    update_navigation_flags (view);
  }
  }*/



static char*
get_title_from_address (const char *address)
{
  if (g_str_has_prefix (address, "file://"))
    return g_strdup (address + 7);
  else if (!strcmp (address, LIBRE_IMPUESTO_ABOUT_SCHEME":plugins"))
    return g_strdup (_("Plugins"));
  else
    return libre_impuesto_string_get_host_name (address);
}

static void
title_changed_cb (WebKitWebView *web_view,
                  GParamSpec *spec,
                  gpointer data)
{
  const char *uri;
  char *title;
  WebKitWebFrame *frame;

  frame = webkit_web_view_get_main_frame (web_view);
  uri = webkit_web_frame_get_uri (frame);

  g_object_get (web_view, "title", &title, NULL);

  libre_impuesto_web_view_set_title (LIBRE_IMPUESTO_WEB_VIEW (web_view), title);
  
  if (!title && uri)
    title = get_title_from_address (uri);

  g_free (title);

}

static void
uri_changed_cb (WebKitWebView *web_view,
                GParamSpec *spec,
                gpointer data)
{
  char *uri;
  const char *current_address;

  g_object_get (web_view, "uri", &uri, NULL);
  current_address = libre_impuesto_web_view_get_address (LIBRE_IMPUESTO_WEB_VIEW (web_view));

  /* We need to check if we get URI notifications without going
     through the usual load process, as this can happen when changing
     location within a page */
  if (g_str_equal (uri, current_address) == FALSE)
    libre_impuesto_web_view_set_address (LIBRE_IMPUESTO_WEB_VIEW (web_view), uri);

  g_free (uri);
}

static void
hovering_over_link_cb (LibreImpuestoWebView *web_view,
                       char *title,
                       char *location,
                       gpointer data)
{
  libre_impuesto_web_view_set_link_message (web_view, location);
}



static void
_libre_impuesto_web_view_load_icon (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  const char* uri;

  if (priv->icon != NULL)
    return;

  uri = webkit_web_view_get_uri (WEBKIT_WEB_VIEW (view));
  priv->icon = webkit_favicon_database_try_get_favicon_pixbuf (webkit_get_favicon_database (), uri,
                                                               FAVICON_SIZE, FAVICON_SIZE);

  g_object_notify (G_OBJECT (view), "icon");
}

static void
_libre_impuesto_web_view_set_icon_address (LibreImpuestoWebView *view,
                                 const char *icon_address)
{
  GObject *object = G_OBJECT (view);
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->icon != NULL) {
    g_object_unref (priv->icon);
    priv->icon = NULL;

    g_object_notify (object, "icon");
  }

  if (icon_address && priv->address)
    _libre_impuesto_web_view_load_icon (view);
}

static void
favicon_cb (LibreImpuestoWebView *view,
            const char *address,
            gpointer user_data)
{
  _libre_impuesto_web_view_set_icon_address (view, address);
}

static void
new_window_cb (LibreImpuestoWebView *view,
               LibreImpuestoWebView *new_view,
               gpointer user_data)
{

}

static void
ge_popup_blocked_cb (LibreImpuestoWebView *view,
                     const char *url,
                     const char *name,
                     const char *features,
                     gpointer user_data)
{
  popups_manager_add (view, url, name, features);
}

static gboolean
mime_type_policy_decision_requested_cb (WebKitWebView *web_view,
                                        WebKitWebFrame *frame,
                                        WebKitNetworkRequest *request,
                                        const char *mime_type,
                                        WebKitWebPolicyDecision *decision,
                                        gpointer user_data)
{
  return FALSE;
}

/*static void
decide_on_geolocation_policy_request (GtkWidget *info_bar,
                                      int response,
                                      WebKitGeolocationPolicyDecision *decision)
{
  gtk_widget_destroy (info_bar);

  // Decide, and drop our ref on the decision object. 
  if (response == GTK_RESPONSE_YES) {
    webkit_geolocation_policy_allow (decision);
    g_object_unref (decision);
    return;
  }

  webkit_geolocation_policy_deny (decision);
  g_object_unref (decision);
}*/

static gboolean
geolocation_policy_decision_requested_cb (WebKitWebView *web_view,
                                          WebKitWebFrame *web_frame,
                                          WebKitGeolocationPolicyDecision *decision,
                                          gpointer data)
{
  return TRUE;
}

static gboolean
delete_web_app_cb (WebKitDOMHTMLElement *button,
                   WebKitDOMEvent *dom_event,
                   LibreImpuestoWebView *web_view)
{
  /*char *id = NULL;

  id = webkit_dom_html_element_get_id (button);
  if (id)
    libre_impuesto_web_application_delete (id);

    g_free (id);*/

  return FALSE;
}


static void
restore_zoom_level (LibreImpuestoWebView *view,
                    const char *address)
{
}

static void
load_status_cb (WebKitWebView *web_view,
                GParamSpec *pspec,
                gpointer user_data)
{
  WebKitLoadStatus status = webkit_web_view_get_load_status (web_view);
  LibreImpuestoWebView *view = LIBRE_IMPUESTO_WEB_VIEW (web_view);
  LibreImpuestoWebViewPrivate *priv = view->priv;
  GObject *object = G_OBJECT (web_view);

  g_object_freeze_notify (object);

  switch (status) {
  /* FIXME: add REDIRECTING and NEGOTIATING states to WebKitGTK */
  case WEBKIT_LOAD_PROVISIONAL: {
    const gchar *loading_uri = NULL;
    WebKitWebFrame *frame;

    WebKitWebDataSource *source;
    WebKitNetworkRequest *request;

    frame = webkit_web_view_get_main_frame (web_view);

    source = webkit_web_frame_get_provisional_data_source (frame);
    request = webkit_web_data_source_get_initial_request (source);
    loading_uri = webkit_network_request_get_uri (request);

    g_signal_emit_by_name (view, "new-document-now", loading_uri);

    if ((priv->address == NULL || priv->address[0] == '\0') &&
        priv->expire_address_now == TRUE) {
      libre_impuesto_web_view_set_address (view, loading_uri);
      libre_impuesto_web_view_set_title (view, NULL);
    }

      libre_impuesto_web_view_set_loading_title (view, loading_uri, TRUE);

      g_free (priv->status_message);
      priv->status_message = g_strdup (priv->loading_title);
      g_object_notify (object, "status-message");

      priv->expire_address_now = TRUE;
      break;
  }
  case WEBKIT_LOAD_COMMITTED: {
    const gchar* uri;
    LibreImpuestoWebViewSecurityLevel security_level;

    /* Title and location. */
    uri = webkit_web_view_get_uri (web_view);
    libre_impuesto_web_view_location_changed (view,
                                    uri);

    libre_impuesto_web_view_set_title (view, NULL);

    /* Security status. */
    if (uri && g_str_has_prefix (uri, "https")) {
      WebKitWebFrame *frame;
      WebKitWebDataSource *source;
      WebKitNetworkRequest *request;
      SoupMessage *message;

      frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(view));
      source = webkit_web_frame_get_data_source (frame);
      request = webkit_web_data_source_get_request (source);
      message = webkit_network_request_get_message (request);

      if (message &&
          (soup_message_get_flags (message) & SOUP_MESSAGE_CERTIFICATE_TRUSTED))
        security_level = LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_HIGH;
      else
        security_level = LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_BROKEN;
    } else
      security_level = LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_UNKNOWN;

    libre_impuesto_web_view_set_security_level (LIBRE_IMPUESTO_WEB_VIEW (web_view), security_level);

    /* Zoom level. */
    restore_zoom_level (view, uri);

    /* History. */
    if (!libre_impuesto_web_view_is_loading_homepage (view)) {
      char *history_uri = NULL;

      /* TODO: move the normalization down to the history service? */
      if (g_str_has_prefix (uri, LIBRE_IMPUESTO_ABOUT_SCHEME))
          history_uri = g_strdup_printf ("about:%s", uri + LIBRE_IMPUESTO_ABOUT_SCHEME_LEN + 1);
      else
        history_uri = g_strdup (uri);

        g_free (history_uri);
    }
    break;
  }
  case WEBKIT_LOAD_FINISHED: {
    SoupURI *uri;

    priv->loading_homepage = FALSE;

    g_free (priv->status_message);
    priv->status_message = NULL;
    g_object_notify (object, "status-message");
    libre_impuesto_web_view_set_loading_title (view, NULL, FALSE);

    if (priv->is_blank)
      g_object_notify (object, "embed-title");

    _libre_impuesto_web_view_hook_into_links (view);

    /* FIXME: It sucks to do this here, but it's not really possible
     * to hook the DOM actions nicely in the about: generator. */
    uri = soup_uri_new (webkit_web_view_get_uri (web_view));
    if (uri &&
        !g_strcmp0 (uri->scheme, "libre-impuesto-about") &&
        !g_strcmp0 (uri->path, "applications")) {
      WebKitDOMDocument *document;
      WebKitDOMNodeList *buttons;
      gulong buttons_n;
      int i;

      document = webkit_web_view_get_dom_document (web_view);
      buttons = webkit_dom_document_get_elements_by_tag_name (document, "input");
      buttons_n = webkit_dom_node_list_get_length (buttons);

      for (i = 0; i < buttons_n; i++) {
        WebKitDOMNode *button;

        button = webkit_dom_node_list_item (buttons, i);
        webkit_dom_event_target_add_event_listener (WEBKIT_DOM_EVENT_TARGET (button), "click",
                                                    G_CALLBACK (delete_web_app_cb), false,
                                                    NULL);
      }
    }

    if (uri)
      soup_uri_free (uri);

    break;
  }
  case WEBKIT_LOAD_FAILED:
    libre_impuesto_web_view_set_link_message (view, NULL);
    libre_impuesto_web_view_set_loading_title (view, NULL, FALSE);

    g_free (priv->status_message);
    priv->status_message = NULL;
    g_object_notify (object, "status-message");

    update_navigation_flags (view);

    break;
  default:
    break;
  }

  g_object_thaw_notify (object);
}

static char *
get_file_content_as_base64 (const char *path)
{
  GFile *file;
  GFileInfo *file_info;
  const char *image_type;
  char *image_raw;
  gsize len;
  char *image_data;
  char *image64;

  file = g_file_new_for_path (path);
  file_info = g_file_query_info (file,
                                 G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                                 G_FILE_QUERY_INFO_NONE,
                                 NULL, NULL);
  image_type = g_file_info_get_content_type (file_info);

  g_object_unref (file);
  g_object_unref (file_info);

  g_file_get_contents (path, &image_raw, &len, NULL);
  image_data = g_base64_encode ((guchar *) image_raw, len);
  image64 = g_strdup_printf ("data:%s;base64,%s", image_type, image_data);

  g_free (image_raw);
  g_free (image_data);

  return image64;
}


void
libre_impuesto_web_view_load_error_page (LibreImpuestoWebView *view,
                               const char *uri,
                               LibreImpuestoWebViewErrorPage page,
                               GError *error)
{
  GString *html = g_string_new ("");
  const char *reason;

  char *hostname;
  char *lang;

  char *page_title;
  char *msg_title;
  char *msg;
  char *button_label;
  const char *html_file;
  const char *stock_icon;

  GtkIconInfo *icon_info;
  char *image_data;

  char *template;

  if (error)
    reason = error->message;
  else
    reason = _("None specified");

  hostname = libre_impuesto_string_get_host_name (uri);

  lang = g_strdup (pango_language_to_string (gtk_get_default_language ()));
  g_strdelimit (lang, "_-@", '\0');

  switch (page) {
    case LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR:
      page_title = g_strdup_printf (_("Oops! Error loading %s"), hostname);

      msg_title = g_strdup (_("Oops! It was not possible to show this website"));
      msg = g_strdup_printf (_("<p>The website at <strong>%s</strong> seems "
                               "to be unavailable. The precise error was:</p>"
                               "<p><em>%s</em></p>"
                               "<p>It could be "
                               "temporarily switched off or moved to a new "
                               "address. Don't forget to check that your "
                               "internet connection is working correctly.</p>"),
                             uri, reason);

      button_label = g_strdup (_("Try again"));

      html_file = libre_impuesto_file ("error.html");
      stock_icon = "dialog-error";
      break;
    case LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_CRASH:
      page_title = g_strdup_printf (_("Oops! Error loading %s"), hostname);

      msg_title = g_strdup (_("Oops! This site might have caused the web "
                              "browser to close unexpectedly"));
      msg = g_strdup_printf (_("<p>This page was loading when the web browser "
                               "closed unexpectedly.</p>"
                               "<p>This might happen again if you "
                               "reload the page. If it does, "
                               "please report the problem to the "
                               "<strong>%s</strong> developers.</p>"),
                             "libre-impuestos");

      button_label = g_strdup (_("Load again anyway"));

      html_file = libre_impuesto_file ("recovery.html");
      stock_icon = "dialog-information";
      break;
    default:
      return;
      break;
  }
  g_free (hostname);

  icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (),
                                          stock_icon,
                                          48,
                                          GTK_ICON_LOOKUP_GENERIC_FALLBACK);

  image_data = icon_info ? get_file_content_as_base64 (gtk_icon_info_get_filename (icon_info)) : NULL;

  g_file_get_contents (html_file, &template, NULL, NULL);

  libre_impuesto_web_view_set_title (view, page_title);
  _libre_impuesto_web_view_set_icon_address (view, NULL);

  g_string_printf (html, template,
                   lang, lang,
                   ((gtk_widget_get_default_direction () == GTK_TEXT_DIR_RTL) ? "rtl" : "ltr"),
                   page_title,
                   uri,
                   image_data ? image_data : "",
                   msg_title, msg, button_label);

  g_free (template);
  g_free (lang);
  g_free (page_title);
  g_free (msg_title);
  g_free (msg);
  g_free (button_label);
  g_free (image_data);

  webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                               html->str, "text/html", "utf8", uri);
  g_string_free (html, TRUE);
}

static gboolean
load_error_cb (WebKitWebView *web_view,
               WebKitWebFrame *frame,
               char *uri,
               GError *error,
               gpointer user_data)
{
  LibreImpuestoWebView *view = LIBRE_IMPUESTO_WEB_VIEW (web_view);

  if (webkit_web_view_get_main_frame (web_view) != frame)
    return FALSE;

  if (error->domain == SOUP_HTTP_ERROR) {
    libre_impuesto_web_view_load_error_page (view, uri, LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR, error);
    return TRUE;
  }

  g_return_val_if_fail ((error->domain == WEBKIT_NETWORK_ERROR) ||
                        (error->domain == WEBKIT_POLICY_ERROR) ||
                        (error->domain == WEBKIT_PLUGIN_ERROR), FALSE);

  switch (error->code) {
  case WEBKIT_NETWORK_ERROR_FAILED:
  case WEBKIT_NETWORK_ERROR_TRANSPORT:
  case WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL:
  case WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST:
  case WEBKIT_POLICY_ERROR_FAILED:
  case WEBKIT_POLICY_ERROR_CANNOT_SHOW_MIME_TYPE:
  case WEBKIT_POLICY_ERROR_CANNOT_SHOW_URL:
  case WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT:
  case WEBKIT_PLUGIN_ERROR_FAILED:
  case WEBKIT_PLUGIN_ERROR_CANNOT_FIND_PLUGIN:
  case WEBKIT_PLUGIN_ERROR_CANNOT_LOAD_PLUGIN:
  case WEBKIT_PLUGIN_ERROR_JAVA_UNAVAILABLE:
  case WEBKIT_PLUGIN_ERROR_CONNECTION_CANCELLED:
    libre_impuesto_web_view_load_error_page (view, uri, LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR, error);
    return TRUE;
  case WEBKIT_NETWORK_ERROR_CANCELLED:
    {
      LibreImpuestoWebViewPrivate *priv = view->priv;

      if (priv->expire_address_now) {
        const char* prev_uri;

        prev_uri = webkit_web_view_get_uri (web_view);

        libre_impuesto_web_view_set_typed_address (view, NULL);
        libre_impuesto_web_view_set_address (view, prev_uri);
      }
    }
    break;
  /* In case we are downloading something or the resource is going to
   * be showed with a plugin just let WebKit do it */
  case WEBKIT_PLUGIN_ERROR_WILL_HANDLE_LOAD:
  case WEBKIT_POLICY_ERROR_FRAME_LOAD_INTERRUPTED_BY_POLICY_CHANGE:
  default:
    break;
  }

  return FALSE;
}

static gboolean
close_web_view_cb (WebKitWebView *web_view,
                   gpointer user_data)
{
  return TRUE;
}

static void
zoom_changed_cb (WebKitWebView *web_view,
                 GParamSpec *pspec,
                 gpointer user_data)
{

}



GtkWidget *
libre_impuesto_web_view_new (void)
{
  return GTK_WIDGET (g_object_new (LIBRE_IMPUESTO_TYPE_WEB_VIEW, NULL));
}

static char*
normalize_or_autosearch_url (LibreImpuestoWebView *view, const char *url)
{
  return NULL;
}

void
libre_impuesto_web_view_load_request (LibreImpuestoWebView *view,
                            WebKitNetworkRequest *request)
{
  WebKitWebFrame *main_frame;
  const char *url;
  char *effective_url;

  g_return_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW(view));
  g_return_if_fail (WEBKIT_IS_NETWORK_REQUEST(request));

  url = webkit_network_request_get_uri (request);
  effective_url = normalize_or_autosearch_url (view, url);
  webkit_network_request_set_uri (request, effective_url);
  g_free (effective_url);

  main_frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(view));
  webkit_web_frame_load_request(main_frame, request);
}

typedef struct {
  LibreImpuestoWebView *view;
  char *original_uri;
} HEADAttemptData;

/*static void
effective_url_head_cb (SoupSession *session,
                       SoupMessage *message,
                       gpointer user_data)
{
  HEADAttemptData *data = (HEADAttemptData*)user_data;

  LibreImpuestoWebView *view = data->view;

  if (message->status_code == SOUP_STATUS_OK) {
    char *uri = soup_uri_to_string (soup_message_get_uri (message), FALSE);

    webkit_web_view_open (WEBKIT_WEB_VIEW (view), uri);

    g_free (uri);
  } else {
    GError *error = NULL;
    GdkScreen *screen;

    screen = gtk_widget_get_screen (GTK_WIDGET (view));
    gtk_show_uri (screen, data->original_uri, GDK_CURRENT_TIME, &error);

    if (error) {
      g_error_free (error);

      // Load the original URI to trigger an error in the view. 
      webkit_web_view_open (WEBKIT_WEB_VIEW (view), data->original_uri);
    }
  }

  g_free (data->original_uri);
  g_slice_free (HEADAttemptData, data);
}*/

void
libre_impuesto_web_view_load_url (LibreImpuestoWebView *view,
                        const char *url)
{
  /*char *effective_url;

  g_return_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view));
  g_return_if_fail (url);

  effective_url = normalize_or_autosearch_url (view, url);

  if (!libre_impuesto_embed_utils_address_has_web_scheme (effective_url)) {
    SoupMessage *message;
    SoupSession *session;
    char *temp_url;
    HEADAttemptData *data;

    temp_url = g_strconcat ("http://", effective_url, NULL);

    session = webkit_get_default_session ();
    message = soup_message_new (SOUP_METHOD_HEAD,
                                temp_url);

    if (message) {
      data = g_slice_new (HEADAttemptData);
      data->view = view;
      data->original_uri = g_strdup (effective_url);
      soup_session_queue_message (session, message,
                                  effective_url_head_cb, data);
    } else {
      webkit_web_view_open (WEBKIT_WEB_VIEW (view), effective_url);
    }

    g_free (temp_url);
  } else if (g_str_has_prefix (effective_url, "javascript:")) {
    char *decoded_url;

    decoded_url = soup_uri_decode (effective_url);
    webkit_web_view_execute_script (WEBKIT_WEB_VIEW (view), decoded_url);
    g_free (decoded_url);
  } else
    webkit_web_view_open (WEBKIT_WEB_VIEW (view), effective_url);

    g_free (effective_url);*/
}

void
libre_impuesto_web_view_copy_back_history (LibreImpuestoWebView *source,
                                 LibreImpuestoWebView *dest)
{
  WebKitWebView *source_view, *dest_view;
  WebKitWebBackForwardList* source_bflist, *dest_bflist;
  WebKitWebHistoryItem *item;
  GList *items;

  g_return_if_fail(LIBRE_IMPUESTO_IS_WEB_VIEW(source));
  g_return_if_fail(LIBRE_IMPUESTO_IS_WEB_VIEW(dest));

  source_view = WEBKIT_WEB_VIEW (source);
  dest_view = WEBKIT_WEB_VIEW (dest);

  source_bflist = webkit_web_view_get_back_forward_list (source_view);
  dest_bflist = webkit_web_view_get_back_forward_list (dest_view);

  items = webkit_web_back_forward_list_get_back_list_with_limit (source_bflist, LIBRE_IMPUESTO_WEBKIT_BACK_FORWARD_LIMIT);
  /* We want to add the items in the reverse order here, so the
     history ends up the same */
  items = g_list_reverse (items);
  for (; items; items = items->next) {
    item = webkit_web_history_item_copy ((WebKitWebHistoryItem*)items->data);
    webkit_web_back_forward_list_add_item (dest_bflist, item);
    g_object_unref (item);
  }
  g_list_free (items);

  /* The gecko behavior is to add the current item of the source
     embed at the end of the back history, so keep doing that */
  item = webkit_web_back_forward_list_get_current_item (source_bflist);
  if (item)
    webkit_web_back_forward_list_add_item (dest_bflist, item);
}

static void
_libre_impuesto_web_view_set_is_blank (LibreImpuestoWebView *view,
                             gboolean is_blank)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->is_blank != is_blank) {
    priv->is_blank = is_blank;
    g_object_notify (G_OBJECT (view), "is-blank");
  }
}

void
libre_impuesto_web_view_set_address (LibreImpuestoWebView *view,
                           const char *address)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  GObject *object = G_OBJECT (view);
  gboolean is_blank;

  g_free (priv->address);
  priv->address = g_strdup (address);

  is_blank = address == NULL ||
             strcmp (address, "about:blank") == 0;
  _libre_impuesto_web_view_set_is_blank (view, is_blank);

  if (libre_impuesto_web_view_is_loading (view) &&
      priv->expire_address_now == TRUE &&
      priv->typed_address != NULL) {
    g_free (priv->typed_address);
    priv->typed_address = NULL;

    g_object_notify (object, "typed-address");
  }

  g_object_notify (object, "address");
}

void
libre_impuesto_web_view_set_title (LibreImpuestoWebView *view,
                         const char *view_title)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  char *title = g_strdup (view_title);

  if (!priv->is_blank && (title == NULL || g_strstrip (title)[0] == '\0')) {
    g_free (title);
    title = get_title_from_address (priv->address);

    /* Fallback */
    if (title == NULL || title[0] == '\0') {
      g_free (title);
      title = g_strdup (EMPTY_PAGE);
      _libre_impuesto_web_view_set_is_blank (view, TRUE);
    }
  } else if (priv->is_blank) {
    g_free (title);
    title = g_strdup (EMPTY_PAGE);
  }

  g_free (priv->title);
  priv->title = libre_impuesto_string_shorten (title, MAX_TITLE_LENGTH);

  g_object_notify (G_OBJECT (view), "embed-title");
}

gboolean
libre_impuesto_web_view_get_is_blank (LibreImpuestoWebView *view)
{
  return view->priv->is_blank;
}

const char *
libre_impuesto_web_view_get_address (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  return priv->address ? priv->address : "about:blank";
}

const char *
libre_impuesto_web_view_get_title (LibreImpuestoWebView *view)
{
  return view->priv->title;
}

void
libre_impuesto_web_view_set_loading_title (LibreImpuestoWebView *view,
                                 const char *title,
                                 gboolean is_address)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  char *freeme = NULL;

  g_free (priv->loading_title);
  priv->loading_title = NULL;

  if (is_address) {
    title = freeme = get_title_from_address (title);
  }

  if (title != NULL && title[0] != '\0') {
    /* translators: %s here is the address of the web page */
    priv->loading_title = g_strdup_printf (_ ("Loading “%s”…"), title);
  } else {
    priv->loading_title = g_strdup (_ ("Loading…"));
  }

  g_free (freeme);
}

static gboolean
libre_impuesto_web_view_file_monitor_reload_cb (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->reload_delay_ticks > 0) {
    priv->reload_delay_ticks--;

    /* Run again */
    return TRUE;
  }

  if (libre_impuesto_web_view_is_loading (view)) {
    /* Wait a bit to reload if we're still loading! */
    priv->reload_delay_ticks = RELOAD_DELAY_MAX_TICKS / 2;

    /* Run again */
    return TRUE;
  }

  priv->reload_scheduled_id = 0;

  webkit_web_view_reload (WEBKIT_WEB_VIEW (view));

  /* don't run again */
  return FALSE;
}

static void
libre_impuesto_web_view_file_monitor_cb (GFileMonitor *monitor,
                               GFile *file,
                               GFile *other_file,
                               GFileMonitorEvent event_type,
                               LibreImpuestoWebView *view)
{
  gboolean should_reload;
  LibreImpuestoWebViewPrivate *priv = view->priv;

  switch (event_type) {
    /* These events will always trigger a reload: */
    case G_FILE_MONITOR_EVENT_CHANGED:
    case G_FILE_MONITOR_EVENT_CREATED:
      should_reload = TRUE;
      break;

    /* These events will only trigger a reload for directories: */
    case G_FILE_MONITOR_EVENT_DELETED:
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
      should_reload = priv->monitor_directory;
      break;

    /* These events don't trigger a reload: */
    case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
    case G_FILE_MONITOR_EVENT_UNMOUNTED:
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
    default:
      should_reload = FALSE;
      break;
  }

  if (should_reload) {
    /* We make a lot of assumptions here, but basically we know
     * that we just have to reload, by construction.
     * Delay the reload a little bit so we don't endlessly
     * reload while a file is written.
     */
    if (priv->reload_delay_ticks == 0) {
      priv->reload_delay_ticks = 1;
    } else {
      /* Exponential backoff */
      priv->reload_delay_ticks = MIN (priv->reload_delay_ticks * 2,
                                      RELOAD_DELAY_MAX_TICKS);
    }

    if (priv->reload_scheduled_id == 0) {
      priv->reload_scheduled_id =
        g_timeout_add (RELOAD_DELAY,
                       (GSourceFunc)libre_impuesto_web_view_file_monitor_reload_cb, view);
    }
  }
}

static void
libre_impuesto_web_view_update_file_monitor (LibreImpuestoWebView *view,
                                   const gchar *address)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;
  gboolean local;
  gchar *anchor;
  gchar *url;
  GFile *file;
  GFileType file_type;
  GFileInfo *file_info;
  GFileMonitor *monitor = NULL;

  if (priv->monitor != NULL &&
      priv->address != NULL && address != NULL &&
      strcmp (priv->address, address) == 0) {
    /* same address, no change needed */
    return;
  }

  libre_impuesto_web_view_file_monitor_cancel (view);

  local = g_str_has_prefix (address, "file://");
  if (local == FALSE) return;

  /* strip off anchors */
  anchor = strchr (address, '#');
  if (anchor != NULL) {
    url = g_strndup (address, anchor - address);
  } else {
    url = g_strdup (address);
  }

  file = g_file_new_for_uri (url);
  file_info = g_file_query_info (file,
                                 G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                 0, NULL, NULL);
  if (file_info == NULL) {
    g_object_unref (file);
    g_free (url);
    return;
  }

  file_type = g_file_info_get_file_type (file_info);
  g_object_unref (file_info);

  if (file_type == G_FILE_TYPE_DIRECTORY) {
    monitor = g_file_monitor_directory (file, 0, NULL, NULL);
    g_signal_connect (monitor, "changed",
                      G_CALLBACK (libre_impuesto_web_view_file_monitor_cb),
                      view);
    priv->monitor_directory = TRUE;
  }
  else if (file_type == G_FILE_TYPE_REGULAR) {
    monitor = g_file_monitor_file (file, 0, NULL, NULL);
    g_signal_connect (monitor, "changed",
                      G_CALLBACK (libre_impuesto_web_view_file_monitor_cb),
                      view);
    priv->monitor_directory = FALSE;
  }
  priv->monitor = monitor;
  g_object_unref (file);
  g_free (url);
}

void
libre_impuesto_web_view_location_changed (LibreImpuestoWebView *view,
                                const char *location)
{
  GObject *object = G_OBJECT (view);

  g_object_freeze_notify (object);

  /* do this up here so we still have the old address around */
  libre_impuesto_web_view_update_file_monitor (view, location);

  /* Do not expose about:blank to the user, an empty address
     bar will do better */
  if (location == NULL || location[0] == '\0' ||
      strcmp (location, "about:blank") == 0) {
    libre_impuesto_web_view_set_address (view, NULL);
    libre_impuesto_web_view_set_title (view, EMPTY_PAGE);
  } else if (g_str_has_prefix (location, LIBRE_IMPUESTO_ABOUT_SCHEME)) {
    char *new_address = g_strdup_printf ("about:%s", location + LIBRE_IMPUESTO_ABOUT_SCHEME_LEN + 1);
    libre_impuesto_web_view_set_address (view, new_address);
    g_free (new_address);
    libre_impuesto_web_view_set_title (view, EMPTY_PAGE);
  } else {
    char *view_address;

    /* we do this to get rid of an eventual password in the URL */
    view_address = libre_impuesto_web_view_get_location (view, TRUE);
    libre_impuesto_web_view_set_address (view, view_address);
    libre_impuesto_web_view_set_loading_title (view, view_address, TRUE);
    g_free (view_address);
  }

  libre_impuesto_web_view_set_link_message (view, NULL);
  _libre_impuesto_web_view_set_icon_address (view, NULL);
  update_navigation_flags (view);

  g_object_notify (object, "embed-title");

  g_object_thaw_notify (object);
}

gboolean
libre_impuesto_web_view_is_loading (LibreImpuestoWebView *view)
{
  WebKitLoadStatus status;

  status = webkit_web_view_get_load_status (WEBKIT_WEB_VIEW (view));

  /* FIRST_VISUALLY_NON_EMPTY_LAYOUT might be emitted after
   * LOAD_FINISHED or LOAD_FAILED. We just ignore any status other
   * than WEBKIT_LOAD_PROVISIONAL once LOAD_FINISHED or LOAD_FAILED
   * have been set, as WEBKIT_LOAD_PROVISIONAL probably means that
   * webview has started a new load.
   */
  if ((view->priv->load_status == WEBKIT_LOAD_FINISHED ||
       view->priv->load_status == WEBKIT_LOAD_FAILED) &&
      status != WEBKIT_LOAD_PROVISIONAL)
    return FALSE;

  view->priv->load_status = status;

  return status != WEBKIT_LOAD_FINISHED && status != WEBKIT_LOAD_FAILED;
}

const char *
libre_impuesto_web_view_get_loading_title (LibreImpuestoWebView *view)
{
  return view->priv->loading_title;
}

GdkPixbuf *
libre_impuesto_web_view_get_icon (LibreImpuestoWebView *view)
{
  return view->priv->icon;
}

LibreImpuestoWebViewDocumentType
libre_impuesto_web_view_get_document_type (LibreImpuestoWebView *view)
{
  return view->priv->document_type;
}

LibreImpuestoWebViewNavigationFlags
libre_impuesto_web_view_get_navigation_flags (LibreImpuestoWebView *view)
{
  return view->priv->nav_flags;
}

const char *
libre_impuesto_web_view_get_status_message (LibreImpuestoWebView *view)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->link_message && priv->link_message[0] != '\0') {
    return priv->link_message;
  } else if (priv->status_message) {
    return priv->status_message;
  } else {
    return NULL;
  }
}

const char *
libre_impuesto_web_view_get_link_message (LibreImpuestoWebView *view)
{
  g_return_val_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view), NULL);

  return view->priv->link_message;
}

gboolean
libre_impuesto_web_view_get_visibility (LibreImpuestoWebView *view)
{
  return view->priv->visibility;
}

void
libre_impuesto_web_view_set_link_message (LibreImpuestoWebView *view,
                                char *link_message)
{
}

void
libre_impuesto_web_view_set_security_level (LibreImpuestoWebView *view,
                                            LibreImpuestoWebViewSecurityLevel level)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->security_level != level) {
    priv->security_level = level;

    g_object_notify (G_OBJECT (view), "security-level");
  }
}

void
libre_impuesto_web_view_set_visibility (LibreImpuestoWebView *view,
                              gboolean visibility)
{
  LibreImpuestoWebViewPrivate *priv = view->priv;

  if (priv->visibility != visibility) {
    priv->visibility = visibility;

    g_object_notify (G_OBJECT (view), "visibility");
  }
}

const char *
libre_impuesto_web_view_get_typed_address (LibreImpuestoWebView *view)
{
  return view->priv->typed_address;
}

void
libre_impuesto_web_view_set_typed_address (LibreImpuestoWebView *view,
                                 const char *address)
{
  LibreImpuestoWebViewPrivate *priv = LIBRE_IMPUESTO_WEB_VIEW (view)->priv;

  g_free (priv->typed_address);
  priv->typed_address = g_strdup (address);
  /* If the page is loading prevent the typed address from going away,
     since Epiphany will try to overwrite the typed address with the
     confirmed full URL when passing through, for example, the
     COMMITTED state. */
  priv->expire_address_now = !libre_impuesto_web_view_is_loading (view);

  g_object_notify (G_OBJECT (view), "typed-address");
}

#define MIN_INPUT_LENGTH 50

gboolean
libre_impuesto_web_view_has_modified_forms (LibreImpuestoWebView *view)
{
  WebKitDOMHTMLCollection *forms = NULL;
  WebKitDOMDocument *document = NULL;
  gulong forms_n;
  int i;

  document = webkit_web_view_get_dom_document (WEBKIT_WEB_VIEW (view));
  forms = webkit_dom_document_get_forms (document);
  forms_n = webkit_dom_html_collection_get_length (forms);

  for (i = 0; i < forms_n; i++) {
    WebKitDOMHTMLCollection *elements;
    WebKitDOMNode *form_element = webkit_dom_html_collection_item (forms, i);
    gulong elements_n;
    int j;
    gboolean modified_input_element = FALSE;

    elements = webkit_dom_html_form_element_get_elements (WEBKIT_DOM_HTML_FORM_ELEMENT (form_element));
    elements_n = webkit_dom_html_collection_get_length (elements);

    for (j = 0; j < elements_n; j++) {
      WebKitDOMNode *element;

      element = webkit_dom_html_collection_item (elements, j);

      if (WEBKIT_DOM_IS_HTML_TEXT_AREA_ELEMENT (element))
        if (webkit_dom_html_text_area_element_is_edited (WEBKIT_DOM_HTML_TEXT_AREA_ELEMENT (element)))
          return TRUE;

      if (WEBKIT_DOM_IS_HTML_INPUT_ELEMENT (element))
        if (webkit_dom_html_input_element_is_edited (WEBKIT_DOM_HTML_INPUT_ELEMENT (element))) {
          glong length;
          char *text;

          /* A small heuristic here. If there's only one input element
           * modified and it does not have a lot of text the user is
           * likely not very interested in saving this work, so do
           * nothing (eg, google search input). */
          if (modified_input_element)
            return TRUE;

          modified_input_element = TRUE;

          text = webkit_dom_html_input_element_get_value (WEBKIT_DOM_HTML_INPUT_ELEMENT (element));
          length = g_utf8_strlen (text, -1);
          g_free (text);

          if (length > MIN_INPUT_LENGTH)
            return TRUE;
        }
    }
  }

  return FALSE;
}

char *
libre_impuesto_web_view_get_location (LibreImpuestoWebView *view,
                            gboolean toplevel)
{
  /* FIXME: follow the toplevel parameter */
  WebKitWebFrame *web_frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (view));
  return g_strdup (webkit_web_frame_get_uri (web_frame));
}

void
libre_impuesto_web_view_get_security_level (LibreImpuestoWebView *view,
                                  LibreImpuestoWebViewSecurityLevel *level,
                                  char **description)
{
  g_return_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view));

  if (level)
    *level = view->priv->security_level;

  if (description)
    *description = NULL;
}

static void
libre_impuesto_web_view_run_print_action (LibreImpuestoWebView *view, GtkPrintOperationAction action)
{

}

void
libre_impuesto_web_view_print (LibreImpuestoWebView *view)
{
  libre_impuesto_web_view_run_print_action (view, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG);
}

const char *
libre_impuesto_web_view_get_title_composite (LibreImpuestoWebView *view)
{
  const char *title = "";
  const char *loading_title;
  gboolean is_loading;

  g_return_val_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view), NULL);

  is_loading = libre_impuesto_web_view_is_loading (view);
  loading_title = libre_impuesto_web_view_get_loading_title (view);
  title = libre_impuesto_web_view_get_title (view);

  if (view->priv->is_blank)
  {
    if (is_loading)
      title = loading_title;
    else
      title = _("Blank page");
  }

  return title != NULL ? title : "";
}

static void
libre_impuesto_web_view_save_sub_resource_start (GList *subresources, char *destination_uri);

static void
libre_impuesto_web_view_close_cb (GOutputStream *ostream, GAsyncResult *result, GString *data)
{
  GList *subresources;
  char *destination_uri;
  GError *error = NULL;

  subresources = (GList*)g_object_get_data (G_OBJECT (ostream),
                                            "libre-impuesto-web-view-save-subresources");

  destination_uri = (char*)g_object_get_data (G_OBJECT (ostream),
                                              "libre-impuesto-web-view-save-dest-uri");

  g_output_stream_close_finish (ostream, result, &error);
  g_object_unref (ostream);

  if (error) {
    g_list_free (subresources);
    g_free (destination_uri);
    g_warning ("Unable to write to file: %s", error->message);
    g_error_free (error);
    return;
  }

  if (!subresources || !subresources->next) {
    g_list_free (subresources);
    g_free (destination_uri);
    return;
  }

  subresources = subresources->next;
  libre_impuesto_web_view_save_sub_resource_start (subresources, destination_uri);
}

static void
libre_impuesto_web_view_save_write_cb (GOutputStream *ostream, GAsyncResult *result, GString *data)
{
  GError *error = NULL;
  gssize written;

  written = g_output_stream_write_finish (ostream, result, &error);
  if (error) {
    GList *subresources;
    char *destination_uri;

    subresources = (GList*)g_object_get_data (G_OBJECT (ostream),
                                             "libre-impuesto-web-view-save-subresources");
    g_list_free (subresources);

    destination_uri = (char*)g_object_get_data (G_OBJECT (ostream),
                                                "libre-impuesto-web-view-save-dest-uri");
    g_free (destination_uri);

    g_string_free (data, FALSE);
    g_object_unref (ostream);

    g_warning ("Unable to write to file: %s", error->message);

    g_error_free (error);
    return;
  }

  if (written == data->len) {
    g_string_free (data, FALSE);
    g_output_stream_close_async (ostream, G_PRIORITY_DEFAULT, NULL,
                                 (GAsyncReadyCallback)libre_impuesto_web_view_close_cb,
                                 NULL);
    return;
  }

  data->len -= written;
  data->str += written;

  g_output_stream_write_async (ostream,
                               data->str, data->len,
                               G_PRIORITY_DEFAULT, NULL,
                               (GAsyncReadyCallback)libre_impuesto_web_view_save_write_cb,
                               data);
}

static void
libre_impuesto_web_view_save_replace_cb (GFile *file, GAsyncResult *result, GString *const_data)
{
  GFileOutputStream *ostream;
  GList *subresources;
  char *destination_uri;
  GString *data;
  GError *error = NULL;

  subresources = (GList*)g_object_get_data (G_OBJECT (file),
                                            "libre-impuesto-web-view-save-subresources");

  destination_uri = (char*)g_object_get_data (G_OBJECT (file),
                                              "libre-impuesto-web-view-save-dest-uri");

  ostream = g_file_replace_finish (file, result, &error);
  if (error) {
    g_warning ("Failed to save page: %s", error->message);
    g_list_free (subresources);
    g_free (destination_uri);
    g_error_free (error);
    return;
  }

  if (const_data) {
    data = g_string_sized_new (const_data->len);
    data->str = const_data->str;
    data->len = const_data->len;
  } else
    data = g_string_new ("");

  /* If we have subresources to handle, pass the information along */
  if (subresources) {
    g_object_set_data (G_OBJECT (ostream),
                       "libre-impuesto-web-view-save-subresources",
                       subresources);

    g_object_set_data (G_OBJECT (ostream),
                       "libre-impuesto-web-view-save-dest-uri",
                       destination_uri);
  }

  g_output_stream_write_async (G_OUTPUT_STREAM (ostream),
                               data->str, data->len,
                               G_PRIORITY_DEFAULT, NULL,
                               (GAsyncReadyCallback)libre_impuesto_web_view_save_write_cb,
                               data);
}

static void
libre_impuesto_web_view_save_sub_resource_start (GList *subresources, char *destination_uri)
{
  WebKitWebResource *resource;
  GFile *file;
  const char *resource_uri;
  char *resource_basename;
  char *resource_name;
  char *resource_dest_uri;
  const GString *data;

  resource = WEBKIT_WEB_RESOURCE (subresources->data);

  resource_uri = webkit_web_resource_get_uri (resource);
  resource_basename = g_path_get_basename (resource_uri);

  resource_name = g_uri_escape_string (resource_basename, NULL, TRUE);
  g_free (resource_basename);

  resource_dest_uri = g_strdup_printf ("%s/%s", destination_uri, resource_name);
  g_free (resource_name);

  file = g_file_new_for_uri (resource_dest_uri);
  g_free (resource_dest_uri);

  g_object_set_data (G_OBJECT (file),
                     "libre-impuesto-web-view-save-dest-uri",
                     destination_uri);

  g_object_set_data (G_OBJECT (file),
                     "libre-impuesto-web-view-save-subresources",
                     subresources);

  data = webkit_web_resource_get_data (resource);

  g_file_replace_async (file, NULL, FALSE,
                        G_FILE_CREATE_REPLACE_DESTINATION|G_FILE_CREATE_PRIVATE,
                        G_PRIORITY_DEFAULT, NULL,
                        (GAsyncReadyCallback)libre_impuesto_web_view_save_replace_cb,
                        (GString*)data);

  g_object_unref (file);
}

static void
libre_impuesto_web_view_save_sub_resources (LibreImpuestoWebView *view, const char *uri, GList *subresources)
{
  GFile *file;
  char *filename;
  char *dotpos;
  char *directory_uri;
  char *tmp;
  char *destination_uri;
  GError *error = NULL;

  /* filename of the main resource without extension */
  filename = g_path_get_basename (uri);
  dotpos = g_strrstr (filename, ".");
  if (dotpos)
    *dotpos = '\0';

  directory_uri = g_path_get_dirname (uri);

  /* Translators: this is the directory name to store auxilary files
   * when saving html files.
   */
  tmp = g_strdup_printf (_("%s Files"), filename);
  g_free (filename);

  destination_uri = g_strdup_printf ("%s/%s", directory_uri, tmp);
  g_free (directory_uri);
  g_free (tmp);

  file = g_file_new_for_uri (destination_uri);

  if (!g_file_make_directory (file, NULL, &error)) {
    if (error->code != G_IO_ERROR_EXISTS) {
      g_warning ("Could not create directory: %s", error->message);
      g_error_free (error);
      g_object_unref (file);
      return;
    }
  }
  g_object_unref (file);

  /* Now, let's start saving sub resources */
  libre_impuesto_web_view_save_sub_resource_start (subresources, destination_uri);
}

void
libre_impuesto_web_view_save (LibreImpuestoWebView *view, const char *uri)
{
  WebKitWebFrame *frame;
  WebKitWebDataSource *data_source;
  GList *subresources;
  const GString *data;
  GFile *file;

  /* Save main resource */
  frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(view));
  data_source = webkit_web_frame_get_data_source (frame);
  data = webkit_web_data_source_get_data (data_source);

  file = g_file_new_for_uri (uri);
  g_file_replace_async (file, NULL, FALSE,
                        G_FILE_CREATE_REPLACE_DESTINATION|G_FILE_CREATE_PRIVATE,
                        G_PRIORITY_DEFAULT, NULL,
                        (GAsyncReadyCallback)libre_impuesto_web_view_save_replace_cb,
                        (GString*)data);

  g_object_unref (file);

  /* If subresources exist, save them */
  subresources = webkit_web_data_source_get_subresources (data_source);
  if (!subresources)
    return;

  libre_impuesto_web_view_save_sub_resources (view, uri, subresources);
}

void
libre_impuesto_web_view_load_homepage (LibreImpuestoWebView *view)
{
}

GdkPixbuf *
libre_impuesto_web_view_get_snapshot (LibreImpuestoWebView *view, int x, int y, int width, int height)
{
  cairo_surface_t *surface;
  cairo_t *cr;
  GdkPixbuf *snapshot;
  GtkAllocation allocation;

  g_return_val_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view), NULL);

  gtk_widget_get_allocation (GTK_WIDGET (view), &allocation);
  surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
                                        allocation.width,
                                        allocation.height);
  cr = cairo_create (surface);
  cairo_rectangle (cr, x, y, width, height);
  cairo_clip (cr);
  gtk_widget_draw (GTK_WIDGET (view), cr);

  snapshot = gdk_pixbuf_get_from_surface (surface, x, y, width, height);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  return snapshot;
}

gboolean
libre_impuesto_web_view_is_loading_homepage (LibreImpuestoWebView *view)
{
  g_return_val_if_fail (LIBRE_IMPUESTO_IS_WEB_VIEW (view), FALSE);

  return view->priv->loading_homepage;
}

static void
impl_loading_homepage (LibreImpuestoWebView *view)
{
  view->priv->loading_homepage = TRUE;
}

static gboolean
libre_impuesto_web_view_key_press_event (GtkWidget *widget, GdkEventKey *event)
{
  LibreImpuestoWebView *web_view = LIBRE_IMPUESTO_WEB_VIEW (widget);
  gboolean key_handled = FALSE;

  key_handled = GTK_WIDGET_CLASS (libre_impuesto_web_view_parent_class)->key_press_event (widget, event);

  if (key_handled)
    return TRUE;

  g_signal_emit_by_name (web_view, "search-key-press", event, &key_handled);

  return key_handled;
}

static gboolean
libre_impuesto_web_view_button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  /* We always show the browser context menu on control-rightclick. */
  if (event->button == 3 && event->state == GDK_CONTROL_MASK)
    return FALSE;

  /* Let WebKitWebView handle this. */
  return GTK_WIDGET_CLASS (libre_impuesto_web_view_parent_class)->button_press_event (widget, event);
}

static void
libre_impuesto_web_view_get_property (GObject *object,
                            guint prop_id,
                            GValue *value,
                            GParamSpec *pspec)
{
  LibreImpuestoWebViewPrivate *priv = LIBRE_IMPUESTO_WEB_VIEW (object)->priv;

  switch (prop_id) {
    case PROP_ADDRESS:
      g_value_set_string (value, priv->address);
      break;
    case PROP_EMBED_TITLE:
      g_value_set_string (value, priv->title);
      break;
    case PROP_TYPED_ADDRESS:
      g_value_set_string (value, priv->typed_address);
      break;
    case PROP_DOCUMENT_TYPE:
      g_value_set_enum (value, priv->document_type);
      break;
    case PROP_HIDDEN_POPUP_COUNT:
      g_value_set_int (value, popup_blocker_n_hidden
                       (LIBRE_IMPUESTO_WEB_VIEW (object)));
      break;
    case PROP_ICON:
      g_value_set_object (value, priv->icon);
      break;
    case PROP_LINK_MESSAGE:
      g_value_set_string (value, priv->link_message);
      break;
    case PROP_NAVIGATION:
      g_value_set_flags (value, priv->nav_flags);
      break;
    case PROP_POPUPS_ALLOWED:
      g_value_set_boolean (value, libre_impuesto_web_view_get_popups_allowed
                           (LIBRE_IMPUESTO_WEB_VIEW (object)));
      break;
    case PROP_SECURITY:
      g_value_set_enum (value, priv->security_level);
      break;
    case PROP_STATUS_MESSAGE:
      g_value_set_string (value, priv->status_message);
      break;
    case PROP_VISIBLE:
      g_value_set_boolean (value, priv->visibility);
      break;
    case PROP_IS_BLANK:
      g_value_set_boolean (value, priv->is_blank);
      break;
    default:
      break;
  }
}

static void
libre_impuesto_web_view_set_property (GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
  switch (prop_id) {
    case PROP_POPUPS_ALLOWED:
      libre_impuesto_web_view_set_popups_allowed (LIBRE_IMPUESTO_WEB_VIEW (object), g_value_get_boolean (value));
      break;
    case PROP_TYPED_ADDRESS:
      libre_impuesto_web_view_set_typed_address (LIBRE_IMPUESTO_WEB_VIEW (object), g_value_get_string (value));
      break;
      break;
    case PROP_ADDRESS:
    case PROP_DOCUMENT_TYPE:
    case PROP_HIDDEN_POPUP_COUNT:
    case PROP_ICON:
    case PROP_LINK_MESSAGE:
    case PROP_NAVIGATION:
    case PROP_SECURITY:
    case PROP_STATUS_MESSAGE:
    case PROP_EMBED_TITLE:
    case PROP_VISIBLE:
    case PROP_IS_BLANK:
      /* read only */
      break;
    default:
      break;
  }
}

static void
libre_impuesto_web_view_finalize (GObject *object)
{
  LibreImpuestoWebViewPrivate *priv = LIBRE_IMPUESTO_WEB_VIEW (object)->priv;

  if (priv->icon != NULL) {
    g_object_unref (priv->icon);
    priv->icon = NULL;
  }

  if (priv->non_search_regex != NULL) {
    g_regex_unref (priv->non_search_regex);
    priv->non_search_regex = NULL;
  }

  libre_impuesto_web_view_popups_manager_reset (LIBRE_IMPUESTO_WEB_VIEW (object));

  g_free (priv->address);
  g_free (priv->typed_address);
  g_free (priv->title);
  g_free (priv->status_message);
  g_free (priv->link_message);
  g_free (priv->loading_title);

  G_OBJECT_CLASS (libre_impuesto_web_view_parent_class)->finalize (object);
}

static void
libre_impuesto_web_view_init (LibreImpuestoWebView *web_view)
{
  LibreImpuestoWebViewPrivate *priv;

  priv = web_view->priv = LIBRE_IMPUESTO_WEB_VIEW_GET_PRIVATE (web_view);

  priv->expire_address_now = TRUE;
  priv->is_blank = TRUE;
  priv->load_status = WEBKIT_LOAD_PROVISIONAL;
  priv->title = g_strdup (EMPTY_PAGE);
  priv->document_type = LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_HTML;
  priv->security_level = LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_UNKNOWN;
  priv->monitor_directory = FALSE;

  priv->non_search_regex = g_regex_new (LIBRE_IMPUESTO_WEB_VIEW_NON_SEARCH_REGEX,
                                        G_REGEX_OPTIMIZE, G_REGEX_MATCH_NOTEMPTY, NULL);

  priv->history_service_cancellable = g_cancellable_new ();

  g_signal_connect (web_view, "mime-type-policy-decision-requested",
                    G_CALLBACK (mime_type_policy_decision_requested_cb),
                    NULL);

  g_signal_connect (web_view, "geolocation-policy-decision-requested",
                    G_CALLBACK (geolocation_policy_decision_requested_cb),
                    NULL);

  g_signal_connect (web_view, "notify::load-status",
                    G_CALLBACK (load_status_cb),
                    NULL);

  g_signal_connect (web_view, "close-web-view",
                    G_CALLBACK (close_web_view_cb),
                    NULL);

  g_signal_connect (web_view, "load-error",
                    G_CALLBACK (load_error_cb),
                    NULL);

  g_signal_connect (web_view, "notify::zoom-level",
                    G_CALLBACK (zoom_changed_cb),
                    NULL);

  g_signal_connect (web_view, "notify::title",
                    G_CALLBACK (title_changed_cb),
                    NULL);

  g_signal_connect (web_view, "notify::uri",
                    G_CALLBACK (uri_changed_cb),
                    NULL);

  g_signal_connect (web_view, "hovering-over-link",
                    G_CALLBACK (hovering_over_link_cb),
                    NULL);

  g_signal_connect (web_view, "icon-loaded",
                    G_CALLBACK (favicon_cb),
                    NULL);

  g_signal_connect (web_view, "new-window",
                    G_CALLBACK (new_window_cb),
                    NULL);

  g_signal_connect (web_view, "ge_popup_blocked",
                    G_CALLBACK (ge_popup_blocked_cb),
                    NULL);

}

static void
libre_impuesto_web_view_dispose (GObject *object)
{
  //LibreImpuestoWebViewPrivate *priv = LIBRE_IMPUESTO_WEB_VIEW (object)->priv;

  libre_impuesto_web_view_file_monitor_cancel (LIBRE_IMPUESTO_WEB_VIEW (object));

  G_OBJECT_CLASS (libre_impuesto_web_view_parent_class)->dispose (object);
}

static void
libre_impuesto_web_view_constructed (GObject *object)
{
  webkit_web_view_set_full_content_zoom (WEBKIT_WEB_VIEW (object), TRUE);
}

static void
libre_impuesto_web_view_class_init (LibreImpuestoWebViewClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gobject_class->dispose = libre_impuesto_web_view_dispose;
  gobject_class->finalize = libre_impuesto_web_view_finalize;
  gobject_class->get_property = libre_impuesto_web_view_get_property;
  gobject_class->set_property = libre_impuesto_web_view_set_property;
  gobject_class->constructed = libre_impuesto_web_view_constructed;

  widget_class->button_press_event = libre_impuesto_web_view_button_press_event;
  widget_class->key_press_event = libre_impuesto_web_view_key_press_event;

  klass->loading_homepage = impl_loading_homepage;

  g_object_class_install_property (gobject_class,
                                   PROP_ADDRESS,
                                   g_param_spec_string ("address",
                                                        "Address",
                                                        "The view's address",
                                                        "",
                                                        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_TYPED_ADDRESS,
                                   g_param_spec_string ("typed-address",
                                                        "Typed Address",
                                                        "The typed address",
                                                        "",
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_EMBED_TITLE,
                                   g_param_spec_string ("embed-title",
                                                        "Title",
                                                        "The view's title",
                                                        EMPTY_PAGE,
                                                        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));


  g_object_class_install_property (gobject_class,
                                   PROP_STATUS_MESSAGE,
                                   g_param_spec_string ("status-message",
                                                        "Status Message",
                                                        "The view's statusbar message",
                                                        NULL,
                                                        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_LINK_MESSAGE,
                                   g_param_spec_string ("link-message",
                                                        "Link Message",
                                                        "The view's link message",
                                                        NULL,
                                                        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_ICON,
                                   g_param_spec_object ("icon",
                                                        "Icon",
                                                        "The view icon's",
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_HIDDEN_POPUP_COUNT,
                                   g_param_spec_int ("hidden-popup-count",
                                                     "Number of Blocked Popups",
                                                     "The view's number of blocked popup windows",
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_POPUPS_ALLOWED,
                                   g_param_spec_boolean ("popups-allowed",
                                                         "Popups Allowed",
                                                         "Whether popup windows are to be displayed",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_VISIBLE,
                                   g_param_spec_boolean ("visibility",
                                                         "Visibility",
                                                         "The view's visibility",
                                                         FALSE,
                                                         G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_class_install_property (gobject_class,
                                   PROP_IS_BLANK,
                                   g_param_spec_boolean ("is-blank",
                                                         "Is blank",
                                                         "If the LibreImpuestoWebView is blank",
                                                         FALSE,
                                                         G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

    g_signal_new ("new-window",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, new_window),
            NULL, NULL,
            g_cclosure_marshal_VOID__OBJECT,
            G_TYPE_NONE,
            1,
            GTK_TYPE_WIDGET);

    g_signal_new ("ge_popup_blocked",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_FIRST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, popup_blocked),
            NULL, NULL,
            g_cclosure_marshal_generic,
            G_TYPE_NONE,
            3,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("ge_search_link",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_FIRST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, search_link),
            NULL, NULL,
            g_cclosure_marshal_generic,
            G_TYPE_NONE,
            3,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("ge_feed_link",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_FIRST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, feed_link),
            NULL, NULL,
            g_cclosure_marshal_generic,
            G_TYPE_NONE,
            3,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("ge_modal_alert",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, modal_alert),
            g_signal_accumulator_true_handled, NULL,
            g_cclosure_marshal_generic,
            G_TYPE_BOOLEAN,
            0);

    g_signal_new ("ge_modal_alert_closed",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, modal_alert_closed),
            NULL, NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE,
            0);

    g_signal_new ("search-key-press",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, search_key_press),
            g_signal_accumulator_true_handled, NULL,
            g_cclosure_marshal_generic,
            G_TYPE_BOOLEAN,
            1,
            GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("content-blocked",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, content_blocked),
            NULL, NULL,
            g_cclosure_marshal_VOID__STRING,
            G_TYPE_NONE,
            1,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("new-document-now",
            LIBRE_IMPUESTO_TYPE_WEB_VIEW,
            G_SIGNAL_RUN_FIRST,
            G_STRUCT_OFFSET (LibreImpuestoWebViewClass, new_document_now),
            NULL, NULL,
            g_cclosure_marshal_VOID__STRING,
            G_TYPE_NONE,
            1,
            G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("loading-homepage",
                  LIBRE_IMPUESTO_TYPE_WEB_VIEW,
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (LibreImpuestoWebViewClass, loading_homepage),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  g_type_class_add_private (gobject_class, sizeof (LibreImpuestoWebViewPrivate));
}
