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


#ifndef LIBRE_IMPUESTO_EMBED_TYPE_BUILTINS_H
#define LIBRE_IMPUESTO_EMBED_TYPE_BUILTINS_H 1

#include <glib-object.h>

G_BEGIN_DECLS


/* --- libre_impuesto-adblock.h --- */
#define LIBRE_IMPUESTO_TYPE_URI_CHECK_TYPE ad_uri_check_type_get_type()
GType ad_uri_check_type_get_type (void);

/* --- libre_impuesto-download.h --- */
#define LIBRE_IMPUESTO_TYPE_DOWNLOAD_ACTION_TYPE libre_impuesto_download_action_type_get_type()
GType libre_impuesto_download_action_type_get_type (void);

/* --- libre_impuesto-embed-shell.h --- */
#define LIBRE_IMPUESTO_TYPE_EMBED_SHELL_MODE libre_impuesto_embed_shell_mode_get_type()
GType libre_impuesto_embed_shell_mode_get_type (void);

/* --- libre_impuesto-permission-manager.h --- */
#define LIBRE_IMPUESTO_TYPE_PERMISSION libre_impuesto_permission_get_type()
GType libre_impuesto_permission_get_type (void);

/* --- libre_impuesto-web-view.h --- */
#define LIBRE_IMPUESTO_TYPE_WEB_VIEW_NAVIGATION_FLAGS libre_impuesto_web_view_navigation_flags_get_type()
GType libre_impuesto_web_view_navigation_flags_get_type (void);
#define LIBRE_IMPUESTO_TYPE_WEB_VIEW_CHROME libre_impuesto_web_view_chrome_get_type()
/*GType libre_impuesto_web_view_chrome_get_type (void);
  #define LIBRE_IMPUESTO_TYPE_WEB_VIEW_SECURITY_LEVEL libre_impuesto_web_view_security_level_get_type()*/
/*GType libre_impuesto_web_view_security_level_get_type (void);
  #define LIBRE_IMPUESTO_TYPE_WEB_VIEW_DOCUMENT_TYPE libre_impuesto_web_view_document_type_get_type()*/
GType libre_impuesto_web_view_document_type_get_type (void);
#define LIBRE_IMPUESTO_TYPE_WEB_VIEW_ERROR_PAGE libre_impuesto_web_view_error_page_get_type()
GType libre_impuesto_web_view_error_page_get_type (void);
G_END_DECLS

#endif /* LIBRE_IMPUESTO_EMBED_TYPE_BUILTINS_H */



