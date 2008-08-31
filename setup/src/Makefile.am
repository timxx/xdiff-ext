## Process this file with automake to produce Makefile.in

INCLUDES = \
  -I$(top_srcdir)/preferences \
  -DSETUP_DATA_DIR=\""$(datadir)"\" \
  -DSETUP_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
  @SETUP_CFLAGS@

bin_PROGRAMS = xdiffextsetup

xdiffextsetup_SOURCES = \
  main.c \
  support.c \
  support.h \
  interface.c \
  interface.h \
  callbacks.c \
  callbacks.h

xdiffextsetup_LDADD = @SETUP_LIBS@ $(INTLLIBS) -L$(top_builddir)/preferences -lpreferences

EXTRA_DIST = setup.glade \
	     setup.gladep 
