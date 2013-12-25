/*
 *  Copyright © 2002 Marco Pesenti Gritti
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

#ifndef EPHY_STRING_H
#define EPHY_STRING_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

gboolean  libre_impuesto_string_to_int	(const char *string,
				 gulong *integer);

char	 *libre_impuesto_string_blank_chr	(char *source);

char	 *libre_impuesto_string_shorten	(char *str,
				 gsize target_length);

char	 *libre_impuesto_string_collate_key_for_domain	(const char *host,
						 gssize len);

guint	  libre_impuesto_string_flags_from_string	(GType type,
					 const char *flags_string);

char     *libre_impuesto_string_flags_to_string	(GType type,
					 guint flags_value);

guint	  libre_impuesto_string_enum_from_string	(GType type,
					 const char *enum_string);

char     *libre_impuesto_string_enum_to_string	(GType type,
					 guint enum_value);

char     *libre_impuesto_string_canonicalize_pathname (const char *cpath);

char     *libre_impuesto_string_get_host_name (const char *url);

char     *libre_impuesto_string_expand_initial_tilde (const char *path);

char    **libre_impuesto_string_commandline_args_to_uris (char **arguments, GError **error);


G_END_DECLS

#endif
