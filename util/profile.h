/*
 *  Copyright © 2009 Xan López
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

#ifndef LIBRE_IMPUESTO_PROFILE_UTILS_H
#define LIBRE_IMPUESTO_PROFILE_UTILS_H

#include <glib.h>
#include <gnome-keyring.h>

#define FORM_USERNAME_KEY "form_username"
#define FORM_PASSWORD_KEY "form_password"

#define LIBRE_IMPUESTO_PROFILE_MIGRATION_VERSION 5

int profile_utils_get_migration_version (void);

gboolean profile_utils_set_migration_version (int version);

void _profile_utils_store_form_auth_data (const char *uri,
                                               const char *form_username,
                                               const char *form_password,
                                               const char *username,
                                               const char *password);

void
_profile_utils_query_form_auth_data (const char *uri,
                                          const char *form_username,
                                          const char *form_password,
                                          GnomeKeyringOperationGetListCallback callback,
                                          gpointer data,
                                          GDestroyNotify destroy_data);

#endif
