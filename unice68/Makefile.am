# ,----------------------------------------------------------------------.
# |                                                                      |
# |  unice68 -- Makefile.am -- Use with automake to produce Makefile.in  |
# |                                                                      |
# |               Copyright (c) 2007-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I m4 --install
LIB_VER         = @LIB_CUR@:@LIB_REV@:@LIB_AGE@

# ,----------------------------------------------------------------------.
# | pkg-config                                                           |
# `----------------------------------------------------------------------'

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = unice68.pc

# ,----------------------------------------------------------------------.
# | Libtool library                                                      |
# `----------------------------------------------------------------------'

lib_LTLIBRARIES       = libunice68.la
libunice68_la_SOURCES = unice68_unpack.c unice68_pack.c unice68_version.c
libunice68_la_CFLAGS  = $(gb_CFLAGS)
libunice68_la_LDFLAGS = -avoid-version $(gb_LDFLAGS)

# ,----------------------------------------------------------------------.
# | Libtool binary                                                       |
# `----------------------------------------------------------------------'

if UNICE68_CLI

bin_PROGRAMS    = unice68
unice68_SOURCES = unice68.c
unice68_LDADD   = libunice68.la

endif

# ,----------------------------------------------------------------------.
# | Headers                                                              |
# `----------------------------------------------------------------------'

include_HEADERS = unice68.h
noinst_HEADERS  = unice68_private.h

# ,----------------------------------------------------------------------.
# | Distrib                                                              |
# `----------------------------------------------------------------------'

EXTRA_DIST = CMakeLists.txt cm_config.h.in
