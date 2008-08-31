# Process this file with autoconf to produce a configure script.

AC_INIT(xdiff-ext, 0.1.0, szorin@comcast.net)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AM_DISABLE_STATIC
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_INTLTOOL()
AC_PROG_LIBTOOL
AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

pkg_modules="thunar-vfs-1 >= 0.9 gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(PREFERENCES, [$pkg_modules])
AC_SUBST(PREFERENCES_CFLAGS)
AC_SUBST(PREFERENCES_LIBS)

pkg_modules="thunarx-1 >= 0.9 exo-0.3 >= 0.3.4 gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(DIFF_EXT, [$pkg_modules])
AC_SUBST(DIFF_EXT_CFLAGS)
AC_SUBST(DIFF_EXT_LIBS)

pkg_modules="gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(SETUP, [$pkg_modules])
AC_SUBST(SETUP_CFLAGS)
AC_SUBST(SETUP_LIBS)

_PKG_CONFIG(EXTENSIONS_DIR,[variable=extensionsdir], [thunarx-1])
AC_ARG_VAR(EXTENSIONS_DIR)
libdir=$pkg_cv_EXTENSIONS_DIR

GETTEXT_PACKAGE=xdiff-ext
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Package name for gettext])

ALL_LINGUAS="ru"
AM_GLIB_GNU_GETTEXT

diff_extlocaledir='${prefix}/${DATADIRNAME}/locale'
AC_SUBST(diff_extlocaledir)

AC_CONFIG_FILES([
Makefile
preferences/Makefile
diff-ext/Makefile
diff-ext/src/Makefile
setup/Makefile
setup/src/Makefile
po/Makefile.in
data/Makefile
])

AC_OUTPUT
