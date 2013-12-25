## Process this file with automake to create Makefile.in
# Copyright 1997-1998, 2005-2010 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Excecutable
bin_PROGRAMS = libre-impuestos

# Impuesto library
noinst_LTLIBRARIES = libimpuesto.la

include_FILES =				\
	libre-impuesto.h		\
	libre-impuesto-window.h		\
	libre-impuesto-common.h		\
	libre-impuesto-window-private.h \
	libre-impuesto-window-actions.h \
	impuesto-command.h


libimpuesto_la_CPPFLAGS =						\
	$(AM_CPPFLAGS)						\
	$(NM_CPPFLAGS)						\
	-I$(top_srcdir)/widgets					\
	-I$(top_srcdir)/impuesto				\
	-I$(top_srcdir)						\
	-DG_LOG_DOMAIN=\"Libre-Impuesto\"			\
	-DLIBRE_IMPUESTO_BINDIR=\""$(bindir)"\"			\
	-DLIBRE_IMPUESTO_IMAGESDIR=\""$(imagesdir)"\"		\
	-DLIBRE_IMPUESTO_LOCALEDIR=\""$(localedir)"\"		\
	-DLIBRE_IMPUESTO_DATADIR=\""$(datadir)"\"		\
	-DLIBRE_IMPUESTO_XML=\""$(privdatadir)/data"\"		\
	-DLIBRE_IMPUESTO_CSS=\""$(privdatadir)/css"\"		\
	-DLIBRE_IMPUESTO_HELPDIR=\""$(libreimpuestohelpdir)"\"	\
	-DLIBRE_IMPUESTO_ICONDIR=\""$(icondir)"\"		\
	-DLIBRE_IMPUESTO_UIDIR=\""$(uidir)"\"			\
	-DLIBRE_IMPUESTO_BASE_URL=\""$(privdatadir)/form"\"	\
	-DPREFIX=\""$(prefix)"\"				\
	-DSYSCONFDIR=\""$(sysconfdir)"\"			\
	-DDATADIR=\""$(datadir)"\"				\
	-DLIBDIR=\""$(datadir)"\"				\
	$(GNOME_PLATFORM_CFLAGS)				


libimpuesto_la_SOURCES =			\
	$(include_FILES)		\
	libre-impuesto.c			\
	libre-impuesto-window.c			\
	libre-impuesto-window-actions.c		\
	libre-impuesto-window-private.c		\
	impuesto-command.c

libimpuesto_la_LDFLAGS = $(NO_UNDEFINED)


libimpuesto_la_LIBADD =					\
	$(top_builddir)/widgets/libwidgets.la		\
	$(GNOME_PLATFORM_LIBS)				

# libre-impuestos executable

libre_impuestos_CPPFLAGS =					\
	$(AM_CPPFLAGS)						\
	$(NM_CPPFLAGS)						\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/widgets					\
	-I$(top_srcdir)/impuesto				\
	-DLIBRE_IMPUESTO_BINDIR=\""$(bindir)"\"			\
	-DLIBRE_IMPUESTO_IMAGESDIR=\""$(imagesdir)"\"		\
	-DLIBRE_IMPUESTO_LOCALEDIR=\""$(localedir)"\"		\
	-DLIBRE_IMPUESTO_DATADIR=\""$(datadir)"\"		\
	-DLIBRE_IMPUESTO_HELPDIR=\""$(libreimpuestohelpdir)"\"	\
	-DLIBRE_IMPUESTO_UIDIR=\""$(uidir)"\"			\
	-DPREFIX=\""$(prefix)"\"				\
	-DLIBRE_IMPUESTO_THEMEDIR=\"$(privdatadir)/theme\" 	\
	-DSYSCONFDIR=\""$(sysconfdir)"\"			\
	-DDATADIR=\""$(datadir)"\"				\
	-DLIBDIR=\""$(datadir)"\"				\
	$(GNOME_PLATFORM_CFLAGS)				

libre_impuestos_SOURCES = main.c

libre_impuestos_LDADD =							\
	libimpuesto.la							\
	$(top_builddir)/util/libutil.la					\
	$(top_builddir)/widgets/libwidgets.la				\
	$(GNOME_PLATFORM_LIBS)						\
	$(EVOLUTIONICON)


