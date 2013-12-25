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

#include <config.h>
#include "libre-impuesto-embed-type-builtins.h"

/* enumerations from "libre_impuesto-adblock.h" */
#include "libre_impuesto-adblock.h"
GType
ad_uri_check_type_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _ad_uri_check_type_values[] = {
      { AD_URI_CHECK_TYPE_OTHER, "AD_URI_CHECK_TYPE_OTHER", "other" },
      { AD_URI_CHECK_TYPE_SCRIPT, "AD_URI_CHECK_TYPE_SCRIPT", "script" },
      { AD_URI_CHECK_TYPE_IMAGE, "AD_URI_CHECK_TYPE_IMAGE", "image" },
      { AD_URI_CHECK_TYPE_STYLESHEET, "AD_URI_CHECK_TYPE_STYLESHEET", "stylesheet" },
      { AD_URI_CHECK_TYPE_OBJECT, "AD_URI_CHECK_TYPE_OBJECT", "object" },
      { AD_URI_CHECK_TYPE_DOCUMENT, "AD_URI_CHECK_TYPE_DOCUMENT", "document" },
      { AD_URI_CHECK_TYPE_SUBDOCUMENT, "AD_URI_CHECK_TYPE_SUBDOCUMENT", "subdocument" },
      { AD_URI_CHECK_TYPE_REFRESH, "AD_URI_CHECK_TYPE_REFRESH", "refresh" },
      { AD_URI_CHECK_TYPE_XBEL, "AD_URI_CHECK_TYPE_XBEL", "xbel" },
      { AD_URI_CHECK_TYPE_PING, "AD_URI_CHECK_TYPE_PING", "ping" },
      { AD_URI_CHECK_TYPE_XMLHTTPREQUEST, "AD_URI_CHECK_TYPE_XMLHTTPREQUEST", "xmlhttprequest" },
      { AD_URI_CHECK_TYPE_OBJECT_SUBREQUEST, "AD_URI_CHECK_TYPE_OBJECT_SUBREQUEST", "object-subrequest" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("AdUriCheckType", _ad_uri_check_type_values);
  }

  return type;
}


/* enumerations from "libre_impuesto-download.h" */
#include "libre_impuesto-download.h"
GType
libre_impuesto_download_action_type_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_download_action_type_values[] = {
      { LIBRE_IMPUESTO_DOWNLOAD_ACTION_NONE, "LIBRE_IMPUESTO_DOWNLOAD_ACTION_NONE", "none" },
      { LIBRE_IMPUESTO_DOWNLOAD_ACTION_AUTO, "LIBRE_IMPUESTO_DOWNLOAD_ACTION_AUTO", "auto" },
      { LIBRE_IMPUESTO_DOWNLOAD_ACTION_BROWSE_TO, "LIBRE_IMPUESTO_DOWNLOAD_ACTION_BROWSE_TO", "browse-to" },
      { LIBRE_IMPUESTO_DOWNLOAD_ACTION_OPEN, "LIBRE_IMPUESTO_DOWNLOAD_ACTION_OPEN", "open" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoDownloadActionType", _libre_impuesto_download_action_type_values);
  }

  return type;
}


/* enumerations from "libre_impuesto-embed-shell.h" */
#include "libre_impuesto-embed-shell.h"
GType
libre_impuesto_embed_shell_mode_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_embed_shell_mode_values[] = {
      { LIBRE_IMPUESTO_EMBED_SHELL_MODE_BROWSER, "LIBRE_IMPUESTO_EMBED_SHELL_MODE_BROWSER", "browser" },
      { LIBRE_IMPUESTO_EMBED_SHELL_MODE_PRIVATE, "LIBRE_IMPUESTO_EMBED_SHELL_MODE_PRIVATE", "private" },
      { LIBRE_IMPUESTO_EMBED_SHELL_MODE_APPLICATION, "LIBRE_IMPUESTO_EMBED_SHELL_MODE_APPLICATION", "application" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoEmbedShellMode", _libre_impuesto_embed_shell_mode_values);
  }

  return type;
}


/* enumerations from "libre_impuesto-permission-manager.h" */
#include "libre_impuesto-permission-manager.h"
GType
libre_impuesto_permission_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_permission_values[] = {
      { LIBRE_IMPUESTO_PERMISSION_ALLOWED, "LIBRE_IMPUESTO_PERMISSION_ALLOWED", "allowed" },
      { LIBRE_IMPUESTO_PERMISSION_DENIED, "LIBRE_IMPUESTO_PERMISSION_DENIED", "denied" },
      { LIBRE_IMPUESTO_PERMISSION_DEFAULT, "LIBRE_IMPUESTO_PERMISSION_DEFAULT", "default" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoPermission", _libre_impuesto_permission_values);
  }

  return type;
}


/* enumerations from "libre_impuesto-web-view.h" */
#include "libre_impuesto-web-view.h"
GType
libre_impuesto_web_view_navigation_flags_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GFlagsValue _libre_impuesto_web_view_navigation_flags_values[] = {
      { LIBRE_IMPUESTO_WEB_VIEW_NAV_BACK, "LIBRE_IMPUESTO_WEB_VIEW_NAV_BACK", "back" },
      { LIBRE_IMPUESTO_WEB_VIEW_NAV_FORWARD, "LIBRE_IMPUESTO_WEB_VIEW_NAV_FORWARD", "forward" },
      { 0, NULL, NULL }
    };

    type = g_flags_register_static ("Libre_ImpuestoWebViewNavigationFlags", _libre_impuesto_web_view_navigation_flags_values);
  }

  return type;
}

GType
libre_impuesto_web_view_chrome_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GFlagsValue _libre_impuesto_web_view_chrome_values[] = {
      { LIBRE_IMPUESTO_WEB_VIEW_CHROME_MENUBAR, "LIBRE_IMPUESTO_WEB_VIEW_CHROME_MENUBAR", "menubar" },
      { LIBRE_IMPUESTO_WEB_VIEW_CHROME_TOOLBAR, "LIBRE_IMPUESTO_WEB_VIEW_CHROME_TOOLBAR", "toolbar" },
      { LIBRE_IMPUESTO_WEB_VIEW_CHROME_STATUSBAR, "LIBRE_IMPUESTO_WEB_VIEW_CHROME_STATUSBAR", "statusbar" },
      { LIBRE_IMPUESTO_WEB_VIEW_CHROME_BOOKMARKSBAR, "LIBRE_IMPUESTO_WEB_VIEW_CHROME_BOOKMARKSBAR", "bookmarksbar" },
      { 0, NULL, NULL }
    };

    type = g_flags_register_static ("Libre_ImpuestoWebViewChrome", _libre_impuesto_web_view_chrome_values);
  }

  return type;
}

GType
libre_impuesto_web_view_security_level_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_web_view_security_level_values[] = {
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_UNKNOWN, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_UNKNOWN", "unknown" },
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_INSECURE, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_INSECURE", "insecure" },
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_BROKEN, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_BROKEN", "broken" },
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_LOW, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_LOW", "secure-low" },
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_MED, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_MED", "secure-med" },
      { LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_HIGH, "LIBRE_IMPUESTO_WEB_VIEW_STATE_IS_SECURE_HIGH", "secure-high" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoWebViewSecurityLevel", _libre_impuesto_web_view_security_level_values);
  }

  return type;
}

GType
libre_impuesto_web_view_document_type_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_web_view_document_type_values[] = {
      { LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_HTML, "LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_HTML", "html" },
      { LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_XML, "LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_XML", "xml" },
      { LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_IMAGE, "LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_IMAGE", "image" },
      { LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_OTHER, "LIBRE_IMPUESTO_WEB_VIEW_DOCUMENT_OTHER", "other" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoWebViewDocumentType", _libre_impuesto_web_view_document_type_values);
  }

  return type;
}

GType
libre_impuesto_web_view_error_page_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static const GEnumValue _libre_impuesto_web_view_error_page_values[] = {
      { LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR, "LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_NETWORK_ERROR", "network-error" },
      { LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_CRASH, "LIBRE_IMPUESTO_WEB_VIEW_ERROR_PAGE_CRASH", "crash" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("Libre_ImpuestoWebViewErrorPage", _libre_impuesto_web_view_error_page_values);
  }

  return type;
}




