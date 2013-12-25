/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=2 sts=2 et: */
/*
 *  Copyright © 2008 Gustavo Noronha Silva
 *  Copyright © 2012 Igalia S.L.
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

#ifndef LIBRE_IMPUESTO_WEB_VIEW_H
#define LIBRE_IMPUESTO_WEB_VIEW_H

#include "libre-impuesto.h"
#include <webkit/webkit.h>

G_BEGIN_DECLS

#define LIBRE_IMPUESTO_TYPE_WEB_VIEW (libre_impuesto_web_view_get_type ())
#define LIBRE_IMPUESTO_WEB_VIEW(o)   (G_TYPE_CHECK_INSTANCE_CAST ((o), LIBRE_IMPUESTO_TYPE_WEB_VIEW, LibreImpuestoWebView))
#define LIBRE_IMPUESTO_WEB_VIEW_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), LIBRE_IMPUESTO_TYPE_WEB_VIEW, LibreImpuestoWebViewClass))
#define LIBRE_IMPUESTO_IS_WEB_VIEW(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBRE_IMPUESTO_TYPE_WEB_VIEW))
#define LIBRE_IMPUESTO_IS_WEB_VIEW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), LIBRE_IMPUESTO_TYPE_WEB_VIEW))
#define LIBRE_IMPUESTO_WEB_VIEW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), LIBRE_IMPUESTO_TYPE_WEB_VIEW, LibreImpuestoWebViewClass))

typedef struct _LibreImpuestoWebViewClass  LibreImpuestoWebViewClass;
typedef struct _LibreImpuestoWebView    LibreImpuestoWebView;
typedef struct _LibreImpuestoWebViewPrivate  LibreImpuestoWebViewPrivate;

typedef enum
{
  LIBRE_IMPUESTO_WEB_VIEW_NAV_BACK    = 1 << 0,
  LIBRE_IMPUESTO_WEB_VIEW_NAV_FORWARD = 1 << 1
} LibreImpuestoWebViewNavigationFlags;

typedef enum
{
  LIBRE_IMPUESTO_WEB_VIEW_CHROME_MENUBAR    = 1 << 0,
  LIBRE_IMPUESTO_WEB_VIEW_CHROME_TOOLBAR    = 1 << 1,
  LIBRE_IMPEUSTO_WEB_VIEW_CHROME_STATUSBAR    = 1 << 2,
  LIBRE_IMPUESTO_WEB_VIEW_CHROME_BOOKMARKSBAR = 1 << 3
} LibreImpuestoWebViewChrome;

#define LIBRE_IMPUESTO_WEB_VIEW_CHROME_ALL (LIBRE_IMPUESTO_WEB_VIEW_CHROME_MENUBAR |  \
                                            LIBRE_IMPUESTO_WEB_VIEW_CHROME_TOOLBAR | \
                                            LIBRE_IMPUESTO_WEB_VIEW_CHROME_STATUSBAR | \
                                            LIBRE_IMPUESTO_WEB_VIEW_CHROME_BOOKMARKSBAR)

typedef enum
{
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_UNKNOWN,
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_INSECURE,
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_BROKEN,
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_LOW,
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_MED,
  LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_HIGH
} LibreImpuestoWebViewSecurityLevel;

typedef enum
{
  LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_HTML,
  LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_XML,
  LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_IMAGE,
  LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_OTHER
} LibreImpuestoWebViewDocumentType;

typedef enum {
  LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR,
  LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_CRASH
} LibreImpuestoWebViewErrorPage;

struct _LibreImpuestoWebView
{
  WebKitWebView parent;

  /*< private >*/
  LibreImpuestoWebViewPrivate *priv;
};

struct _LibreImpuestoWebViewClass
{
  WebKitWebViewClass parent_class;

  /* Signals */
  void   (* feed_link)          (LibreImpuestoWebView *view,
                                 const char *type,
                                 const char *title,
                                 const char *address);
  void   (* search_link)        (LibreImpuestoWebView *view,
                                 const char *type,
                                 const char *title,
                                 const char *address);
  void   (* popup_blocked)      (LibreImpuestoWebView *view,
                                 const char *address,
                                 const char *target,
                                 const char *features);
  void   (* content_blocked)    (LibreImpuestoWebView *view,
                                 const char *uri);
  gboolean (* modal_alert)      (LibreImpuestoWebView *view);
  void   (* modal_alert_closed) (LibreImpuestoWebView *view);
  void   (* new_window)         (LibreImpuestoWebView *view,
                                 LibreImpuestoWebView *new_view);
  gboolean (* search_key_press) (LibreImpuestoWebView *view,
                                 GdkEventKey *event);
  void   (* new_document_now)   (LibreImpuestoWebView *view,
                                 const char *uri);
  void   (* loading_homepage)   (LibreImpuestoWebView *view);
};

GType                      libre_impuesto_web_view_get_type                 (void);
GType                      libre_impuesto_web_view_chrome_get_type          (void);
GType                      libre_impuesto_web_view_security_level_get_type  (void);
GtkWidget *                libre_impuesto_web_view_new                      (void);
void                       libre_impuesto_web_view_load_request             (LibreImpuestoWebView               *view,
                                                                   WebKitNetworkRequest      *request);
void                       libre_impuesto_web_view_load_url                 (LibreImpuestoWebView               *view,
                                                                   const char                *url);
void                       libre_impuesto_web_view_copy_back_history        (LibreImpuestoWebView               *source,
                                                                   LibreImpuestoWebView               *dest);
gboolean                   libre_impuesto_web_view_is_loading               (LibreImpuestoWebView               *view);
const char *               libre_impuesto_web_view_get_loading_title        (LibreImpuestoWebView               *view);
GdkPixbuf *                libre_impuesto_web_view_get_icon                 (LibreImpuestoWebView               *view);
LibreImpuestoWebViewDocumentType    libre_impuesto_web_view_get_document_type        (LibreImpuestoWebView               *view);
LibreImpuestoWebViewNavigationFlags libre_impuesto_web_view_get_navigation_flags     (LibreImpuestoWebView               *view);
const char *               libre_impuesto_web_view_get_status_message       (LibreImpuestoWebView               *view);
const char *               libre_impuesto_web_view_get_link_message         (LibreImpuestoWebView               *view);
gboolean                   libre_impuesto_web_view_get_visibility           (LibreImpuestoWebView               *view);
void                       libre_impuesto_web_view_set_link_message         (LibreImpuestoWebView               *view,
                                                                   char                      *link_message);
void                       libre_impuesto_web_view_set_security_level       (LibreImpuestoWebView               *view,
                                                                   LibreImpuestoWebViewSecurityLevel   level);
void                       libre_impuesto_web_view_set_visibility           (LibreImpuestoWebView               *view,
                                                                   gboolean                   visibility);
const char *               libre_impuesto_web_view_get_typed_address        (LibreImpuestoWebView               *view);
void                       libre_impuesto_web_view_set_typed_address        (LibreImpuestoWebView               *view,
                                                                   const char                *address);
gboolean                   libre_impuesto_web_view_get_is_blank             (LibreImpuestoWebView               *view);
gboolean                   libre_impuesto_web_view_has_modified_forms       (LibreImpuestoWebView               *view);
char *                     libre_impuesto_web_view_get_location             (LibreImpuestoWebView               *view,
                                                                   gboolean                   toplevel);
void                       libre_impuesto_web_view_get_security_level       (LibreImpuestoWebView               *view,
                                                                   LibreImpuestoWebViewSecurityLevel  *level,
                                                                   char                     **description);
void                       libre_impuesto_web_view_print                    (LibreImpuestoWebView               *view);
void                       libre_impuesto_web_view_set_title                (LibreImpuestoWebView               *view,
                                                                   const char                *view_title);
const char *               libre_impuesto_web_view_get_title                (LibreImpuestoWebView               *view);
const char *               libre_impuesto_web_view_get_address              (LibreImpuestoWebView               *view);
const char *               libre_impuesto_web_view_get_title_composite      (LibreImpuestoWebView               *view);

void                       libre_impuesto_web_view_load_error_page          (LibreImpuestoWebView               *view,
                                                                   const char                *uri,
                                                                   LibreImpuestoWebViewErrorPage       page,
                                                                   GError                    *error);
/* These should be private */
void                       libre_impuesto_web_view_set_address              (LibreImpuestoWebView               *view,
                                                                   const char                *address);
void                       libre_impuesto_web_view_location_changed         (LibreImpuestoWebView               *view,
                                                                   const char                *location);
void                       libre_impuesto_web_view_set_loading_title        (LibreImpuestoWebView               *view,
                                                                   const char                *title,
                                                                   gboolean                   is_address);
void                       libre_impuesto_web_view_popups_manager_reset     (LibreImpuestoWebView               *view);
void                       libre_impuesto_web_view_save                     (LibreImpuestoWebView               *view,
                                                                   const char                *uri);
void                       libre_impuesto_web_view_load_homepage            (LibreImpuestoWebView               *view);

char *                     libre_impuesto_web_view_create_web_application   (LibreImpuestoWebView               *view,
                                                                   const char                *title,
                                                                   GdkPixbuf                 *icon);
GdkPixbuf *                libre_impuesto_web_view_get_snapshot             (LibreImpuestoWebView               *view,
                                                                   int                        x,
                                                                   int                        y,
                                                                   int                        width,
                                                                   int                        height);
gboolean                   libre_impuesto_web_view_is_loading_homepage      (LibreImpuestoWebView               *view);

G_END_DECLS

#endif

