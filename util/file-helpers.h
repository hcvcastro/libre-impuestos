/*
 *  Copyright © 2002 Jorn Baayen
 *  Copyright © 2003, 2004 Marco Pesenti Gritti
 *  Copyright © 2004, 2005, 2006 Christian Persch
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

#ifndef LIBRE_IMPUESTO_FILE_HELPERS_H
#define LIBRE_IMPUESTO_FILE_HELPERS_H

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

/* From libgnome */
#define GNOME_DOT_GNOME ".gnome2"

extern GQuark libre_impuesto_file_helpers_error_quark;
#define LIBRE_IMPUESTO_FILE_HELPERS_ERROR_QUARK	(libre_impuesto_file_helpers_error_quark)

G_BEGIN_DECLS

typedef enum
{
	LIBRE_IMPUESTO_MIME_PERMISSION_SAFE	= 1,
	LIBRE_IMPUESTO_MIME_PERMISSION_UNSAFE	= 2,
	LIBRE_IMPUESTO_MIME_PERMISSION_UNKNOWN	= 3
} LibreImpuestoMimePermission;

#define LIBRE_IMPUESTO_UUID_ENVVAR	"LIBRE_IMPUESTO_UNIQUE"

gboolean           libre_impuesto_file_helpers_init        (const char  *profile_dir,
                                                  gboolean     private_profile,
                                                  gboolean     keep_temp_dir,
                                                  GError     **error);
const char *       libre_impuesto_file                     (const char  *filename);
const char *       libre_impuesto_dot_dir                  (void);
void               libre_impuesto_file_helpers_shutdown    (void);
char	   *       libre_impuesto_file_get_downloads_dir   (void);
char       *       libre_impuesto_file_desktop_dir         (void);
const char *       libre_impuesto_file_tmp_dir             (void);
char       *       libre_impuesto_file_tmp_filename        (const char  *base,
                                                  const char  *extension);
gboolean           libre_impuesto_ensure_dir_exists        (const char  *dir,
                                                  GError **);
GSList     *       libre_impuesto_file_find                (const char  *path,
                                                  const char  *fname,
                                                  gint         maxdepth);
gboolean           libre_impuesto_file_switch_temp_file    (GFile       *file_dest,
                                                  GFile       *file_temp);
void               libre_impuesto_file_delete_on_exit      (GFile       *file);
void               libre_impuesto_file_add_recent_item     (const char  *uri,
                                                  const char  *mime_type);
LibreImpuestoMimePermission libre_impuesto_file_check_mime          (const char  *mime_type);
gboolean           libre_impuesto_file_launch_desktop_file (const char  *filename,
                                                  const char  *parameter,
                                                  guint32      user_time,
                                                  GtkWidget   *widget);
gboolean           libre_impuesto_file_launch_application  (GAppInfo    *app,
                                                  GList       *files,
                                                  guint32      user_time,
                                                  GtkWidget   *widget);
gboolean           libre_impuesto_file_launch_handler      (const char  *mime_type,
                                                  GFile       *file,
                                                  guint32      user_time);
gboolean           libre_impuesto_file_browse_to           (GFile       *file,
                                                  guint32      user_time);
gboolean           libre_impuesto_file_delete_dir_recursively (GFile *file,
                                                     GError      **error);
void               libre_impuesto_file_delete_uri          (const char  *uri);
void               libre_impuesto_file_load_accels (void);
void               libre_impuesto_file_save_accels (void);

G_END_DECLS

#endif /* LIBRE_IMPUESTO_FILE_HELPERS_H */
