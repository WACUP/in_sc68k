# ,----------------------------------------------------------------------.
# |                                                                      |
# |  desa68 -- Makefile.am -- Use with automake to produce Makefile.in   |
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
pkgconfig_DATA = desa68.pc

# ,----------------------------------------------------------------------.
# | Libtool library                                                      |
# `----------------------------------------------------------------------'

lib_LTLIBRARIES      = libdesa68.la
libdesa68_la_SOURCES = desa68.c desa68.h desa68_private.h
libdesa68_la_CFLAGS  = $(gb_CFLAGS)
libdesa68_la_LDFLAGS = -avoid-version $(gb_LDFLAGS)

# ,----------------------------------------------------------------------.
# | Headers                                                              |
# `----------------------------------------------------------------------'

include_HEADERS = desa68.h

# ,----------------------------------------------------------------------.
# | Distrib                                                              |
# `----------------------------------------------------------------------'

EXTRA_DIST = CMakeLists.txt cm_config.h.in
