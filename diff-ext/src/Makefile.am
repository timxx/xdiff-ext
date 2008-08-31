# Process this file with automake to produce Makefile.in

INCLUDES = \
  -I$(top_srcdir)/preferences \
  -DDIFF_EXT_DATA_DIR=\""$(pkgdatadir)"\" \
  -DDIFF_EXT_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
  @DIFF_EXT_CFLAGS@

AM_CFLAGS =-Wall -g

libxdiff_ext_la_SOURCES = main.c

lib_LTLIBRARIES = libxdiff-ext.la

libxdiff_ext_la_LDFLAGS = -module -avoid-version

libxdiff_ext_la_LIBADD = -L$(top_builddir)/preferences -lpreferences

EXTRA_libxdiff_ext_la_SOURCES = main.c

#libxdiff_ext_la_LIBADD = @MENU@
#libxdiff_ext_la_DEPENDENCIES = @MENU@

EXTRA_DIST = 
