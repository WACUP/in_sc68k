# ,----------------------------------------------------------------------.
# |                                                                      |
# |  info68 -- Makefile.am -- Use with automake to produce Makefile.in   |
# |                                                                      |
# |               Copyright (c) 2006-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I m4 --install

# ,----------------------------------------------------------------------.
# | Libtool program                                                      |
# `----------------------------------------------------------------------'

bin_PROGRAMS   = info68
info68_SOURCES = info68.c
info68_CFLAGS  = $(file68_CFLAGS) $(gb_CFLAGS)
info68_LDFLAGS = $(gb_LDFLAGS)
info68_LDADD   = $(FILE68_LA) $(file68_LIBS)

if SOURCE_FILE68

FILE68_LA = $(file68_builddir)/libfile68.la
BUILT_SOURCES = $(FILE68_LA)

@SET_MAKE@
$(FILE68_LA):
	cd "$(file68_builddir)" && $(MAKE) $(AM_MAKEFLAGS)

endif
