dnl Process this file with autoconf to produce a configure script.


dnl libre-impuestos Versions
m4_define([libre_impuestos_mayor_version], [0])
m4_define([libre_impuestos_minor_version], [0])
m4_define([libre_impuestos_micro_version], [5])
m4_define([libre_impuestos_version],
	[libre_impuestos_mayor_version.libre_impuestos_minor_version.libre_impuestos_micro_version])
m4_define([libre_impuestos_stable_version],
	[m4_if(m4_eval(libre_impuestos_minor_version % 2), [1],
	[libre_impuestos_major_version.m4_eval(libre_impuestos_minor_version - 1)],
	[libre_impuestos_version])])

dnl Required Packages
m4_define([glib_minimum_version], [2.30])
m4_define([gtk_minimum_version], [3.2.0])
m4_define([eds_minimum_version], [libre_impuesto_version])
m4_define([gtkhtml_minimum_version], [4.1.2])
m4_define([gnome_desktop_minimum_version], [2.91.3])
m4_define([gnome_icon_theme_minimum_version], [2.30.2.1])
m4_define([gsettings_desktop_schemas_minimum_version], [2.91.92])
m4_define([gconf_minimum_version], [2.0.0])		dnl XXX Just a Guess
m4_define([libgdata_minimum_version], [0.10])
m4_define([libsoup_minimum_version], [2.31.2])
m4_define([libxml_minimum_version], [2.7.3])
m4_define([shared_mime_info_minimum_version], [0.22])
m4_define([libpst_minimum_version], [0.6.54])
m4_define([libnotify_minimum_version], [0.5.1])


BASE_VERSION=0.0
m4_define([base_version], [0.0])

dnl Upgrade Revision: This is for triggering migration calls between
dnl varying versions. Reset to 0 whenever BASE_VERSION changes.
m4_define([upgrade_revision], [0])

dnl Autoconf / Automake Initialization
#AC_PREREQ([2.69])
AC_INIT([libre-impuestos],[libre_impuestos_version],[hcvcastro@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([no-dependencies -Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([impuesto/main.c])
AM_MAINTAINER_MODE
GNOME_MAINTAINER_MODE_DEFINES

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_LANG(C)
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

GLIB_GSETTINGS
GNOME_COMPILE_WARNINGS(maximum)


dnl ******************************
dnl Initialize libtool
dnl ******************************
LT_PREREQ(2.2)
LT_INIT(disable-static win32-dll)


dnl ******************************
dnl Compiler Warning Flags
dnl ******************************
AS_COMPILER_FLAGS(WARNING_FLAGS,
	"-Wall -Wextra
	-Wno-missing-field-initializers
	-Wno-sign-compare
	-Wno-unused-parameter
	-Wno-deprecated-declarations
	-Wdeclaration-after-statement
	-Werror-implicit-function-declaration
	-Wformat-security -Winit-self
	-Wmissing-declarations -Wmissing-include-dirs
	-Wmissing-noreturn -Wnested-externs -Wpointer-arith
	-Wredundant-decls -Wundef -Wwrite-strings")
AC_SUBST(WARNING_FLAGS)

AS_COMPILER_FLAGS(CPP_WARNING_FLAGS, "-Wall")
AC_SUBST(CPP_WARNING_FLAGS)


dnl Other useful compiler warnings for test builds only.
dnl These may produce warnings we have no control over,
dnl or false positives we don't always want to see.
dnl
dnl	-Wmissing-format-attribute
dnl	-Wshadow
dnl	-Wformat-nonliteral
dnl	-Wstrict-aliasing=2
dnl	-DE_BOOK_DISABLE_DEPRECATED
dnl	-DE_CAL_DISABLE_DEPRECATED

AM_CPPFLAGS="$WARNING_FLAGS -fno-strict-aliasing"
AC_SUBST(AM_CPPFLAGS)

CPP_CPPFLAGS="$CPP_WARNING_FLAGS"
AC_SUBST(CPP_CPPFLAGS)

AC_SUBST([BASE_VERSION], [base_version])
AC_DEFINE_UNQUOTED(BASE_VERSION, ["$BASE_VERSION"], [Base version (Major.Minor)])

AC_SUBST([UPGRADE_REVISION],[upgrade_revision])
AC_DEFINE_UNQUOTED(UPGRADE_REVISION, ["$UPGRADE_REVISION"], [The number of times we've upgraded since the BASE_VERSION release])

AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(VERSION_COMMENT, "", [Define if you want a comment appended to the version number])


dnl ###########################################################################
dnl Dependencies
dnl ###########################################################################


GLIB_REQUIRED=2.31
GIO_REQUIRED=2.25.10
GTK_REQUIRED=3.0
LIBXSLT_REQUIRED=1.1.7
WEBKIT_GTK_REQUIRED=1.7.92
GNOME_KEYRING_REQUIRED=2.26.0

dnl **********************************
dnl Check for base dependencies early.
dnl **********************************
PKG_CHECK_MODULES([GNOME_PLATFORM], [
	gtk+-3.0 >= gtk_minimum_version
	gio-2.0 >= glib_minimum_version
	cairo-gobject
	libxml-2.0 >= libxml_minimum_version
	libxslt >= $LIBXSLT_REQUIRED
	libre-impuestos-webkitgtk-3.0 >= $WEBKIT_GTK_REQUIRED
])
AC_SUBST(GNOME_PLATFORM_CFLAGS)
AC_SUBST(GNOME_PLATFORM_LIBS)



GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

AC_CHECK_LIB(m, log)


# ****************
# Distributor name
# ****************

AC_ARG_WITH([distributor-name],
  AS_HELP_STRING([--with-distributor-name=name],[Set the distributor name]),
  [LSB_DISTRIBUTOR="$withval"])

if test -z "$LSB_DISTRIBUTOR"; then
	AC_CHECK_PROGS([LSB_RELEASE], [lsb_release],)
	if test -n "$LSB_RELEASE"; then
		# Fallback on lsb_release if available
		LSB_DISTRIBUTOR=$($LSB_RELEASE -i -s)
	else
		# Fallback on the product name
		LSB_DISTRIBUTOR="Epiphany"
	fi
fi


# ******************
# Portability checks
# ******************

AC_CHECK_FUNCS([localtime_r memchr memmove memset mkdir mkdtemp realpath sqrt strchr strcspn strdup strerror strrchr strtol strtoul strstr])


dnl *******************
dnl Special directories
dnl *******************

dnl If you add something here, consider whether or not you also need to add it to one or more .pc.in
dnl files (for Connector, etc)

privlibdir='${libdir}'/libre-impuestos/$BASE_VERSION
AC_SUBST(privlibdir)

privlibexecdir='${libexecdir}'/libre-impuestos/$BASE_VERSION
AC_SUBST(privlibexecdir)

privdatadir='${datadir}'/libre-impuestos/$BASE_VERSION
AC_SUBST(privdatadir)

privincludedir='${includedir}'/libre-impuestos-$BASE_VERSION
AC_SUBST(privincludedir)

moduledir="$privlibdir/modules"
AC_SUBST(moduledir)

uidir="$privdatadir/ui"
AC_SUBST(uidir)

libreimpuestohelpdir="$privdatadir/help"
AC_SUBST(libreimpuestohelpdir)

icondir="$privdatadir/icons"
AC_SUBST(icondir)

imagesdir="$privdatadir/css/images"
AC_SUBST(imagesdir)

baseuridir="$privdatadir"
AC_SUBST(baseuridir)

dnl ###########################################################################
dnl Internationalization
dnl ###########################################################################

IT_PROG_INTLTOOL([0.40.0])

#AM_GNU_GETTEXT_VERSION([0.17])
#AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=libre-impuestos-$BASE_VERSION
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Package name for gettext])

localedir='$(prefix)/$(DATADIRNAME)/locale'
AC_SUBST(localedir)


dnl ###########################################################################
dnl Files to generate
dnl ###########################################################################

AC_OUTPUT([
po/Makefile.in
man/Makefile
data/Makefile
data/libre-impuestos.desktop.in
data/images/Makefile
data/icons/Makefile
util/Makefile
widgets/Makefile
impuesto/Makefile
Makefile
])
