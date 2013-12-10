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

NULL =

public_icons = \
	hicolor_apps_16x16_libre-impuestos.png		\
	hicolor_apps_22x22_libre-impuestos.png		\
	hicolor_apps_24x24_libre-impuestos.png		\
	hicolor_apps_256x256_libre-impuestos.png	\
	hicolor_apps_32x32_libre-impuestos.png		\
	hicolor_apps_48x48_libre-impuestos.png		\
	$(NULL)


private_icons = \
	hicolor_actions_16x16_avatar-default.png	\
	hicolor_actions_16x16_folder.png		\
	hicolor_actions_16x16_gtk-file.png		\
	hicolor_actions_16x16_gtk-home.png		\
	hicolor_actions_16x16_info.png			\
	hicolor_actions_22x22_avatar-default.png	\
	hicolor_actions_22x22_folder.png		\
	hicolor_actions_22x22_gtk-file.png		\
	hicolor_actions_22x22_gtk-home.png		\
	hicolor_actions_22x22_info.png			\
	hicolor_actions_24x24_avatar-default.png	\
	hicolor_actions_24x24_folder.png		\
	hicolor_actions_24x24_gtk-file.png		\
	hicolor_actions_24x24_gtk-home.png		\
	hicolor_actions_24x24_info.png			\
	hicolor_actions_32x32_avatar-default.png	\
	hicolor_actions_32x32_folder.png		\
	hicolor_actions_32x32_gtk-file.png		\
	hicolor_actions_32x32_gtk-home.png		\
	hicolor_actions_32x32_info.png			\
	hicolor_actions_48x48_avatar-default.png	\
	hicolor_actions_48x48_folder.png		\
	hicolor_actions_48x48_gtk-file.png		\
	hicolor_actions_48x48_gtk-home.png		\
	hicolor_actions_48x48_info.png			\
	hicolor_apps_16x16_libre-impuestos.png		\
	hicolor_apps_22x22_libre-impuestos.png		\
	hicolor_apps_24x24_libre-impuestos.png		\
	hicolor_apps_256x256_libre-impuestos.png	\
	hicolor_apps_32x32_libre-impuestos.png		\
	hicolor_apps_48x48_libre-impuestos.png		\
	$(NULL)

stock_private_icons = \
	$(NULL)

noinst_DATA = \
	$(NULL)

EXTRA_DIST = \
	$(public_icons)		\
	$(private_icons)	\
	$(stock_private_icons)	\
	$(noinst_DATA)		\
	$(NULL)

###############################################################################

gtk_update_icon_cache = gtk-update-icon-cache -f -t

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache) $(datadir)/icons/hicolor; \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache) $(datadir)/icons/hicolor"; \
	fi

install-icons:
	for icon in $(public_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		mkdir -p $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done; \
	for icon in $(private_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		mkdir -p $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done
	for icon in $(stock_private_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f5`; \
		mkdir -p $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT/stock_$$ICONFILE; \
	done

uninstall-icons:
	-for icon in $(public_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		rm -f $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done; \
	for icon in $(private_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		rm -f $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done
	for icon in $(stock_private_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f5`; \
		rm -f $(DESTDIR)$(pkgdatadir)/$(BASE_VERSION)/icons/$$THEME/$$SIZE/$$CONTEXT/stock_$$ICONFILE; \
	done

install-data-local: install-icons update-icon-cache

uninstall-local: uninstall-icons update-icon-cache


