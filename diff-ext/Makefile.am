# Process this file with automake to produce Makefile.in

SUBDIRS = src

themedir = $(datadir)/icons/hicolor
size = 16x16
context = actions

iconsdir = $(themedir)/$(size)/$(context)

icons_DATA =            \
	icons/clear.png \
	icons/diff3.png \
	icons/diff3_with.png \
	icons/diff_with.png \
	icons/diff_later.png \
	icons/diff.png

icons_SOURCE =          \
	icons/clear.png \
	icons/diff3.png \
	icons/diff3_with.png \
	icons/diff_with.png \
	icons/diff_later.png \
	icons/diff.png

EXTRA_DIST =            \
	$(icons_DATA)   \
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
