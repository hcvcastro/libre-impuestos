/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 *  Copyright © 2002 Jorn Baayen
 *  Copyright © 2003, 2004 Marco Pesenti Gritti
 *  Copyright © 2003, 2004, 2005, 2006, 2008 Christian Persch
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
 * Written December 2013 by Henry Castro <hcvcastro@gmail.com>.  
 */

#include "gtkstack.h"
#include "impuesto-command.h"
#include "libre-impuesto-window-private.h"

typedef struct
{
	ImpuestoCommand command;
	gchar * arg;
} Command;

#define LIBRE_IMPUESTO_COMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LIBRE_IMPUESTO_TYPE_COMMAND, LibreImpuestoCommandPrivate))

struct _LibreImpuestoCommandPrivate
{
  GList *windows;
  GList *tool_windows;
  GtkWidget *resume_infobar;
  GtkWidget *resume_window;

  GQueue *queue;
  guint queue_idle_id;

  GtkWidget *quit_interact_dialog;

  guint dont_save : 1;
  guint quit_while_resuming : 1;
  guint loading_homepage : 1;
};

#define BOOKMARKS_EDITOR_ID	"BookmarksEditor"
#define HISTORY_WINDOW_ID	"HistoryWindow"
#define SESSION_CRASHED		"type:session_crashed"

enum
{
  PROP_0,
  PROP_ACTIVE_WINDOW
};

G_DEFINE_TYPE (LibreImpuestoCommand, libre_impuesto_command, G_TYPE_OBJECT)


static void
libre_impuesto_command_free (Command *command)
{
  g_assert (command != NULL);
  g_free (command->arg);
  g_free (command);
}

static int
libre_impuesto_command_find (const Command *cmd,
			     gpointer cmdptr)
{
  ImpuestoCommand command = GPOINTER_TO_INT (cmdptr);

  return command != cmd->command;
}


static void
libre_impuesto_command_queue_clear (LibreImpuestoCommand *impuesto_command)
{
  LibreImpuestoCommandPrivate *priv = impuesto_command->priv;

  if (priv->queue_idle_id != 0)	{
    g_source_remove (priv->queue_idle_id);
    priv->queue_idle_id = 0;
  }

  if (priv->queue != NULL) {
    g_queue_foreach (priv->queue, (GFunc)libre_impuesto_command_free, NULL);
    g_queue_free (priv->queue);
    priv->queue = NULL;
  }
}

static void
libre_impuesto_command_dispose (GObject *object)
{
  LibreImpuestoCommand *impuesto_command = LIBRE_IMPUESTO_COMMAND (object);
  //ImpuestoCommandPrivate *priv = command->priv;

  libre_impuesto_command_queue_clear (impuesto_command);

  G_OBJECT_CLASS (libre_impuesto_command_parent_class)->dispose (object);
}


static void
libre_impuesto_command_finalize (GObject *object)
{
  //ImpuestoCommand *session = IMPUESTO_COMMAND (object);

  G_OBJECT_CLASS (libre_impuesto_command_parent_class)->finalize (object);
}

static gboolean
libre_impuesto_command_dispatch (LibreImpuestoCommand *impuesto_command)
{
  Command *cmd;
  gboolean run_again = TRUE;
  LibreImpuestoCommandPrivate *priv = impuesto_command->priv;

  cmd = g_queue_pop_head (priv->queue);
  g_assert (cmd != NULL);

  libre_impuesto_command_process(cmd->command, cmd->arg);

  /* Look if there's anything else to dispatch */
  if (g_queue_is_empty (priv->queue)) {
    priv->queue_idle_id = 0;
    run_again = FALSE;
  }

  return run_again;
}

static void
libre_impuesto_command_queue_next (LibreImpuestoCommand *impuesto_command)
{
  LibreImpuestoCommandPrivate *priv = impuesto_command->priv;

  if (!g_queue_is_empty (priv->queue) &&
      priv->queue_idle_id == 0) {
    priv->queue_idle_id = g_idle_add (
			      (GSourceFunc) libre_impuesto_command_dispatch,
			      impuesto_command);
  }
}

void
libre_impuesto_queue_command (LibreImpuestoCommand *impuesto_command,
			      ImpuestoCommand command,
			      const char * arg,
			      gboolean priority)
{
  LibreImpuestoCommandPrivate *priv;
  GList *element;
  Command *cmd;

  g_return_if_fail (LIBRE_IMPUESTO_IS_COMMAND (impuesto_command));

  priv = impuesto_command->priv;

  /* First look if the same command is already queued */
  if (command > IMPUESTO_COMMAND_START && command < IMPUESTO_COMMAND_LAST) {
    element = g_queue_find_custom (priv->queue, GINT_TO_POINTER (command),
				   (GCompareFunc) libre_impuesto_command_find);
    if (element != NULL) {
      return;
    }
  }

  cmd = g_new0 (Command, 1);
  cmd->arg = arg ? g_strdup (arg) : NULL;
  cmd->command = command;

  if (priority) {
    g_queue_push_head (priv->queue, cmd);
  }
  else {
    g_queue_push_tail (priv->queue, cmd);
  }

  libre_impuesto_command_queue_next (impuesto_command);

}

static void
libre_impuesto_command_init (LibreImpuestoCommand *impuesto_command)
{
  LibreImpuestoCommandPrivate *priv;

  priv = impuesto_command->priv = LIBRE_IMPUESTO_COMMAND_GET_PRIVATE (impuesto_command);
  priv->queue = g_queue_new ();
}



static void
libre_impuesto_command_class_init (LibreImpuestoCommandClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = libre_impuesto_command_dispose;
  object_class->finalize = libre_impuesto_command_finalize;

  g_type_class_add_private (object_class, sizeof (LibreImpuestoCommandPrivate));
}


