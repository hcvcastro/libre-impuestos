/*
 *  Copyright © 2002 Jorn Baayen
 *  Copyright © 2003 Marco Pesenti Gritti
 *  Copyright © 2003 Christian Persch
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
 *
 * Written December 2013 by Henry Castro <hcvcastro@gmail.com>.  
 */

#ifndef IMPUESTO_COMMNAD_H
#define IMPUESTO_COMMAND_H

#include "libre-impuesto.h"

G_BEGIN_DECLS

#define LIBRE_IMPUESTO_TYPE_COMMAND	(libre_impuesto_command_get_type ())
#define LIBRE_IMPUESTO_COMMAND(o)	(G_TYPE_CHECK_INSTANCE_CAST ((o), LIBRE_IMPUESTO_TYPE_COMMAND, LibreImpuestoCommand))
#define LIBRE_IMPUESTO_COMMAND_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), LIBRE_IMPUESTO_TYPE_COMMAND, LibreImpuestoCommandClass))
#define LIBRE_IMPUESTO_IS_COMMAND(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBRE_IMPUESTO_TYPE_COMMAND))
#define LIBRE_IMPUESTO_IS_COMMAND_CLASS(k)(G_TYPE_CHECK_CLASS_TYPE ((k), LIBRE_IMPUESTO_TYPE_COMMAND))
#define LIBRE_IMPUESTO_COMMAND_GET_CLASS(o)(G_TYPE_INSTANCE_GET_CLASS ((o), LIBRE_IMPUESTO_TYPE_COMMAND, LibreImpuestoCommandClass))

typedef struct _LibreImpuestoCommand		LibreImpuestoCommand;
typedef struct _LibreImpuestoCommandPrivate	LibreImpuestoCommandPrivate;
typedef struct _LibreImpuestoCommandClass	LibreImpuestoCommandClass;

typedef enum
{
  IMPUESTO_COMMAND_START,
  IMPUESTO_COMMAND_NOT_IMPLEMENTED,
  IMPUESTO_COMMAND_MAIN_WINDOW,
  IMPUESTO_COMMAND_OPEN_FORM,
  IMPUESTO_COMMAND_SYNC_SELECTION,
  IMPUESTO_COMMAND_LAST
} ImpuestoCommand;

struct _LibreImpuestoCommand
{
  GObject parent;

  /*< private >*/
  LibreImpuestoCommandPrivate *priv;
};

struct _LibreImpuestoCommandClass
{
  GObjectClass parent_class;
};

GType libre_impuesto_command_get_type (void);
void  libre_impuesto_queue_command (LibreImpuestoCommand *impuesto_command,
				    ImpuestoCommand command,
				    const char * arg,
				    gboolean priority);
void
libre_impuesto_command_process(ImpuestoCommand command, const gchar *args);

LibreImpuestoCommand *
libre_impuesto_get_command(LibreImpuesto *impuesto);


G_END_DECLS

#endif
