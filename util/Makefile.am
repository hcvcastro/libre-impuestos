# Process this file with automake to create Makefile.in
#
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
#


noinst_LTLIBRARIES = libutil.la

include_FILES = 				\
		util.h					

libutil_la_CPPFLAGS =							\
	$(AM_CPPFLAGS)							\
	-I$(top_srcdir)							\
	-I$(top_srcdir)/widgets						\
	-DG_LOG_DOMAIN=\"Libre-Impuesto-Util\"				\
	-DLIBRE_IMPUESTO_BINDIR=\""$(bindir)"\"				\
	-DLIBRE_IMPUESTO_DATADIR=\""$(datadir)"\"			\
	-DLIBRE_IMPUESTO_HELPDIR=\""$(libreimpuestohelpdir)"\"		\
	-DLIBRE_IMPUESTO_ICONDIR=\""$(icondir)"\"			\
	-DLIBRE_IMPUESTO_IMAGESDIR=\""$(imagesdir)"\"			\
	-DLIBRE_IMPUESTO_LIBDIR=\""$(datadir)"\"			\
	-DLIBRE_IMPUESTO_LIBEXECDIR=\""$(privlibexecdir)"\"		\
	-DLIBRE_IMPUESTO_LOCALEDIR=\""$(localedir)"\"			\
	-DLIBRE_IMPUESTO_PREFIX=\""$(prefix)"\"				\
	-DLIBRE_IMPUESTO_PRIVDATADIR=\""$(privdatadir)"\"		\
	-DLIBRE_IMPUESTO_SOUNDDIR=\""$(soundsdir)"\"			\
	-DLIBRE_IMPUESTO_SYSCONFDIR=\""$(sysconfdir)"\"			\
	-DLIBRE_IMPUESTO_TOOLSDIR=\""$(privlibexecdir)"\"		\
	-DLIBRE_IMPUESTO_UIDIR=\""$(uidir)"\"				\
	$(GNOME_PLATFORM_CFLAGS)

libutil_la_SOURCES =				\
	$(include_FILES)			\
	util.c					\
	$(PLATFORM_SOURCES)

libutil_la_LDFLAGS = $(NO_UNDEFINED)

libutil_la_LIBADD = 			\
	$(GNOME_PLATFORM_LIBS)		
	$(INTLLIBS)

BUILT_SOURCES = $(ENUM_GENERATED) $(MARSHAL_GENERATED) $(error_DATA)
CLEANFILES    = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)


