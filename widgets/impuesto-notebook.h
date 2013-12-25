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

#ifndef IMPUESTO_NOTEBOOK_H
#define IMPUESTO_NOTEBOOK_H

#include "libre-impuesto.h"

G_BEGIN_DECLS

/* Drag & Drop target names. */
#define IMPUESTO_DND_URI_LIST_TYPE          "text/uri-list"
#define IMPUESTO_DND_TEXT_TYPE              "text/plain"
#define IMPUESTO_DND_URL_TYPE               "_NETSCAPE_URL"
#define IMPUESTO_DND_TOPIC_TYPE		"libre_impuesto_topic_node"


#define IMPUESTO_TYPE_NOTEBOOK		(impuesto_notebook_get_type ())
#define IMPUESTO_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), IMPUESTO_TYPE_NOTEBOOK, ImpuestoNotebook))
#define IMPUESTO_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), IMPUESTO_TYPE_NOTEBOOK, ImpuestoNotebookClass))
#define IMPUESTO_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), IMPUESTO_TYPE_NOTEBOOK))
#define IMPUESTO_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), IMPUESTO_TYPE_NOTEBOOK))
#define IMPUESTO_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), IMPUESTO_TYPE_NOTEBOOK, ImpuestoNotebookClass))

typedef struct _ImpuestoNotebookClass	ImpuestoNotebookClass;
typedef struct _ImpuestoNotebook	ImpuestoNotebook;
typedef struct _ImpuestoNotebookPrivate	ImpuestoNotebookPrivate;

struct _ImpuestoNotebook
{
  GtkNotebook parent;

  /*< private >*/
  ImpuestoNotebookPrivate *priv;
};

struct _ImpuestoNotebookClass
{
  GtkNotebookClass parent_class;

  /* Signals */
  void	 (* tab_close_req)  (ImpuestoNotebook *notebook,
			     GtkWidget *embed);
};

GType impuesto_notebook_get_type (void);

int impuesto_notebook_add_tab (ImpuestoNotebook *nb,
			       GtkWidget *embed,
			       int position,
			       gboolean jump_to);
	
void impuesto_notebook_set_show_tabs (ImpuestoNotebook *nb,
				      gboolean show_tabs);

GtkWidget * impuesto_notebook_new ();


gint 
libre_impuesto_notebook_find_tab_page (ImpuestoNotebook *notebook, 
				       const gchar * name );

G_END_DECLS

#endif /* IMPUESTO_NOTEBOOK_H */
