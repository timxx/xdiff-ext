## Process this file with automake to produce Makefile.in

SUBDIRS = src

extradir = ${pkgdatadir}

themedir = $(datadir)/icons/hicolor
size = 16x16
context = apps

iconsdir = $(themedir)/$(size)/$(context)

icons_DATA =            \
        pixmaps/de.png

icons_SOURCE =          \
        pixmaps/de.png

EXTRA_DIST =            \
        $(icons_SOURCE)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi
