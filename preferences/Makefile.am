# Process this file with automake to produce Makefile.in
INCLUDES = \
  -DDIFF_EXT_DATA_DIR=\""$(pkgdatadir)"\" \
  -DDIFF_EXT_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
  @PREFERENCES_CFLAGS@

AM_CFLAGS =-Wall -g

noinst_LTLIBRARIES = libpreferences.la

libpreferences_la_SOURCES = preferences.c preferences.h

libpreferences_la_LDFLAGS = -module -avoid-version

libpreferences_la_LIBADD = @PREFERENCES_LIBS@

#EXTRA_libpreferences_la_SOURCES = preferences.c

EXTRA_DIST = 

