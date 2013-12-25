/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/*
 *  Copyright © 2007 Xan Lopez
 *  Copyright © 2009 Igalia S.L.
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

#ifndef LIBRE_IMPUESTO_EMBED_H
#define LIBRE_IMPUESTO_EMBED_H

#include "libre-impuesto.h"

G_BEGIN_DECLS

#define LIBRE_IMPUESTO_TYPE_EMBED     (libre_impuesto_embed_get_type ())
#define LIBRE_IMPUESTO_EMBED(o)       (G_TYPE_CHECK_INSTANCE_CAST ((o), LIBRE_IMPUESTO_TYPE_EMBED, LibreImpuestoEmbed))
#define LIBRE_IMPUESTO_EMBED_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), LIBRE_IMPUESTO_TYPE_EMBED, LibreImpuestoEmbedClass))
#define LIBRE_IMPUESTO_IS_EMBED(o)    (G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBRE_IMPUESTO_TYPE_EMBED))
#define LIBRE_IMPUESTO_IS_EMBED_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), LIBRE_IMPUESTO_TYPE_EMBED))
#define LIBRE_IMPUESTO_EMBED_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), LIBRE_IMPUESTO_TYPE_EMBED, LibreImpuestoEmbedClass))

typedef struct _LibreImpuestoEmbedClass LibreImpuestoEmbedClass;
typedef struct _LibreImpuestoEmbed LibreImpuestoEmbed;
typedef struct _LibreImpuestoEmbedPrivate LibreImpuestoEmbedPrivate;

struct _LibreImpuestoEmbed {
  GtkBox parent_instance;

  /*< private >*/
  LibreImpuestoEmbedPrivate *priv;
};

struct _LibreImpuestoEmbedClass {
  GtkBoxClass parent_class;
};

GType libre_impuesto_embed_get_type (void);

WebKitWebView*
libre_impuesto_embed_get_web_view (LibreImpuestoEmbed *embed);


G_END_DECLS

#endif  // LIBRE_IMPUESTO_EMBED_H

