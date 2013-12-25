/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 *  Written December 2013 by Henry Castro <hcvcastro@gmail.com>.  
 */

#include "config.h"

#include "file-helpers.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include <gdk/gdk.h>
#include <libxml/xmlreader.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define LIBRE_IMPUESTO_UUID		"0d82d98f-7079-401c-abff-203fcde1ece3"
#define LIBRE_IMPUESTO_UUID_ENVSTRING	LIBRE_IMPUESTO_UUID_ENVVAR "=" LIBRE_IMPEUESTO_UUID

#define DELAY_MAX_TICKS	64
#define INITIAL_TICKS	2

static GHashTable *files = NULL;
static GHashTable *mime_table = NULL;

static gboolean keep_temp_directory = FALSE; /* for debug purposes */
static char *dot_dir = NULL;
static char *tmp_dir = NULL;
static GList *del_on_exit = NULL;

GQuark libre_impuesto_file_helpers_error_quark;

const char *
libre_impuesto_file_tmp_dir (void)
{
	if (tmp_dir == NULL)
	{
		char *partial_name;
		char *full_name;

		partial_name = g_strconcat ("epiphany-", g_get_user_name (),
					    "-XXXXXX", NULL);
		full_name = g_build_filename (g_get_tmp_dir (), partial_name,
					      NULL);
#ifdef HAVE_MKDTEMP
		tmp_dir = mkdtemp (full_name);
#else
#error no mkdtemp implementation
#endif
		g_free (partial_name);

		if (tmp_dir == NULL)
		{
			g_free (full_name);
		}
	}

	return tmp_dir;
}

char *
libre_impuesto_file_get_downloads_dir (void)
{
	return NULL;
}

char *
libre_impuesto_file_desktop_dir (void)
{
	const char *xdg_desktop_dir;

	xdg_desktop_dir = g_get_user_special_dir (G_USER_DIRECTORY_DESKTOP);
	if (xdg_desktop_dir != NULL)
		return g_strdup (xdg_desktop_dir);

	/* If we don't have XDG user dirs info, return an educated guess. */
	return g_build_filename	(g_get_home_dir (), _("Desktop"), NULL);
}

char *
libre_impuesto_file_tmp_filename (const char *base,
			const char *extension)
{
	int fd;
	char *name = g_strdup (base);

	fd = g_mkstemp (name);

	if (fd != -1)
	{
		unlink (name);
		close (fd);
	}
	else
	{
		g_free (name);

		return NULL;
	}

	if (extension)
	{
		char *tmp;
		tmp = g_strconcat (name, ".",
				   extension, NULL);
		g_free (name);
		name = tmp;
	}

	return name;
}

const char *
libre_impuesto_file (const char *filename)
{
	g_warning ("Failed to find %s\n", filename);

	return NULL;
}

const char *
libre_impuesto_dot_dir (void)
{
	return dot_dir;
}

gboolean
libre_impuesto_file_helpers_init (const char *profile_dir,
			gboolean private_profile,
			gboolean keep_temp_dir,
			GError **error)
{
	const char *uuid;

	/* See if we've been calling ourself, and abort if we have */
	uuid = g_getenv (LIBRE_IMPUESTO_UUID_ENVVAR);
	if (uuid && strcmp (uuid, LIBRE_IMPUESTO_UUID) == 0)
	{
		g_warning ("Self call detected, exiting!\n");
		exit (1);
	}

	/* Put marker in env */
	g_setenv (LIBRE_IMPUESTO_UUID_ENVVAR, LIBRE_IMPUESTO_UUID, TRUE);

	libre_impuesto_file_helpers_error_quark = g_quark_from_static_string ("libre-impuesto-file-helpers-error");

	files = g_hash_table_new_full (g_str_hash,
				       g_str_equal,
				       (GDestroyNotify) g_free,
				       (GDestroyNotify) g_free);

	keep_temp_directory = keep_temp_dir;

	if (private_profile && profile_dir != NULL)
	{
		dot_dir = g_strdup (profile_dir);
	}
	else if (private_profile)
	{
		if (libre_impuesto_file_tmp_dir () == NULL)
		{
			g_set_error (error,
				     LIBRE_IMPUESTO_FILE_HELPERS_ERROR_QUARK,
				     0,
				     _("Could not create a temporary directory in “%s”."),
				     g_get_tmp_dir ());
			return FALSE;
		}

		dot_dir = g_build_filename (libre_impuesto_file_tmp_dir (),
					    "epiphany",
					    NULL);
	}
	else
	{
		dot_dir = g_build_filename (g_get_home_dir (),
					    GNOME_DOT_GNOME,
					    "epiphany",
					    NULL);
	}
	
	return libre_impuesto_ensure_dir_exists (libre_impuesto_dot_dir (), error);
}

static void
delete_files (GList *l)
{
	for (; l != NULL; l = l->next)
	{
		unlink (l->data);
	}
}

void
libre_impuesto_file_helpers_shutdown (void)
{
	g_hash_table_destroy (files);

	del_on_exit = g_list_reverse (del_on_exit);
	delete_files (del_on_exit);
	g_list_foreach (del_on_exit, (GFunc)g_free, NULL);
	g_list_free (del_on_exit);
	del_on_exit = NULL;

	if (mime_table != NULL)
	{
		g_hash_table_destroy (mime_table);
		mime_table = NULL;
	}

	g_free (dot_dir);
	dot_dir = NULL;

	if (tmp_dir != NULL)
	{
		if (!keep_temp_directory)
		{
			GFile *tmp_dir_file;
			tmp_dir_file = g_file_new_for_path (tmp_dir);

			/* recursively delete the contents and the
			 * directory */
				libre_impuesto_file_delete_dir_recursively (tmp_dir_file,
							  NULL);
			g_object_unref (tmp_dir_file);
		}

		g_free (tmp_dir);
		tmp_dir = NULL;
	}
}

gboolean
libre_impuesto_ensure_dir_exists (const char *dir,
		        GError **error)
{
	if (g_file_test (dir, G_FILE_TEST_EXISTS) &&
	    !g_file_test (dir, G_FILE_TEST_IS_DIR))
	{
		g_set_error (error,
			     LIBRE_IMPUESTO_FILE_HELPERS_ERROR_QUARK,
			     0,
			     _("The file “%s” exists. Please move it out of the way."),
			     dir);
		return FALSE;
	}

	if (!g_file_test (dir, G_FILE_TEST_EXISTS) &&
            g_mkdir_with_parents (dir, 488) != 0)
	{
		g_set_error (error,
			     LIBRE_IMPUESTO_FILE_HELPERS_ERROR_QUARK,
			     0,
			     _("Failed to create directory “%s”."),
			     dir);
		return FALSE;
	}

	return TRUE;
}

static void
libre_impuesto_find_file_recursive (const char *path,
			  const char *fname,
			  GSList **list,
			  gint depth,
			  gint maxdepth)
{
	GDir *dir;
	const gchar *file;

	dir = g_dir_open (path, 0, NULL);
	if (dir != NULL)
	{
		while ((file = g_dir_read_name (dir)))
		{
			if (depth < maxdepth)
			{
				char *new_path = g_build_filename (path, file, NULL);
				libre_impuesto_find_file_recursive (new_path, fname, list,
							  depth + 1, maxdepth);
				g_free (new_path);
			}
			if (strcmp (file, fname) == 0)
			{
				char *new_path = g_build_filename (path, file, NULL);
				*list = g_slist_prepend (*list, new_path);
			}
		}

		g_dir_close (dir);
	}
}

GSList *
libre_impuesto_file_find (const char *path,
	        const char *fname,
	        gint maxdepth)
{
	GSList *ret = NULL;
	libre_impuesto_find_file_recursive (path, fname, &ret, 0, maxdepth);
	return ret;
}

gboolean
libre_impuesto_file_switch_temp_file (GFile *file_dest,
			    GFile *file_temp)
{
	char *file_dest_path, *file_temp_path;
	char *backup_path;
	gboolean dest_exists;
	gboolean retval = TRUE;
	GFile *backup_file;

	file_dest_path = g_file_get_path (file_dest);
	file_temp_path = g_file_get_path (file_temp);

	dest_exists = g_file_test (file_dest_path, G_FILE_TEST_EXISTS);

	backup_path = g_strconcat (file_dest_path, ".old", NULL);
	backup_file = g_file_new_for_path (backup_path);

	if (dest_exists)
	{
		if (g_file_move (file_dest, backup_file,
				 G_FILE_COPY_OVERWRITE,
				 NULL, NULL, NULL, NULL) == FALSE)
		{
			g_warning ("Failed to backup %s to %s",
				   file_dest_path, backup_path);

			retval = FALSE;
			goto failed;
		}
	}

	if (g_file_move (file_temp, file_dest,
			 G_FILE_COPY_OVERWRITE,
			 NULL, NULL, NULL, NULL) == FALSE)
	{
		g_warning ("Failed to replace %s with %s",
			   file_temp_path, file_dest_path);

		if (g_file_move (backup_file, file_dest,
				 G_FILE_COPY_OVERWRITE,
				 NULL, NULL, NULL, NULL) == FALSE)
		{
			g_warning ("Failed to restore %s from %s",
				   file_dest_path, file_temp_path);
		}

		retval = FALSE;
		goto failed;
	}

	if (dest_exists)
	{
		if (g_file_delete (backup_file,
				   NULL, NULL) == FALSE)
		{
			g_warning ("Failed to delete old file %s", backup_path);
		}
	}

failed:
	g_free (file_dest_path);
	g_free (file_temp_path);

	g_free (backup_path);
	g_object_unref (backup_file);

	return retval;
}

void
libre_impuesto_file_delete_on_exit (GFile *file)
{
	/* does nothing now */
}

void
libre_impuesto_file_add_recent_item (const char *uri,
			   const char *mime_type)
{	
	GtkRecentManager *manager = gtk_recent_manager_get_default ();

	g_return_if_fail (mime_type != NULL && uri != NULL);

	gtk_recent_manager_add_item (manager, uri);
}

static void
load_mime_from_xml (void)
{
	xmlTextReaderPtr reader;
	const char *xml_file;
	int ret;
	LibreImpuestoMimePermission permission = LIBRE_IMPUESTO_MIME_PERMISSION_UNKNOWN;

	g_return_if_fail (mime_table == NULL);

	mime_table = g_hash_table_new_full (g_str_hash, g_str_equal,
					    xmlFree, NULL);

	xml_file = libre_impuesto_file ("mime-types-permissions.xml");
	if (xml_file == NULL)
	{
		g_warning ("MIME types permissions file not found!\n");
		return;
	}

	reader = xmlNewTextReaderFilename (xml_file);
	if (reader == NULL)
	{
		g_warning ("Could not load MIME types permissions file!\n");
		return;
	}

	ret = xmlTextReaderRead (reader);
	while (ret == 1)
	{
		const xmlChar *tag;
		xmlReaderTypes type;

		tag = xmlTextReaderConstName (reader);
		type = xmlTextReaderNodeType (reader);

		if (xmlStrEqual (tag, (const xmlChar *)"safe") && type == XML_READER_TYPE_ELEMENT)
		{
			permission = LIBRE_IMPUESTO_MIME_PERMISSION_SAFE;
		}
		else if (xmlStrEqual (tag, (const xmlChar *)"unsafe") && type == XML_READER_TYPE_ELEMENT)
		{
			permission = LIBRE_IMPUESTO_MIME_PERMISSION_UNSAFE;
		}
		else if (xmlStrEqual (tag, (const xmlChar *)"mime-type"))
		{
			xmlChar *type;

			type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"type");
			g_hash_table_insert (mime_table, type,
					     GINT_TO_POINTER (permission));
		}

		ret = xmlTextReaderRead (reader);
	}

	xmlFreeTextReader (reader);
}

LibreImpuestoMimePermission
libre_impuesto_file_check_mime (const char *mime_type)
{
	LibreImpuestoMimePermission permission;
	gpointer tmp;

	g_return_val_if_fail (mime_type != NULL, LIBRE_IMPUESTO_MIME_PERMISSION_UNKNOWN);

	if (mime_table == NULL)
	{
		load_mime_from_xml ();
	}

	tmp = g_hash_table_lookup (mime_table, mime_type);
	if (tmp == NULL)
	{
		permission = LIBRE_IMPUESTO_MIME_PERMISSION_UNKNOWN;
	}
	else
	{
		permission = GPOINTER_TO_INT (tmp);
	}

	return permission;
}

gboolean
libre_impuesto_file_launch_application (GAppInfo *app,
			      GList *files,
			      guint32 user_time,
			      GtkWidget *widget)
{
	GdkAppLaunchContext *context;
	GdkDisplay *display;
	GdkScreen *screen;
	gboolean res;

	if (widget)
	{
		display = gtk_widget_get_display (widget);
		screen = gtk_widget_get_screen (widget);
	}
	else
	{
		display = gdk_display_get_default ();
		screen = gdk_screen_get_default ();
	}

	context = gdk_display_get_app_launch_context (display);
	gdk_app_launch_context_set_screen (context, screen);
	gdk_app_launch_context_set_timestamp (context, user_time);

	res = g_app_info_launch (app, files,
				 G_APP_LAUNCH_CONTEXT (context), NULL);
	g_object_unref (context);

	return res;
}

gboolean
libre_impuesto_file_launch_desktop_file (const char *filename,
			       const char *parameter,
			       guint32 user_time,
			       GtkWidget *widget)
{
	GDesktopAppInfo *app;
	GFile *file = NULL;
	GList *list = NULL;
	gboolean ret;

	app = g_desktop_app_info_new (filename);
	if (parameter)
	{
		file = g_file_new_for_path (parameter);
		list = g_list_append (list, file);
	}
	
	ret = libre_impuesto_file_launch_application (G_APP_INFO (app), list, user_time, widget);
	g_list_free (list);
	if (file)
		g_object_unref (file);
	return ret;
}

gboolean
libre_impuesto_file_launch_handler (const char *mime_type,
			  GFile *file,
			  guint32 user_time)
{
	GAppInfo *app = NULL;
	gboolean ret = FALSE;

	g_return_val_if_fail (file != NULL, FALSE);

	if (mime_type != NULL)
	{
		app = g_app_info_get_default_for_type (mime_type,
						       FALSE);
	}
	else
	{
		GFileInfo *file_info;
		char *type;

		/* Sniff mime type and check if it's safe to open */
		file_info = g_file_query_info (file,
					       G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
					       0, NULL, NULL);
		if (file_info == NULL) {
			return FALSE;
		}
		type = g_strdup (g_file_info_get_content_type (file_info));
		
		g_object_unref (file_info);

		if (type != NULL && type[0] != '\0' &&
		    libre_impuesto_file_check_mime (type) == LIBRE_IMPUESTO_MIME_PERMISSION_SAFE)
		{
			/* FIXME rename tmp file to right extension ? */
			app = g_app_info_get_default_for_type (type, FALSE);
		}
		g_free (type);
	}

	if (app != NULL)
	{
		GList *list = NULL;
		
		list = g_list_append (list, file);
		ret = libre_impuesto_file_launch_application (app, list, user_time, NULL);
		g_list_free (list);
	}
	else
		ret = FALSE;

	return ret;
}

gboolean
libre_impuesto_file_browse_to (GFile *file,
		     guint32 user_time)
{
	return libre_impuesto_file_launch_handler ("inode/directory", file, user_time);
}

gboolean
libre_impuesto_file_delete_dir_recursively (GFile *directory, GError **error)
{
	GFileEnumerator *children = NULL;
	GFileInfo *info;
	gboolean ret = TRUE;

	children = g_file_enumerate_children (directory,
					      "standard::name,standard::type",
					      0, NULL, error);
	if (error)
		goto out;

	info = g_file_enumerator_next_file (children, NULL, error);
	while (info || error) {
		GFile *child;
		const char *name;
		GFileType type;

		if (error)
			goto out;

		name = g_file_info_get_name (info);
		child = g_file_get_child (directory, name);
		type = g_file_info_get_file_type (info);

	
		if (type == G_FILE_TYPE_DIRECTORY)
			ret = libre_impuesto_file_delete_dir_recursively (child, error);
		else if (type == G_FILE_TYPE_REGULAR)
			ret =	g_file_delete (child, NULL, error);

		g_object_unref (info);

		if (!ret)
			goto out;

		info = g_file_enumerator_next_file (children, NULL, error);
	}

	ret = TRUE;

	g_file_delete (directory, NULL, error);

out:
	if (children)
		g_object_unref (children);

	return ret;
}

void
libre_impuesto_file_delete_uri (const char *uri)
{
	GFile *file;
	gboolean ret;

	g_return_if_fail (uri);

	file = g_file_new_for_uri (uri);

	ret = g_file_delete (file, NULL, NULL);

	if (ret == TRUE)
	{
	}
	else
	{
	}
	g_object_unref (file);
}

static gchar *
get_accels_filename (void)
{
        const char *home;

        home = g_get_home_dir();
        if (!home)
                return NULL;
        return g_build_filename (home, GNOME_DOT_GNOME, "accels", PACKAGE, NULL);
}

void
libre_impuesto_file_load_accels (void)
{
        char *filename;

        filename = get_accels_filename ();
        if (!filename)
                return;

        gtk_accel_map_load (filename);
        g_free (filename);
}

void
libre_impuesto_file_save_accels (void)
{
        char *filename;

        filename = get_accels_filename ();
        if (!filename)
                return;

        gtk_accel_map_save (filename);
        g_free (filename);
}
