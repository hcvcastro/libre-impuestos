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


SUBDIRS = images icons

desktopdir = $(datadir)/applications
desktop_in_files = libre-impuestos.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

xmldir = $(privdatadir)/data
cssdir = $(privdatadir)/css
uidir = $(privdatadir)/ui
htmldir = $(privdatadir)/form

dist_ui_DATA =				\
	libre-impuestos-menu.ui

dist_css_DATA =				\
	libre-impuestos.css

dist_xml_DATA =				\
	tree-data-consultor.xml		\
	tree-data-formulario.xml	\
	tree-data-general.xml

dist_html_DATA =			\
	test.html			\
	user.form			\
	F110-independientes.form	\
	F110-dependientes.form

CLEANFILES =					\
	$(desktop_DATA)

EXTRA_DIST =					\
	$(desktop_in_files)			

DISTCLEANFILES =		



