/*
 * libre-impuesto-window-actions.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the program; if not, see <http://www.gnu.org/licenses/>
 *
 *
 * Copyright (C) 2013 Libre Software Bolivia. Henry Castro <hcvcastro@gmail.com>
 *
 */

#ifndef LIBRE_IMPUESTO_WINDOW_ACTIONS_H
#define LIBRE_IMPUESTO_WINDOW_ACTIONS_H

#define LIBRE_IMPUESTO_WINDOW_ACTION(window, name)			\
	(libre_impuesto_window_get_action (LIBRE_IMPUESTO_WINDOW (window), (name)))

#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP(window, name) \
	(libre_impuesto_window_get_action_group (LIBRE_IMPUESTO_WINDOW (window), (name)))

/* Actions */
#define LIBRE_IMPUESTO_WINDOW_ACTION_ABOUT(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "about")
#define LIBRE_IMPUESTO_WINDOW_ACTION_CLOSE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "close")
#define LIBRE_IMPUESTO_WINDOW_ACTION_CONTENTS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "contents")
#define LIBRE_IMPUESTO_WINDOW_ACTION_COPY_CLIPBOARD(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "copy-clipboard")
#define LIBRE_IMPUESTO_WINDOW_ACTION_CUT_CLIPBOARD(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "cut-clipboard")
#define LIBRE_IMPUESTO_WINDOW_ACTION_DELETE_SELECTION(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "delete-selection")
#define LIBRE_IMPUESTO_WINDOW_ACTION_FORGET_PASSWORDS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "forget-passwords")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GAL_CUSTOM_VIEW(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "gal-custom-view")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GAL_DEFINE_VIEWS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "gal-define-views")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GAL_SAVE_CUSTOM_VIEW(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "gal-save-custom-view")
#define LIBRE_IMPUESTO_WINDOW_ACTION_IMPORT(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "import")
#define LIBRE_IMPUESTO_WINDOW_ACTION_NEW_WINDOW(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "new-window")
#define LIBRE_IMPUESTO_WINDOW_ACTION_PAGE_SETUP(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "page-setup")
#define LIBRE_IMPUESTO_WINDOW_ACTION_PASTE_CLIPBOARD(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "paste-clipboard")
#define LIBRE_IMPUESTO_WINDOW_ACTION_PREFERENCES(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "preferences")
#define LIBRE_IMPUESTO_WINDOW_ACTION_QUICK_REFERENCE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "quick-reference")
#define LIBRE_IMPUESTO_WINDOW_ACTION_QUIT(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "quit")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_ADVANCED(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-advanced")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_CLEAR(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-clear")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_EDIT(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-edit")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_OPTIONS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-options")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_QUICK(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-quick")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SEARCH_SAVE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "search-save")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SELECT_ALL(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "select-all")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SHOW_SIDEBAR(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "show-sidebar")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SHOW_SWITCHER(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "show-switcher")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SHOW_TASKBAR(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "show-taskbar")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SHOW_TOOLBAR(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "show-toolbar")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SUBMIT_BUG(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "submit-bug")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_INITIAL(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-initial")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_MENU(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-menu")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_STYLE_BOTH(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-style-both")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_STYLE_ICONS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-style-icons")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_STYLE_TEXT(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-style-text")
#define LIBRE_IMPUESTO_WINDOW_ACTION_SWITCHER_STYLE_USER(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "switcher-style-user")
#define LIBRE_IMPUESTO_WINDOW_ACTION_WORK_OFFLINE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "work-offline")
#define LIBRE_IMPUESTO_WINDOW_ACTION_WORK_ONLINE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION ((window), "work-online")

/* Action Groups */
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_CUSTOM_RULES(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "custom-rules")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_GAL_VIEW(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "gal-view")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_LOCKDOWN_APPLICATION_HANDLERS(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "lockdown-application-handlers")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_LOCKDOWN_PRINTING(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "lockdown-printing")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_LOCKDOWN_PRINT_SETUP(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "lockdown-print-setup")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_LOCKDOWN_SAVE_TO_DISK(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "lockdown-save-to-disk")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_NEW_ITEM(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "new-item")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_NEW_SOURCE(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "new-source")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_IMPUESTO(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "impuesto")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_SWITCHER(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "switcher")
#define LIBRE_IMPUESTO_WINDOW_ACTION_GROUP_NEW_WINDOW(window) \
	LIBRE_IMPUESTO_WINDOW_ACTION_GROUP ((window), "new-window")


#endif /* LIBRE_IMPUESTO_WINDOW_ACTIONS_H */
