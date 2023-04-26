# ,----------------------------------------------------------------------.
# |                                                                      |
# |   sc68 -- Makefile.am -- Use with automake to produce Makefile.in    |
# |                                                                      |
# |               Copyright (c) 2007-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I m4 --install

# ,----------------------------------------------------------------------.
# | Libtool program                                                      |
# `----------------------------------------------------------------------'

bin_PROGRAMS  = sc68

sc68_SOURCES  = sc68.c
sc68_CFLAGS   = $(libsc68_CFLAGS) $(gb_CFLAGS)
sc68_CPPFLAGS = $(file68_CFLAGS) $(EVIL_HARDCODE)
sc68_LDFLAGS  = $(gb_LDFLAGS)
sc68_LDADD    = $(LIBSC68_LA) $(libsc68_LIBS)

if SOURCE_LIBSC68

LIBSC68_LA = $(libsc68_builddir)/libsc68.la
BUILT_SOURCES = $(LIBSC68_LA)

@SET_MAKE@
$(LIBSC68_LA):
	cd "$(libsc68_builddir)" && $(MAKE) $(AM_MAKEFLAGS)

EVIL_HARDCODE = -I$(top_builddir)/../file68

endif
